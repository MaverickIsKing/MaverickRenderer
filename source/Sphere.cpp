#include "Sphere.h"
#include "SceneObject.h"

Sphere::Sphere(SceneObject* pSceneObject, float R) : Primitive(pSceneObject), mRadius(R) {
	mArea = 4.0f * PI * R * R;
}

bool Sphere::Intersect(Ray ray, Intersection& isect) const {
	// ray transfrom to local coordinate
	Ray r = m_pSceneObject->GetWorldToObject() * ray;

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

	isect.position = Vector3f(m_pSceneObject->GetObjectToWorld() * Vector4f(p, 1.0f)); // world intersection
	isect.normal = glm::normalize(Vector3f(m_pSceneObject->GetObjectToWorld() * Vector4f(n, 0.0f))); // world normal
	isect.t = t;

	float theta = std::acosf(n.z);
	float phi = std::atan2f(p.y, p.x);
	if (phi < 0)
	{
		phi += 2 * PI;
	}

	isect.uv.x = phi * 0.5f * INV_PI;
	isect.uv.y = theta * INV_PI;

	// dp / du
	Vector3f dpdu(-p.y, p.x, 0);
	isect.tangent = m_pSceneObject->GetObjectToWorld() * Vector4f(dpdu, 0);
	isect.tangent = glm::normalize(isect.tangent);
	return true;
}

void Sphere::Sample(Vector3f& p, Vector3f& normal, float& pdf) const
{
	p = UniformSampleSphere();
	normal = p;
	p *= mRadius;
	// translate p to world coord
	p = Vector3f(m_pSceneObject->GetObjectToWorld() * Vector4f(p, 1.0f));
	normal = glm::normalize(Vector3f(m_pSceneObject->GetObjectToWorld() * Vector4f(normal, 1.0f)));
	pdf = 1.0f / mArea;
}
