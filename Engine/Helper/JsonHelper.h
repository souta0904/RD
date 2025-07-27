#pragma once
#include "Color.h"
#include "Quaternion.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <nlohmann/json.hpp>

// json用ヘルパー関数
namespace JsonHelper
{
	// 読み取り用
	bool GetBool(const nlohmann::json& data, const char* name, bool& out);
	bool GetInt32(const nlohmann::json& data, const char* name, int32_t& out);
	bool GetUint32(const nlohmann::json& data, const char* name, uint32_t& out);
	bool GetFloat(const nlohmann::json& data, const char* name, float& out);
	bool GetString(const nlohmann::json& data, const char* name, std::string& out);
	bool GetColor(const nlohmann::json& data, const char* name, Color& out);
	bool GetQuaternion(const nlohmann::json& data, const char* name, Quaternion& out);
	bool GetVector2(const nlohmann::json& data, const char* name, Vector2& out);
	bool GetVector3(const nlohmann::json& data, const char* name, Vector3& out);
	bool GetVector4(const nlohmann::json& data, const char* name, Vector4& out);

	// 書き込み用
	void SetBool(nlohmann::json& data, const char* name, bool in);
	void SetInt32(nlohmann::json& data, const char* name, int32_t in);
	void SetUint32(nlohmann::json& data, const char* name, uint32_t in);
	void SetFloat(nlohmann::json& data, const char* name, float in);
	void SetString(nlohmann::json& data, const char* name, const std::string& in);
	void SetColor(nlohmann::json& data, const char* name, const Color& in);
	void SetQuaternion(nlohmann::json& data, const char* name, const Quaternion& in);
	void SetVector2(nlohmann::json& data, const char* name, const Vector2& in);
	void SetVector3(nlohmann::json& data, const char* name, const Vector3& in);
	void SetVector4(nlohmann::json& data, const char* name, const Vector4& in);
}
