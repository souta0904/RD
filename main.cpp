#include "RdEngine.h"// Engine.hに変更
#include "Helper/Helper.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Helper::Log("Hello,RD!\n");

	// TODO: ここからウィンドウサイズとウィンドウタイトルを設定
	gEngine = std::make_unique<Engine>();
	gEngine->Run(1920, 1080, "RD Galaxy");

	return 0;
}
