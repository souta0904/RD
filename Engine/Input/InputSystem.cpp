#include "Input/InputSystem.h"
#include "Helper/Helper.h"
#include "MyMath.h"
#include "Window.h"

bool InputSystem::Initialize(Window* window)
{
	mHWnd = window->GetHWnd();

	// DirectInputインターフェースを作成
	HRESULT hr = DirectInput8Create(
		window->GetHInst(),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(mDirectInput.GetAddressOf()),
		nullptr
	);
	if (FAILED(hr))
	{
		return false;
	}

	// キーボードの初期化
	hr = mDirectInput->CreateDevice(GUID_SysKeyboard, mKeyboardDevice.GetAddressOf(), nullptr);
	if (FAILED(hr))
	{
		return false;
	}
	hr = mKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
	{
		return false;
	}
	hr = mKeyboardDevice->SetCooperativeLevel(mHWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(hr))
	{
		return false;
	}

	// マウスの初期化
	hr = mDirectInput->CreateDevice(GUID_SysMouse, mMouseDevice.GetAddressOf(), nullptr);
	if (FAILED(hr))
	{
		return false;
	}
	hr = mMouseDevice->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(hr))
	{
		return false;
	}
	hr = mMouseDevice->SetCooperativeLevel(mHWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
	{
		return false;
	}

	// 入力受付開始
	mKeyboardDevice->Acquire();
	mMouseDevice->Acquire();

	return true;
}

void InputSystem::Terminate()
{
	mMouseDevice->Unacquire();
	mKeyboardDevice->Unacquire();
}

void InputSystem::Update()
{
	// キーボードを更新
	Keyboard& keyboard = mState.mKeyboard;
	memcpy(keyboard.mPrevKeys, keyboard.mCurrKeys, sizeof(keyboard.mCurrKeys));
	HRESULT hr = mKeyboardDevice->GetDeviceState(sizeof(keyboard.mCurrKeys), keyboard.mCurrKeys);
	if (FAILED(hr))
	{
		mKeyboardDevice->Acquire();
		mKeyboardDevice->GetDeviceState(sizeof(keyboard.mCurrKeys), keyboard.mCurrKeys);
	}

	// マウスを更新
	Mouse& mouse = mState.mMouse;
	DIMOUSESTATE2 mouseState = {};
	hr = mMouseDevice->GetDeviceState(sizeof(mouseState), &mouseState);
	if (FAILED(hr))
	{
		mMouseDevice->Acquire();
		mMouseDevice->GetDeviceState(sizeof(mouseState), &mouseState);
	}
	memcpy(mouse.mPrevButtons, mouse.mCurrButtons, sizeof(mouse.mCurrButtons));
	memcpy(mouse.mCurrButtons, mouseState.rgbButtons, sizeof(mouseState.rgbButtons));
	mouse.mMovement = Vector2(static_cast<float>(mouseState.lX), static_cast<float>(mouseState.lY));
	mouse.mWheel = static_cast<float>(mouseState.lZ);
	// マウスカーソル座標
	POINT mousePosition = {};
	GetCursorPos(&mousePosition);
	ScreenToClient(mHWnd, &mousePosition);
	mouse.mPosition = Vector2(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));

	// ゲームパッドを更新
	Gamepad& pad = mState.mGamepad;
	pad.mPrevButton = pad.mCurrButton;
	XINPUT_STATE padState = {};
	DWORD result = XInputGetState(0, &padState);
	if (result == ERROR_SUCCESS)
	{
		pad.mIsConnected = true;
		pad.mCurrButton = padState.Gamepad.wButtons;
		pad.mLeftStick = NormalizeStick(padState.Gamepad.sThumbLX, padState.Gamepad.sThumbLY, pad.mLeftStickDeadZone);
		pad.mRightStick = NormalizeStick(padState.Gamepad.sThumbRX, padState.Gamepad.sThumbRY, pad.mRightStickDeadZone);
		pad.mLeftTrigger = NormalizeTrigger(padState.Gamepad.bLeftTrigger, pad.mTriggerDeadZone);
		pad.mRightTrigger = NormalizeTrigger(padState.Gamepad.bRightTrigger, pad.mTriggerDeadZone);
	}
	else
	{
		pad.mIsConnected = false;
	}
}

// スティックを-1.0から1.0に正規化
Vector2 InputSystem::NormalizeStick(int16_t inputX, int16_t inputY, float deadZone)
{
	const float kMax = 32767.0f;
	float len = std::sqrtf(static_cast<float>(inputX * inputX + inputY * inputY));
	Vector2 stick;
	if (len > deadZone)
	{
		float normLen = (len - deadZone) / (kMax - deadZone);
		normLen = RdMath::Clamp(normLen, 0.0f, 1.0f);
		stick = Vector2(inputX * normLen / len, inputY * normLen / len);
	}
	return stick;
}

// トリガーを0.0から1.0に正規化
float InputSystem::NormalizeTrigger(int8_t input, float deadZone)
{
	const float kMax = 255.0f;
	float norm = 0.0f;
	if (input > deadZone)
	{
		norm = static_cast<float>(input - deadZone) / (kMax - deadZone);
	}
	return norm;
}
