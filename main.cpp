#include "RdEngine.h"// Engine.hに変更

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// TODO: ここからウィンドウサイズとウィンドウタイトルを設定
	gEngine = std::make_unique<Engine>();
	gEngine->Run();

	return 0;
}
