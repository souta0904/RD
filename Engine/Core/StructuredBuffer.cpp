#include "StructuredBuffer.h"
#include "GraphicsCommon.h"
#include "GraphicsEngine.h"
#include <cassert>

StructuredBuffer::~StructuredBuffer()
{
	// デスクリプタハンドルを解放
	gDirectXCore->GetHeapSRV()->Free(mHandleSRV);
}

bool StructuredBuffer::Create(uint32_t size, uint32_t numElement, void* initData)
{
	mSize = size;
	mNumElement = numElement;

	// 構造化バッファを作成
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = mSize * mNumElement;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	auto device = gDirectXCore->GetDevice();
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

	mBuff->Map(0, nullptr, &mData);
	if (initData)
	{
		Copy(initData);
	}

	// デスクリプタハンドルを割り当て
	mHandleSRV = gDirectXCore->GetHeapSRV()->Alloc();

	// シェーダリソースビューを作成
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
	viewDesc.Format = DXGI_FORMAT_UNKNOWN;
	viewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	viewDesc.Buffer.FirstElement = 0;
	viewDesc.Buffer.NumElements = mNumElement;
	viewDesc.Buffer.StructureByteStride = mSize;
	viewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	device->CreateShaderResourceView(mBuff.Get(), &viewDesc, mHandleSRV->mCPU);

	return true;
}

void StructuredBuffer::Bind(ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t rootParamIdx)
{
	assert(cmdList);
	assert(mBuff);
	cmdList->SetGraphicsRootDescriptorTable(rootParamIdx, mHandleSRV->mGPU);
}

void StructuredBuffer::Copy(void* data)
{
	memcpy(mData, data, mSize * mNumElement);
}
