#pragma once
#include <string>

// ヘルパー関数
namespace Helper
{
	// 出力ウィンドウへ出力
	void Log(const std::string& str);
	void Log(const std::wstring& str);

	// 文字列を変換
	std::wstring ConvertString(const std::string& str);
	std::string ConvertString(const std::wstring& str);

	// ファイル名操作
	std::string GetFilename(const std::string& path);
	std::string GetStem(const std::string& path);
	std::string GetExtension(const std::string& path);
}
