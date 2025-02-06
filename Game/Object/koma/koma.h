#pragma once

#include "gameFrameWork/gameObject/gameObject.h"
namespace ym {

	enum class KomaType
	{
		None,
		Black,
		White
	};

	enum class KomaState
	{
		Normal,
		Flip
	};

	//コピー用テンプレート
	class Koma : public Object
	{
	public:
		void Init()override;
		void FixedUpdate()override;
		void Update()override;
		void Draw()override;
		void Uninit()override;
		std::shared_ptr<Object>Clone()override;

		KomaType GetKomaType() { return _komaType; }
		void SetKomaType(KomaType komaType) { _komaType = komaType; }

		KomaState GetKomaState() { return _komaState; }
		void SetKomaState(KomaState komaState) { _komaState = komaState; }

		void SetFlipStartTimer(float time) { _flipStartTimer = time; }

	private:
		KomaType _komaType;
		KomaState _komaState;
		float _flipStartTimer = 0;
		Timer timer;
	};
}