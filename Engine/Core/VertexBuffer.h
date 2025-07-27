#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

// 頂点バッファ
class VertexBuffer
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	bool Create(uint32_t size, uint32_t stride, void* initData = nullptr);
	void Bind(ComPtr<ID3D12GraphicsCommandList> cmdList);

	// バッファを更新
	void Copy(void* data);

	const D3D12_VERTEX_BUFFER_VIEW& GetView() const { return mView; }
	void* GetData() const { return mData; }

private:
	ComPtr<ID3D12Resource> mBuff;
	D3D12_VERTEX_BUFFER_VIEW mView;
	void* mData;
};
