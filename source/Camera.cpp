#include "Camera.h"

void Camera::Initialize(const Vector3f& p, const Vector3f& target, const Vector3f& up, float fov, float n, float f, int W, int H) {
	mPosition = p;
	
	// view matrix
	//Vector3f l = glm::normalize(target - p);
	//Vector3f r = glm::normalize(glm::cross(up, l));
	//Vector3f u = glm::cross(l, r);

	//Matrix4x4 viewMatrix = glm::transpose(Matrix4x4(r.x, r,y, r.z, 0.0f,
	//												u.x, u.y, u.z, 0.0f, 
	//												l.x, l.y, l.z, 0.0f
	//												0.0f, 0.0f, 0.0f, 1.0f)) * MakeTranslation(-p);

	// LH = left hand
	Matrix4x4 viewMatrix = glm::lookAtLH(p, target, up);

	// projection matrix

	//glm::transpose(Matrix4x4(1 / ((static_cast<float>(W) / H) * tanf(fov / 2.0f), 0.0f, 0.0f, 0.0f
	//							0.0f, 1 / tanf(fov / 2.0f), 0.0f, 0.0f
	//							0.0f, 0,0f, f / f-n, - (f * n) / f - n,
	//							0.0f, 0.0f, 1.0f, 0.0f))

	// ZO = NDC [0, 1] (NO = NDC [-1,1])
	Matrix4x4 projectionMatrix = glm::perspectiveFovLH_ZO(fov, (float)W, (float)H, n, f);

	// viewport matrix
	Matrix4x4 viewportMatrix = Matrix4x4(
		W / 2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, - H / 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		W / 2.0f, H / 2.0f, 0.0f, 1.0f
	);

	Matrix4x4 combinedMatrix = viewportMatrix * projectionMatrix * viewMatrix;
	Matrix4x4 invCombinedMatrix = glm::inverse(combinedMatrix);

	mCombinedMatrix = combinedMatrix;
	mInvCombinedMatrix = invCombinedMatrix;
}

Ray Camera::GetRay(int x, int y) const {
	Ray ray;
	ray.o = mPosition;
	
	Vector4f p(x, y, 0.0f, 1.0f);
	Vector4f worldPos = mInvCombinedMatrix * p;
	worldPos /= worldPos.w;

	ray.d = glm::normalize(Vector3f(worldPos) - mPosition);
	
	return ray;
}
