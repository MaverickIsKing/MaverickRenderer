#include "Triangle.h"
#include "SceneObject.h"

Triangle::Triangle(SceneObject* pSceneObject, const Vector3f positions[], const Vector2f uvs[]) : Primitive(pSceneObject) {
	Matrix4x4 worldMatrix = m_pSceneObject->GetObjectToWorld();

	for (int i = 0; i < 3; i++)
	{
		mPositions[i] = Vector3f(worldMatrix * Vector4f(positions[i], 1.0f));
		mTexCoords[i] = uvs[i];
	}

	Vector3f edge1 = mPositions[1] - mPositions[0];
	Vector3f edge2 = mPositions[2] - mPositions[0];
	mNormal = glm::normalize(glm::cross(edge1, edge2));
	mArea = 0.5f * glm::length(glm::cross(edge1, edge2));

	// dp / du
	float du1 = mTexCoords[1].x - mTexCoords[0].x;
	float dv1 = mTexCoords[1].y - mTexCoords[0].y;
	float du2 = mTexCoords[2].x - mTexCoords[0].x;
	float dv2 = mTexCoords[2].y - mTexCoords[0].y;

	float det = du1 * dv2 - du2 * dv1;
	if (fabs(det) < 1e-10f)
	{
		mTangent = Vector3f(1.0f, 0.0f, 0.0f);
	}
	else
	{
		mTangent = glm::normalize((dv2 * edge1 - dv1 * edge2) / det);
	}
}

bool Triangle::Intersect(Ray ray, Intersection& isect) const {
	Vector3f p0 = mPositions[0];
	Vector3f p1 = mPositions[1];
	Vector3f p2 = mPositions[2];

	Vector3f e1 = p1 - p0;
	Vector3f e2 = p2 - p0;
	Vector3f s = ray.o - p0;

	Vector3f s1 = glm::cross(ray.d, e2);
	Vector3f s2 = glm::cross(s, e1);

	float det = glm::dot(s1, e1);
	if (fabs(det) < 1e-6f) {
		return false;
	}
	float invDet = 1.0f / det;
	float b1 = glm::dot(s1, s) * invDet;
	float b2 = glm::dot(s2, ray.d) * invDet;
	float t = glm::dot(s2, e2) * invDet;
	float b0 = 1 - b1 - b2;
	if (t < ray.mint || t > ray.maxt) {
		return false;
	}
	if (b1 < 0.0f || b2 < 0.0f || b0 < 0.0f) {
		return false;
	}
	isect.position = ray.o + t * ray.d;
	isect.normal = mNormal;
	isect.tangent = mTangent;
	isect.uv = b0 * mTexCoords[0] + b1 * mTexCoords[1] + b2 * mTexCoords[2];
	isect.t = t;
	return true;
}

void Triangle::Sample(Vector3f& p, Vector3f& normal, float& pdf) const
{
	p = UniformSampleTriangle(mPositions[0], mPositions[1], mPositions[2]);
	normal = mNormal;
	pdf = 1.0f / mArea;
}
