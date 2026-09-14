#include "Disk.h"
#include "SceneObject.h"

Disk::Disk(SceneObject* pSceneObject, float R) : Primitive(pSceneObject), mRadius(R) {
	mArea = PI * R * R;
}

bool Disk::Intersect(Ray ray, Intersection& isect) const {
	// transform ray to local
	Ray r = m_pSceneObject->GetWorldToObject() * ray;

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

	isect.position = Vector3f(m_pSceneObject->GetObjectToWorld() * Vector4f(p, 1.0f)); // world intersection point
	isect.normal = glm::normalize(Vector3f(m_pSceneObject->GetObjectToWorld() * Vector4f(0, 0, 1, 0))); // world normal (transformed from z-axis = (0,0,1,0))
	isect.t = t;
	
	return true;
}

void Disk::Sample(Vector3f& p, Vector3f& normal, float& pdf) const
{
	Vector2f p2 = UniformSampleDisk(mRadius);
	p = Vector3f(p2.x, p2.y, 0);
	// p needs to be translated to world coord
	p = Vector3f(m_pSceneObject->GetObjectToWorld() * Vector4f(p, 1.0f));
	normal = glm::normalize(Vector3f(m_pSceneObject->GetObjectToWorld() * Vector4f(0, 0, 1, 0)));
	pdf = 1.0f / mArea;
}
