#include "Keyboard.h"
#include <cstdlib>
#include <cassert>

// キーを押しているか
bool Keyboard::GetKey(uint8_t keyCode) const
{
	assert(keyCode >= 0 && keyCode < _countof(mCurrKeys));
	return mCurrKeys[keyCode] & 0x80;
}

// キーを離した瞬間か
bool Keyboard::GetKeyUp(uint8_t keyCode) const
{
	assert(keyCode >= 0 && keyCode < _countof(mCurrKeys));
	return !(mCurrKeys[keyCode] & 0x80) && mPrevKeys[keyCode] & 0x80;
}

// キーを押した瞬間か
bool Keyboard::GetKeyDown(uint8_t keyCode) const
{
	assert(keyCode >= 0 && keyCode < _countof(mCurrKeys));
	return mCurrKeys[keyCode] & 0x80 && !(mPrevKeys[keyCode] & 0x80);
}
