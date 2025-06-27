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

			// ファイルがキャッシュにない場合、新規追加
			if (it == prvReadFile.end())
			{
				auto newFile = std::make_unique<File>(filename);
				ym::ConsoleLogRelease("FileObserver::IsModified() : file is not found\n");
				prvReadFile[filename] = std::move(newFile);
				return true;
			}

			// キャッシュがあり、interval 内なら変更なし
			if (timer.elapsedTime() < interval)
			{				
				return false;
			}
			timer.reset();
			// ファイルを新規ロード
			auto newFile = std::make_unique<File>(filename);

			// サイズが異なる場合は更新 & true を返す
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
		const float interval = 0.0f;//秒
		std::unordered_map<std::string, std::unique_ptr<File>> prvReadFile{};
	};

}