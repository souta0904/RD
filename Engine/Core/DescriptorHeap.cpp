#include "DescriptorHeap.h"
#include "GraphicsEngine.h"
#include <cassert>

// デスクリプタヒープを作成
bool DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors, bool isShaderVisible)
{
	mDesc.Type = type;
	mDesc.NumDescriptors = numDescriptors;
	mDesc.Flags = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	auto device = gDirectXCore->GetDevice();
	HRESULT hr = device->CreateDescriptorHeap(&mDesc, IID_PPV_ARGS(mDescriptorHeap.GetAddressOf()));
	if (FAILED(hr))
	{
		return false;
	}

	// ハンドル管理者の作成
	// TODO: コンストラクタで要素数を指定
	mHandlePool = std::make_unique<Pool<DescriptorHandle>>(mDesc.NumDescriptors);
	//mHandlePool->Initialize(mDesc.NumDescriptors);

	// デスクリプタのインクリメントサイズを取得
	mIncrementSize = device->GetDescriptorHandleIncrementSize(mDesc.Type);

	return true;
}

void DescriptorHeap::Bind(ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	assert(mDescriptorHeap);
	assert(cmdList);
	ID3D12DescriptorHeap* heaps[] = { mDescriptorHeap.Get() };
	cmdList->SetDescriptorHeaps(1, heaps);
}

DescriptorHandle* DescriptorHeap::Alloc()
{
	assert(mHandlePool);

	auto initFunc = [&](uint32_t idx, DescriptorHandle& handle)
	{
		// CPUハンドルを計算
		handle.mCPU = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		handle.mCPU.ptr += mIncrementSize * idx;
		// GPUハンドルを計算
		if (mDesc.Flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
		{
			handle.mGPU = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			handle.mGPU.ptr += mIncrementSize * idx;
		}
	};
	return mHandlePool->Alloc(initFunc);
}

void DescriptorHeap::Free(DescriptorHandle*& handle)
{
	assert(mHandlePool);

	if (handle)
	{
		mHandlePool->Free(handle);
		handle = nullptr;
	}
}
