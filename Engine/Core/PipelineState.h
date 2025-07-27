#pragma once
#include "RootSignature.h"
#include "Graphics/Shader.h"
#include <memory>
#include <vector>
#include <wrl.h>

// パイプライン初期化用
struct PSOInit
{
	RootSignature* mRootSignature;
	Shader* mVertexShader;
	Shader* mPixelShader;
	D3D12_BLEND_DESC mBlendDesc;
	D3D12_RASTERIZER_DESC mRasterizerDesc;
	D3D12_DEPTH_STENCIL_DESC mDepthStencilDesc;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayouts;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE mPrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
};

// パイプラインステート
class PipelineState
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	PipelineState();
	PipelineState(const PipelineState& ps);
	PipelineState& operator=(const PipelineState& ps);

	bool Create(const PSOInit& psoInit);
	void Bind(ComPtr<ID3D12GraphicsCommandList> cmdList);

	ComPtr<ID3D12PipelineState> GetPipelineState() const { return mPipelineState; }

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC mDesc;
	ComPtr<ID3D12PipelineState> mPipelineState;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayouts;// 頂点レイアウト
};
