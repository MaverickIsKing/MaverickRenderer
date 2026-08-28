#include "Disk.h"

Disk::Disk(const Vector3f& center, const Vector3f& euler, float R) :mRadius(R) {
	mObjectToWorld = MakeWorldTransform(center, euler, 1.0f);
	mWorldToObject = glm::inverse(mObjectToWorld);

}

bool Disk::Intersect(Ray ray, Intersection& isect) const {
	// transform ray to local
	Ray r = mWorldToObject * ray;

	// t = -o.z / d.z
	if (fabs(r.d.z) < 1e-6f) { // ray parallel to disk
		return false;
	}
	float t = -r.o.z / r.d.z; // local coordinate for disk has z = 0

	if (t < r.mint || t > r.maxt) {
		return false;
	}

	Vector3f p = r.o + t * r.d; // local intersection coord
	// |p| <= radius?
	if (glm::dot(p, p) > mRadius * mRadius) {
		return false;
	}

	isect.position = Vector3f(mObjectToWorld * Vector4f(p, 1.0f)); // world intersection point
	isect.normal = glm::normalize(Vector3f(mObjectToWorld * Vector4f(0, 0, 1, 0))); // world normal (transformed from z-axis = (0,0,1,0))
	isect.t = t;
	
	return true;
}
