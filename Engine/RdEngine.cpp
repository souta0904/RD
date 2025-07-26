#include "RdEngine.h"
#include "Core/GraphicsEngine.h"
#include "Editor.h"
#include "Loader/JsonLoader.h"
#include "Random.h"

std::unique_ptr<Engine> gEngine = nullptr;

namespace
{
	// エンジン初期化ファイルのパス
	const std::string kEngineInitFilePath = "assets/EngineInit.json";
}

const std::string Engine::kName = "RD ENGINE";
const std::string Engine::kVersion = "0.1.0";

void Engine::Run()
{
	Initialize();

	// メインループ
	while (!mWindow->ProcessMessage())
	{
		if (!mIsRunning)
		{
			break;
		}

		Input();
		Update();
		Render();
	}

	Terminate();
}

// 初期化
void Engine::Initialize()
{
	Console::Log(std::format("{} v.{}\n", kName, kVersion));

	[[maybe_unused]] HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	// TODO: 成功チェック
	mWindow = std::make_shared<Window>();
	mWindow->Initialize();

	gDirectXCore = std::make_shared<DirectXCore>();
	gDirectXCore->Initialize(mWindow.get());

	mRenderer = std::make_shared<Renderer>();
	mRenderer->Initialize();

	mInput = std::make_shared<InputSystem>();
	mInput->Initialize(mWindow.get());

	mAudio = std::make_shared<AudioSystem>();
	mAudio->Initialize();

	mCollisionMgr = std::make_shared<CollisionManager>();
	mCollisionMgr->Initialize();

	mSceneMgr = std::make_shared<SceneManager>();
	mSceneMgr->Initialize();

	// TODO: 名前空間からグローバル変数へ
	Editor::Initialize(mWindow.get());

	Random::Initialize();

	// エンジン初期化ファイルの読み込み
	if (JsonLoader::LoadEngineInit(this, kEngineInitFilePath))
	{
		Console::Log(std::format("Succeeded: Loading \"{}\"\n", kEngineInitFilePath));
	}
	else
	{
		Console::Log(std::format("Failed: Loading \"{}\"\n", kEngineInitFilePath));
	}
}

// 終了処理
void Engine::Terminate()
{
	// エンジン初期化ファイルの保存
	if (JsonLoader::SaveEngineInit(this, kEngineInitFilePath))
	{
		Console::Log(std::format("Succeeded: Saving \"{}\"\n", kEngineInitFilePath));
	}
	else
	{
		Console::Log(std::format("Failed: Saving \"{}\"\n", kEngineInitFilePath));
	}

	Random::Terminate();

	Editor::Terminate();

	if (mSceneMgr)
	{
		mSceneMgr->Terminate();
	}
	if (mCollisionMgr)
	{
		mCollisionMgr->Terminate();
	}
	if (mAudio)
	{
		mAudio->Terminate();
	}
	if (mInput)
	{
		mInput->Terminate();
	}
	if (mRenderer)
	{
		mRenderer->Terminate();
	}
	if (gDirectXCore)
	{
		gDirectXCore->Terminate();
	}
	if (mWindow)
	{
		mWindow->Terminate();
	}
	CoUninitialize();
}

// 入力処理
void Engine::Input()
{
	// エディタここから
	Editor::Begin();

	// 次のシーンがあれば遷移
	mSceneMgr->TransitionScene();

	mInput->Update();

	const InputSystem::State& inputState = mInput->GetState();

	Editor::Input(inputState);
	if (Editor::IsGamePlaying())
	{
		mSceneMgr->Input(inputState);
	}
}

// 更新処理
void Engine::Update()
{
	const float kDeltaTime = 1.0f / 60.0f;

	Editor::ShowEditor(this);

	Editor::Update(kDeltaTime);
	if (Editor::IsGamePlaying())
	{
		mSceneMgr->UpdateForGame(kDeltaTime);
	}
	else
	{
		// ワールド行列のみ更新
		mSceneMgr->UpdateForEditor();
	}
}

// 描画処理
void Engine::Render()
{
	gDirectXCore->BindHeapSRV();

	// シーンを描画
	auto cmdList = gDirectXCore->GetCmdList();
	mRenderer->Render(cmdList);

	// エディタここまで
	Editor::End();

	// レンダリング前処理
	gDirectXCore->Begin();

	// 最終的なレンダーターゲットに描画
	mRenderer->RenderFinalRT(cmdList);

	// レンダリング後処理
	gDirectXCore->End();
}
