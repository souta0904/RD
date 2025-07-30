#pragma once
#include "Color.h"
#include "core/DescriptorHeap.h"
#include "Texture.h"
#include <memory>

// 書き込み可テクスチャ
class RenderTexture : public Texture
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	~RenderTexture();

	bool Create(uint32_t width, uint32_t height, bool useDepth = true);

	// 書き込み前後処理
	// この間のバインドは不可
	void BeginRender(ComPtr<ID3D12GraphicsCommandList> cmdList);
	void EndRender(ComPtr<ID3D12GraphicsCommandList> cmdList);

private:
	bool mUseDepth;// 深度バッファの有無
	DescriptorHandle* mHandleRTV;
	ComPtr<ID3D12Resource> mDepthBuff;
	DescriptorHandle* mHandleDSV;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissor;
	Color mClearColor = Color::kRed;
};
