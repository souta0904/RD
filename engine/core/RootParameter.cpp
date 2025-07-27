#include "RootParameter.h"

RootParameter::~RootParameter()
{
	if (mRootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
	{
		delete[] mRootParameter.DescriptorTable.pDescriptorRanges;
	}
}

// 定数バッファビューとして初期化
void RootParameter::InitConstants(uint32_t shaderRegister, D3D12_SHADER_VISIBILITY shaderVisibility)
{
	mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	mRootParameter.Constants.ShaderRegister = shaderRegister;
	mRootParameter.Constants.RegisterSpace = 0;
	mRootParameter.Constants.Num32BitValues = 0;
	mRootParameter.ShaderVisibility = shaderVisibility;
}

// デスクリプタテーブルとして初期化
void RootParameter::InitDescriptorTable(uint32_t numDescriptorRanges, D3D12_SHADER_VISIBILITY shaderVisibility)
{
	mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	mRootParameter.DescriptorTable.NumDescriptorRanges = numDescriptorRanges;
	// TODO: できればスマートポインタで管理
	mRootParameter.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[numDescriptorRanges];
	mRootParameter.ShaderVisibility = shaderVisibility;
}

// デスクリプタレンジを初期化
void RootParameter::InitDescriptorRange(uint32_t idx, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, uint32_t numDescriptors, uint32_t baseShaderRegister)
{
	D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(&mRootParameter.DescriptorTable.pDescriptorRanges[idx]);
	range->RangeType = rangeType;
	range->NumDescriptors = numDescriptors;
	range->BaseShaderRegister = baseShaderRegister;
	range->RegisterSpace = 0;
	range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
}
