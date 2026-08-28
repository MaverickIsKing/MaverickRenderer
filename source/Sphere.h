#pragma once
#include "Ray.h"



class Sphere {
public:
	Sphere(const Vector3f center, float R);

	bool Intersect(Ray ray, Intersection& intersect) const;

private:
	float mRadius;
	Matrix4x4 mObjectToWorld;
	Matrix4x4 mWorldToObject;
};