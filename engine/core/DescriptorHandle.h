#pragma once
#include <d3d12.h>

// デスクリプタハンドル
struct DescriptorHandle
{
	D3D12_CPU_DESCRIPTOR_HANDLE mCPU;// CPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE mGPU;// GPUハンドル
	bool HasCPU() { return mCPU.ptr != 0; }
	bool HasGPU() { return mGPU.ptr != 0; }
};
