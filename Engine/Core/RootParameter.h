#pragma once
#include <cstdint>
#include <d3d12.h>

// ルートパラメータ
class RootParameter
{
	friend class RootSignature;

public:
	~RootParameter();
	void InitConstants(uint32_t shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL);
	void InitDescriptorTable(uint32_t numDescriptorRanges, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL);
	void InitDescriptorRange(uint32_t idx, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, uint32_t numDescriptors, uint32_t baseShaderRegister);

private:
	D3D12_ROOT_PARAMETER mRootParameter;
};
