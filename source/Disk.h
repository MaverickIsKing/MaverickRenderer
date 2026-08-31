#pragma once
#include "Primitive.h"

class Disk : public Primitive {
public: 
	Disk(SceneObject* pSceneObject, float R);
	virtual bool Intersect(Ray ray, Intersection& isect) const override;
private:
	float mRadius;
};