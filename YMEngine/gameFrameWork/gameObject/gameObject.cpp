
#include "gameObject.h"
#include "gameFrameWork/components/component.h"
#include "gameFrameWork/gameObject/gameObjectManager.h"


namespace ym
{
	std::shared_ptr<Object> Object::Clone()
	{
		auto copy = std::make_shared<Object>(*this); // コピーコンストラクタを使用
		// 子オブジェクトは深いコピーを行う
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}
	bool Object::AddChild(std::shared_ptr<Object> child)
	{
		if (!child || child.get() == this) return false;

		// 循環参照チェックを追加
		auto ancestor = shared_from_this();
		while (ancestor) {
			if (ancestor == child) {
				return false; // 循環参照が発生するため追加しない
			}
			ancestor = ancestor->_parent;
		}

		if (child->_parent) {
			child->_parent->DeleteChild(child); // 既存の親から削除
		}

		child->SetParent(shared_from_this()); // 新しい親を設定
		_childs.push_back(child);
		return true;
	}

	bool Object::DeleteChild(const std::shared_ptr<Object> &child)
	{
		if (!child) return false;

		auto it = std::find(_childs.begin(), _childs.end(), child);
		if (it != _childs.end()) {
			child->SetParent(nullptr); // 親情報をクリア
			_childs.erase(it);
			return true;
		}
		return false;
	}
	void Object::UpdateHierarchy() {
		//親がいるか
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

		// unique_ptr から shared_ptr に変換して所有権を移す
		std::shared_ptr<Component> shared = std::move(component);
		components.push_back(shared);
	}


	std::shared_ptr<Object> Object::GetParent() const {
		return _parent;
	}

	void Object::SetParent(std::shared_ptr<Object> newParent) {
		if (_parent != newParent) {

			UpdateHierarchy();
			// 1. 変更前のワールド行列を保存
			XMMATRIX worldMatrix = worldTransform.GetMatrix();

			_parent = newParent;

			//ローカル座標を再計算
			XMMATRIX parentMatrix = newParent ? newParent->worldTransform.GetMatrix() : XMMatrixIdentity();
			XMMATRIX localMatrix = XMMatrixMultiply(worldMatrix, XMMatrixInverse(nullptr, parentMatrix));
			localTransform = Transform::FromMatrix(localMatrix);

			//NotifyUpdate(); // 親変更時に更新通知
			UpdateHierarchy();
		}
	}
	Object::Object():_parent(nullptr)
	{
	}

	Object::~Object()
	{
		ym::ConsoleLog("[%s]削除 \n", name.c_str());
		//参照を持つだけなので削除しない
		objectManager = nullptr;
		// 自身がデストラクタで削除されるとき、子オブジェクトも削除
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
		//子供の固定更新
		for (auto &child : _childs)
		{
			child->FixedUpdate();
		}
		//コンポーネントの固定更新
		for (auto &component : components)
		{
			if (!component->isEnabled)continue; // コンポーネントが無効な場合は更新しない
			component->FixedUpdate();
		}
	}

	void Object::Update()
	{
		if (!isActive)return;
		UpdateHierarchy();
		//子供の更新
		for (auto &child : _childs)
		{
			child->Update();
		}
		//コンポーネントの更新
		for (auto &component : components)
		{
			if (!component->isEnabled)continue; // コンポーネントが無効な場合は更新しない
			component->Update();
		}
		//NotifyUpdate();
		//NotifyGlobalUpdate();
		//NotifyGlobalUpdate();

	}

	void Object::Draw()
	{
		if (!isActive)return;
			// 子も同様に表示
		for (auto &child : _childs)
		{
			child->Draw(); // 子も ImGui 表示＋描画される
		}
		//コンポーネントの描画
		for (auto &component : components)
		{
			if (!component->isEnabled)continue; // コンポーネントが無効な場合は描画しない
			component->Draw();
		}

	}

	void Object::DrawImgui()
	{
		std::string treeId = ym::MakeImGuiID(name.c_str(), this);

		ImGui::PushStyleColor(ImGuiCol_Text, isActive ?
			ImVec4(0.2f, 1.0f, 0.2f, 1.0f) : // アクティブなら緑
			ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // 非アクティブならグレー

		bool open = ImGui::TreeNode(treeId.c_str());

		ImGui::PopStyleColor(); // ← ここで必ず戻す

		if (open)
		{
			//アクティブ状態の切り替え
			ImGui::Checkbox("Active", &isActive);

			// ローカル座標
			Vector3 &pos = localTransform.Position;
			Vector3 &rot = localTransform.Rotation;
			Vector3 &scl = localTransform.Scale;

			// ImGuiでスライダー＋直接入力できるフィールドを表示
			ImGui::Text("Transform");

			// 位置
			ImGui::PushID("Position");
			ImGui::Text("Position:");
			ImGui::DragFloat3("##Position", &pos.x, 0.1f, -1000.0f, 1000.0f, "%.2f");
			ImGui::PopID();

			// 回転（度数表示が欲しければ変換が必要）
			ImGui::PushID("Rotation");
			ImGui::Text("Rotation:");
			ImGui::DragFloat3("##Rotation", &rot.x, 1.0f, -360.0f, 360.0f, "%.1f");
			ImGui::PopID();

			// 拡大
			ImGui::PushID("Scale");
			ImGui::Text("Scale:");
			ImGui::DragFloat3("##Scale", &scl.x, 0.1f, -100.0f, 100.0f, "%.2f");
			ImGui::PopID();

			ImGui::Separator(); // 区切り線

			// コンポーネントの表示
			ImGui::Text("Components:");
			for (auto &component : components)
			{
				if (component)
				{
					component->DrawImgui(); // コンポーネントのImGui表示
				}
			}

			ImGui::Separator();
			ImGui::Text("Add Component:");

			// 状態保持用 static 変数
			static char searchBuffer[64] = "";
			static std::string selectedComponent;

			// ?? Enterキー押下判定付きの検索入力欄
			bool enterPressed = ImGui::InputText(
				"##SearchComponent",
				searchBuffer,
				IM_ARRAYSIZE(searchBuffer),
				ImGuiInputTextFlags_EnterReturnsTrue
			);

			// ?? 入力された文字列を小文字に変換（検索用フィルター）
			std::string filterStr(searchBuffer);
			std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), ::tolower);

			// ?? 候補抽出：重複除外 + 小文字フィルタ一致
			std::vector<std::string> filteredCandidates;
			for (const auto &compName : ym::ComponentRegistry::GetRegisteredNames())
			{
				// すでに追加済みのコンポーネントを除外
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

				// 検索フィルターに一致するか（小文字部分一致）
				std::string compNameLower = compName;
				std::transform(compNameLower.begin(), compNameLower.end(), compNameLower.begin(), ::tolower);

				if (filterStr.empty() || compNameLower.find(filterStr) != std::string::npos)
				{
					filteredCandidates.push_back(compName);
				}
			}

			// ?? 候補リスト表示（BeginCombo形式）
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

			// ?? Enterキーが押されたら自動AddComponent（1候補 or 完全一致）
			if (enterPressed)
			{
				std::string matchedName;

				if (filteredCandidates.size() == 1)
				{
					matchedName = filteredCandidates[0]; // 自動選択
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

					// 入力状態クリア
					selectedComponent.clear();
					memset(searchBuffer, 0, sizeof(searchBuffer));
				}
			}

			// ?? 手動Addボタンでも追加可能
			if (!selectedComponent.empty())
			{
				ImGui::SameLine();
				if (ImGui::Button("Add"))
				{
					auto comp = ym::ComponentRegistry::Create(selectedComponent, this);
					this->AddComponent(std::move(comp));

					// 入力状態クリア
					selectedComponent.clear();
					memset(searchBuffer, 0, sizeof(searchBuffer));
				}
			}
			ImGui::Separator(); // 区切り線

			// 子も表示
			for (auto &child : _childs)
			{
				child->DrawImgui(); // 再帰呼び出し
			}
			ImGui::TreePop();
		}
	}

	void Object::DrawInspector()
	{
		std::string treeId = ym::MakeImGuiID(name.c_str(), this);
		// 名前編集用の入力欄
		ImGui::Text("Name:");

		ImGui::PushStyleColor(ImGuiCol_Text, isActive ?
			ImVec4(0.2f, 1.0f, 0.2f, 1.0f) : // アクティブなら緑
			ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // 非アクティブならグレー
		
		// 名前変更用のバッファ（毎回初期化）
		char nameBuffer[64] = {};
		strncpy(nameBuffer, name.c_str(), sizeof(nameBuffer));

		ImGui::SameLine();
		if (ImGui::InputText("##ObjectName", nameBuffer, IM_ARRAYSIZE(nameBuffer),
			ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			name = nameBuffer; // エンターで反映
		}

		ImGui::PopStyleColor(); // ← ここで必ず戻す

		
			//アクティブ状態の切り替え
			ImGui::Checkbox("Active", &isActive);

			// ローカル座標
			Vector3 &pos = localTransform.Position;
			Vector3 &rot = localTransform.Rotation;
			Vector3 &scl = localTransform.Scale;

			// ImGuiでスライダー＋直接入力できるフィールドを表示
			ImGui::Text("Transform");

			// 位置
			ImGui::PushID("Position");
			ImGui::Text("Position:");
			ImGui::DragFloat3("##Position", &pos.x, 0.1f, -1000.0f, 1000.0f, "%.2f");
			ImGui::PopID();

			// 回転（度数表示が欲しければ変換が必要）
			ImGui::PushID("Rotation");
			ImGui::Text("Rotation:");
			ImGui::DragFloat3("##Rotation", &rot.x, 1.0f, -360.0f, 360.0f, "%.1f");
			ImGui::PopID();

			// 拡大
			// ----------------- スケール UI -----------------
			static bool   uniformScale = false;
			static Vector3 baseScale = scl;   // 編集開始時の基準スケール
			static Vector3 displayScale = scl;   // UI に表示する値

			ImGui::Checkbox("Uniform Scale Edit", &uniformScale);
			ImGui::PushID("ScaleUI");
			ImGui::Text("Scale:");

			if (uniformScale)
			{
				// ■ 編集フィールド
				Vector3 input = displayScale;

				// DragFloat3 + 手打ち両対応
				if (ImGui::DragFloat3("##UScale", &input.x, 0.1f, -100.0f, 100.0f, "%.2f"))
				{
					// どの軸が編集されたかを検出（前回値との差分で判定）
					Vector3 diff = input - displayScale;
					float   factor = 1.0f;

					if (std::abs(diff.x) > 1e-4f)          factor = input.x / baseScale.x;
					else if (std::abs(diff.y) > 1e-4f)     factor = input.y / baseScale.y;
					else if (std::abs(diff.z) > 1e-4f)     factor = input.z / baseScale.z;

					// 連動拡縮
					scl = baseScale * factor;

					// UI 表示値を更新
					displayScale = baseScale * factor;
					input = displayScale;           // すぐ次のフレームで反映される
				}

				/*―― 編集開始を検出して基準スケールを記録 ――*/
				if (ImGui::IsItemActivated()) {            // 最初にクリック or フォーカスした瞬間
					baseScale = scl;                    // 今の比率を保持
					displayScale = scl;
				}

				/*―― 編集終了（Enter 確定 or フォーカス外し）でフラグ整理 ――*/
				if (ImGui::IsItemDeactivatedAfterEdit()) {
					baseScale = scl;                    // 確定した値を次回の基準に
					displayScale = scl;
				}
			}
			else
			{
				ImGui::DragFloat3("##Scale", &scl.x, 0.1f, -100.0f, 100.0f, "%.2f");
				// Uniform モードを抜けたら表示用もリセット
				baseScale = scl;
				displayScale = scl;
			}

			ImGui::PopID();



			ImGui::Separator(); // 区切り線

			ImGui::Text("Components:");
			for (size_t i = 0; i < components.size(); ++i)
			{
				auto &comp = components[i];
				if (!comp) continue;

				ImGui::PushID(static_cast<int>(i));
				if (ImGui::TreeNode(comp->GetName()))
				{
					comp->DrawImgui();  // コンポーネント固有UI

					ImGui::Separator();
					if (ImGui::Button("Remove Component"))
					{
						components.erase(components.begin() + i);
						--i; // 次のループでもう一回 i 番目を見る
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

			// 状態保持用 static 変数
			static char searchBuffer[64] = "";
			static std::string selectedComponent;

			// ?? Enterキー押下判定付きの検索入力欄
			bool enterPressed = ImGui::InputText(
				"##SearchComponent",
				searchBuffer,
				IM_ARRAYSIZE(searchBuffer),
				ImGuiInputTextFlags_EnterReturnsTrue
			);

			// ?? 入力された文字列を小文字に変換（検索用フィルター）
			std::string filterStr(searchBuffer);
			std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), ::tolower);

			// ?? 候補抽出：重複除外 + 小文字フィルタ一致
			std::vector<std::string> filteredCandidates;
			for (const auto &compName : ym::ComponentRegistry::GetRegisteredNames())
			{
				// すでに追加済みのコンポーネントを除外
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

				// 検索フィルターに一致するか（小文字部分一致）
				std::string compNameLower = compName;
				std::transform(compNameLower.begin(), compNameLower.end(), compNameLower.begin(), ::tolower);

				if (filterStr.empty() || compNameLower.find(filterStr) != std::string::npos)
				{
					filteredCandidates.push_back(compName);
				}
			}

			// ?? 候補リスト表示（BeginCombo形式）
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

			// ?? Enterキーが押されたら自動AddComponent（1候補 or 完全一致）
			if (enterPressed)
			{
				std::string matchedName;

				if (filteredCandidates.size() == 1)
				{
					matchedName = filteredCandidates[0]; // 自動選択
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

					// 入力状態クリア
					selectedComponent.clear();
					memset(searchBuffer, 0, sizeof(searchBuffer));
				}
			}

			// ?? 手動Addボタンでも追加可能
			if (!selectedComponent.empty())
			{
				ImGui::SameLine();
				if (ImGui::Button("Add"))
				{
					auto comp = ym::ComponentRegistry::Create(selectedComponent, this);
					this->AddComponent(std::move(comp));

					// 入力状態クリア
					selectedComponent.clear();
					memset(searchBuffer, 0, sizeof(searchBuffer));
				}
			}
			ImGui::Separator(); // 区切り線

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
		//子供の解放
		for (auto &child : _childs)
		{
			child->Uninit();
		}
		_childs.clear();
		//コンポーネントの解放
		for (auto &component : components)
		{
			component->Uninit();
		}
		components.clear();
	}
	std::vector<std::unique_ptr<IPropertyProvider>> Object::CollectPropertyProviders()
	{
		std::vector<std::unique_ptr<IPropertyProvider>> list;

		// ① Transform は特別扱い
		list.emplace_back(std::make_unique<TransformPropertyProvider>(&localTransform));

		// ② すでに付いている Component で IPropertyProvider を実装している物を追加
		for (auto &c : components) {
			if (auto p = dynamic_cast<IPropertyProvider *>(c.get())) {
				// Component 自体は Object が所有しているので raw ポインタでラップ
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
