#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include "Gamepad.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <dinput.h>
#include <wrl.h>

#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "xinput.lib" )
#pragma comment( lib, "dxguid.lib" )

class Window;

// 入力状況
struct InputState
{
	Keyboard mKeyboard;
	Mouse mMouse;
	Gamepad mGamepad;
};

// 入力システム
class InputSystem
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	bool Initialize(Window* window);
	void Terminate();

	void Update();

	const InputState& GetState() const { return mState; }

private:
	// スティックを-1.0から1.0に正規化
	Vector2 NormalizeStick(int16_t inputX, int16_t inputY, float deadZone);
	// トリガーを0.0から1.0に正規化
	float NormalizeTrigger(int8_t input, float deadZone);

private:
	HWND mHWnd;
	ComPtr<IDirectInput8> mDirectInput;
	ComPtr<IDirectInputDevice8> mKeyboardDevice;
	ComPtr<IDirectInputDevice8> mMouseDevice;
	InputState mState;
};
