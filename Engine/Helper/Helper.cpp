#include "Helper.h"
#include <Windows.h>

namespace Helper
{
	void Log(const std::string& str)
	{
#ifdef _DEBUG
		// デバッグ時のみ出力
		OutputDebugStringA(str.c_str());
#endif
	}

	void Log(const std::wstring& str)
	{
		Log(ConvertString(str));
	}

	// std::stringをstd::wstringへ
	std::wstring ConvertString(const std::string& str)
	{
		if (str.empty())
		{
			return std::wstring();
		}
		auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);
		if (sizeNeeded < 1)
		{
			return std::wstring();
		}
		std::wstring result(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded);
		return result;
	}

	// std::wstringをstd::stringへ
	std::string ConvertString(const std::wstring& str)
	{
		if (str.empty())
		{
			return std::string();
		}
		auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
		if (sizeNeeded < 1)
		{
			return std::string();
		}

		std::string result(sizeNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, nullptr, nullptr);
		return result;
	}

	// ファイル名を取得
	std::string GetFilename(const std::string& path)
	{
		auto pos = path.rfind('\\');
		if (pos != std::string::npos)
		{
			return path.substr(pos + 1);
		}
		pos = path.rfind('/');
		if (pos != std::string::npos)
		{
			return path.substr(pos + 1);
		}
		return path;
	}

	// 拡張子を取得
	std::string GetExtension(const std::string& path)
	{
		auto pos = path.rfind('.');
		if (pos != std::string::npos)
		{
			return path.substr(pos + 1);
		}
		return "";
	}

	// 拡張子を除いたファイル名を取得
	std::string GetStem(const std::string& path)
	{
		auto filename = GetFilename(path);
		auto pos = filename.rfind('.');
		if (pos != std::string::npos)
		{
			return filename.substr(0, pos);
		}
		return filename;
	}
}
