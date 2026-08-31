#pragma once
#include <atomic>
#include "Common.h"
#include "Camera.h"
#include "Sphere.h"
#include "Disk.h"
#include "Triangle.h"
#include "Scene.h"
#include <vector>

class Renderer {
public:
	Renderer(int w, int h, int SamplePerPixel, const char* filepath);
	virtual ~Renderer();
	void Run();

private:
	Color RenderPixel(int x, int y);

	Color RenderSubPixel(float x, float y);

	void RunRenderThread();

	int mViewportWidth = 800;
	int mViewportHeight = 600;

	int mSamplePerPixel = 100;

	uint32_t* mBuffer = nullptr;
	
	std::atomic<int> mCurrentPixelIndex = 0;

	Scene* mScene = nullptr;
};