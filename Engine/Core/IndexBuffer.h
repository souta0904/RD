#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

// インデックスバッファ
class IndexBuffer
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	bool Create(uint32_t size, void* initData = nullptr);
	void Bind(ComPtr<ID3D12GraphicsCommandList> cmdList);

	// バッファを更新
	void Copy(void* data);

private:
	ComPtr<ID3D12Resource> mBuff;
	D3D12_INDEX_BUFFER_VIEW mView;
	void* mData;
};
