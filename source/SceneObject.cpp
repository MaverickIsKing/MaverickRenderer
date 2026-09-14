#include "SceneObject.h"

SceneObject::~SceneObject()
{
	for (auto& primitive : mPrimitives) {
		if (primitive) {
			delete primitive;
		}
	}
}

bool SceneObject::Intersect(Ray ray, Intersection& isect) const
{
	bool hit = false;
	for (const auto& primitive : mPrimitives) {
		if (primitive->Intersect(ray, isect)) {
			ray.maxt = isect.t;
			hit = true;
		}
	}
	return hit;
}

void SceneObject::Sample(Vector3f& p, Vector3f& normal, float& pdf)
{
	// choose a primitive randomly
	int index = RandomInt(0, mPrimitives.size() - 1);
	mPrimitives[index]->Sample(p, normal, pdf);
	
	float totalArea = 0.0f;
	for (Primitive* mPrimitive : mPrimitives)
	{
		totalArea += mPrimitive->getArea();
	}
	float div = std::max(1e-3f, totalArea);
	pdf = pdf * mPrimitives[index]->getArea() / div;
}
