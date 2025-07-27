#include "GraphicsEngine.h"
#include "GraphicsCommon.h"
#include "Helper/Helper.h"
#include "Window.h"
#include <format>

std::shared_ptr<DirectXCore> gDirectXCore = nullptr;

// 初期化
bool DirectXCore::Initialize(Window* window)
{
	if (!CreateDevice())
	{
		Helper::Log("Failed to CreateDevice().\n");
		return false;
	}

	DirectXCommonSettings::Initialize();

	if (!CreateCmdQueue())
	{
		Helper::Log("Failed to CreateCmdQueue().\n");
	}
	if (!CreateSwapChain(window))
	{
		Helper::Log("Failed to CreateSwapChain().\n");
	}
	if (!CreateCmdList())
	{
		Helper::Log("Failed to CreateCmdList().\n");
	}
	if (!CreateDescHeaps())
	{
		Helper::Log("Failed to CreateDescHeaps().\n");
	}
	if (!CreateRTV())
	{
		Helper::Log("Failed to CreateRTV().\n");
	}
	if (!CreateDSV(window))
	{
		Helper::Log("Failed to CreateDSV().\n");
	}
	if (!CreateFence())
	{
		Helper::Log("Failed to CreateFence().\n");
	}

	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = static_cast<FLOAT>(window->GetWidth());
	mViewport.Height = static_cast<FLOAT>(window->GetHeight());
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	mScissor.left = 0;
	mScissor.top = 0;
	mScissor.right = window->GetWidth();
	mScissor.bottom = window->GetHeight();

	if (!InitDXC())
	{
		Helper::Log("Failed to InitDXC().\n");
	}

	return true;
}

// 終了処理
void DirectXCore::Terminate()
{
	if (mFenceEvent)
	{
		CloseHandle(mFenceEvent);
		mFenceEvent = nullptr;
	}
	mHeapDSV->Free(mHandleDSV);
	for (uint32_t i = 0; i < kFrameCount; ++i)
	{
		mHeapRTV->Free(mHandleRTVs[i]);
	}

	DirectXCommonSettings::Terminate();
}

// レンダリング前処理
void DirectXCore::Begin()
{
	// バックバッファのインデックスを取得
	mBackBuffIdx = mSwapChain->GetCurrentBackBufferIndex();

	// 表示からレンダーターゲットへ
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mBackBuffs[mBackBuffIdx].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	mCmdList->ResourceBarrier(1, &barrier);

	// レンダーターゲットを設定
	auto hRTV = mHandleRTVs[mBackBuffIdx]->mCPU;
	auto hDSV = mHandleDSV->mCPU;
	mCmdList->OMSetRenderTargets(1, &hRTV, false, &hDSV);
	// クリア
	float clearColor[4] = { 0.0f,0.0f,0.5f,0.0f };
	mCmdList->ClearRenderTargetView(hRTV, clearColor, 0, nullptr);
	mCmdList->ClearDepthStencilView(hDSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	mCmdList->RSSetViewports(1, &mViewport);
	mCmdList->RSSetScissorRects(1, &mScissor);
}

// レンダリング後処理
void DirectXCore::End()
{
	// レンダーターゲットから表示へ
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mBackBuffs[mBackBuffIdx].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	mCmdList->ResourceBarrier(1, &barrier);

	// コマンドを実行
	ExecuteCmdList();

	// 表示
	mSwapChain->Present(1, 0);

	// GPUの処理を待つ
	WaitGPU();

	// コマンドをリセット
	ResetCmdList();
}

// コマンドを実行
void DirectXCore::ExecuteCmdList()
{
	mCmdList->Close();
	ID3D12CommandList* cmdLists[] = { mCmdList.Get() };
	mCmdQueue->ExecuteCommandLists(1, cmdLists);
}

// GPUの処理完了を待つ
void DirectXCore::WaitGPU()
{
	++mFenceVal;
	mCmdQueue->Signal(mFence.Get(), mFenceVal);
	if (mFence->GetCompletedValue() < mFenceVal)
	{
		mFence->SetEventOnCompletion(mFenceVal, mFenceEvent);
		WaitForSingleObject(mFenceEvent, INFINITE);
	}
}

// コマンドをリセット
void DirectXCore::ResetCmdList()
{
	mCmdAllocator->Reset();
	mCmdList->Reset(mCmdAllocator.Get(), nullptr);
}

// シェーダコンパイル
DirectXCore::ComPtr<IDxcBlob> DirectXCore::CompileHLSL(const std::string& path, const std::string& profile)
{
	std::wstring wpath = Helper::ConvertString(path);
	std::wstring wprofile = Helper::ConvertString(profile);
	// hlslファイルを読み込む
	ComPtr<IDxcBlobEncoding> source = nullptr;
	HRESULT hr = mDxcUtils->LoadFile(wpath.c_str(), nullptr, source.GetAddressOf());
	if (FAILED(hr))
	{
		return nullptr;
	}

	// コンパイル
	DxcBuffer sourceBuff = {};
	sourceBuff.Ptr = source->GetBufferPointer();
	sourceBuff.Size = source->GetBufferSize();
	sourceBuff.Encoding = DXC_CP_UTF8;
	// コンパイルオプション
	LPCWSTR options[] =
	{
		wpath.c_str(),
		L"-E", L"main",
		L"-T", wprofile.c_str(),
		L"-Zi", L"-Qembed_debug",
		L"-Od",
		L"-Zpr"
	};
	ComPtr<IDxcResult> compileResult = nullptr;
	hr = mDxcCompiler->Compile(
		&sourceBuff,
		options,
		_countof(options),
		mIncludeHandler.Get(),
		IID_PPV_ARGS(compileResult.GetAddressOf())
	);
	if (FAILED(hr))
	{
		return nullptr;
	}
	ComPtr<IDxcBlobUtf8> compileError = nullptr;
	compileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(compileError.GetAddressOf()), nullptr);
	if (compileError && compileError->GetStringLength() != 0)
	{
		// 警告やエラーを出力
		Helper::Log(compileError->GetStringPointer());
		return nullptr;
	}

	// シェーダオブジェクトを取得
	ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = compileResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(shaderBlob.GetAddressOf()), nullptr);
	if (FAILED(hr))
	{
		return nullptr;
	}

	return shaderBlob;
}

// デバイスを作成
bool DirectXCore::CreateDevice()
{
#ifdef _DEBUG
	// デバッグレイヤーを有効化
	ComPtr<ID3D12Debug1> debug = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()))))
	{
		debug->EnableDebugLayer();
		debug->SetEnableGPUBasedValidation(true);
	}
#endif

	// DXGIファクトリを作成
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(mFactory.GetAddressOf()));
	if (FAILED(hr))
	{
		return false;
	}

	for (uint32_t i = 0;; ++i)
	{
		// パフォーマンス順でアダプタを取得
		hr = mFactory->EnumAdapterByGpuPreference(
			i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(mUseAdapter.GetAddressOf())
		);
		if (hr == DXGI_ERROR_NOT_FOUND)
		{
			break;
		}

		DXGI_ADAPTER_DESC3 desc = {};
		hr = mUseAdapter->GetDesc3(&desc);
		if (SUCCEEDED(hr))
		{
			// ソフトウェアアダプタはNG
			if (!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
			{
				Helper::Log(std::format(L"Use adapter: {}\n", desc.Description));
				break;
			}
		}
	}
	if (!mUseAdapter)
	{
		return false;
	}

	// 機能レベル
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0
	};
	const char* str[] = { "12.2","12.1","12.0" };
	for (uint32_t i = 0; i < _countof(featureLevels); ++i)
	{
		// デバイスを作成
		hr = D3D12CreateDevice(mUseAdapter.Get(), featureLevels[i], IID_PPV_ARGS(mDevice.GetAddressOf()));
		if (SUCCEEDED(hr))
		{
			Helper::Log(std::format("Feature level: {}\n", str[i]));
			break;
		}
	}
	if (!mDevice)
	{
		return false;
	}

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(mDevice->QueryInterface(IID_PPV_ARGS(infoQueue.GetAddressOf()))))
	{
		// 警告やエラーが出たら停止
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		// どうしようもないエラーは無視する
		// https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_MESSAGE_ID ids[] = { D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE };
		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		filter.DenyList.NumIDs = _countof(ids);
		filter.DenyList.pIDList = ids;
		infoQueue->PushStorageFilter(&filter);
	}
#endif

	return true;
}

// コマンドキューを作成
bool DirectXCore::CreateCmdQueue()
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	HRESULT hr = mDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(mCmdQueue.GetAddressOf()));
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

// スワップチェインを作成
bool DirectXCore::CreateSwapChain(Window* window)
{
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Width = window->GetWidth();
	desc.Height = window->GetHeight();
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = kFrameCount;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	[[maybe_unused]] HRESULT hr = mFactory->CreateSwapChainForHwnd(
		mCmdQueue.Get(),
		window->GetHWnd(),
		&desc,
		nullptr,
		nullptr,
		reinterpret_cast<IDXGISwapChain1**>(mSwapChain.GetAddressOf())
	);
	if (FAILED(hr))
	{
		return false;
	}

	// バックバッファのインデックスを取得
	mBackBuffIdx = mSwapChain->GetCurrentBackBufferIndex();

	return true;
}

// コマンドリストを作成
bool DirectXCore::CreateCmdList()
{
	// コマンドアロケータを作成
	HRESULT hr = mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCmdAllocator.GetAddressOf()));
	if (FAILED(hr))
	{
		return false;
	}

	// コマンドリストを作成
	hr = mDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mCmdAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(mCmdList.GetAddressOf())
	);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

// デスクリプタヒープを作成
bool DirectXCore::CreateDescHeaps()
{
	mHeapRTV = std::make_unique<DescriptorHeap>();
	if (!mHeapRTV->Create(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 128, false))
	{
		return false;
	}

	mHeapDSV = std::make_unique<DescriptorHeap>();
	if (!mHeapDSV->Create(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 64, false))
	{
		return false;
	}

	mHeapSRV = std::make_unique<DescriptorHeap>();
	if (!mHeapSRV->Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 512, true))
	{
		return false;
	}

	return true;
}

// レンダーターゲットビューを作成
bool DirectXCore::CreateRTV()
{
	for (uint32_t i = 0; i < kFrameCount; ++i)
	{
		// スワップチェインからバックバッファを取得
		[[maybe_unused]] HRESULT hr = mSwapChain->GetBuffer(i, IID_PPV_ARGS(mBackBuffs[i].GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}

		// デスクリプタハンドル割り当て
		mHandleRTVs[i] = mHeapRTV->Alloc();

		// レンダーターゲットビューを作成
		D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
		viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		mDevice->CreateRenderTargetView(mBackBuffs[i].Get(), &viewDesc, mHandleRTVs[i]->mCPU);
	}

	return true;
}

// 深度ステンシルビューを作成
bool DirectXCore::CreateDSV(Window* window)
{
	// 深度バッファを作成
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = window->GetWidth();
	desc.Height = window->GetHeight();
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	D3D12_CLEAR_VALUE clearVal = {};
	clearVal.Format = DXGI_FORMAT_D32_FLOAT;
	clearVal.DepthStencil.Depth = 1.0f;
	[[maybe_unused]] HRESULT hr = mDevice->CreateCommittedResource(
		&DirectXCommonSettings::gHeapPropDefault,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearVal,
		IID_PPV_ARGS(mDepthBuff.GetAddressOf())
	);
	if (FAILED(hr))
	{
		return false;
	}

	// デスクリプタハンドル割り当て
	mHandleDSV = mHeapDSV->Alloc();

	// 深度ステンシルビューを作成
	D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
	viewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	mDevice->CreateDepthStencilView(mDepthBuff.Get(), &viewDesc, mHandleDSV->mCPU);

	return true;
}

// フェンスを作成
bool DirectXCore::CreateFence()
{
	mFenceVal = 0;

	// フェンスを作成
	[[maybe_unused]] HRESULT hr = mDevice->CreateFence(mFenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf()));
	if (FAILED(hr))
	{
		return false;
	}

	// フェンスイベントを作成
	mFenceEvent = CreateEvent(nullptr, false, false, nullptr);
	if (!mFenceEvent)
	{
		return false;
	}

	return true;
}

// DirectX Shader Compilerを初期化
bool DirectXCore::InitDXC()
{
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(mDxcUtils.GetAddressOf()));
	if (FAILED(hr))
	{
		return false;
	}

	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(mDxcCompiler.GetAddressOf()));
	if (FAILED(hr))
	{
		return false;
	}

	hr = mDxcUtils->CreateDefaultIncludeHandler(mIncludeHandler.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}
