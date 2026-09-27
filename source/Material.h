#pragma once
#include "Ray.h"

class Material
{
public:
	virtual ~Material() = default;
	virtual Color BRDF(const Vector3f& wo, const Vector3f& wi) const = 0;
	virtual Color BTDF(const Vector3f& wo, const Vector3f& wi) const { return Color(0, 0, 0); }
	virtual bool IsSpecular() const { return false; }
	virtual bool SampleWt(const Vector3f& wo, Vector3f& wt) const { return false; }
	virtual Vector3f SampleWi(const Vector3f& wo, float& pdf) const { return Vector3f(0, 0, 0); }
};

class LambertMaterial : public Material
{
public:
	LambertMaterial(const Color& albedo) : mAlbedo(albedo) {}

	virtual Color BRDF(const Vector3f& wo, const Vector3f& wi) const override;
	virtual Vector3f SampleWi(const Vector3f& wo, float& pdf) const override;
private:
	Color mAlbedo;
};

class ConductorSpecularMaterial : public Material
{
public:
	ConductorSpecularMaterial(const Color& eta, const Color& absorptionCoef, const Color& reflectionColor) : mEta(eta), mAbsorptionCoef(absorptionCoef), mReflectionColor(reflectionColor) {}

	virtual bool IsSpecular() const { return true; }
	virtual Color BRDF(const Vector3f& wo, const Vector3f& wi) const override;
	virtual Vector3f SampleWi(const Vector3f& wo, float& pdf) const override;
private:
	Color mEta; // refraction coeff
	Color mAbsorptionCoef;
	Color mReflectionColor; // for object color manipulation
};

class DielectricSpecularMaterial : public Material
{
public:
	DielectricSpecularMaterial(float eta, const Color& transmissionColor) : mEta(eta), mTransmissionColor(transmissionColor) {}

	virtual bool IsSpecular() const { return true; }
	virtual Color BRDF(const Vector3f& wo, const Vector3f& wi) const override;
	virtual Color BTDF(const Vector3f& wo, const Vector3f& wi) const override;
	virtual bool SampleWt(const Vector3f& wo, Vector3f& wt) const override;
	virtual Vector3f SampleWi(const Vector3f& wo, float& pdf) const override;
private:
	static float Fresnel(float eta_i, float eta_t, float cos_i, float cos_t);
	float mEta;
	Color mTransmissionColor; // for object color manipulation
};

// Oren-Nayar (lambert with roughness)
class OrenNayarMaterial : public Material
{
public:
	OrenNayarMaterial(const Color& albedo, float sigma) : mAlbedo(albedo), mSigma(sigma){}
	virtual Color BRDF(const Vector3f& wo, const Vector3f& wi) const override;
	virtual Vector3f SampleWi(const Vector3f& wo, float& pdf) const override;

private:
	Color mAlbedo;
	float mSigma;
};

// Torrance-Sparrow (conduct specular with roughness)
class TorranceSparrowMaterial : public Material
{
public:
	TorranceSparrowMaterial(float e, const Color& eta, const Color& absorptionCoef, const Color& reflectionColor) : mExponent(e), mEta(eta), mAbsorptionCoef(absorptionCoef), mReflectionColor(reflectionColor) {}

	virtual Color BRDF(const Vector3f& wo, const Vector3f& wi) const override;
	virtual Vector3f SampleWi(const Vector3f& wo, float& pdf) const override;
private:
	float mExponent; // "e" describes smoothness
	Color mEta; // refraction coeff
	Color mAbsorptionCoef;
	Color mReflectionColor; // for object color manipulation
};