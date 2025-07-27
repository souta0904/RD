#pragma once
#include "Vector2.h"
#include <cstdint>
#include <Windows.h>
#include <Xinput.h>

// ゲームパッド
class Gamepad
{
	friend class InputSystem;

public:
	bool GetButton(int button) const;
	bool GetButtonUp(int button) const;
	bool GetButtonDown(int button) const;

	bool GetIsConnected() const { return mIsConnected; }
	const Vector2& GetLeftStick() const { return mLeftStick; }
	const Vector2& GetRightStick() const { return mRightStick; }
	float GetLeftTrigger() const { return mLeftTrigger; }
	float GetRightTrigger() const { return mRightTrigger; }

	void SetLeftStickDeadZone(float deadZone) { mLeftStickDeadZone = deadZone; }
	void SetRightStickDeadZone(float deadZone) { mRightStickDeadZone = deadZone; }
	void SetTriggerDeadZone(float deadZone) { mTriggerDeadZone = deadZone; }

private:
	bool mIsConnected;
	WORD mCurrButton;
	WORD mPrevButton;
	Vector2 mLeftStick;
	Vector2 mRightStick;
	float mLeftTrigger;
	float mRightTrigger;

	// デッドゾーン
	float mLeftStickDeadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	float mRightStickDeadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
	float mTriggerDeadZone = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
};
