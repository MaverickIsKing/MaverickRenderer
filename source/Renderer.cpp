#include "Renderer.h"
#include "Material.h"
#include <MiniFB.h>
#include <thread>
#include <vector>

const int numThreads = std::thread::hardware_concurrency();
// const int numOfSSAA = 100;

Renderer::Renderer(int w, int h, int minDepth, int maxDepth, int SamplePerPixel, const char* filepath) : mViewportWidth(w), mViewportHeight(h), mMinDepth(minDepth), mMaxDepth(maxDepth), mSamplePerPixel(SamplePerPixel) {
    mCurrentPixelIndex = 0;

    mScene = Scene::LoadSceneFromXML(filepath, w, h);
}

Renderer::~Renderer() {
    if (mScene) {
        delete mScene;
    }
}



void Renderer::Run() {
    struct mfb_window* window = mfb_open_ex("Maverick Renderer", mViewportWidth, mViewportHeight, MFB_WF_RESIZABLE);
    if (window == NULL)
        return;

    mBuffer = (uint32_t*) malloc(mViewportWidth * mViewportHeight * 4);

    std::vector<std::thread> renderThreads(numThreads);
    for (int i = 0; i < numThreads; i++) {
        renderThreads[i] = std::thread(&Renderer::RunRenderThread, this);
        renderThreads[i].detach();
    }

    mfb_update_state state;
    do {
        // TODO: add some fancy rendering to the buffer of size 800 * 600

        state = mfb_update_ex(window, mBuffer, mViewportWidth, mViewportHeight);

        if (state != MFB_STATE_OK)
            break;

    } while (mfb_wait_sync(window));

    //renderThread.join();
    free(mBuffer);
    mBuffer = NULL;
    window = NULL;
}

Color Renderer::RenderPixel(int x, int y) {
    // SSAA
    Color resultColor(0, 0, 0);
    const int N = mSamplePerPixel;
    for (int i = 0; i < N; i++) {

        float px = x + glm::linearRand(0.0f, 1.0f);
        float py = y + glm::linearRand(0.0f, 1.0f);

        Color color = RenderSubPixel(px, py);

        resultColor += color / (float)N;
    }

    return resultColor;
}

Color Renderer::RenderSubPixel(float x, float y) {
    // Get the ray emiting from camera origin through some p in the world where p depends on the pixel coordinate (x, y). ie: p = invCombinedMatrix((x, y, 0, 1))
    Ray ray = mScene->GetCamera().GetRay(x, y);
    
    return GetRadiance(ray, 0, false);
}

//Color Renderer::GetIrradiance(const Ray& ray) // no brdf considered, just irradiance
//{   
//    Intersection isect;
//    if (!mScene->Intersect(ray, isect)) {
//        return Color(0, 0, 0);
//    }
//
//    // E(p)
//
//    Color E(0, 0, 0);
//    for (Light* pLight : mScene->GetLights()) {
//        Vector3f sourcePos;
//        Color L = pLight->GetRadiance(isect.position, sourcePos);
//
//        // test if the light can be received
//        Vector3f d = sourcePos - isect.position;
//        Ray shadowRay;
//        shadowRay.o = isect.position;
//        shadowRay.d = glm::normalize(d);
//        shadowRay.mint = 1e-3f;
//        shadowRay.maxt = glm::length(d);
//
//        Intersection shadow_isect;
//        if (mScene->Intersect(shadowRay, shadow_isect)) { // if shadowRay intersect some scene obj, p is in the shadow, hence continue
//            continue;
//        }
//
//        float cosTheta = glm::dot(isect.normal, shadowRay.d);
//        E += L * glm::max(cosTheta, 0.0f);
//    }
//    return E;
//}

Color Renderer::GetRadiance(const Ray& ray, int depth, bool prevIsSpecular) // depth for indirect light recursion depth
{
    if (depth > mMaxDepth) {
        return Color(0, 0, 0);
    }
    // russian roulette
    static const float SurvivalProbability = 0.8f;
    float RewardFactor = 1.0f;
    if (depth >= mMinDepth) {
        float K = Random01();
        if (K > SurvivalProbability) {
            return Color(0, 0, 0);
        }
        RewardFactor = 1.0f / SurvivalProbability;
    }

    Intersection isect;
    SceneObject* pSceneObject = mScene->Intersect(ray, isect);
    if (pSceneObject == nullptr) {
        return Color(0, 0, 0);
    }

    Material* pMaterial = pSceneObject->GetMaterial();
    // Lo(p)

    Color Lo(0, 0, 0);

    Matrix3x3 localToWorld = MakeCoordinateSystem(isect.normal);
    Matrix3x3 worldToLocal = glm::transpose(localToWorld);

    Vector3f wo = worldToLocal * (-ray.d);

    // emissive light
    Color Le = pSceneObject->GetEmissive();
    if (prevIsSpecular || depth ==0 )
    {
        Lo += Le;
    }

    // direct light
    if (!pMaterial->IsSpecular())
    {
        for (Light* pLight : mScene->GetLights()) {
            Vector3f sourcePos;
            float pdf;
            Color L = pLight->GetRadiance(isect.position, sourcePos, pdf);

            // test if the light can be received
            Vector3f d = sourcePos - isect.position;
            Ray shadowRay;
            shadowRay.o = isect.position;
            shadowRay.d = glm::normalize(d);
            shadowRay.mint = 1e-3f;
            shadowRay.maxt = glm::length(d) - 1e-4f;

            Intersection shadow_isect;
            if (mScene->Intersect(shadowRay, shadow_isect)) { // if shadowRay intersect some scene obj, p is in the shadow, hence continue
                continue;
            }
            Vector3f wi = worldToLocal * shadowRay.d;
            float cosTheta = glm::dot(isect.normal, shadowRay.d);
            Color brdf = pMaterial->BRDF(wo, wi);
            Lo += brdf * L * glm::max(cosTheta, 0.0f) / std::max(1e-5f, pdf);
        }
    }
    // indirect light
    //reflection
    {
        float pdf;
        Vector3f wi = pMaterial->SampleWi(wo, pdf);
        Color brdf = pMaterial->BRDF(wo, wi);
        Ray r;
        r.d = localToWorld * wi;
        r.o = isect.position;
        r.mint = 1e-3f;
        Color Li = GetRadiance(r, depth + 1, pMaterial->IsSpecular());

        Lo += brdf * Li * fabs(wi.z) / std::max(1e-5f, pdf);
    }

    //refraction
    {    
        Vector3f wi;
        if (pMaterial->SampleWt(wo, wi))
        {
            Color btdf = pMaterial->BTDF(wo, wi);
            Ray r;
            r.d = localToWorld * wi;
            r.o = isect.position;
            r.mint = 1e-3f;
            Color Li = GetRadiance(r, depth + 1, pMaterial->IsSpecular());
            Lo += btdf * Li * fabs(wi.z);
        } 
    }
    // indirect light (monte carlo integral # of trails = # SSAA)
    //if (pMaterial->IsSpecular()) // for spacular indirect light come from only one possible direction
    //{   // reflection
    //    {
    //        Vector3f wi(-wo.x, -wo.y, wo.z);
    //        Color brdf = pMaterial->BRDF(wo, wi);
    //        Ray r;
    //        r.d = localToWorld * wi;
    //        r.o = isect.position;
    //        r.mint = 1e-3f;
    //        Color Li = GetRadiance(r, depth + 1);
    //        Lo += brdf * Li * fabs(wi.z);
    //    }
    //    // refrection
    //    {
    //        Vector3f wi;
    //        if (pMaterial->SampleWt(wo, wi))
    //        {
    //            Color btdf = pMaterial->BTDF(wo, wi);
    //            Ray r;
    //            r.d = localToWorld * wi;
    //            r.o = isect.position;
    //            r.mint = 1e-3f;
    //            Color Li = GetRadiance(r, depth + 1);
    //            Lo += btdf * Li * fabs(wi.z);
    //        }
    //    }
    //}
    //else
    //{   // integral_{0}^{2pi}integral_{0}^{pi/2}brdf(theta, phi) * L(theta, phi) * cos(theta) * sin(theta) * (dtheta)(dphi)
    //    // not uniform
    //    //const float theta = Random(0.0f, PI * 0.5f);
    //    //const float phi = Random(0.0f, PI * 2.0f);
    //    //Vector3f wi = GetSphericalCoordinate(theta, phi);

    //    // now use pdf/cdf uniform
    //    // Vector3f wi = UniformSampleHemisphere();
    //    float pdf;
    //    Vector3f wi = pMaterial->SampleWi(wo, pdf);
    //    Color brdf = pMaterial->BRDF(wo, wi);
    //    Ray r;
    //    r.d = localToWorld * wi;
    //    r.o = isect.position;
    //    r.mint = 1e-3f;
    //    Color Li = GetRadiance(r, depth + 1);

    //    // Lo += brdf * Li * cos(theta) * sin(theta) * PI * PI
    //    // Lo += brdf * Li * wi.z * 2.0f * PI;
    //    Lo += brdf * Li * wi.z / std::max(1e-5f, pdf);
    //}
    return Lo * RewardFactor;
}

// render thread's entry
void Renderer::RunRenderThread() {
    //multi threads:
    
    // fetch the next pixel in current viewport waiting to be rendered
    while (true) {
        int pixelIndex = mCurrentPixelIndex.fetch_add(1);
        if (pixelIndex >= mViewportWidth * mViewportHeight) {
            break;
        }
        
        int x = pixelIndex % mViewportWidth;
        int y = pixelIndex / mViewportWidth;

        Color color = RenderPixel(x, y);
        uint32_t r = glm::clamp((uint32_t)std::round(color.r * 255.0f), 0u, 255u);
        uint32_t g = glm::clamp((uint32_t)std::round(color.g * 255.0f), 0u, 255u);
        uint32_t b = glm::clamp((uint32_t)std::round(color.b * 255.0f), 0u, 255u);
        uint32_t a = 255;
        mBuffer[y * mViewportWidth + x] = (a << 24) | (r << 16) | (g << 8) | b;
    }
}

