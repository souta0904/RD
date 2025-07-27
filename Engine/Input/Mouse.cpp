#include "Mouse.h"
#include <cstdlib>
#include <cassert>

// ボタンを押しているか
bool Mouse::GetButton(uint8_t button) const
{
	assert(button >= 0 && button < _countof(mCurrButtons));
	return mCurrButtons[button] & 0x80;
}

// ボタンを離した瞬間か
bool Mouse::GetButtonUp(uint8_t button) const
{
	assert(button >= 0 && button < _countof(mCurrButtons));
	return !(mCurrButtons[button] & 0x80) && mPrevButtons[button] & 0x80;
}

// ボタンを押した瞬間か
bool Mouse::GetButtonDown(uint8_t button) const
{
	assert(button >= 0 && button < _countof(mCurrButtons));
	return mCurrButtons[button] & 0x80 && !(mPrevButtons[button] & 0x80);
}
