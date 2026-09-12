#pragma once
#include <iostream>
#include <random>
#include <MiniFB.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/ext.hpp>

using Vector2f = glm::vec2;
using Vector3f = glm::vec3;
using Vector4f = glm::vec4;
using Vector2i = glm::ivec2;
using Vector3i = glm::ivec3;
using Vector4i = glm::ivec4;
using Color = glm::vec3;
using Matrix2x2 = glm::mat2x2;
using Matrix3x3 = glm::mat3x3;
using Matrix4x4 = glm::mat4x4;
const float PI = glm::pi<float>();
const float INV_PI = 1.0f / PI;

inline void DumpVector(const Vector3f& v) {
    std::cout << "Vector3f( " << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

inline Matrix4x4 MakeTranslation(const Vector3f& t) {
    return Matrix4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        t.x, t.y, t.z, 1.0f
    );
}

inline Matrix4x4 MakeRotation(const Vector3f& euler) {
    float cx = cosf(euler.x);
    float sx = sinf(euler.x);
    float cy = cosf(euler.y);
    float sy = sinf(euler.y);
    float cz = cosf(euler.z);
    float sz = sinf(euler.z);

    Matrix4x4 rx(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cx, sx, 0.0f,
        0.0f, -sx, cx, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    Matrix4x4 ry(
        cy, 0.0f, -sy, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sy, 0.0f, cy, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    Matrix4x4 rz(
        cz, sz, 0.0f, 0.0f,
        -sz, cz, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    return rz * ry * rx;

}

inline Matrix4x4 MakeScale(float s) {
    return Matrix4x4(
        s, 0.0f, 0.0f, 0.0f,
        0.0f, s, 0.0f, 0.0f,
        0.0f, 0.0f, s, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

inline Matrix4x4 MakeWorldTransform(const Vector3f& position, const Vector3f& rotation, float s) {
    Matrix4x4 T = MakeTranslation(position);
    Matrix4x4 R = MakeRotation(rotation);
    Matrix4x4 S = MakeScale(s);
    return T * R * S;
}

inline Matrix3x3 MakeCoordinateSystem(const Vector3f& w) {
    Vector3f u(1.0f, 0.0f, 0.0f);

    if (glm::dot(w, u) > 0.99f) {
        u = Vector3f(0.0f, 1.0f, 0.0f);
    }

    Vector3f v = glm::cross(w, u);
    u = glm::cross(v, w);

    u = glm::normalize(u);
    v = glm::normalize(v);

    return Matrix3x3(u, v, w);
}

inline float Random01()
{
    thread_local std::mt19937 gen(std::random_device{}());
    thread_local std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(gen);
}

inline float Random(float a, float b)
{
    return a + (b - a) * Random01();
}

// spherical to cartesian (on unit sphere)
inline Vector3f GetSphericalCoordinate(float theta, float phi)
{
    return Vector3f(sinf(theta) * cosf(phi), sinf(theta) * sinf(phi), cosf(theta));
}

inline bool ComputeRefractVector(const Vector3f& wi, float eta_i, float eta_t, Vector3f& wt)
{
    float cos_i = wi.z;
    float sin_i = sqrtf(glm::max(1e-5f, 1.0f - cos_i * cos_i));
    float sin_t = sin_i * eta_i / eta_t;

    // total internal reflection (no refraction)
    if (sin_t >= 1.0f) {
        return false;
    }

    float cos_t = sqrtf(glm::max(1e-5f, 1.0f - sin_t * sin_t));
    wt.z = wi.z > 0 ? -cos_t : cos_t;
    wt.x = -wi.x * sin_t / glm::max(1e-5f, sin_i);
    wt.y = -wi.y * sin_t / glm::max(1e-5f, sin_i);
    wt = glm::normalize(wt);
    return true;
}

// uniform disk sampling
inline Vector2f UniformSampleDisk(float R)
{
    float e1 = Random01();
    float e2 = Random01();

    float r = R * std::sqrt(e1);
    float theta = 2.0f * PI * e2;
    // polar to cartesian
    return Vector2f(r * std::cos(theta), r * std::sin(theta));
}

// uniform hemisephere sampling
inline Vector3f UniformSampleHemisphere()
{
    float e1 = Random01();
    float e2 = Random01();

    float cosTheta = e1;
    float sinTheta = std::sqrt(glm::max(1.0f - cosTheta * cosTheta, 1e-5f));
    float phi = 2.0f * PI * e2;

    return Vector3f(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
}

inline Vector3f UniformSampleTriangle(const Vector3f p0, const Vector3f p1, const Vector3f p2)
{
    float e1 = Random01();
    float e2 = Random01();

    float sqrt_e1 = std::sqrt(e1);
    float u = 1.0f - sqrt_e1;
    float v = e2 * sqrt_e1;

    return (1.0f - u - v) * p0 + u * p1 + v * p2;
}