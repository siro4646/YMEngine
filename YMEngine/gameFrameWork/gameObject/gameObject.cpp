
#include "gameObject.h"
#include "gameFrameWork/components/component.h"
#include "gameFrameWork/gameObject/gameObjectManager.h"


namespace ym
{
	std::shared_ptr<Object> Object::Clone()
	{
		auto copy = std::make_shared<Object>(*this); // �R�s�[�R���X�g���N�^���g�p
		// �q�I�u�W�F�N�g�͐[���R�s�[���s��
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}
	bool Object::AddChild(std::shared_ptr<Object> child)
	{
		if (!child || child.get() == this) return false;

		// �z�Q�ƃ`�F�b�N��ǉ�
		auto ancestor = shared_from_this();
		while (ancestor) {
			if (ancestor == child) {
				return false; // �z�Q�Ƃ��������邽�ߒǉ����Ȃ�
			}
			ancestor = ancestor->_parent;
		}

		if (child->_parent) {
			child->_parent->DeleteChild(child); // �����̐e����폜
		}

		child->SetParent(shared_from_this()); // �V�����e��ݒ�
		_childs.push_back(child);
		return true;
	}

	bool Object::DeleteChild(const std::shared_ptr<Object> &child)
	{
		if (!child) return false;

		auto it = std::find(_childs.begin(), _childs.end(), child);
		if (it != _childs.end()) {
			child->SetParent(nullptr); // �e�����N���A
			_childs.erase(it);
			return true;
		}
		return false;
	}
	void Object::UpdateHierarchy() {
		//�e�����邩
		if (_parent != nullptr)
		{
			worldTransform = localTransform.GetWorldTransform(_parent->worldTransform);
		}
		else
		{
			worldTransform = localTransform;
		}
		
	}

	void Object::AddComponent(std::unique_ptr<Component> component)
	{
		if (!component)
			return;

		component->Init();
		if (component->isFailed)
		{
			ym::ConsoleLog("Component %s initialization failed.\n", typeid(*component).name());
			return;
		}

		// unique_ptr ���� shared_ptr �ɕϊ����ď��L�����ڂ�
		std::shared_ptr<Component> shared = std::move(component);
		components.push_back(shared);
	}


	std::shared_ptr<Object> Object::GetParent() const {
		return _parent;
	}

	void Object::SetParent(std::shared_ptr<Object> newParent) {
		if (_parent != newParent) {

			UpdateHierarchy();
			// 1. �ύX�O�̃��[���h�s���ۑ�
			XMMATRIX worldMatrix = worldTransform.GetMatrix();

			_parent = newParent;

			//���[�J�����W���Čv�Z
			XMMATRIX parentMatrix = newParent ? newParent->worldTransform.GetMatrix() : XMMatrixIdentity();
			XMMATRIX localMatrix = XMMatrixMultiply(worldMatrix, XMMatrixInverse(nullptr, parentMatrix));
			localTransform = Transform::FromMatrix(localMatrix);

			//NotifyUpdate(); // �e�ύX���ɍX�V�ʒm
			UpdateHierarchy();
		}
	}
	Object::Object():_parent(nullptr)
	{
	}

	Object::~Object()
	{
		ym::ConsoleLog("[%s]�폜 \n", name.c_str());
		//�Q�Ƃ��������Ȃ̂ō폜���Ȃ�
		objectManager = nullptr;
		// ���g���f�X�g���N�^�ō폜�����Ƃ��A�q�I�u�W�F�N�g���폜
		for (auto &child : _childs) {
			child->SetParent(nullptr);
		}
		_childs.clear();
	}

	void Object::Init()
	{
		name = "Object";
		localTransform.SetObject(this);
		worldTransform.SetObject(this);
	}

	void Object::FixedUpdate()
	{
		if (!isActive)return;

		//UpdateHierarchy();
		//�q���̌Œ�X�V
		for (auto &child : _childs)
		{
			child->FixedUpdate();
		}
		//�R���|�[�l���g�̌Œ�X�V
		for (auto &component : components)
		{
			if (!component->isEnabled)continue; // �R���|�[�l���g�������ȏꍇ�͍X�V���Ȃ�
			component->FixedUpdate();
		}
	}

	void Object::Update()
	{
		if (!isActive)return;
		UpdateHierarchy();
		//�q���̍X�V
		for (auto &child : _childs)
		{
			child->Update();
		}
		//�R���|�[�l���g�̍X�V
		for (auto &component : components)
		{
			if (!component->isEnabled)continue; // �R���|�[�l���g�������ȏꍇ�͍X�V���Ȃ�
			component->Update();
		}
		//NotifyUpdate();
		//NotifyGlobalUpdate();
		//NotifyGlobalUpdate();

	}

	void Object::Draw()
	{
		if (!isActive)return;
			// �q�����l�ɕ\��
		for (auto &child : _childs)
		{
			child->Draw(); // �q�� ImGui �\���{�`�悳���
		}
		//�R���|�[�l���g�̕`��
		for (auto &component : components)
		{
			if (!component->isEnabled)continue; // �R���|�[�l���g�������ȏꍇ�͕`�悵�Ȃ�
			component->Draw();
		}

	}

	void Object::DrawImgui()
	{
		std::string treeId = ym::MakeImGuiID(name.c_str(), this);

		ImGui::PushStyleColor(ImGuiCol_Text, isActive ?
			ImVec4(0.2f, 1.0f, 0.2f, 1.0f) : // �A�N�e�B�u�Ȃ��
			ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // ��A�N�e�B�u�Ȃ�O���[

		bool open = ImGui::TreeNode(treeId.c_str());

		ImGui::PopStyleColor(); // �� �����ŕK���߂�

		if (open)
		{
			//�A�N�e�B�u��Ԃ̐؂�ւ�
			ImGui::Checkbox("Active", &isActive);

			// ���[�J�����W
			Vector3 &pos = localTransform.Position;
			Vector3 &rot = localTransform.Rotation;
			Vector3 &scl = localTransform.Scale;

			// ImGui�ŃX���C�_�[�{���ړ��͂ł���t�B�[���h��\��
			ImGui::Text("Transform");

			// �ʒu
			ImGui::PushID("Position");
			ImGui::Text("Position:");
			ImGui::DragFloat3("##Position", &pos.x, 0.1f, -1000.0f, 1000.0f, "%.2f");
			ImGui::PopID();

			// ��]�i�x���\�����~������Εϊ����K�v�j
			ImGui::PushID("Rotation");
			ImGui::Text("Rotation:");
			ImGui::DragFloat3("##Rotation", &rot.x, 1.0f, -360.0f, 360.0f, "%.1f");
			ImGui::PopID();

			// �g��
			ImGui::PushID("Scale");
			ImGui::Text("Scale:");
			ImGui::DragFloat3("##Scale", &scl.x, 0.1f, -100.0f, 100.0f, "%.2f");
			ImGui::PopID();

			ImGui::Separator(); // ��؂��

			// �R���|�[�l���g�̕\��
			ImGui::Text("Components:");
			for (auto &component : components)
			{
				if (component)
				{
					component->DrawImgui(); // �R���|�[�l���g��ImGui�\��
				}
			}

			ImGui::Separator();
			ImGui::Text("Add Component:");

			// ��ԕێ��p static �ϐ�
			static char searchBuffer[64] = "";
			static std::string selectedComponent;

			// ?? Enter�L�[��������t���̌������͗�
			bool enterPressed = ImGui::InputText(
				"##SearchComponent",
				searchBuffer,
				IM_ARRAYSIZE(searchBuffer),
				ImGuiInputTextFlags_EnterReturnsTrue
			);

			// ?? ���͂��ꂽ��������������ɕϊ��i�����p�t�B���^�[�j
			std::string filterStr(searchBuffer);
			std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), ::tolower);

			// ?? ��⒊�o�F�d�����O + �������t�B���^��v
			std::vector<std::string> filteredCandidates;
			for (const auto &compName : ym::ComponentRegistry::GetRegisteredNames())
			{
				// ���łɒǉ��ς݂̃R���|�[�l���g�����O
				bool alreadyAdded = false;
				for (const auto &c : components)
				{
					if (c && std::string(c->GetName()) == compName)
					{
						alreadyAdded = true;
						break;
					}
				}
				if (alreadyAdded)
					continue;

				// �����t�B���^�[�Ɉ�v���邩�i������������v�j
				std::string compNameLower = compName;
				std::transform(compNameLower.begin(), compNameLower.end(), compNameLower.begin(), ::tolower);

				if (filterStr.empty() || compNameLower.find(filterStr) != std::string::npos)
				{
					filteredCandidates.push_back(compName);
				}
			}

			// ?? ��⃊�X�g�\���iBeginCombo�`���j
			if (ImGui::BeginCombo("##ComponentList", selectedComponent.empty() ? "Select Component" : selectedComponent.c_str()))
			{
				for (const auto &name : filteredCandidates)
				{
					bool isSelected = (selectedComponent == name);
					if (ImGui::Selectable(name.c_str(), isSelected))
					{
						selectedComponent = name;
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			// ?? Enter�L�[�������ꂽ�玩��AddComponent�i1��� or ���S��v�j
			if (enterPressed)
			{
				std::string matchedName;

				if (filteredCandidates.size() == 1)
				{
					matchedName = filteredCandidates[0]; // �����I��
				}
				else
				{
					for (const auto &name : filteredCandidates)
					{
						if (name == searchBuffer)
						{
							matchedName = name;
							break;
						}
					}
				}

				if (!matchedName.empty())
				{
					auto comp = ym::ComponentRegistry::Create(matchedName, this);
					this->AddComponent(std::move(comp));

					// ���͏�ԃN���A
					selectedComponent.clear();
					memset(searchBuffer, 0, sizeof(searchBuffer));
				}
			}

			// ?? �蓮Add�{�^���ł��ǉ��\
			if (!selectedComponent.empty())
			{
				ImGui::SameLine();
				if (ImGui::Button("Add"))
				{
					auto comp = ym::ComponentRegistry::Create(selectedComponent, this);
					this->AddComponent(std::move(comp));

					// ���͏�ԃN���A
					selectedComponent.clear();
					memset(searchBuffer, 0, sizeof(searchBuffer));
				}
			}
			ImGui::Separator(); // ��؂��

			// �q���\��
			for (auto &child : _childs)
			{
				child->DrawImgui(); // �ċA�Ăяo��
			}
			ImGui::TreePop();
		}
	}

	void Object::DrawInspector()
	{
		std::string treeId = ym::MakeImGuiID(name.c_str(), this);
		// ���O�ҏW�p�̓��͗�
		ImGui::Text("Name:");

		ImGui::PushStyleColor(ImGuiCol_Text, isActive ?
			ImVec4(0.2f, 1.0f, 0.2f, 1.0f) : // �A�N�e�B�u�Ȃ��
			ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // ��A�N�e�B�u�Ȃ�O���[
		
		// ���O�ύX�p�̃o�b�t�@�i���񏉊����j
		char nameBuffer[64] = {};
		strncpy(nameBuffer, name.c_str(), sizeof(nameBuffer));

		ImGui::SameLine();
		if (ImGui::InputText("##ObjectName", nameBuffer, IM_ARRAYSIZE(nameBuffer),
			ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			name = nameBuffer; // �G���^�[�Ŕ��f
		}

		ImGui::PopStyleColor(); // �� �����ŕK���߂�

		
			//�A�N�e�B�u��Ԃ̐؂�ւ�
			ImGui::Checkbox("Active", &isActive);

			// ���[�J�����W
			Vector3 &pos = localTransform.Position;
			Vector3 &rot = localTransform.Rotation;
			Vector3 &scl = localTransform.Scale;

			// ImGui�ŃX���C�_�[�{���ړ��͂ł���t�B�[���h��\��
			ImGui::Text("Transform");

			// �ʒu
			ImGui::PushID("Position");
			ImGui::Text("Position:");
			ImGui::DragFloat3("##Position", &pos.x, 0.1f, -1000.0f, 1000.0f, "%.2f");
			ImGui::PopID();

			// ��]�i�x���\�����~������Εϊ����K�v�j
			ImGui::PushID("Rotation");
			ImGui::Text("Rotation:");
			ImGui::DragFloat3("##Rotation", &rot.x, 1.0f, -360.0f, 360.0f, "%.1f");
			ImGui::PopID();

			// �g��
			// ----------------- �X�P�[�� UI -----------------
			static bool   uniformScale = false;
			static Vector3 baseScale = scl;   // �ҏW�J�n���̊�X�P�[��
			static Vector3 displayScale = scl;   // UI �ɕ\������l

			ImGui::Checkbox("Uniform Scale Edit", &uniformScale);
			ImGui::PushID("ScaleUI");
			ImGui::Text("Scale:");

			if (uniformScale)
			{
				// �� �ҏW�t�B�[���h
				Vector3 input = displayScale;

				// DragFloat3 + ��ł����Ή�
				if (ImGui::DragFloat3("##UScale", &input.x, 0.1f, -100.0f, 100.0f, "%.2f"))
				{
					// �ǂ̎����ҏW���ꂽ�������o�i�O��l�Ƃ̍����Ŕ���j
					Vector3 diff = input - displayScale;
					float   factor = 1.0f;

					if (std::abs(diff.x) > 1e-4f)          factor = input.x / baseScale.x;
					else if (std::abs(diff.y) > 1e-4f)     factor = input.y / baseScale.y;
					else if (std::abs(diff.z) > 1e-4f)     factor = input.z / baseScale.z;

					// �A���g�k
					scl = baseScale * factor;

					// UI �\���l���X�V
					displayScale = baseScale * factor;
					input = displayScale;           // �������̃t���[���Ŕ��f�����
				}

				/*�\�\ �ҏW�J�n�����o���Ċ�X�P�[�����L�^ �\�\*/
				if (ImGui::IsItemActivated()) {            // �ŏ��ɃN���b�N or �t�H�[�J�X�����u��
					baseScale = scl;                    // ���̔䗦��ێ�
					displayScale = scl;
				}

				/*�\�\ �ҏW�I���iEnter �m�� or �t�H�[�J�X�O���j�Ńt���O���� �\�\*/
				if (ImGui::IsItemDeactivatedAfterEdit()) {
					baseScale = scl;                    // �m�肵���l������̊��
					displayScale = scl;
				}
			}
			else
			{
				ImGui::DragFloat3("##Scale", &scl.x, 0.1f, -100.0f, 100.0f, "%.2f");
				// Uniform ���[�h�𔲂�����\���p�����Z�b�g
				baseScale = scl;
				displayScale = scl;
			}

			ImGui::PopID();



			ImGui::Separator(); // ��؂��

			ImGui::Text("Components:");
			for (size_t i = 0; i < components.size(); ++i)
			{
				auto &comp = components[i];
				if (!comp) continue;

				ImGui::PushID(static_cast<int>(i));
				if (ImGui::TreeNode(comp->GetName()))
				{
					comp->DrawImgui();  // �R���|�[�l���g�ŗLUI

					ImGui::Separator();
					if (ImGui::Button("Remove Component"))
					{
						components.erase(components.begin() + i);
						--i; // ���̃��[�v�ł������ i �Ԗڂ�����
						ImGui::TreePop();
						ImGui::PopID();
						continue;
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}


			ImGui::Separator();
			ImGui::Text("Add Component:");

			// ��ԕێ��p static �ϐ�
			static char searchBuffer[64] = "";
			static std::string selectedComponent;

			// ?? Enter�L�[��������t���̌������͗�
			bool enterPressed = ImGui::InputText(
				"##SearchComponent",
				searchBuffer,
				IM_ARRAYSIZE(searchBuffer),
				ImGuiInputTextFlags_EnterReturnsTrue
			);

			// ?? ���͂��ꂽ��������������ɕϊ��i�����p�t�B���^�[�j
			std::string filterStr(searchBuffer);
			std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), ::tolower);

			// ?? ��⒊�o�F�d�����O + �������t�B���^��v
			std::vector<std::string> filteredCandidates;
			for (const auto &compName : ym::ComponentRegistry::GetRegisteredNames())
			{
				// ���łɒǉ��ς݂̃R���|�[�l���g�����O
				bool alreadyAdded = false;
				for (const auto &c : components)
				{
					if (c && std::string(c->GetName()) == compName)
					{
						alreadyAdded = true;
						break;
					}
				}
				if (alreadyAdded)
					continue;

				// �����t�B���^�[�Ɉ�v���邩�i������������v�j
				std::string compNameLower = compName;
				std::transform(compNameLower.begin(), compNameLower.end(), compNameLower.begin(), ::tolower);

				if (filterStr.empty() || compNameLower.find(filterStr) != std::string::npos)
				{
					filteredCandidates.push_back(compName);
				}
			}

			// ?? ��⃊�X�g�\���iBeginCombo�`���j
			if (ImGui::BeginCombo("##ComponentList", selectedComponent.empty() ? "Select Component" : selectedComponent.c_str()))
			{
				for (const auto &name : filteredCandidates)
				{
					bool isSelected = (selectedComponent == name);
					if (ImGui::Selectable(name.c_str(), isSelected))
					{
						selectedComponent = name;
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			// ?? Enter�L�[�������ꂽ�玩��AddComponent�i1��� or ���S��v�j
			if (enterPressed)
			{
				std::string matchedName;

				if (filteredCandidates.size() == 1)
				{
					matchedName = filteredCandidates[0]; // �����I��
				}
				else
				{
					for (const auto &name : filteredCandidates)
					{
						if (name == searchBuffer)
						{
							matchedName = name;
							break;
						}
					}
				}

				if (!matchedName.empty())
				{
					auto comp = ym::ComponentRegistry::Create(matchedName, this);
					this->AddComponent(std::move(comp));

					// ���͏�ԃN���A
					selectedComponent.clear();
					memset(searchBuffer, 0, sizeof(searchBuffer));
				}
			}

			// ?? �蓮Add�{�^���ł��ǉ��\
			if (!selectedComponent.empty())
			{
				ImGui::SameLine();
				if (ImGui::Button("Add"))
				{
					auto comp = ym::ComponentRegistry::Create(selectedComponent, this);
					this->AddComponent(std::move(comp));

					// ���͏�ԃN���A
					selectedComponent.clear();
					memset(searchBuffer, 0, sizeof(searchBuffer));
				}
			}
			ImGui::Separator(); // ��؂��

			ImGui::Text("Performance:");

			float fps = ImGui::GetIO().Framerate;
			float updateTimeMs = 1000.0f / fps;

			ImGui::Text("FPS: %.1f", fps);
			ImGui::Text("Update Time: %.2f ms/frame", updateTimeMs);

		
	}

	void Object::Uninit()
	{
		DebugLog("Object %s Uninit\n", name.c_str());
		if (!isUninit)
		{
			isUninit = true;
		}
		else
		{
			return;
		}
		//�q���̉��
		for (auto &child : _childs)
		{
			child->Uninit();
		}
		_childs.clear();
		//�R���|�[�l���g�̉��
		for (auto &component : components)
		{
			component->Uninit();
		}
		components.clear();
	}
	std::vector<std::unique_ptr<IPropertyProvider>> Object::CollectPropertyProviders()
	{
		std::vector<std::unique_ptr<IPropertyProvider>> list;

		// �@ Transform �͓��ʈ���
		list.emplace_back(std::make_unique<TransformPropertyProvider>(&localTransform));

		// �A ���łɕt���Ă��� Component �� IPropertyProvider ���������Ă��镨��ǉ�
		for (auto &c : components) {
			if (auto p = dynamic_cast<IPropertyProvider *>(c.get())) {
				// Component ���̂� Object �����L���Ă���̂� raw �|�C���^�Ń��b�v
				struct Wrapper : IPropertyProvider {
					IPropertyProvider *p; Wrapper(IPropertyProvider *ptr) : p(ptr) {}
					std::string GetProviderName() const override { return p->GetProviderName(); }
					std::vector<PropertyInfo> GetProperties() override { return p->GetProperties(); }
				};
				list.emplace_back(std::make_unique<Wrapper>(p));
			}
		}
		return list;
	}

} // namespace ym
