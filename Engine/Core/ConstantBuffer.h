#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

// TODO: デスクリプタヒープに対応

// 定数バッファ
class ConstantBuffer
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	bool Create(uint32_t size, void* initData = nullptr);
	void Bind(ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t rootParamIdx);

	// バッファを更新
	void Copy(void* data);
	template <typename T>
	void Copy(T& data)
	{
		Copy(&data);
	}

	// TODO: 仮想アドレスは隠ぺい
	void* GetData() const { return mData; }

private:
	uint32_t mSize;
	uint32_t mAlignedSize;// 256アライメントされたサイズ
	ComPtr<ID3D12Resource> mBuff;
	void* mData;// 仮想アドレス
};
