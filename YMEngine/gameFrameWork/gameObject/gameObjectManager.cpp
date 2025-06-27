#include "gameObjectManager.h"
#include "gameObject.h"

#include <algorithm>
#include <mutex>
#include <execution> // ������s�|���V�[�𗘗p����ꍇ
#include <thread>

#include "../system/threadPool/threadPool.h"

#include "gameFrameWork/camera/cameraManager.h"
#include "gameFrameWork/camera/camera.h"
#include "gameFrameWork/light/pointLight.h"

#include "imgui/imguiRender.h"

#include "components/animation/editorWindow/animationEditorWindow.h"
#include "components/animation/animator/animator.h"

namespace ym
{

	GameObjectManager::GameObjectManager()
	{

	}

	GameObjectManager::~GameObjectManager()
	{

	}

	void GameObjectManager::Init()
	{
	}


	//void GameObjectManager::FixedUpdate() {

	//	//std::vector<std::shared_ptr<Object>> objectsToDelete;
	//	//std::mutex deleteMutex;
	//	//std::mutex fixedUpdateMutex;

	//	//// ����� FixedUpdate �����s
	//	//std::for_each(std::execution::par, gameObjects_.begin(), gameObjects_.end(), [&](auto &gameObject) {
	//	//	if (gameObject->type == Object::Type::Delete) {
	//	//		std::lock_guard<std::mutex> lock(deleteMutex); // �폜���X�g�ւ̃A�N�Z�X��ی�
	//	//		objectsToDelete.push_back(gameObject);
	//	//	}
	//	//	else {
	//	//		std::lock_guard<std::mutex> lock(fixedUpdateMutex);// FixedUpdate�̃A�N�Z�X��ی�
	//	//		gameObject->FixedUpdate();
	//	//	}
	//	//	});

	//	//// �폜�����̓V���O���X���b�h�Ŏ��s
	//	//for (auto &gameObject : objectsToDelete) {
	//	//	gameObject->Uninit();
	//	//	std::lock_guard<std::mutex> lock(deleteMutex);
	//	//	gameObjects_.erase(std::remove(gameObjects_.begin(), gameObjects_.end(), gameObject), gameObjects_.end());
	//	//}

	//	

	//	std::vector<std::shared_ptr<Object>> objectsToDelete;
	//	
	//	for (auto obj : gameObjects_)
	//	{
	//		if (!obj)continue;

	//		if (obj->type == Object::Type::Delete)
	//		{
	//			objectsToDelete.push_back(obj);
	//		}
	//		else
	//		{
	//			obj->FixedUpdate();
	//		}
	//	}

	//	for (auto deleteObj : objectsToDelete)
	//	{
	//		//��������

	//		deleteObj->Uninit();

	//		gameObjects_.erase(std::remove(gameObjects_.begin(), gameObjects_.end(), deleteObj), gameObjects_.end());

	//	}

	//	//for (auto deleteObj : objectsToDelete)
	//	//{
	//	//	//��������

	//	//}
	//	
	//}

	void GameObjectManager::FixedUpdate() {
		std::vector<std::shared_ptr<Object>> objectsToDelete;
		std::mutex deleteMutex;

		std::vector<std::future<void>> futures;
		for (auto obj : gameObjects_)
		{
			if (!obj) continue;

			auto future = ThreadPool::Instance().enqueue([&, obj]() {
				if (obj->type == Object::Type::Delete)
				{
					std::lock_guard<std::mutex> lock(deleteMutex);
					objectsToDelete.push_back(obj);
				}
				else
				{
					obj->FixedUpdate();
				}
				});

			futures.push_back(std::move(future));
		}

		for (auto &future : futures)
		{
			future.get();
		}

		for (auto deleteObj : objectsToDelete)
		{
			deleteObj->Uninit();

			gameObjects_.erase(std::remove(gameObjects_.begin(), gameObjects_.end(), deleteObj), gameObjects_.end());
		}
	}



	//void GameObjectManager::Update() {
	//	//std::vector<std::shared_ptr<Object>> objectsToDelete;
	//	//std::mutex deleteMutex;
	//	//std::mutex updateMutex;

	//	////addObjects_�̒��g��gameObjects_�Ɉڂ�

	//	//for (auto obj : addObjects_)
	//	//{
	//	//	gameObjects_.push_back(obj);
	//	//	//obj->Init();
	//	//}

	//	//// ����� Update �����s
	//	//std::for_each(std::execution::par, gameObjects_.begin(), gameObjects_.end(), [&](auto &gameObject) {
	//	//	if (gameObject->type == Object::Type::Delete) {
	//	//		std::lock_guard<std::mutex> lock(deleteMutex); // �폜���X�g�ւ̃A�N�Z�X��ی�
	//	//		objectsToDelete.push_back(gameObject);
	//	//	}
	//	//	else {
	//	//		std::lock_guard<std::mutex> lock(updateMutex);// Update�̃A�N�Z�X��ی�
	//	//		gameObject->Update();
	//	//	}
	//	//	});

	//	//// �폜�����̓V���O���X���b�h�Ŏ��s
	//	//for (auto &gameObject : objectsToDelete) {
	//	//	gameObject->Uninit();
	//	//	std::lock_guard<std::mutex> lock(deleteMutex);
	//	//	gameObjects_.erase(std::remove(gameObjects_.begin(), gameObjects_.end(), gameObject), gameObjects_.end());
	//	//}

	//	//addObjects_�̒��g��gameObjects_�Ɉڂ�

	//	for (auto obj : addObjects_)
	//	{
	//		gameObjects_.push_back(obj);
	//		//obj->Init();
	//	}
	//	addObjects_.clear();
	//	std::vector<std::shared_ptr<Object>> objectsToDelete;

	//	for (auto obj : gameObjects_)
	//	{
	//		if (!obj)continue;
	//		if (obj->type == Object::Type::Delete)
	//		{
	//			objectsToDelete.push_back(obj);
	//		}
	//		else
	//		{
	//			obj->Update();
	//		}
	//	}

	//	for (auto deleteObj : objectsToDelete)
	//	{
	//		//��������

	//		deleteObj->Uninit();

	//		gameObjects_.erase(std::remove(gameObjects_.begin(), gameObjects_.end(), deleteObj), gameObjects_.end());

	//	}


	//	//for (auto deleteObj : objectsToDelete)
	//	//{
	//	//	//��������

	//	//}
	//}

	void GameObjectManager::Update() {
		// AddObjects�̈ړ�
		for (auto obj : addObjects_)
		{
			gameObjects_.push_back(obj);
		}
		addObjects_.clear();

		std::vector<std::shared_ptr<Object>> objectsToDelete;
		std::mutex deleteMutex;

		// ThreadPool �Ƀ^�X�N�𓊂���
		std::vector<std::future<void>> futures;
		for (auto obj : gameObjects_)
		{
			if (!obj) continue;

			auto future = ThreadPool::Instance().enqueue([&, obj]() {
				if (obj->type == Object::Type::Delete)
				{
					std::lock_guard<std::mutex> lock(deleteMutex);
					objectsToDelete.push_back(obj);
				}
				else
				{
					obj->Update();
				}
				});

			futures.push_back(std::move(future));
		}

		// �S�^�X�N�����҂�
		for (auto &future : futures)
		{
			future.get();
		}

		// �폜�����i���C���X���b�h�ōs���j
		for (auto deleteObj : objectsToDelete)
		{
			deleteObj->Uninit();

			gameObjects_.erase(std::remove(gameObjects_.begin(), gameObjects_.end(), deleteObj), gameObjects_.end());
		}
	}


	void GameObjectManager::Draw()
	{
		ImGui::Begin(u8"�V�[���̑���Ə��");
		ImGui::Text("YMEngine TestScene");
		//�}�E�X�J�[�\���̈ʒu���擾
		auto mousePos = ImGui::GetMousePos();
		ImGui::Text(u8"�}�E�X�J�[�\���̈ʒu: %.1f, %.1f", mousePos.x, mousePos.y);
		auto mainCamera = CameraManager::Instance().GetMainCamera();
		ImGui::Text(u8"�J�����̈ʒu %f %f %f", mainCamera->GetEye().x, mainCamera->GetEye().y, mainCamera->GetEye().z);
		ImGui::Text("Framerate(avg) %.3f ms/frame (%.If FPS)",
			1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text(u8"�g�p�X���b�h��/�S�̂̃X���b�h��: %d / %d", ym::ThreadPool::Instance().GetBusyThreadCount(), ym::ThreadPool::Instance().GetThreadCount());

		ImGui::Separator(); // ��؂��
		//ImGui::Text("PointLight");
		//if (ImGui::Button("Add PointLight"))
		//{
		//	auto mainCamera = CameraManager::Instance().GetMainCamera();
		//	auto obj = AddGameObject(std::make_shared<ym::PointLight>());
		//	obj->localTransform.Position.x = mainCamera->GetEye().x;
		//	obj->localTransform.Position.y = mainCamera->GetEye().y;
		//	obj->localTransform.Position.z = mainCamera->GetEye().z;
		//}
		//ImGui::Separator(); // ��؂��
		//auto lights = FindGameObjects<ym::PointLight>();

		//// 4. ���ꂼ��̃��C�g�ɑ΂��� UI ���i���쐬
		//for (int i = 0; i < (int)lights.size(); ++i)
		//{
		//	auto &light = lights[i];
		//	ImGui::PushID(i); // �d�����Ȃ�����ID��ݒ�

		//	// 4-2. ���C�g�p�����[�^�̒���
		//	auto &d = light->GetData();  // ��قǗp�ӂ����A�N�Z�T�œ����f�[�^�����o��

		//	// �ʒu (X,Y,Z) ���X���C�_�[�ő���
		//	float position[3];
		//	position[0] = d.position.x;
		//	position[1] = d.position.y;
		//	position[2] = d.position.z;
		//	ImGui::SliderFloat3("Position", position, -5.0f, 5.0f);
		//	// �ʒu���X�V

		//	d.position.x = position[0];
		//	d.position.y = position[1];
		//	d.position.z = position[2];
		//	light->localTransform.Position.x = d.position.x; // Transform �ɔ��f
		//	light->localTransform.Position.y = d.position.y;
		//	light->localTransform.Position.z = d.position.z;
		//	// �F (RGB) ���J���[�s�b�J�[�ő���
		//	// ColorEdit3 �� ImGui �̑g�ݍ��݂ŁAfloat[3] �� 0.0�`1.0 �ň����܂�
		//	ImGui::ColorEdit3("Color", &d.color.x);
		//	// ���x�Ɖe���͈�
		//	ImGui::SliderFloat("Intensity", &d.intensity, 0.0f, 100.0f);
		//	ImGui::SliderFloat("Radius", &d.radius, 0.0f, 10.0f);

		//	ImGui::PopID();
		//	ImGui::Separator();
		//}
		ImGui::End();

		DrawHierarchy();

		//// --- �q�G�����L�[�r���[ ---
		//ImGui::Begin("Hierarchy");

		//static Object *selectedObject = nullptr;

		//for (auto &obj : gameObjects_)
		//{
		//	if (!obj || obj->type == Object::Type::Delete)
		//		continue;

		//	// �n�C���C�g�t���őI���\��
		//	bool isSelected = (selectedObject == obj.get());
		//	if (ImGui::Selectable(obj->name.c_str(), isSelected))
		//	{
		//		selectedObject = obj.get(); // �C���X�y�N�^�[�\���Ώۂɐݒ�
		//	}
		//}
		//ImGui::End();

		//// --- �C���X�y�N�^�[�r���[ ---
		//ImGui::Begin("Inspector");

		//if (selectedObject)
		//{
		//	selectedObject->DrawInspector(); // �V������`����֐�
		//}

		//ImGui::End();



		std::mutex drawMutex;
		// ����� Draw �����s
		std::for_each(std::execution::par, gameObjects_.begin(), gameObjects_.end(), [&](auto &gameObject) {
			if (gameObject->type != Object::Type::Delete) {
				std::lock_guard<std::mutex> lock(drawMutex);// Update�̃A�N�Z�X��ی�
				gameObject->Draw();				
			}
			});

		/*for (auto &gameObject : gameObjects_)
		{
			if (gameObject->type != Object::Type::Delete)
			gameObject->Draw();
		}*/
	}

	void GameObjectManager::Uninit()
	{
		gameObjects_.clear();
		addObjects_.clear();


	}

	std::shared_ptr<Object> GameObjectManager::AddGameObject(std::shared_ptr<Object> gameObject, Object *const parent)
	{		
		if (parent != nullptr)
		{
			parent->AddChild(gameObject);
		}
		addObjects_.push_back(gameObject);
		gameObject->objectManager = this;
		gameObject->Init();
		return gameObject;
	}

	void GameObjectManager::RemoveGameObject(std::shared_ptr<Object> gameObject)
	{
	}

	std::vector<std::shared_ptr<Object>> GameObjectManager::FindGameObjects(const std::string &name)
	{
		
		ym::ConsoleLog("FindGameObjects�֐����Ă΂�܂���:%s",name);

		// ��v����I�u�W�F�N�g���i�[����z��
		std::vector<std::shared_ptr<Object>> objects;

		// gameObjects_���󂩊m�F
		if (gameObjects_.empty()) {
			ym::ConsoleLog("gameObjects_�͋�ł��B");
			return objects;
		}

		// �e�I�u�W�F�N�g���m�F
		for (const auto &gameObject : gameObjects_) {
			if (!gameObject) { // null�`�F�b�N
				ym::ConsoleLog("�x��: gameObjects_��null�̃I�u�W�F�N�g������܂��B");
				continue;
			}

			if (gameObject->name == name) { // ���O�̔�r
				objects.push_back(gameObject);
			}
		}

		ym::ConsoleLog("��v����I�u�W�F�N�g��:&s",std::to_string(objects.size()));
		return objects;
	}

	std::shared_ptr<Object> GameObjectManager::GetSharedPtrFromRaw(Object *raw)
	{
		for (auto &obj : gameObjects_)
		{
			if (obj.get() == raw)
				return obj;

			// �q���ċA�I�ɒT��
			auto found = SearchInChildren(obj, raw);
			if (found) return found;
		}
		return nullptr;
	}

	std::shared_ptr<Object> GameObjectManager::SearchInChildren(std::shared_ptr<Object> &parent, Object *raw)
	{
		for (auto &child : parent->_childs)
		{
			if (child.get() == raw)
				return child;

			auto found = SearchInChildren(child, raw);
			if (found) return found;
		}
		return nullptr;
	}


	void GameObjectManager::DrawHierarchy()
	{
		static std::vector<Object *> selected;
		static Object *openOnNextFrame = nullptr;
		static Object *lastClicked = nullptr;

		// �I������Ă��邩����
		auto IsSelected = [&](Object *o) {
			return std::find(selected.begin(), selected.end(), o) != selected.end();
			};

		// �I�u�W�F�N�g�̃t���b�g���X�g�쐬�iShift�͈͑I��p�j
		std::vector<Object *> flatList;
		std::function<void(Object *)> Flatten = [&](Object *o) {
			flatList.push_back(o);
			for (auto &c : o->_childs)
				if (c) Flatten(c.get());
			};
		for (auto &obj : gameObjects_)
			if (obj && !obj->GetParent())
				Flatten(obj.get());

		ImGui::Begin("Hierarchy");

		std::function<void(Object *)> DrawObjectNode = [&](Object *obj)
			{
				ImGuiTreeNodeFlags flags =
					(IsSelected(obj) ? ImGuiTreeNodeFlags_Selected : 0) |
					(obj->_childs.empty() ? ImGuiTreeNodeFlags_Leaf : 0) |
					ImGuiTreeNodeFlags_OpenOnArrow;

				if (obj == openOnNextFrame) {
					ImGui::SetNextItemOpen(true, ImGuiCond_Always);
					openOnNextFrame = nullptr;
				}

				ImGui::PushID(obj);
				bool open = ImGui::TreeNodeEx(obj->name.c_str(), flags);
				ImGui::PopID();

				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					bool ctrl = ImGui::GetIO().KeyCtrl;
					bool shift = ImGui::GetIO().KeyShift;

					if (!ctrl && !shift)
						selected.clear();

					if (shift && lastClicked)
					{
						auto itStart = std::find(flatList.begin(), flatList.end(), lastClicked);
						auto itEnd = std::find(flatList.begin(), flatList.end(), obj);
						if (itStart != flatList.end() && itEnd != flatList.end())
						{
							if (itStart > itEnd) std::swap(itStart, itEnd);
							for (auto it = itStart; it <= itEnd; ++it)
								if (!IsSelected(*it))
									selected.push_back(*it);
						}
					}
					else if (ctrl && IsSelected(obj))
					{
						selected.erase(std::remove(selected.begin(), selected.end(), obj), selected.end());
					}
					else
					{
						selected.push_back(obj);
					}
					lastClicked = obj;
				}

				// �h���b�O�\�[�X
				if (ImGui::BeginDragDropSource())
				{
					if (!IsSelected(obj))
						selected = { obj };

					ImGui::SetDragDropPayload("HIERARCHY_OBJECTS", selected.data(), sizeof(Object *) * selected.size());
					ImGui::Text("%zu objects", selected.size());
					ImGui::EndDragDropSource();
				}

				// �h���b�v�^�[�Q�b�g
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload *p = ImGui::AcceptDragDropPayload("HIERARCHY_OBJECTS"))
					{
						auto *rawPtrArray = static_cast<Object **>(p->Data);
						size_t count = p->DataSize / sizeof(Object *);

						for (size_t i = 0; i < count; ++i)
						{
							if (auto dropped = rawPtrArray[i]->GetSharedPtr())
								if (dropped.get() != obj)
									obj->AddChild(dropped);
						}
						openOnNextFrame = obj;
					}
					ImGui::EndDragDropTarget();
				}

				if (open)
				{
					for (auto &child : obj->_childs)
						if (child) DrawObjectNode(child.get());
					ImGui::TreePop();
				}
			};

		// �c���[�`��i���[�g�̂݁j
		for (auto &obj : gameObjects_)
			if (obj && !obj->GetParent())
				DrawObjectNode(obj.get());

		ImGui::Separator();

		// --- Add �{�^�� ---
		if (ImGui::Button("Add GameObject"))
		{
			auto newObj = std::make_shared<Object>();
			newObj->name = "NewObject";
			AddGameObject(newObj, nullptr);
		}

		ImGui::SameLine();

		// --- Delete ---
		if (ImGui::Button("Delete Selected"))
		{
			for (auto *obj : selected)
			{
				if (!obj) continue;
				if (!obj->GetParent())
				{
					gameObjects_.erase(std::remove_if(gameObjects_.begin(), gameObjects_.end(),
						[&](const std::shared_ptr<Object> &o) {
							if (o.get() == obj)
							{
								o->Uninit(); // �I�u�W�F�N�g�̉������
								return true; // �폜�Ώ�
							}
							return false; // �폜�Ώۂł͂Ȃ�
						}), gameObjects_.end());
				}
				else
				{
					auto parent = obj->GetParent();
					if (parent)
					{
						parent->_childs.erase(std::remove_if(parent->_childs.begin(), parent->_childs.end(),
							[&](const std::shared_ptr<Object> &o) {
								if (o.get() == obj)
								{
									o->Uninit(); // �I�u�W�F�N�g�̉������
									return true; // �폜�Ώ�
								}
								return false; // �폜�Ώۂł͂Ȃ�
							}), parent->_childs.end());
					}
				}
			}
		}

		// --- Unparent�i�����I��Ή��j ---
		for (auto *obj : selected)
		{
			if (!obj || !obj->GetParent()) continue;

			ImGui::SameLine();
			if (ImGui::Button("Unparent"))
			{
				obj->GetParent()->DeleteChild(obj->GetSharedPtr());
				obj->SetParent(nullptr);
			}
			break; // ��x�����{�^���\���i�����іh�~�j
		}

		ImGui::End();

		// --- Inspector �\�� ---
		if (selected.size() == 1)
		{
			ImGui::Begin("Inspector");
			selected.front()->DrawInspector();
			ImGui::End();
		}
		else if (!selected.empty())
		{
			ImGui::Begin("Inspector (Multi)");
			if (ImGui::BeginTabBar("MultiInspectorTabs"))
			{
				for (Object *o : selected)
				{
					if (ImGui::BeginTabItem(o->name.c_str()))
					{
						o->DrawInspector();
						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();
			}
			ImGui::End();
		}

		// --- Animation Editor ---
		static ym::AnimationEditorWindow animEditor;

		//if (selected.size() == 1)
		//{
		//	auto animator = selected.front()->GetComponent<ym::Animator>();
		//	if (animator)
		//	{
		//		animEditor.Draw(animator);
		//	}
		//}
	}

	//=================================	
	bool GameObjectManager::IsDescendantOf(Object *parent, Object *child) const {
		if (!parent) return false;
		for (const auto &c : parent->_childs) {
			if (c.get() == child) return true;
			if (IsDescendantOf(c.get(), child)) return true;
		}
		return false;
	}



}