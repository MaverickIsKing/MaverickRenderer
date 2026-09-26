#include "Material.h"

Color LambertMaterial::BRDF(const Vector3f& wo, const Vector3f& wi) const
{
	if (wo.z <= 0 || wi.z <= 0) { return Color(0.0f); }
	return mAlbedo * INV_PI;
}

Vector3f LambertMaterial::SampleWi(const Vector3f& wo, float& pdf) const
{
	float e1 = Random01();
	float e2 = Random01();

	float cosTheta = sqrt(1.0f - e1);
	float sinTheta = sqrt(e1);
	float phi = 2.0f * PI * e2;
	Vector3f wi;
	wi.x = sinTheta * cos(phi);
	wi.y = sinTheta * sin(phi);
	wi.z = cosTheta;
	pdf = cosTheta * INV_PI;
	return wi;
}


Color ConductorSpecularMaterial::BRDF(const Vector3f& wo, const Vector3f& wi) const
{
	if (!(fabs(wo.x + wi.x) < 1e-4f && fabs(wo.y + wi.y) < 1e-4f && fabs(wo.z - wi.z) < 1e-4f))
	{
		return Color(0.0f, 0.0f, 0.0f);
	}
	float cosTheta = wi.z;
	Color k = mAbsorptionCoef;
	// r parallel
	Color r1 = ((mEta * mEta + k * k) * cosTheta * cosTheta - 2.0f * mEta * cosTheta + 1.0f) / ((mEta * mEta + k * k) * cosTheta * cosTheta + 2.0f * mEta * cosTheta + 1.0f);
	// r perp
	Color r2 = (mEta * mEta + k * k - 2.0f * mEta * cosTheta + cosTheta * cosTheta) / (mEta * mEta + k * k + 2.0f * mEta * cosTheta + cosTheta * cosTheta);
	// fresnel
	Color Fr = (r1 + r2) * 0.5f;
	return Fr * mReflectionColor / std::max(fabs(cosTheta), 1e-4f);
}

Vector3f ConductorSpecularMaterial::SampleWi(const Vector3f& wo, float& pdf) const
{	
	Vector3f wi(-wo.x, -wo.y, wo.z);
	pdf = 1.0f;
	return wi;
}

Color DielectricSpecularMaterial::BRDF(const Vector3f& wo, const Vector3f& wi) const
{	
	float cos_i = fabs(wi.z);
	float eta_i, eta_t;
	if (wi.z > 0)
	{
		eta_i = 1.0f;
		eta_t = mEta;
	}
	else
	{
		eta_t = 1.0f;
		eta_i = mEta;
	}
	Vector3f wt;

	float Fr = 1.0f;
	if (ComputeRefractVector(wi, eta_i, eta_t, wt))
	{
		float cos_t = fabs(wt.z);
		Fr = Fresnel(eta_i, eta_t, cos_i, cos_t);
	}
	return Color(Fr) / std::max(cos_i, 1e-3f);
}

Color DielectricSpecularMaterial::BTDF(const Vector3f& wo, const Vector3f& wi) const
{
	float cos_i = fabs(wi.z);
	float eta_i, eta_t;
	if (wi.z > 0)
	{
		eta_i = 1.0f;
		eta_t = mEta;
	}
	else
	{
		eta_t = 1.0f;
		eta_i = mEta;
	}
	float cos_t = fabs(wo.z);
	float Fr = Fresnel(eta_i, eta_t, cos_i, cos_t);
	return mTransmissionColor * glm::max(0.0f, 1.0f - Fr) * (eta_i * eta_i) / (eta_t * eta_t * std::max(cos_i, 1e-3f));
}

bool DielectricSpecularMaterial::SampleWt(const Vector3f& wi, Vector3f& wt) const
{
	float cos_i = std::fabs(wi.z);
	float eta_i, eta_t;
	if (wi.z > 0)
	{
		eta_i = 1.0f;
		eta_t = mEta;
	}
	else
	{
		eta_t = 1.0f;
		eta_i = mEta;
	}
	return ComputeRefractVector(wi, eta_i, eta_t, wt);
}

Vector3f DielectricSpecularMaterial::SampleWi(const Vector3f& wo, float& pdf) const
{
	Vector3f wi(-wo.x, -wo.y, wo.z);
	pdf = 1.0f;
	return wi;
}

float DielectricSpecularMaterial::Fresnel(float eta_i, float eta_t, float cos_i, float cos_t)
{
	float r1 = (eta_t * cos_i - eta_i * cos_t) / (eta_t * cos_i + eta_i * cos_t);
	float r2 = (eta_i * cos_i - eta_t * cos_t) / (eta_i * cos_i + eta_t * cos_t);
	return 0.5f * (r1 * r1 + r2 * r2);
}

Color OrenNayarMaterial::BRDF(const Vector3f& wo, const Vector3f& wi) const
{	
	if (wo.z <= 0 || wi.z <= 0) { return Color(0.0f); }
	static const float epsilon = 1e-5f;
	float sigma2 = mSigma * mSigma;
	float A = 1.0f - (sigma2 / (2.0f * (sigma2 + 0.33f)));
	float B = 0.45f * sigma2 / (sigma2 + 0.09f);

	float cosTheta_i = wi.z;
	float sinTheta_i = std::sqrt(std::max(epsilon, 1.0f - cosTheta_i * cosTheta_i));
	float cosPhi_i = wi.x / std::max(epsilon, sinTheta_i);
	float sinPhi_i = wi.y / std::max(epsilon, sinTheta_i);
	float tanTheta_i = sinTheta_i / std::max(epsilon, cosTheta_i);

	float cosTheta_o = wo.z;
	float sinTheta_o = std::sqrt(std::max(epsilon, 1.0f - cosTheta_o * cosTheta_o));
	float cosPhi_o = wo.x / std::max(epsilon, sinTheta_o);
	float sinPhi_o = wo.y / std::max(epsilon, sinTheta_o);
	float tanTheta_o = sinTheta_o / std::max(epsilon, cosTheta_o);

	float sinAlpha = std::max(sinTheta_i, sinTheta_o);
	float tanBeta = std::min(tanTheta_i, tanTheta_o);

	float cosDeltaPhi = cosPhi_i * cosPhi_o + sinPhi_i * sinPhi_o;

	return mAlbedo * INV_PI * (A + B *std::max(0.0f, cosDeltaPhi) * sinAlpha * tanBeta);
}

Vector3f OrenNayarMaterial::SampleWi(const Vector3f& wo, float& pdf) const
{// same as lambert sampleWi
	float e1 = Random01();
	float e2 = Random01();

	float cosTheta = sqrt(1.0f - e1);
	float sinTheta = sqrt(e1);
	float phi = 2.0f * PI * e2;
	Vector3f wi;
	wi.x = sinTheta * cos(phi);
	wi.y = sinTheta * sin(phi);
	wi.z = cosTheta;
	pdf = cosTheta * INV_PI;
	return wi;
}
