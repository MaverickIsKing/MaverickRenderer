#pragma once
#include <atomic>
#include "Common.h"
#include "Camera.h"
#include "Sphere.h"
#include "Disk.h"
#include "Triangle.h"
#include "Scene.h"
#include "Texture.h"
#include <vector>

class Renderer {
public:
	Renderer(int w, int h, int minDepth, int maxDepth, int SamplePerPixel, const char* filepath);
	virtual ~Renderer();
	void Run();

private:
	Color RenderPixel(int x, int y);

	Color RenderSubPixel(float x, float y);

	Color GetIrradiance(const Ray& ray);

	Color GetRadiance(const Ray& ray, int depth, bool prevIsSpecular); // depth for indirect light recursion depth

	void RunRenderThread();

	int mViewportWidth = 800;
	int mViewportHeight = 600;

	int mSamplePerPixel = 100;
	int mMaxDepth = 10;
	int mMinDepth = 3;
	uint32_t* mBuffer = nullptr;
	
	std::atomic<int> mCurrentPixelIndex = 0;

	Scene* mScene = nullptr;

};