#include "Light.h"

Color DirectionalLight::GetRadiance(const Vector3f& p, Vector3f& sourcePos, float& pdf) const {
	sourcePos = p - mDirection * 100000.0f; // assuming source is at infinity
	pdf = 1.0f;
	return mRadiance;
}

Color PointLight::GetRadiance(const Vector3f& p, Vector3f& sourcePos, float& pdf) const {
	sourcePos = mPosition;
	float R = glm::length(p - mPosition);
	float attenuation = 1.0f / glm::max(1e-5f, (mAttenuations.x * R * R + mAttenuations.y * R + mAttenuations.z));
	pdf = 1.0f;
	return mIntensity * attenuation;
}

Color SpotLight::GetRadiance(const Vector3f& p, Vector3f& sourcePos, float& pdf) const {
	sourcePos = mPosition;
	float R = glm::length(p - mPosition);
	// distance decay coeff k1
	float k1 = 1.0f / glm::max(1e-5f, (mAttenuations.x * R * R + mAttenuations.y * R + mAttenuations.z));
	// angle decay coeff k2
	Vector3f L = glm::normalize(p - mPosition);
	float cosTheta = glm::dot(L, mDirection);
	float k2 = (cosTheta - mCosOuterAngle) / (mCosInnerAngle - mCosOuterAngle);
	pdf = 1.0f;
	return mIntensity * k1 * glm::clamp(k2, 0.0f, 1.0f);
}

Color AreaLight::GetRadiance(const Vector3f& p, Vector3f& sourcePos, float& pdf) const
{
	Vector3f A;
	Vector3f normalA;
	float pdf_A;
	m_pSceneObject->Sample(A, normalA, pdf_A);

	sourcePos = A;
	Vector3f d = p - A;
	float R = glm::length(d);
	d /= std::max(1e-5f, R);

	float cosThetaA = glm::dot(d, normalA);
	if (cosThetaA < 1e-5f)
	{	
		pdf = 1.0f;
		return Color(0.0f);
	}

	pdf = R * R * pdf_A / cosThetaA;

	return m_pSceneObject->GetEmissive();
}
