#pragma once
#include "Ray.h"
#include "Primitive.h"
#include <vector>

class SceneObject {
public:
	SceneObject(const Vector3f& position, const Vector3f& euler, float scale) {
		mObjectToWorld = MakeWorldTransform(position, euler, scale);
		mWorldToObject = glm::inverse(mObjectToWorld);
	}

	virtual ~SceneObject();

	//void AddPrimitive(Primitive* primitive) { mPrimitives.push_back(primitive); }

	bool Intersect(Ray ray, Intersection& isect) const;

	Matrix4x4 GetObjectToWorld() const { return mObjectToWorld; }

	Matrix4x4 GetWorldToObject() const { return mWorldToObject; }

	template<typename T, typename...Args>
	T* CreatePrimitive(Args&&... args) {
		T* primitive = new T(this, std::forward<Args>(args)...);
		mPrimitives.push_back(primitive);
		return primitive;
	}

private:
	Matrix4x4 mObjectToWorld;
	Matrix4x4 mWorldToObject;

	std::vector<Primitive*> mPrimitives;
};