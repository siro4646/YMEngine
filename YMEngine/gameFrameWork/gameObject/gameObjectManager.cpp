#include "gameObjectManager.h"
#include "gameObject.h"

#include <algorithm>
#include <mutex>
#include <execution> // 並列実行ポリシーを利用する場合
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

	//	//// 並列に FixedUpdate を実行
	//	//std::for_each(std::execution::par, gameObjects_.begin(), gameObjects_.end(), [&](auto &gameObject) {
	//	//	if (gameObject->type == Object::Type::Delete) {
	//	//		std::lock_guard<std::mutex> lock(deleteMutex); // 削除リストへのアクセスを保護
	//	//		objectsToDelete.push_back(gameObject);
	//	//	}
	//	//	else {
	//	//		std::lock_guard<std::mutex> lock(fixedUpdateMutex);// FixedUpdateのアクセスを保護
	//	//		gameObject->FixedUpdate();
	//	//	}
	//	//	});

	//	//// 削除処理はシングルスレッドで実行
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
	//		//消す処理

	//		deleteObj->Uninit();

	//		gameObjects_.erase(std::remove(gameObjects_.begin(), gameObjects_.end(), deleteObj), gameObjects_.end());

	//	}

	//	//for (auto deleteObj : objectsToDelete)
	//	//{
	//	//	//消す処理

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

	//	////addObjects_の中身をgameObjects_に移す

	//	//for (auto obj : addObjects_)
	//	//{
	//	//	gameObjects_.push_back(obj);
	//	//	//obj->Init();
	//	//}

	//	//// 並列に Update を実行
	//	//std::for_each(std::execution::par, gameObjects_.begin(), gameObjects_.end(), [&](auto &gameObject) {
	//	//	if (gameObject->type == Object::Type::Delete) {
	//	//		std::lock_guard<std::mutex> lock(deleteMutex); // 削除リストへのアクセスを保護
	//	//		objectsToDelete.push_back(gameObject);
	//	//	}
	//	//	else {
	//	//		std::lock_guard<std::mutex> lock(updateMutex);// Updateのアクセスを保護
	//	//		gameObject->Update();
	//	//	}
	//	//	});

	//	//// 削除処理はシングルスレッドで実行
	//	//for (auto &gameObject : objectsToDelete) {
	//	//	gameObject->Uninit();
	//	//	std::lock_guard<std::mutex> lock(deleteMutex);
	//	//	gameObjects_.erase(std::remove(gameObjects_.begin(), gameObjects_.end(), gameObject), gameObjects_.end());
	//	//}

	//	//addObjects_の中身をgameObjects_に移す

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
	//		//消す処理

	//		deleteObj->Uninit();

	//		gameObjects_.erase(std::remove(gameObjects_.begin(), gameObjects_.end(), deleteObj), gameObjects_.end());

	//	}


	//	//for (auto deleteObj : objectsToDelete)
	//	//{
	//	//	//消す処理

	//	//}
	//}

	void GameObjectManager::Update() {
		// AddObjectsの移動
		for (auto obj : addObjects_)
		{
			gameObjects_.push_back(obj);
		}
		addObjects_.clear();

		std::vector<std::shared_ptr<Object>> objectsToDelete;
		std::mutex deleteMutex;

		// ThreadPool にタスクを投げる
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

		// 全タスク完了待ち
		for (auto &future : futures)
		{
			future.get();
		}

		// 削除処理（メインスレッドで行う）
		for (auto deleteObj : objectsToDelete)
		{
			deleteObj->Uninit();

			gameObjects_.erase(std::remove(gameObjects_.begin(), gameObjects_.end(), deleteObj), gameObjects_.end());
		}
	}


	void GameObjectManager::Draw()
	{
		ImGui::Begin(u8"シーンの操作と情報");
		ImGui::Text("YMEngine TestScene");
		//マウスカーソルの位置を取得
		auto mousePos = ImGui::GetMousePos();
		ImGui::Text(u8"マウスカーソルの位置: %.1f, %.1f", mousePos.x, mousePos.y);
		auto mainCamera = CameraManager::Instance().GetMainCamera();
		ImGui::Text(u8"カメラの位置 %f %f %f", mainCamera->GetEye().x, mainCamera->GetEye().y, mainCamera->GetEye().z);
		ImGui::Text("Framerate(avg) %.3f ms/frame (%.If FPS)",
			1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text(u8"使用スレッド数/全体のスレッド数: %d / %d", ym::ThreadPool::Instance().GetBusyThreadCount(), ym::ThreadPool::Instance().GetThreadCount());

		ImGui::Separator(); // 区切り線
		//ImGui::Text("PointLight");
		//if (ImGui::Button("Add PointLight"))
		//{
		//	auto mainCamera = CameraManager::Instance().GetMainCamera();
		//	auto obj = AddGameObject(std::make_shared<ym::PointLight>());
		//	obj->localTransform.Position.x = mainCamera->GetEye().x;
		//	obj->localTransform.Position.y = mainCamera->GetEye().y;
		//	obj->localTransform.Position.z = mainCamera->GetEye().z;
		//}
		//ImGui::Separator(); // 区切り線
		//auto lights = FindGameObjects<ym::PointLight>();

		//// 4. それぞれのライトに対して UI 部品を作成
		//for (int i = 0; i < (int)lights.size(); ++i)
		//{
		//	auto &light = lights[i];
		//	ImGui::PushID(i); // 重複しない内部IDを設定

		//	// 4-2. ライトパラメータの調整
		//	auto &d = light->GetData();  // 先ほど用意したアクセサで内部データを取り出す

		//	// 位置 (X,Y,Z) をスライダーで操作
		//	float position[3];
		//	position[0] = d.position.x;
		//	position[1] = d.position.y;
		//	position[2] = d.position.z;
		//	ImGui::SliderFloat3("Position", position, -5.0f, 5.0f);
		//	// 位置を更新

		//	d.position.x = position[0];
		//	d.position.y = position[1];
		//	d.position.z = position[2];
		//	light->localTransform.Position.x = d.position.x; // Transform に反映
		//	light->localTransform.Position.y = d.position.y;
		//	light->localTransform.Position.z = d.position.z;
		//	// 色 (RGB) をカラーピッカーで操作
		//	// ColorEdit3 は ImGui の組み込みで、float[3] を 0.0～1.0 で扱います
		//	ImGui::ColorEdit3("Color", &d.color.x);
		//	// 強度と影響範囲
		//	ImGui::SliderFloat("Intensity", &d.intensity, 0.0f, 100.0f);
		//	ImGui::SliderFloat("Radius", &d.radius, 0.0f, 10.0f);

		//	ImGui::PopID();
		//	ImGui::Separator();
		//}
		ImGui::End();

		DrawHierarchy();

		//// --- ヒエラルキービュー ---
		//ImGui::Begin("Hierarchy");

		//static Object *selectedObject = nullptr;

		//for (auto &obj : gameObjects_)
		//{
		//	if (!obj || obj->type == Object::Type::Delete)
		//		continue;

		//	// ハイライト付きで選択可能に
		//	bool isSelected = (selectedObject == obj.get());
		//	if (ImGui::Selectable(obj->name.c_str(), isSelected))
		//	{
		//		selectedObject = obj.get(); // インスペクター表示対象に設定
		//	}
		//}
		//ImGui::End();

		//// --- インスペクタービュー ---
		//ImGui::Begin("Inspector");

		//if (selectedObject)
		//{
		//	selectedObject->DrawInspector(); // 新しく定義する関数
		//}

		//ImGui::End();



		std::mutex drawMutex;
		// 並列に Draw を実行
		std::for_each(std::execution::par, gameObjects_.begin(), gameObjects_.end(), [&](auto &gameObject) {
			if (gameObject->type != Object::Type::Delete) {
				std::lock_guard<std::mutex> lock(drawMutex);// Updateのアクセスを保護
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
		
		ym::ConsoleLog("FindGameObjects関数が呼ばれました:%s",name);

		// 一致するオブジェクトを格納する配列
		std::vector<std::shared_ptr<Object>> objects;

		// gameObjects_が空か確認
		if (gameObjects_.empty()) {
			ym::ConsoleLog("gameObjects_は空です。");
			return objects;
		}

		// 各オブジェクトを確認
		for (const auto &gameObject : gameObjects_) {
			if (!gameObject) { // nullチェック
				ym::ConsoleLog("警告: gameObjects_にnullのオブジェクトがあります。");
				continue;
			}

			if (gameObject->name == name) { // 名前の比較
				objects.push_back(gameObject);
			}
		}

		ym::ConsoleLog("一致するオブジェクト数:&s",std::to_string(objects.size()));
		return objects;
	}

	std::shared_ptr<Object> GameObjectManager::GetSharedPtrFromRaw(Object *raw)
	{
		for (auto &obj : gameObjects_)
		{
			if (obj.get() == raw)
				return obj;

			// 子も再帰的に探す
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

		// 選択されているか判定
		auto IsSelected = [&](Object *o) {
			return std::find(selected.begin(), selected.end(), o) != selected.end();
			};

		// オブジェクトのフラットリスト作成（Shift範囲選択用）
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

				// ドラッグソース
				if (ImGui::BeginDragDropSource())
				{
					if (!IsSelected(obj))
						selected = { obj };

					ImGui::SetDragDropPayload("HIERARCHY_OBJECTS", selected.data(), sizeof(Object *) * selected.size());
					ImGui::Text("%zu objects", selected.size());
					ImGui::EndDragDropSource();
				}

				// ドロップターゲット
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

		// ツリー描画（ルートのみ）
		for (auto &obj : gameObjects_)
			if (obj && !obj->GetParent())
				DrawObjectNode(obj.get());

		ImGui::Separator();

		// --- Add ボタン ---
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
								o->Uninit(); // オブジェクトの解放処理
								return true; // 削除対象
							}
							return false; // 削除対象ではない
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
									o->Uninit(); // オブジェクトの解放処理
									return true; // 削除対象
								}
								return false; // 削除対象ではない
							}), parent->_childs.end());
					}
				}
			}
		}

		// --- Unparent（複数選択対応） ---
		for (auto *obj : selected)
		{
			if (!obj || !obj->GetParent()) continue;

			ImGui::SameLine();
			if (ImGui::Button("Unparent"))
			{
				obj->GetParent()->DeleteChild(obj->GetSharedPtr());
				obj->SetParent(nullptr);
			}
			break; // 一度だけボタン表示（横並び防止）
		}

		ImGui::End();

		// --- Inspector 表示 ---
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