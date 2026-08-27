#include "Renderer.h"
#include <MiniFB.h>
#include <thread>
#include <vector>

const int numThreads = std::thread::hardware_concurrency();

Renderer::Renderer(int w, int h) : mViewportWidth(w), mViewportHeight(h) {
    mCurrentPixelIndex = 0;
    
    mCamera.Initialize(
        Vector3f(10.0f, 5.0f, 20.0f), // camera position
        Vector3f(10.0f, 10.0f, 30.0f), // target
        Vector3f(0.0f, 1.0f, 0.0f), // up
        glm::radians(60.0f), // FOV
        0.1f, // near panel
        1000.0f, // far panel
        w, h); // viewport W, H
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
    // Get the ray emiting from camera origin through some p in the world where p depends on the pixel coordinate (x, y). ie: p = invCombinedMatrix((x, y, 0, 1))
    Ray ray = mCamera.GetRay(x, y);
    
    Vector3f d = ray.d;
    Color color = d * 0.5f + 0.5f;

    //Color color;
    //color.r = (float)x / mViewportWidth;
    //color.g = (float)y / mViewportHeight;
    //color.b = 0.0f;
    // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    return color;
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
    // fetch the next pixel on current viewport waiting to be rendered
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

