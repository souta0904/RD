#include "RenderTarget.h"
#include "core/GraphicsCommon.h"
#include "core/GraphicsEngine.h"
#include <cassert>

RenderTexture::~RenderTexture()
{
	// デスクリプタハンドル解放
	gDirectXCore->GetHeapRTV()->Free(mHandleRTV);
	if (mUseDepth)
	{
		gDirectXCore->GetHeapDSV()->Free(mHandleDSV);
	}
}

bool RenderTexture::Create(uint32_t width, uint32_t height, bool useDepth)
{
	mWidth = width;
	mHeight = height;
	mUseDepth = useDepth;

	// テクスチャバッファを作成
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = mWidth;
	desc.Height = mHeight;
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleDesc.Count = 1;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	D3D12_CLEAR_VALUE clearVal = {};
	clearVal.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	memcpy(clearVal.Color, &mClearColor.r, sizeof(float) * 4);
	auto device = gDirectXCore->GetDevice();
	[[maybe_unused]] HRESULT hr = device->CreateCommittedResource(
		&DirectXCommonSettings::gHeapPropDefault,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearVal,
		IID_PPV_ARGS(&mTexBuff)
	);
	if (FAILED(hr))
	{
		return false;
	}

	// シェーダリソースビュー
	mHandleSRV = gDirectXCore->GetHeapSRV()->Alloc();
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	gDirectXCore->GetDevice()->CreateShaderResourceView(mTexBuff.Get(), &srvDesc, mHandleSRV->mCPU);

	// レンダーターゲットビュー
	mHandleRTV = gDirectXCore->GetHeapRTV()->Alloc();
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	device->CreateRenderTargetView(mTexBuff.Get(), &rtvDesc, mHandleRTV->mCPU);

	if (mUseDepth)
	{
		// 深度バッファを作成
		desc.Format = DXGI_FORMAT_D32_FLOAT;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		clearVal.Format = DXGI_FORMAT_D32_FLOAT;
		clearVal.DepthStencil.Depth = 1.0f;
		hr = device->CreateCommittedResource(
			&DirectXCommonSettings::gHeapPropDefault,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearVal,
			IID_PPV_ARGS(mDepthBuff.GetAddressOf())
		);
		if (FAILED(hr))
		{
			return false;
		}

		// 深度バッファビュー
		mHandleDSV = gDirectXCore->GetHeapDSV()->Alloc();
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		device->CreateDepthStencilView(mDepthBuff.Get(), &dsvDesc, mHandleDSV->mCPU);
	}

	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = static_cast<FLOAT>(width);
	mViewport.Height = static_cast<FLOAT>(height);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	mScissor.left = 0;
	mScissor.top = 0;
	mScissor.right = width;
	mScissor.bottom = height;

	return true;
}

// 書き込み開始処理
void RenderTexture::BeginRender(ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	assert(cmdList);

	// シェーダリソースからレンダーターゲットへ
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mTexBuff.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	cmdList->ResourceBarrier(1, &barrier);

	// レンダーターゲットを設定
	auto hRTV = mHandleRTV->mCPU;
	auto hDSV = mHandleDSV->mCPU;
	if (mUseDepth)
	{
		cmdList->OMSetRenderTargets(1, &hRTV, false, &hDSV);
	}
	else
	{
		cmdList->OMSetRenderTargets(1, &hRTV, false, nullptr);
	}

	// クリア
	cmdList->ClearRenderTargetView(hRTV, &mClearColor.r, 0, nullptr);
	if (mUseDepth)
	{
		cmdList->ClearDepthStencilView(hDSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	}

	cmdList->RSSetViewports(1, &mViewport);
	cmdList->RSSetScissorRects(1, &mScissor);
}

// 書き込み終了処理
void RenderTexture::EndRender(ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	assert(cmdList);

	// レンダーターゲットからシェーダリソースへ
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mTexBuff.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	cmdList->ResourceBarrier(1, &barrier);
}
