#include "ModelCommon.h"
#include "Core/GraphicsCommon.h"
#include "Graphics/Camera.h"
#include "Graphics/Renderer.h"

const std::string ModelCommon::kModelPath = "Assets/Model/";

ID3D12GraphicsCommandList* ModelCommon::mCmdList = nullptr;
Renderer* ModelCommon::mRenderer = nullptr;
std::unique_ptr<RootSignature> ModelCommon::mRootSignature;
PipelineState ModelCommon::mPsos[3];
PipelineState ModelCommon::mSkinnedPsos[3];
std::unique_ptr<ConstantBuffer> ModelCommon::mCBuff;

void ModelCommon::Initialize(Renderer* renderer)
{
	assert(renderer);
	mRenderer = renderer;

	// ルートシグネチャ
	//mRootSignature.Initialize(6, 1);
	mRootSignature = std::make_unique<RootSignature>(6, 1);
	mRootSignature->Parameters(0).InitConstants(0);
	mRootSignature->Parameters(1).InitConstants(1);
	mRootSignature->Parameters(2).InitConstants(2);
	mRootSignature->Parameters(3).InitDescriptorTable(1);
	mRootSignature->Parameters(3).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	mRootSignature->Parameters(4).InitConstants(3);
	mRootSignature->Parameters(5).InitDescriptorTable(1);
	mRootSignature->Parameters(5).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	mRootSignature->Samplers(0) = DirectXCommonSettings::gSamplerLinearWrap;
	mRootSignature->Create();

	// シェーダ
	Shader* defaultVs = renderer->GetVs("Assets/Shader/Model/DefaultVs.hlsl");
	Shader* skinnedVs = renderer->GetVs("Assets/Shader/Model/SkinnedVs.hlsl");
	Shader* defaultPs = renderer->GetPs("Assets/Shader/Model/DefaultPs.hlsl");
	Shader* unlightPs = renderer->GetPs("Assets/Shader/Model/UnlightPs.hlsl");

	// パイプラインステート
	/*mPsos[uint32_t(Type::Default)].SetRootSignature(mRootSignature.get());
	mPsos[uint32_t(Type::Default)].SetVertexShader(defaultVs);
	mPsos[uint32_t(Type::Default)].SetPixelShader(defaultPs);
	mPsos[uint32_t(Type::Default)].SetBlendState(DirectXCommonSettings::gBlendAlpha);
	mPsos[uint32_t(Type::Default)].SetRasterizerState(DirectXCommonSettings::gRasterizerDefault);
	mPsos[uint32_t(Type::Default)].SetDepthStencilState(DirectXCommonSettings::gDepthEnable);
	D3D12_INPUT_ELEMENT_DESC inputLayouts[3] = {};
	inputLayouts[0].SemanticName = "POSITION";
	inputLayouts[0].SemanticIndex = 0;
	inputLayouts[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayouts[1].SemanticName = "NORMAL";
	inputLayouts[1].SemanticIndex = 0;
	inputLayouts[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayouts[2].SemanticName = "TEXCOORD";
	inputLayouts[2].SemanticIndex = 0;
	inputLayouts[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputLayouts[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	mPsos[uint32_t(Type::Default)].SetInputLayout(_countof(inputLayouts), inputLayouts);*/

	PSOInit init = {};
	init.mRootSignature = mRootSignature.get();
	init.mVertexShader = defaultVs;
	init.mPixelShader = defaultPs;
	init.mBlendDesc = DirectXCommonSettings::gBlendAlpha;
	init.mRasterizerDesc = DirectXCommonSettings::gRasterizerDefault;
	init.mDepthStencilDesc = DirectXCommonSettings::gDepthEnable;
	std::vector<D3D12_INPUT_ELEMENT_DESC>inputLayouts(3);
	inputLayouts[0].SemanticName = "POSITION";
	inputLayouts[0].SemanticIndex = 0;
	inputLayouts[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayouts[1].SemanticName = "NORMAL";
	inputLayouts[1].SemanticIndex = 0;
	inputLayouts[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayouts[2].SemanticName = "TEXCOORD";
	inputLayouts[2].SemanticIndex = 0;
	inputLayouts[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputLayouts[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	init.mInputLayouts = inputLayouts;
	mPsos[uint32_t(Type::Default)].Create(init);

	init.mPixelShader = unlightPs;
	mPsos[uint32_t(Type::Unlight)].Create(init);

	init.mRasterizerDesc = DirectXCommonSettings::gRasterizerWireframe;
	mPsos[uint32_t(Type::Wireframe)].Create(init);

	/*mPsos[uint32_t(Type::Unlight)] = mPsos[uint32_t(Type::Default)];
	mPsos[uint32_t(Type::Unlight)].SetPixelShader(unlightPs);
	mPsos[uint32_t(Type::Wireframe)] = mPsos[uint32_t(Type::Default)];
	mPsos[uint32_t(Type::Wireframe)].SetRasterizerState(DirectXCommonSettings::gRasterizerWireframe);
	for (auto& p : mPsos)
	{
		p.Create();
	}*/

	// スキンアニメーション用
	init.mRootSignature = mRootSignature.get();
	init.mVertexShader = skinnedVs;
	init.mPixelShader = defaultPs;
	init.mBlendDesc = DirectXCommonSettings::gBlendAlpha;
	init.mRasterizerDesc = DirectXCommonSettings::gRasterizerDefault;
	init.mDepthStencilDesc = DirectXCommonSettings::gDepthEnable;

	/*mSkinnedPsos[uint32_t(Type::Default)].SetRootSignature(mRootSignature.get());
	mSkinnedPsos[uint32_t(Type::Default)].SetVertexShader(skinnedVs);
	mSkinnedPsos[uint32_t(Type::Default)].SetPixelShader(defaultPs);
	mSkinnedPsos[uint32_t(Type::Default)].SetBlendState(DirectXCommonSettings::gBlendAlpha);
	mSkinnedPsos[uint32_t(Type::Default)].SetRasterizerState(DirectXCommonSettings::gRasterizerDefault);
	mSkinnedPsos[uint32_t(Type::Default)].SetDepthStencilState(DirectXCommonSettings::gDepthEnable);*/
	std::vector<D3D12_INPUT_ELEMENT_DESC>skinnedInputLayouts(5);
	skinnedInputLayouts[0].SemanticName = "POSITION";
	skinnedInputLayouts[0].SemanticIndex = 0;
	skinnedInputLayouts[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	skinnedInputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	skinnedInputLayouts[1].SemanticName = "NORMAL";
	skinnedInputLayouts[1].SemanticIndex = 0;
	skinnedInputLayouts[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	skinnedInputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	skinnedInputLayouts[2].SemanticName = "TEXCOORD";
	skinnedInputLayouts[2].SemanticIndex = 0;
	skinnedInputLayouts[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	skinnedInputLayouts[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	skinnedInputLayouts[3].SemanticName = "WEIGHTS";
	skinnedInputLayouts[3].SemanticIndex = 0;
	skinnedInputLayouts[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	skinnedInputLayouts[3].InputSlot = 1;//
	skinnedInputLayouts[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	skinnedInputLayouts[4].SemanticName = "BONEINDICES";
	skinnedInputLayouts[4].SemanticIndex = 0;
	skinnedInputLayouts[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;
	skinnedInputLayouts[4].InputSlot = 1;//
	skinnedInputLayouts[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	init.mInputLayouts = skinnedInputLayouts;
	//mSkinnedPsos[uint32_t(Type::Default)].SetInputLayout(_countof(skinnedInputLayouts), skinnedInputLayouts);
	mSkinnedPsos[uint32_t(Type::Default)].Create(init);

	init.mPixelShader = unlightPs;
	mSkinnedPsos[uint32_t(Type::Unlight)].Create(init);

	init.mRasterizerDesc = DirectXCommonSettings::gRasterizerWireframe;
	mSkinnedPsos[uint32_t(Type::Wireframe)].Create(init);

	/*for (auto& p : mSkinnedPsos)
	{
		p.Create();
	}*/

	mCBuff = std::make_unique<ConstantBuffer>();
	mCBuff->Create(sizeof(Constant));
}

// レンダリング前
void ModelCommon::PreRendering(ID3D12GraphicsCommandList* cmdList)
{
	assert(cmdList);
	mCmdList = cmdList;
	mRootSignature->Bind(mCmdList);
	mPsos[uint32_t(Type::Default)].Bind(mCmdList);// とりま
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// カメラ
	Camera* camera = mRenderer->GetCurrCamera();
	if (camera)
	{
		Constant data;
		data.mView = camera->GetView();
		data.mProj = camera->GetProj();
		data.mCameraPos = camera->GetPosition();
		mCBuff->Copy(data);
	}
	mCBuff->Bind(cmdList, 1);
	// ライト
	mRenderer->GetLightManager()->Bind(cmdList, 4);
}

// レンダリング後
void ModelCommon::PostRendering()
{
	mCmdList = nullptr;
}

void ModelCommon::SetPso(Type type)
{
	assert(mCmdList);
	mPsos[uint32_t(type)].Bind(mCmdList);
}

void ModelCommon::SetSkinnedPso(Type type)
{
	assert(mCmdList);
	mSkinnedPsos[uint32_t(type)].Bind(mCmdList);
}
