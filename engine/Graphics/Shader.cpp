#include "Shader.h"
#include "GraphicsEngine.h"
#include "Helper/Helper.h"
#include <format>

Shader::Shader()
	: mFilePath()
	, mBlob(nullptr)
{}

// Vertex Shader
bool Shader::CompileVs(const std::string& filePath)
{
	mFilePath = filePath;
	mBlob = gDirectXCore->CompileHLSL(filePath, "vs_6_0");
	return CheckResult();
}

// Geometry Shader
bool Shader::CompileGs(const std::string& filePath)
{
	mFilePath = filePath;
	mBlob = gDirectXCore->CompileHLSL(filePath, "gs_6_0");
	return CheckResult();
}

// Pixel Shader
bool Shader::CompilePs(const std::string& filePath)
{
	mFilePath = filePath;
	mBlob = gDirectXCore->CompileHLSL(filePath, "ps_6_0");
	return CheckResult();
}

bool Shader::CheckResult()
{
	if (mBlob)
	{
		Helper::Log(
			std::format("Success: Compile \"{}\"\n", mFilePath));
		return true;
	}
	else
	{
		Helper::Log(
			std::format("Failure: Compile \"{}\"\n", mFilePath));
		return false;
	}
}
