#include "Material.h"
#include "Graphics/Renderer.h"
#include "Graphics/Texture.h"

Material::Material()
	: mCBuff(nullptr)
	, mName("None")
	, mTexturePath(Texture::kTexturePath + "Default.png")
	, mTexture(nullptr)
	, mBaseColor(Color::kWhite)
	, mSpecular(100.0f)
	, mIsShadowCast(true)
{

}

void Material::Create(Renderer* renderer)
{
	assert(renderer);
	Constant initData = {};
	initData.mBaseColor = mBaseColor;
	initData.mSpecular = mSpecular;
	initData.mIsShadowCast = mIsShadowCast;
	mCBuff = std::make_unique<ConstantBuffer>();
	mCBuff->Create(sizeof(Constant), &initData);
	mTexture = renderer->GetTexture(mTexturePath);
	assert(mTexture);
}

void Material::Bind(ID3D12GraphicsCommandList* cmdList, uint32_t matRootParamIdx, uint32_t texRootParamIdx)
{
	assert(cmdList);
	Constant data = {};
	data.mBaseColor = mBaseColor;
	data.mSpecular = mSpecular;
	data.mIsShadowCast = mIsShadowCast;
	mCBuff->Copy(data);
	mCBuff->Bind(cmdList, matRootParamIdx);
	mTexture->Bind(cmdList, texRootParamIdx);
}
