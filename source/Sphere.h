#pragma once
#include "Primitive.h"



class Sphere : public Primitive {
public:
	Sphere(SceneObject* pSceneObject, float R);

	virtual bool Intersect(Ray ray, Intersection& intersect) const override;

private:
	float mRadius;
};