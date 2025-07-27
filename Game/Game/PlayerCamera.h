#pragma once
#include "Actor/Actor.h"

// プレイヤーカメラ
class PlayerCamera : public Actor
{
public:
	PlayerCamera(Scene* scene);
	void ActorInput(const InputState& input) override;
	void ActorUpdate(float deltaTime) override;
	void ActorUpdateForDev() override;
	Type GetType() const { return Type::PlayerCamera; }

private:
	const float kRotMax = RdMath::ToRadians(80.0f);
	const float kRotMin = RdMath::ToRadians(-45.0f);
	Actor* mTarget;
	float mDistance;
	float mRot;
	float mRotVel;
	float mRotSpeed;

	float mLeeway;
};
