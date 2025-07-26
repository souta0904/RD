#include "ConstantBuffer.h"
#include "GraphicsCommon.h"
#include "GraphicsEngine.h"
#include <cassert>

bool ConstantBuffer::Create(uint32_t size, void* initData)
{
	mSize = size;
	mAlignedSize = (mSize + 0xFF) & ~0xFF;

	// 定数バッファを作成
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = mAlignedSize;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	HRESULT hr = gDirectXCore->GetDevice()->CreateCommittedResource(
		&DirectXCommonSettings::gHeapPropUpload,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mBuff)
	);
	if (FAILED(hr))
	{
		return false;
	}

	mBuff->Map(0, nullptr, &mData);
	// 初期化データがあればコピー
	if (initData)
	{
		Copy(initData);
	}

	return true;
}

void ConstantBuffer::Bind(ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t rootParamIdx)
{
	assert(cmdList);
	assert(mBuff);
	cmdList->SetGraphicsRootConstantBufferView(rootParamIdx, mBuff->GetGPUVirtualAddress());
}

void ConstantBuffer::Copy(void* data)
{
	assert(mData);
	memcpy(mData, data, mSize);
}
