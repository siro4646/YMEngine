#pragma once

#include <string>
#include <cctype>
#include <algorithm>

namespace ym
{
	inline std::string NormalizePath(const std::string &path)
	{
		std::string fixed = path;

		// Windowsの区切り文字 `\` を `/` に置き換える
		std::replace(fixed.begin(), fixed.end(), '\\', '/');

		// 先頭や末尾の不要な空白や改行などを削除したい場合はこちらも追加
		// fixed.erase(fixed.find_last_not_of(" \n\r\t") + 1);
		// fixed.erase(0, fixed.find_first_not_of(" \n\r\t"));

		return fixed;
	}

	inline std::string GetExtent(const std::string &name)
	{
		std::string ret;
		auto pos = name.rfind('.');
		if (pos == std::string::npos)
		{
			return ret;
		}

		ret = name.substr(pos);
		std::transform(ret.begin(), ret.end(), ret.begin(), [](unsigned char c) { return std::tolower(c); });
		return ret;
	}
	inline std::wstring GetDirectoryPath(const std::wstring &origin)
	{
		namespace fs = std::filesystem;
		fs::path p = origin.c_str();
		return p.remove_filename().c_str();
	}
	inline std::wstring ReplaceExtension(const std::wstring &origin, const char *ext)
	{
		namespace fs = std::filesystem;
		fs::path p = origin.c_str();
		return p.replace_extension(ext).c_str();
	}

	inline std::string ConvertYenToSlash(const std::string &path)
	{
		std::string ret = path;
		std::transform(ret.begin(), ret.end(), ret.begin(), [](char c) { return (c == '\\') ? '/' : c; });
		return ret;
	}

	inline std::string GetFileName(const std::string &path)
	{
		std::string ret = ConvertYenToSlash(path);
		auto pos = ret.rfind('/');
		if (pos != std::string::npos)
		{
			ret = ret.substr(pos + 1);
		}
		return ret;
	}

	inline std::string GetFileNameWithoutExtent(const std::string &path)
	{
		std::string ret = GetFileName(path);
		auto pos = ret.rfind('.');
		if (pos != std::string::npos)
		{
			ret = ret.erase(pos);
		}
		return ret;
	}

	inline std::string GetFilePath(const std::string &path)
	{
		std::string ret = ConvertYenToSlash(path);
		auto pos = ret.rfind('/');
		if (pos != std::string::npos)
		{
			ret = ret.substr(0, pos + 1);
		}
		else
		{
			ret = "./";
		}
		return ret;
	}
	inline std::wstring Utf8ToUtf16(const std::string &utf8)
	{
		if (utf8.empty()) return std::wstring();

		// 必要なサイズを計算（終端文字を含むため +1）
		int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
		if (sizeNeeded == 0) {
			throw std::runtime_error("Failed to convert UTF-8 to UTF-16.");
		}

		// 変換
		std::wstring utf16(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &utf16[0], sizeNeeded);

		// 終端文字を取り除く
		utf16.resize(sizeNeeded - 1);
		return utf16;
	}

	inline std::string Utf16ToUtf8(const std::wstring &utf16)
	{
		if (utf16.empty()) return std::string();

		// 必要なサイズを計算（終端文字を含むため +1）
		int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, NULL, 0, NULL, NULL);
		if (sizeNeeded == 0) {
			throw std::runtime_error("Failed to convert UTF-16 to UTF-8.");
		}

		// 変換
		std::string utf8(sizeNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, &utf8[0], sizeNeeded, NULL, NULL);

		// 終端文字を取り除く
		utf8.resize(sizeNeeded - 1);
		return utf8;
	}

	inline std::string MakeImGuiID(const std::string &label, const void *ptr)
	{
		std::ostringstream oss;
		oss << label << "##" << ptr;
		return oss.str();
	}
	inline std::filesystem::path GetSolutionDirectory()
	{
		wchar_t buffer[MAX_PATH] = {};
		GetModuleFileNameW(nullptr, buffer, MAX_PATH);
		std::filesystem::path exePath = buffer;

		// exe -> bin/Debug/app.exe → ソリューションルートに戻る
		std::filesystem::path solutionDir = exePath.parent_path().parent_path().parent_path();
		return solutionDir;
	}

	inline std::string GetRelativePathToSolution(const std::filesystem::path &droppedPath)
	{
		auto solutionDir = GetSolutionDirectory();
		std::filesystem::path relative = std::filesystem::relative(droppedPath, solutionDir);
		return relative.string(); // または .wstring() if Unicode
	}


	// 任意のオブジェクトからクラス名（末尾部分）を取得
	template <typename T>
	inline std::string GetShortClassName(const T &obj) {
		std::string fullName = typeid(obj).name();

		// "::" の最後の位置を探す（MSVCでは"::"が含まれない場合があるので':'だけにする）
		size_t pos = fullName.find_last_of(':');
		if (pos != std::string::npos) {
			return fullName.substr(pos + 1);
		}
		return fullName; // ':' が見つからなければそのまま返す
	}
}	// namespace ym


//	EOF
