#include "Window.h"
#include "Helper/Helper.h"
#include "RdEngine.h"
#include <ImGui/imgui_impl_win32.h>// TODO: ImGuiをimguiへ

bool Window::Initialize(uint32_t width, uint32_t height, const std::string& title)
{
	mWidth = width;
	mHeight = height;

	// インスタンスハンドルの取得
	mHInst = GetModuleHandle(nullptr);
	if (!mHInst)
	{
		return false;
	}

	// ウィンドウクラスの設定
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = mHInst;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = L"RdWindowClass";
	// ウィンドウクラスを登録
	if (!RegisterClassEx(&wc))
	{
		return false;
	}

	// クライアントサイズからウィンドウサイズを計算
	RECT rc = { 0,0,static_cast<LONG>(mWidth),static_cast<LONG>(mHeight) };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

	std::wstring wtitle = Helper::ConvertString(title);
	// ウィンドウを作成
	mHWnd = CreateWindow(
		wc.lpszClassName,
		wtitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		mHInst,
		nullptr
	);
	if (!mHWnd)
	{
		return false;
	}

	ShowWindow(mHWnd, SW_SHOW);

	return true;
}

void Window::Terminate()
{
	CloseWindow(mHWnd);
}

bool Window::ProcessMessage()
{
	MSG msg = {};
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msg.message == WM_QUIT)
	{
		// ゲームループ終了
		return true;
	}
	return false;
}

// ImGuiのウィンドウプロシージャの前方宣言
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ウィンドウプロシージャ
LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wp, lp))
	{
		return true;
	}

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}
