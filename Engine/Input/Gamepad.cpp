#include "Gamepad.h"

// ボタンを押しているか
bool Gamepad::GetButton(int button) const
{
	return mCurrButton & button;
}

// ボタンを離した瞬間か
bool Gamepad::GetButtonUp(int button) const
{
	return !(mCurrButton & button) && mPrevButton & button;
}

// ボタンを押した瞬間か
bool Gamepad::GetButtonDown(int button) const
{
	return mCurrButton & button && !(mPrevButton & button);
}
