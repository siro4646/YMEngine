#pragma once

#include <string>
#include <cctype>
#include <algorithm>

namespace ym
{
	inline std::string NormalizePath(const std::string &path)
	{
		std::string fixed = path;

		// Windows�̋�؂蕶�� `\` �� `/` �ɒu��������
		std::replace(fixed.begin(), fixed.end(), '\\', '/');

		// �擪�▖���̕s�v�ȋ󔒂���s�Ȃǂ��폜�������ꍇ�͂�������ǉ�
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

		// exe -> bin/Debug/app.exe �� �\�����[�V�������[�g�ɖ߂�
		std::filesystem::path solutionDir = exePath.parent_path().parent_path().parent_path();
		return solutionDir;
	}

	inline std::string GetRelativePathToSolution(const std::filesystem::path &droppedPath)
	{
		auto solutionDir = GetSolutionDirectory();
		std::filesystem::path relative = std::filesystem::relative(droppedPath, solutionDir);
		return relative.string(); // �܂��� .wstring() if Unicode
	}


	// �C�ӂ̃I�u�W�F�N�g����N���X���i���������j���擾
	template <typename T>
	inline std::string GetShortClassName(const T &obj) {
		std::string fullName = typeid(obj).name();

		// "::" �̍Ō�̈ʒu��T���iMSVC�ł�"::"���܂܂�Ȃ��ꍇ������̂�':'�����ɂ���j
		size_t pos = fullName.find_last_of(':');
		if (pos != std::string::npos) {
			return fullName.substr(pos + 1);
		}
		return fullName; // ':' ��������Ȃ���΂��̂܂ܕԂ�
	}
}	// namespace ym


//	EOF
