#pragma once
#include "Primitive.h"

class Disk : public Primitive {
public: 
	Disk(SceneObject* pSceneObject, float R);
	virtual bool Intersect(Ray ray, Intersection& isect) const override;
	virtual void Sample(Vector3f& p, Vector3f& normal, float& pdf) const override;
private:
	float mRadius;
};