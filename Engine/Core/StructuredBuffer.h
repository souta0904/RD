#pragma once
#include "DescriptorHeap.h"
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

// 構造化バッファ
class StructuredBuffer
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	~StructuredBuffer();
	bool Create(uint32_t size, uint32_t numElement, void* initData = nullptr);
	void Bind(ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t rootParamIdx);

	// バッファを更新
	void Copy(void* data);
	template <typename T>
	void Copy(T& data)
	{
		Copy(&data);
	}

	void* GetData() const { return mData; }

private:
	uint32_t mSize;// 要素サイズ
	uint32_t mNumElement;// 要素数
	ComPtr<ID3D12Resource> mBuff;
	void* mData;
	DescriptorHandle* mHandleSRV;// デスクリプタハンドル
};
