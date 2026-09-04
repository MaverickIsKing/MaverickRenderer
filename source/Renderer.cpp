#include "Renderer.h"
#include <MiniFB.h>
#include <thread>
#include <vector>

const int numThreads = std::thread::hardware_concurrency();
// const int numOfSSAA = 100;

Renderer::Renderer(int w, int h, int SamplePerPixel, const char* filepath) : mViewportWidth(w), mViewportHeight(h), mSamplePerPixel(SamplePerPixel) {
    mCurrentPixelIndex = 0;

    mScene = Scene::LoadSceneFromXML(filepath, w, h);
    
    //mScene->CreateLight<DirectionalLight>(Vector3f(-1, -1, -1), Color(1, 0, 0));
    //mScene->CreateLight<PointLight>(Vector3f(0, 5, 0), Color(0, 1.0f, 0), Vector3f(1, 0,0));
    
    
    //mScene = new Scene();
    ////setting camera
    //Camera camera;
    //camera.Initialize(
    //    Vector3f(0.0f, 0.0f, 0.0f), // camera position
    //    Vector3f(0.0f, 0.0f, 1.0f), // target
    //    Vector3f(0.0f, 1.0f, 0.0f), // up
    //    glm::radians(60.0f), // FOV
    //    0.1f, // near panel
    //    1000.0f, // far panel
    //    w, h); // viewport W, H

    //mScene->SetCamera(camera);

    ////setting scene (adding SceneObjects)

    //SceneObject* pSceneObject = mScene->CreateSceneObject(Vector3f(0, 0, 5), Vector3f(0, 0, 0), 2.0f);
    //pSceneObject->CreatePrimitive<Triangle>(Vector3f(-1, -1, 0), Vector3f(1, -1, 0), Vector3f(1, 1, 0));
    //pSceneObject->CreatePrimitive<Triangle>(Vector3f(-1, -1, 0), Vector3f(1, 1, 0), Vector3f(-1, 1, 0));

    //SceneObject* pSceneObject2 = mScene->CreateSceneObject(Vector3f(0, 0, 2.0f), Vector3f(0, 0, 0), 1.0f);
    //pSceneObject2->CreatePrimitive<Sphere>(0.5f);
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
    
    
    //Intersection isect;
    //Color color(0, 0, 0);

    //if (mScene->Intersect(ray, isect)) {
    //    color = isect.normal * 0.5f + 0.5f;
    //}
    
    
    //bool hit = false;
    //for (const auto& primitive : mPrimitives) {
    //    if (primitive->Intersect(ray, isect)) {
    //        ray.maxt = isect.t;
    //        hit = true;
    //    }
    //}

    //if (hit) {
    //    color = isect.normal * 0.5f + 0.5f;
    //}
    return GetIrradiance(ray);
}

Color Renderer::GetIrradiance(const Ray& ray)
{   
    Intersection isect;
    if (!mScene->Intersect(ray, isect)) {
        return Color(0, 0, 0);
    }

    // E(p)

    Color E(0, 0, 0);
    for (Light* pLight : mScene->GetLights()) {
        Vector3f sourcePos;
        Color L = pLight->GetRadiance(isect.position, sourcePos);

        // test if the light can be received
        Vector3f d = sourcePos - isect.position;
        Ray shadowRay;
        shadowRay.o = isect.position;
        shadowRay.d = glm::normalize(d);
        shadowRay.mint = 1e-3f;
        shadowRay.maxt = glm::length(d);

        Intersection shadow_isect;
        if (mScene->Intersect(shadowRay, shadow_isect)) { // if shadowRay intersect some scene obj, p is in the shadow, hence continue
            continue;
        }

        float cosTheta = glm::dot(isect.normal, shadowRay.d);
        E += L * glm::max(cosTheta, 0.0f);
    }
    return E;
}

// render thread's entry
void Renderer::RunRenderThread() {
    //single thread:
    
    //for (int y = 0; y < mViewportHeight; y++) {
    //    for (int x = 0; x < mViewportWidth; x++) {
    //        Color color = RenderPixel(x, y);
    //        uint32_t r = glm::clamp((uint32_t)std::round(color.r * 255.0f), 0u, 255u);
    //        uint32_t g = glm::clamp((uint32_t)std::round(color.g * 255.0f), 0u, 255u);
    //        uint32_t b = glm::clamp((uint32_t)std::round(color.b * 255.0f), 0u, 255u);
    //        uint32_t a = 255;
    //        mBuffer[y * mViewportWidth + x] = (a << 24) | (r << 16) | (g << 8) | b;
    //    }
    //}

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

