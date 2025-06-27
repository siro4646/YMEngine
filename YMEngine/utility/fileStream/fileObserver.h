#pragma once

#include "utility/fileStream/fileStream.h"

namespace ym
{

	class File;

	class FileObserver
	{
	public:
		static FileObserver *Instance()
		{
			static FileObserver instance;
			return &instance;
		}
		bool IsModified(const char *filename)
		{
			auto it = prvReadFile.find(filename);

			// �t�@�C�����L���b�V���ɂȂ��ꍇ�A�V�K�ǉ�
			if (it == prvReadFile.end())
			{
				auto newFile = std::make_unique<File>(filename);
				ym::ConsoleLogRelease("FileObserver::IsModified() : file is not found\n");
				prvReadFile[filename] = std::move(newFile);
				return true;
			}

			// �L���b�V��������Ainterval ���Ȃ�ύX�Ȃ�
			if (timer.elapsedTime() < interval)
			{				
				return false;
			}
			timer.reset();
			// �t�@�C����V�K���[�h
			auto newFile = std::make_unique<File>(filename);

			// �T�C�Y���قȂ�ꍇ�͍X�V & true ��Ԃ�
			if (it->second->GetSize() != newFile->GetSize())
			{
				ym::ConsoleLogRelease("FileObserver::IsModified() : file size is different\n");
				it->second = std::move(newFile);
				return true;
			}

			//ym::ConsoleLogRelease("FileObserver::IsModified() : file is not modified\n");
			return false;
		}

		void Uninit()
		{
			for (auto &file : prvReadFile)
			{
				file.second->Destroy();
			}
		}

	private:
		Timer timer;
		const float interval = 0.0f;//�b
		std::unordered_map<std::string, std::unique_ptr<File>> prvReadFile{};
	};

}