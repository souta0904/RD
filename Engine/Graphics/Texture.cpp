#include "Texture.h"
#include "core/GraphicsCommon.h"
#include "core/GraphicsEngine.h"
#include "helper/Helper.h"
#include <cassert>
#include <DirectXTex/d3dx12.h>

Texture::~Texture()
{
	gDirectXCore->GetHeapSRV()->Free(mHandleSRV);
}

bool Texture::Create(const std::string& path)
{
	mPath = path;

	// テクスチャファイルを読み込む
	std::wstring wpath = Helper::ConvertString(path);
	DirectX::ScratchImage scratchImage = {};
	HRESULT hr = DirectX::LoadFromWICFile(wpath.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, scratchImage);
	if (FAILED(hr))
	{
		return false;
	}

	// ミップマップを生成
	DirectX::ScratchImage mipChain = {};
	hr = GenerateMipMaps(
		scratchImage.GetImages(),
		scratchImage.GetImageCount(),
		scratchImage.GetMetadata(),
		DirectX::TEX_FILTER_SRGB,
		0,
		mipChain
	);
	if (FAILED(hr))
	{
		return false;
	}

	// テクスチャサイズを取得
	DirectX::TexMetadata metadata = mipChain.GetMetadata();
	mWidth = static_cast<uint32_t>(metadata.width);
	mHeight = static_cast<uint32_t>(metadata.height);

	// テクスチャバッファを作成
	D3D12_RESOURCE_DESC buffDesc = {};
	buffDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	buffDesc.Width = mWidth;
	buffDesc.Height = mHeight;
	buffDesc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
	buffDesc.MipLevels = static_cast<UINT16>(metadata.mipLevels);
	buffDesc.Format = metadata.format;
	buffDesc.SampleDesc.Count = 1;
	auto device = gDirectXCore->GetDevice();
	hr = device->CreateCommittedResource(
		&DirectXCommonSettings::gHeapPropDefault,
		D3D12_HEAP_FLAG_NONE,
		&buffDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,// コピー先
		nullptr,
		IID_PPV_ARGS(mTexBuff.GetAddressOf())
	);
	if (FAILED(hr))
	{
		return false;
	}

	// 中間リソースを作成
	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	DirectX::PrepareUpload(device.Get(), mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), subresource);
	D3D12_RESOURCE_DESC intermediateDesc = {};
	intermediateDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	intermediateDesc.Width = GetRequiredIntermediateSize(mTexBuff.Get(), 0, static_cast<UINT>(subresource.size()));
	intermediateDesc.Height = 1;
	intermediateDesc.DepthOrArraySize = 1;
	intermediateDesc.MipLevels = 1;
	intermediateDesc.SampleDesc.Count = 1;
	intermediateDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ComPtr<ID3D12Resource> intermediateRes = nullptr;
	hr = device->CreateCommittedResource(
		&DirectXCommonSettings::gHeapPropUpload,
		D3D12_HEAP_FLAG_NONE,
		&intermediateDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(intermediateRes.GetAddressOf())
	);
	if (FAILED(hr))
	{
		return false;
	}

	// 中間リソースからテクスチャバッファへコピー
	auto cmdList = gDirectXCore->GetCmdList();
	UpdateSubresources(
		cmdList.Get(),
		mTexBuff.Get(),
		intermediateRes.Get(),
		0,
		0,
		static_cast<UINT>(subresource.size()),
		subresource.data()
	);
	// コピー先からシェーダリソースへ
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mTexBuff.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	cmdList->ResourceBarrier(1, &barrier);
	// コマンドを実行
	gDirectXCore->ExecuteCmdList();
	gDirectXCore->WaitGPU();
	gDirectXCore->ResetCmdList();

	// デスクリプタハンドルを割り当て
	mHandleSRV = gDirectXCore->GetHeapSRV()->Alloc();

	// シェーダリソースビューを作成
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
	viewDesc.Format = DirectX::MakeSRGB(metadata.format);
	viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	viewDesc.Texture2D.MipLevels = uint32_t(metadata.mipLevels);
	gDirectXCore->GetDevice()->CreateShaderResourceView(mTexBuff.Get(), &viewDesc, mHandleSRV->mCPU);

	return true;
}

void Texture::Bind(ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t rootParamIdx)
{
	assert(cmdList);
	assert(mTexBuff);
	cmdList->SetGraphicsRootDescriptorTable(rootParamIdx, mHandleSRV->mGPU);
}
