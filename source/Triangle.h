#pragma once
#include "Primitive.h"

class Triangle : public Primitive {
public:
	Triangle(SceneObject* pSceneObject, const Vector3f positions[], const Vector2f uvs[], const Vector3f normals[]);
	
	virtual bool Intersect(Ray ray, Intersection& isect) const override;
	
	virtual void Sample(Vector3f& p, Vector3f& normal, float& pdf) const override;

private:
	// vertices in world coord
	Vector3f mPositions[3];
	Vector3f mNormals[3]; // normals of the 3 vertices
	Vector2f mTexCoords[3];
	Vector3f mNormal;
	Vector3f mTangent; // tangent wrt du
};