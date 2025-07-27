#pragma once
#include <cstdint>

// キーボード
class Keyboard
{
	friend class InputSystem;
public:
	bool GetKey(uint8_t keyCode) const;
	bool GetKeyUp(uint8_t keyCode) const;
	bool GetKeyDown(uint8_t keyCode) const;
	// TODO: GetAnyKey関数を作成
private:
	uint8_t mCurrKeys[256];
	uint8_t mPrevKeys[256];
};
