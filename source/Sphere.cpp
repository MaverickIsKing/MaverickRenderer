#include "Sphere.h"

Sphere::Sphere(const Vector3f center, float R) :mRadius(R) {
	mObjectToWorld = MakeWorldTransform(center, Vector3f(0.0f, 0.0f, 0.0f), 1.0f);
	mWorldToObject = glm::inverse(mObjectToWorld);
}

bool Sphere::Intersect(Ray ray, Intersection& isect) const {
	// ray transfrom to local coordinate
	Ray r = mWorldToObject * ray;

	//equation to solve t: d^2t^2 + 2(o.d)t + o^2 - R^2 = 0
	float A = glm::dot(r.d, r.d);
	float B = 2.0f * glm::dot(r.d, r.o);
	float C = glm::dot(r.o, r.o) - (mRadius * mRadius);
	
	float delta = B * B - 4.0f * A * C;
	if (delta < 0) {
		return false;
	}
	
	float sqrtDelta = sqrtf(delta);
	float t1 = (-B - sqrtDelta) / (2.0f * A);
	float t2 = (-B + sqrtDelta) / (2.0f * A);
	// t1 < t2:
	if (t2 < r.mint || t1 > r.maxt) {
		return false;
	}

	float t = t1;
	if (t1 < r.mint) {
		t = t2;
		if (t > r.maxt) {
			return false;
		}
	}
	
	Vector3f p = r.o + t * r.d; // local intersection 
	Vector3f n = glm::normalize(p); // local normal
	
	isect.position = Vector3f(mObjectToWorld * Vector4f(p, 1.0f)); // world intersection
	isect.normal = glm::normalize(Vector3f(mObjectToWorld * Vector4f(n, 0.0f))); // world normal
	isect.t = t;

	return true;
}
