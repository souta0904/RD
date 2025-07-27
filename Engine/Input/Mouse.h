#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include <dinput.h>

// マウス
class Mouse
{
	friend class InputSystem;
public:
	bool GetButton(uint8_t button) const;
	bool GetButtonUp(uint8_t button) const;
	bool GetButtonDown(uint8_t button) const;

	const Vector2& GetMovement() const { return mMovement; }
	float GetWheel() const { return mWheel; }
	const Vector2& GetPosition() const { return mPosition; }

private:
	BYTE mCurrButtons[8];
	BYTE mPrevButtons[8];
	Vector2 mMovement;
	float mWheel;
	Vector2 mPosition;
};
