#pragma once

#include <string>
#include <cctype>
#include <algorithm>

namespace ym
{

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

		// �K�v�ȃT�C�Y���v�Z�i�I�[�������܂ނ��� +1�j
		int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
		if (sizeNeeded == 0) {
			throw std::runtime_error("Failed to convert UTF-8 to UTF-16.");
		}

		// �ϊ�
		std::wstring utf16(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &utf16[0], sizeNeeded);

		// �I�[��������菜��
		utf16.resize(sizeNeeded - 1);
		return utf16;
	}

	inline std::string Utf16ToUtf8(const std::wstring &utf16)
	{
		if (utf16.empty()) return std::string();

		// �K�v�ȃT�C�Y���v�Z�i�I�[�������܂ނ��� +1�j
		int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, NULL, 0, NULL, NULL);
		if (sizeNeeded == 0) {
			throw std::runtime_error("Failed to convert UTF-16 to UTF-8.");
		}

		// �ϊ�
		std::string utf8(sizeNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, &utf8[0], sizeNeeded, NULL, NULL);

		// �I�[��������菜��
		utf8.resize(sizeNeeded - 1);
		return utf8;
	}

}	// namespace ym


//	EOF
