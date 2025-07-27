#include "JsonHelper.h"

namespace JsonHelper
{
#pragma region Read

	// bool
	bool GetBool(const nlohmann::json& data, const char* name, bool& out)
	{
		auto it = data.find(name);
		if (it != data.end())
		{
			auto val = data.at(name);
			if (val.is_boolean())
			{
				out = val.get<bool>();
				return true;
			}
		}
		return false;
	}

	// int32_t
	bool GetInt32(const nlohmann::json& data, const char* name, int32_t& out)
	{
		auto it = data.find(name);
		if (it != data.end())
		{
			auto val = data.at(name);
			if (val.is_number_integer())
			{
				out = val.get<int32_t>();
				return true;
			}
		}
		return false;
	}

	// uint32_t
	bool GetUint32(const nlohmann::json& data, const char* name, uint32_t& out)
	{
		auto it = data.find(name);
		if (it != data.end())
		{
			auto val = data.at(name);
			if (val.is_number_unsigned())
			{
				out = val.get<uint32_t>();
				return true;
			}
		}
		return false;
	}

	// float
	bool GetFloat(const nlohmann::json& data, const char* name, float& out)
	{
		auto it = data.find(name);
		if (it != data.end())
		{
			auto val = data.at(name);
			if (val.is_number_float())
			{
				out = val.get<float>();
				return true;
			}
		}
		return false;
	}

	// string
	bool GetString(const nlohmann::json& data, const char* name, std::string& out)
	{
		auto it = data.find(name);
		if (it != data.end())
		{
			auto val = data.at(name);
			if (val.is_string())
			{
				out = val.get<std::string>();
				return true;
			}
		}
		return false;
	}

	// Color
	bool GetColor(const nlohmann::json& data, const char* name, Color& out)
	{
		auto it = data.find(name);
		if (it != data.end())
		{
			auto val = data.at(name);
			if (val.is_array() && val.size() == 4)
			{
				for (uint32_t i = 0; i < 4; ++i)
				{
					if (!val[i].is_number_float())
					{
						return false;
					}
				}
				out.r = val[0].get<float>();
				out.g = val[1].get<float>();
				out.b = val[2].get<float>();
				out.a = val[3].get<float>();
				return true;
			}
		}
		return false;
	}

	// Quaternion
	bool GetQuaternion(const nlohmann::json& data, const char* name, Quaternion& out)
	{
		auto it = data.find(name);
		if (it != data.end())
		{
			auto val = data.at(name);
			if (val.is_array() && val.size() == 4)
			{
				for (uint32_t i = 0; i < 4; ++i)
				{
					if (!val[i].is_number_float())
					{
						return false;
					}
				}
				out.w = val[0].get<float>();
				out.x = val[1].get<float>();
				out.y = val[2].get<float>();
				out.z = val[3].get<float>();
				return true;
			}
		}
		return false;
	}

	// Vector2
	bool GetVector2(const nlohmann::json& data, const char* name, Vector2& out)
	{
		auto it = data.find(name);
		if (it != data.end())
		{
			auto val = data.at(name);
			if (val.is_array() && val.size() == 2)
			{
				for (uint32_t i = 0; i < 2; ++i)
				{
					if (!val[i].is_number_float())
					{
						return false;
					}
				}
				out.x = val[0].get<float>();
				out.y = val[1].get<float>();
				return true;
			}
		}
		return false;
	}

	// Vector3
	bool GetVector3(const nlohmann::json& data, const char* name, Vector3& out)
	{
		auto it = data.find(name);
		if (it != data.end())
		{
			auto val = data.at(name);
			if (val.is_array() && val.size() == 3)
			{
				for (uint32_t i = 0; i < 3; ++i)
				{
					if (!val[i].is_number_float())
					{
						return false;
					}
				}
				out.x = val[0].get<float>();
				out.y = val[1].get<float>();
				out.z = val[2].get<float>();
				return true;
			}
		}
		return false;
	}

	// Vector4
	bool GetVector4(const nlohmann::json& data, const char* name, Vector4& out)
	{
		auto it = data.find(name);
		if (it != data.end())
		{
			auto val = data.at(name);
			if (val.is_array() && val.size() == 4)
			{
				for (uint32_t i = 0; i < 4; ++i)
				{
					if (!val[i].is_number_float())
					{
						return false;
					}
				}
				out.x = val[0].get<float>();
				out.y = val[1].get<float>();
				out.z = val[2].get<float>();
				out.w = val[3].get<float>();
				return true;
			}
		}
		return false;
	}

#pragma endregion

#pragma region Write

	// bool
	void SetBool(nlohmann::json& data, const char* name, bool in)
	{
		nlohmann::json val(in);
		data[name] = val;
	}

	// int32_t
	void SetInt32(nlohmann::json& data, const char* name, int32_t in)
	{
		nlohmann::json val(in);
		data[name] = val;
	}

	// uint32_t
	void SetUint32(nlohmann::json& data, const char* name, uint32_t in)
	{
		nlohmann::json val(in);
		data[name] = val;
	}

	// float
	void SetFloat(nlohmann::json& data, const char* name, float in)
	{
		nlohmann::json val(in);
		data[name] = val;
	}

	// string
	void SetString(nlohmann::json& data, const char* name, const std::string& in)
	{
		nlohmann::json val(in);
		data[name] = val;
	}

	// Color
	void SetColor(nlohmann::json& data, const char* name, const Color& in)
	{
		nlohmann::json val;
		val.push_back(in.r);
		val.push_back(in.g);
		val.push_back(in.b);
		val.push_back(in.a);
		data[name] = val;
	}

	// Quaternion
	void SetQuaternion(nlohmann::json& data, const char* name, const Quaternion& in)
	{
		nlohmann::json val;
		val.push_back(in.w);
		val.push_back(in.x);
		val.push_back(in.y);
		val.push_back(in.z);
		data[name] = val;
	}

	// Vector2
	void SetVector2(nlohmann::json& data, const char* name, const Vector2& in)
	{
		nlohmann::json val;
		val.push_back(in.x);
		val.push_back(in.y);
		data[name] = val;
	}

	// Vector3
	void SetVector3(nlohmann::json& data, const char* name, const Vector3& in)
	{
		nlohmann::json val;
		val.push_back(in.x);
		val.push_back(in.y);
		val.push_back(in.z);
		data[name] = val;
	}

	// Vector4
	void SetVector4(nlohmann::json& data, const char* name, const Vector4& in)
	{
		nlohmann::json val;
		val.push_back(in.x);
		val.push_back(in.y);
		val.push_back(in.z);
		val.push_back(in.w);
		data[name] = val;
	}

#pragma endregion
}
