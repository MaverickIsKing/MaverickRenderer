#include "Scene.h"

SceneObject* Scene::Intersect(Ray ray, Intersection& isect) const {
	SceneObject* pHitObject = nullptr;
	for (const auto& pSceneObject : mSceneObjects) {
		if (pSceneObject->Intersect(ray, isect)) {
			ray.maxt = isect.t;
			pHitObject = pSceneObject;
		}
	}
	return pHitObject;
}

SceneObject* Scene::CreateSceneObject(const Vector3f& position, const Vector3f& euler, float scale) {
	SceneObject* pSceneObject = new SceneObject(position, euler, scale);
	mSceneObjects.push_back(pSceneObject);
	return pSceneObject;
}

Texture* Scene::GetOrCreateTexture(const std::string& filePath)
{
	if (mTextures.find(filePath) != mTextures.end())
	{
		return mTextures[filePath];
	}

	Texture* pTexture = Texture::CreateFromFile(filePath);
	if (pTexture == nullptr)
		return nullptr;

	mTextures[filePath] = pTexture;
	return pTexture;
}

Scene::~Scene() {
	for (SceneObject* pSceneObject : mSceneObjects) {
		if (pSceneObject) {
			delete pSceneObject;
		}
	}

	for (Light* pLight : mLights) {
		if (pLight) {
			delete pLight;
		}
	}

	for (auto& pair : mMaterials) {
		delete pair.second;
	}
}
