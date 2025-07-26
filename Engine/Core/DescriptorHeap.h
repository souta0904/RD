#pragma once
#include "DescriptorHandle.h"
#include "Helper/Pool.h"
#include <d3d12.h>
#include <wrl.h>

// デスクリプタヒープ
class DescriptorHeap
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	bool Create(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors, bool isShaderVisible);
	void Bind(ComPtr<ID3D12GraphicsCommandList> cmdList);

	// デスクリプタハンドル割り当て
	DescriptorHandle* Alloc();
	// デスクリプタハンドル解放
	void Free(DescriptorHandle*& handle);

	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const { return mDescriptorHeap; }
	Pool<DescriptorHandle>* GetHandlePool() const { return mHandlePool.get(); }

private:
	D3D12_DESCRIPTOR_HEAP_DESC mDesc;
	ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;
	std::unique_ptr<Pool<DescriptorHandle>> mHandlePool;// ハンドル管理
	uint32_t mIncrementSize;
};
