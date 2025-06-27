#pragma once
namespace ym
{
	class DropFileManager : public Singleton<DropFileManager>
	{
	public:

		//<summary>
		//	ドロップされたファイルのリストを取得
		/// </summary>
		/// <returns>ドロップされたファイルのリスト</returns>
		std::vector<std::string>GetDroppedFiles() { 
			auto buf = droppedFiles_;
			for (auto &file : buf)
			{
				ym::ConsoleLogRelease("ドロップされたファイル: %s\n", file.c_str());
			}
			return buf;
		}

		void Update()
		{
			frameCount_++; // フレームカウントを更新
			if (droppedThisFrame_)
			{
				if (frameCount_ > 1) // ドロップされたファイルがある場合、次のフレームでリセット
				{
					droppedThisFrame_ = false; // このフレームでドロップされたかどうかをリセット
					droppedFiles_.clear(); // ドロップされたファイルのリストをクリア
					frameCount_ = 0; // フレームカウントをリセット
				}
			}
		}

		void DrawDroppedFiles()
		{
			if (droppedFiles_.empty())
			{
				ImGui::Text("No files dropped.");
				return;
			}

			ImGui::Text("Dropped Files:");
			for (const auto &file : droppedFiles_)
			{
				ImGui::BulletText("%s", file.c_str());
			}
		}
		bool IsFileDroppedThisFrame() const {
			return droppedThisFrame_;
		}

		/// <summary>
		/// ドロップされたファイルを追加
		/// </summary>
		void AddDroppedFile(const std::string &filePath) { 
			droppedFiles_.push_back(filePath); 
			droppedThisFrame_ = true;
			frameCount_ = 0; // フレームカウントをリセット
		}
	private:
		int frameCount_ = 0; // フレームカウント
		std::vector<std::string> droppedFiles_; // ドロップされたファイルのリスト
		bool droppedThisFrame_ = false; // このフレームでドロップされたかどうか
	};
}