#pragma once
#include "Audio/AudioSystem.h"
#include "Collision/CollisionManager.h"
#include "Graphics/Renderer.h"
#include "Input/InputSystem.h"
#include "Scene/SceneManager.h"
#include "Window.h"
#include <memory>

// エンジン
// TODO: Engine.hにリネーム
class Engine
{
public:
	// 実行
	void Run(uint32_t width = 1920, uint32_t height = 1080, const std::string& appName = "AppName");

	// shared_ptrからunique_ptrへ
	std::shared_ptr<Window> GetWindow() const { return mWindow; }
	std::shared_ptr<Renderer> GetRenderer() const { return mRenderer; }
	std::shared_ptr<InputSystem> GetInput() const { return mInput; }
	std::shared_ptr<AudioSystem> GetAudio() const { return mAudio; }
	std::shared_ptr<CollisionManager> GetCollisionMgr() const { return mCollisionMgr; }
	std::shared_ptr<SceneManager> GetSceneMgr() const { return mSceneMgr; }

private:
	void Initialize(uint32_t width, uint32_t height, const std::string& appName);
	void Terminate();

	// ゲームループ用ヘルパー関数
	void Input();
	void Update();
	void Render();

public:
	static const std::string kName;// エンジン名
	static const std::string kVersion;// バージョン
private:
	bool mIsRunning = true;
	std::string mAppName;
	// shared_ptrからunique_ptrへ
	std::shared_ptr<Window> mWindow;
	std::shared_ptr<Renderer> mRenderer;
	std::shared_ptr<InputSystem> mInput;
	std::shared_ptr<AudioSystem> mAudio;
	std::shared_ptr<CollisionManager> mCollisionMgr;
	std::shared_ptr<SceneManager> mSceneMgr;

	// TODO: chronoでデルタタイムを計算
};

extern std::unique_ptr<Engine> gEngine;
