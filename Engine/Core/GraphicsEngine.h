#pragma once
#include "DescriptorHeap.h"
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <memory>
#include <string>
#include <wrl.h>

#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3d12.lib" )
#pragma comment( lib, "dxcompiler.lib" )

class Window;

class DirectXCore
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	bool Initialize(Window* window);
	void Terminate();

	// レンダリング前後処理
	void Begin();
	void End();
	// コマンドを実行
	void ExecuteCmdList();
	// GPUの処理完了を待つ
	void WaitGPU();
	// コマンドをリセット
	void ResetCmdList();

	// シェーダコンパイル
	// TODO: Shaderへ移行
	ComPtr<IDxcBlob> CompileHLSL(const std::string& path, const std::string& profile);

	ComPtr<ID3D12Device> GetDevice() const { return mDevice; }
	ComPtr<ID3D12GraphicsCommandList> GetCmdList() const { return mCmdList; }
	DescriptorHeap* GetHeapRTV() const { return mHeapRTV.get(); }
	DescriptorHeap* GetHeapDSV() const { return mHeapDSV.get(); }
	DescriptorHeap* GetHeapSRV() const { return mHeapSRV.get(); }

private:
	bool CreateDevice();
	bool CreateCmdQueue();
	bool CreateSwapChain(Window* window);
	bool CreateCmdList();
	bool CreateDescHeaps();
	bool CreateRTV();
	bool CreateDSV(Window* window);
	bool CreateFence();

	// DirectX Shader Compiler
	bool InitDXC();

public:
	static const uint32_t kFrameCount = 2;
private:
	ComPtr<IDXGIFactory7> mFactory;
	ComPtr<IDXGIAdapter4> mUseAdapter;
	ComPtr<ID3D12Device> mDevice;
	ComPtr<ID3D12CommandQueue> mCmdQueue;
	ComPtr<IDXGISwapChain4> mSwapChain;
	ComPtr<ID3D12CommandAllocator> mCmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> mCmdList;
	std::unique_ptr<DescriptorHeap> mHeapRTV;
	std::unique_ptr<DescriptorHeap> mHeapDSV;
	std::unique_ptr<DescriptorHeap> mHeapSRV;
	ComPtr<ID3D12Resource> mBackBuffs[2];
	DescriptorHandle* mHandleRTVs[2];
	ComPtr<ID3D12Resource> mDepthBuff;
	DescriptorHandle* mHandleDSV;
	UINT64 mFenceVal;
	ComPtr<ID3D12Fence> mFence;
	HANDLE mFenceEvent;
	UINT mBackBuffIdx;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissor;

	// DirectX Shader Compiler
	ComPtr<IDxcUtils> mDxcUtils;
	ComPtr<IDxcCompiler3> mDxcCompiler;
	ComPtr<IDxcIncludeHandler> mIncludeHandler;
};

extern std::shared_ptr<DirectXCore> gDirectXCore;
