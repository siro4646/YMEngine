#pragma once
namespace ym
{
	class DropFileManager : public Singleton<DropFileManager>
	{
	public:

		//<summary>
		//	�h���b�v���ꂽ�t�@�C���̃��X�g���擾
		/// </summary>
		/// <returns>�h���b�v���ꂽ�t�@�C���̃��X�g</returns>
		std::vector<std::string>GetDroppedFiles() { 
			auto buf = droppedFiles_;
			for (auto &file : buf)
			{
				ym::ConsoleLogRelease("�h���b�v���ꂽ�t�@�C��: %s\n", file.c_str());
			}
			return buf;
		}

		void Update()
		{
			frameCount_++; // �t���[���J�E���g���X�V
			if (droppedThisFrame_)
			{
				if (frameCount_ > 1) // �h���b�v���ꂽ�t�@�C��������ꍇ�A���̃t���[���Ń��Z�b�g
				{
					droppedThisFrame_ = false; // ���̃t���[���Ńh���b�v���ꂽ���ǂ��������Z�b�g
					droppedFiles_.clear(); // �h���b�v���ꂽ�t�@�C���̃��X�g���N���A
					frameCount_ = 0; // �t���[���J�E���g�����Z�b�g
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
		/// �h���b�v���ꂽ�t�@�C����ǉ�
		/// </summary>
		void AddDroppedFile(const std::string &filePath) { 
			droppedFiles_.push_back(filePath); 
			droppedThisFrame_ = true;
			frameCount_ = 0; // �t���[���J�E���g�����Z�b�g
		}
	private:
		int frameCount_ = 0; // �t���[���J�E���g
		std::vector<std::string> droppedFiles_; // �h���b�v���ꂽ�t�@�C���̃��X�g
		bool droppedThisFrame_ = false; // ���̃t���[���Ńh���b�v���ꂽ���ǂ���
	};
}