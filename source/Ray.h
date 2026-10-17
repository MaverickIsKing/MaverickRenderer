#pragma once
#include "Common.h"


struct Ray {
	Vector3f o;
	Vector3f d;

	float mint = 0.0f;
	float maxt = FLT_MAX;
};

inline Ray operator*(const Matrix4x4& m, const Ray& r) {
	Ray result;
	result.o = Vector3f(m * Vector4f(r.o, 1.0f));
	result.d = Vector3f(m * Vector4f(r.d, 0.0f));
	result.mint = r.mint;
	result.maxt = r.maxt;
	return result;
}

struct Intersection {
	Vector3f position; // intersection point (coordinate in world)
	Vector3f normal; // normal vector at intersection
	Vector3f tangent; // tangent vector at intersection (wrt du direction in texture coordinate u, v)
	Vector2f uv; // uv coord of the intersection
	float t; // t in Ray, denote distance from ray origin to the intersection point
};