#include "PipelineState.h"
#include "GraphicsEngine.h"
#include <cassert>

PipelineState::PipelineState()
{
	// デフォルトの設定
	mDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	mDesc.NumRenderTargets = 1;
	mDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	mDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	mDesc.SampleDesc.Count = 1;
}

// コピー
PipelineState::PipelineState(const PipelineState& ps)
{
	*this = ps;
}

// 代入
PipelineState& PipelineState::operator=(const PipelineState& ps)
{
	mDesc = ps.mDesc;
	mInputLayouts = ps.mInputLayouts;
	mDesc.InputLayout.pInputElementDescs = mInputLayouts.data();
	return *this;
}

bool PipelineState::Create(const PSOInit& psoInit)
{
	// ルートシグネチャ
	mDesc.pRootSignature = psoInit.mRootSignature->GetRootSignature().Get();
	// 頂点シェーダ
	auto blob = psoInit.mVertexShader->GetBlob();
	mDesc.VS.pShaderBytecode = blob->GetBufferPointer();
	mDesc.VS.BytecodeLength = blob->GetBufferSize();
	// ピクセルシェーダ
	blob = psoInit.mPixelShader->GetBlob();
	mDesc.PS.pShaderBytecode = blob->GetBufferPointer();
	mDesc.PS.BytecodeLength = blob->GetBufferSize();
	// ブレンド
	mDesc.BlendState = psoInit.mBlendDesc;
	// ラスタライザ
	mDesc.RasterizerState = psoInit.mRasterizerDesc;
	// 深度ステンシル
	mDesc.DepthStencilState = psoInit.mDepthStencilDesc;
	// 頂点レイアウト
	mInputLayouts = psoInit.mInputLayouts;
	mDesc.InputLayout.NumElements = static_cast<UINT>(mInputLayouts.size());
	if (mDesc.InputLayout.NumElements > 0)
	{
		mDesc.InputLayout.pInputElementDescs = mInputLayouts.data();
	}
	// プリミティブトポロジー
	mDesc.PrimitiveTopologyType = psoInit.mPrimitiveTopology;

	// パイプラインステートを作成
	[[maybe_unused]] HRESULT hr = gDirectXCore->GetDevice()->CreateGraphicsPipelineState(
		&mDesc,
		IID_PPV_ARGS(mPipelineState.GetAddressOf())
	);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

void PipelineState::Bind(ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	assert(cmdList);
	assert(mPipelineState);
	cmdList->SetPipelineState(mPipelineState.Get());
}
