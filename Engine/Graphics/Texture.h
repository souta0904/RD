#pragma once
#include "core/DescriptorHeap.h"
#include <d3d12.h>
#include <DirectXTex/DirectXTex.h>
#include <string>
#include <wrl.h>

// テクスチャ
class Texture
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	~Texture();

	bool Create(const std::string& path);
	void Bind(ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t rootParamIdx);

	const std::string& GetPath() const { return mPath; }
	uint32_t GetWidth() const { return mWidth; }
	uint32_t GetHeight() const { return mHeight; }
	ComPtr<ID3D12Resource> GetTexBuff() const { return mTexBuff; }
	DescriptorHandle* GetHandleSRV() const { return mHandleSRV; }

protected:
	std::string mPath;
	uint32_t mWidth;
	uint32_t mHeight;
	ComPtr<ID3D12Resource> mTexBuff;
	DescriptorHandle* mHandleSRV;
};
