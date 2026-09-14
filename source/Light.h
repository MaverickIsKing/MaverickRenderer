#pragma once
#include "Ray.h"
#include "SceneObject.h"

class Light {
public:
	// for point p, calculate L(p) radiance
	virtual Color GetRadiance(const Vector3f& p, Vector3f& sourcePos, float& pdf) const = 0;
};

class DirectionalLight : public Light {
public:

	DirectionalLight(const Vector3f& direction, const Color& radiance) : mDirection(glm::normalize(direction)), mRadiance(radiance) {}

	virtual Color GetRadiance(const Vector3f& p, Vector3f& sourcePos, float& pdf) const override;

private:
	Vector3f mDirection; // light direction
	Color mRadiance;
};

class PointLight : public Light {
public:

	PointLight(const Vector3f& position, const Color& intensity, const Vector3f& attenuations) : mPosition(position), mIntensity(intensity), mAttenuations(attenuations) {}
	
	virtual Color GetRadiance(const Vector3f& p, Vector3f& sourcePos, float& pdf) const override;

private:
	Vector3f mPosition; // light source pos
	Color mIntensity;
	Vector3f mAttenuations; // decay parameters 1 / Ar^2 + Br + C
};

class SpotLight : public Light {
public:

	SpotLight(const Vector3f& position, const Vector3f& direction, const Color& intensity, float innerAngle, float outerAngle, const Vector3f& attenuations)
		: mPosition(position), mDirection(glm::normalize(direction)), mIntensity (intensity), mCosInnerAngle(std::cos(innerAngle)), mCosOuterAngle(std::cos(outerAngle)), mAttenuations(attenuations) {}

	virtual Color GetRadiance(const Vector3f& p, Vector3f& sourcePos, float& pdf) const override;

private:
	Vector3f mDirection;
	Vector3f mPosition;
	Color mIntensity;
	Vector3f mAttenuations;
	float mCosInnerAngle;
	float mCosOuterAngle;
};

// area light
class AreaLight : public Light
{
public:
	AreaLight(SceneObject* pSceneObject) : m_pSceneObject(pSceneObject) {}

	virtual Color GetRadiance(const Vector3f& p, Vector3f& sourcePos, float& pdf) const override;
private:
	SceneObject* m_pSceneObject;
};