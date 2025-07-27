#include "VertexBuffer.h"
#include "GraphicsCommon.h"
#include "GraphicsEngine.h"
#include <cassert>

bool VertexBuffer::Create(uint32_t size, uint32_t stride, void* initData)
{
	// 頂点バッファを作成
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = size;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	[[maybe_unused]] HRESULT hr = gDirectXCore->GetDevice()->CreateCommittedResource(
		&DirectXCommonSettings::gHeapPropUpload,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mBuff.GetAddressOf())
	);
	if (FAILED(hr))
	{
		return false;
	}

	// 頂点バッファビュー
	mView.BufferLocation = mBuff->GetGPUVirtualAddress();
	mView.SizeInBytes = size;
	mView.StrideInBytes = stride;

	mBuff->Map(0, nullptr, &mData);
	if (initData)
	{
		Copy(initData);
	}

	return true;
}

void VertexBuffer::Bind(ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	assert(cmdList);
	assert(mBuff);
	cmdList->IASetVertexBuffers(0, 1, &mView);
}

void VertexBuffer::Copy(void* data)
{
	memcpy(mData, data, mView.SizeInBytes);
}
