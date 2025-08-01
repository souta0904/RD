#include "DebugCamera.h"
#include "RdEngine.h"
#include "Window.h"
#include "MyMath.h"

DebugCamera::DebugCamera()
	: mCamera(std::make_unique<Camera>())
	, mRotVel(Vector3::kZero)
	, mVelocity(Vector3::kZero)
	, mTarget(Vector3::kZero)
	, mDistance(10.0f)
{

}

void DebugCamera::Input(const InputState& input)
{
	mRotVel = Vector3::kZero;
	mVelocity = Vector3::kZero;
	Vector2 move = input.mMouse.GetMovement();
	// 回転
	if (input.mMouse.GetButton(1))// 右クリック
	{
		auto window = gEngine->GetWindow();
		mRotVel.x = move.y / window->GetWidth() * kRotSpeed;
		mRotVel.y = move.x / window->GetHeight() * kRotSpeed;
	}
	// 平行移動
	if (input.mMouse.GetButton(2))// 中ボタン
	{
		mVelocity = Vector3(-move.x * kSpeed, move.y * kSpeed, 0.0f);
	}
	// ズームイン、ズームアウト
	mRotVel.z = input.mMouse.GetWheel() * kZoomSpeed;
}

void DebugCamera::Update(float /*deltaTime*/)
{
	Quaternion rotation = mCamera->GetRotation();
	mDistance += -mRotVel.z;
	mDistance = RdMath::Clamp(mDistance, kMinDist, kMaxDist);

	// 回転
	Vector3 offset = Vector3(0.0f, 0.0f, -mDistance) * rotation;
	Vector3 yAxis = Vector3(0.0f, 1.0f, 0.0f) * rotation;
	// Y Axis
	Quaternion yRot = Quaternion(Vector3(0.0f, 1.0f, 0.0f), mRotVel.y);
	offset *= yRot;
	yAxis *= yRot;
	Vector3 zAxis = Normalize(-offset);
	Vector3 xAxis = Normalize(Cross(yAxis, zAxis));
	// X Axis
	Quaternion xRot = Quaternion(xAxis, mRotVel.x);
	offset *= xRot;
	yAxis *= xRot;
	rotation = rotation * yRot * xRot;

	// 平行移動
	mVelocity *= rotation;
	mTarget += mVelocity;
	mCamera->SetPosition(mTarget + offset);
	mCamera->SetRotation(rotation);
}
