#pragma once
#include "RootParameter.h"
#include <cassert>
#include <memory>
#include <wrl.h>

// ルートシグネチャ
class RootSignature
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	RootSignature(uint32_t numParameters, uint32_t numSamplers);

	// ルートパラメータ、サンプラーにアクセス
	RootParameter& Parameters(uint32_t idx)
	{
		assert(idx >= 0 && idx < mNumParameters);
		return mParameters[idx];
	}
	D3D12_STATIC_SAMPLER_DESC& Samplers(uint32_t idx)
	{
		assert(idx >= 0 && idx < mNumSamplers);
		return mSamplers[idx];
	}

	bool Create();
	void Bind(ComPtr<ID3D12GraphicsCommandList> cmdList);

	ComPtr<ID3D12RootSignature> GetRootSignature() const { return mRootSignature; }

private:
	ComPtr<ID3D12RootSignature> mRootSignature;
	// ルートパラメータ
	uint32_t mNumParameters;
	std::unique_ptr<RootParameter[]> mParameters;
	// サンプラー
	uint32_t mNumSamplers;
	std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> mSamplers;
};
