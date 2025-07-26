#pragma once
#include <cstdint>
#include <string>
#include <Windows.h>

// TODO: フルスクリーン化
// TODO: 解像度変更

// ウィンドウ
class Window
{
public:
	bool Initialize(uint32_t width, uint32_t height, const std::string& title);
	void Terminate();

	// メッセージを処理
	// 戻り値がtrueでゲームループ終了
	bool ProcessMessage();

	uint32_t GetWidth() const { return mWidth; }
	uint32_t GetHeight() const { return mHeight; }
	HINSTANCE GetHInst() const { return mHInst; }
	HWND GetHWnd() const { return mHWnd; }

private:
	// ウィンドウプロシージャ
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

private:
	// ウィンドウサイズ
	uint32_t mWidth;
	uint32_t mHeight;
	// インスタンスハンドル
	HINSTANCE mHInst;
	// ウィンドウハンドル
	HWND mHWnd;
};
