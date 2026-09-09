#include "Light.h"

Color DirectionalLight::GetRadiance(const Vector3f& p, Vector3f& sourcePos) const {
	sourcePos = p - mDirection * 100000.0f; // assuming source is at infinity
	return mRadiance;
}

Color PointLight::GetRadiance(const Vector3f& p, Vector3f& sourcePos) const {
	sourcePos = mPosition;
	float R = glm::length(p - mPosition);
	float attenuation = 1.0f / glm::max(1e-5f, (mAttenuations.x * R * R + mAttenuations.y * R + mAttenuations.z));
	return mIntensity * attenuation;
}

Color SpotLight::GetRadiance(const Vector3f& p, Vector3f& sourcePos) const {
	sourcePos = mPosition;
	float R = glm::length(p - mPosition);
	// distance decay coeff k1
	float k1 = 1.0f / glm::max(1e-5f, (mAttenuations.x * R * R + mAttenuations.y * R + mAttenuations.z));
	// angle decay coeff k2
	Vector3f L = glm::normalize(p - mPosition);
	float cosTheta = glm::dot(L, mDirection);
	float k2 = (cosTheta - mCosOuterAngle) / (mCosInnerAngle - mCosOuterAngle);

	return mIntensity * k1 * glm::clamp(k2, 0.0f, 1.0f);
}
