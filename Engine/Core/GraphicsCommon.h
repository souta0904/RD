#pragma once
#include <d3d12.h>

namespace DirectXCommonSettings
{
	// ヒーププロパティ
	extern D3D12_HEAP_PROPERTIES gHeapPropDefault;
	extern D3D12_HEAP_PROPERTIES gHeapPropUpload;

	// ブレンド
	extern D3D12_BLEND_DESC gBlendNone;
	extern D3D12_BLEND_DESC gBlendAlpha;
	extern D3D12_BLEND_DESC gBlendAdd;
	extern D3D12_BLEND_DESC gBlendSubtract;
	extern D3D12_BLEND_DESC gBlendMultiply;
	extern D3D12_BLEND_DESC gBlendScreen;

	// ラスタライザ
	extern D3D12_RASTERIZER_DESC gRasterizerDefault;
	extern D3D12_RASTERIZER_DESC gRasterizerCullNone;
	extern D3D12_RASTERIZER_DESC gRasterizerWireframe;

	// 深度ステンシル
	extern D3D12_DEPTH_STENCIL_DESC gDepthEnable;
	extern D3D12_DEPTH_STENCIL_DESC gDepthDisable;
	extern D3D12_DEPTH_STENCIL_DESC gDepthReadOnly;

	// サンプラー
	extern D3D12_STATIC_SAMPLER_DESC gSamplerLinearWrap;
	extern D3D12_STATIC_SAMPLER_DESC gSamplerLinearClamp;

	void Initialize();
	void Terminate();
}
