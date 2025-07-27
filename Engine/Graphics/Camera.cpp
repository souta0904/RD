#include "Camera.h"
#include "RdEngine.h"
#include "Window.h"

Camera::Camera()
	: mRotation(Quaternion::kIdentity)
	, mPosition(Vector3::kZero)
	, mFov(RdMath::kPiOver2)
	, mNearZ(0.1f)
	, mFarZ(10000.0f)
	, mView(Matrix4::kIdentity)
	, mProj(Matrix4::kIdentity)
	, mViewProj(Matrix4::kIdentity)
	, mBillboard(Matrix4::kIdentity)
{

}

void Camera::UpdateViewProj()
{
	Matrix4 rotate = Matrix4::CreateRotate(mRotation);
	Matrix4 translate = Matrix4::CreateTranslate(mPosition);
	// View
	mView = rotate * translate;
	mView = Inverse(mView);
	// Projection
	auto window = gEngine->GetWindow();
	mProj = Matrix4::CreatePerspectiveFov(mFov, float(window->GetWidth()) / float(window->GetHeight()), mNearZ, mFarZ);
	// View Projection
	mViewProj = mView * mProj;
	// Billboard
	mBillboard = rotate;
}
