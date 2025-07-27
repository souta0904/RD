#include "RootSignature.h"
#include "GraphicsEngine.h"

RootSignature::RootSignature(uint32_t numParameters, uint32_t numSamplers)
	: mNumParameters(numParameters)
	, mNumSamplers(numSamplers)
{
	if (mNumParameters > 0)
	{
		mParameters.reset(new RootParameter[mNumParameters]);
	}
	if (mNumSamplers > 0)
	{
		mSamplers.reset(new D3D12_STATIC_SAMPLER_DESC[mNumSamplers]);
	}
}

bool RootSignature::Create()
{
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = mNumParameters;
	if (desc.NumParameters > 0)
	{
		desc.pParameters = &mParameters[0].mRootParameter;
	}
	desc.NumStaticSamplers = mNumSamplers;
	desc.pStaticSamplers = mSamplers.get();
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// シリアライズ
	ComPtr<ID3DBlob> rsBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, rsBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// ルートシグネチャを作成
	hr = gDirectXCore->GetDevice()->CreateRootSignature(
		0,
		rsBlob->GetBufferPointer(),
		rsBlob->GetBufferSize(),
		IID_PPV_ARGS(mRootSignature.GetAddressOf())
	);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void RootSignature::Bind(ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	assert(cmdList);
	assert(mRootSignature);
	cmdList->SetGraphicsRootSignature(mRootSignature.Get());
}
