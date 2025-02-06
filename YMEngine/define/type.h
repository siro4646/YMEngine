#pragma once

namespace ym
{
	typedef int8_t		s8;
	typedef int16_t		s16;
	typedef int32_t		s32;
	typedef int64_t		s64;
	typedef uint8_t		u8;
	typedef uint16_t	u16;
	typedef uint32_t	u32;
	typedef uint64_t	u64;

	struct DummyTex
	{
		enum Type
		{

			Black,
			White,
			Checker,
			Purple,
			Cyen,
			LightGreen,
			Green,
			FlatNormal,

			Max
		};
	};	// struct DummyTex

	// 頂点構造体
	struct Vertex3D
	{
		DirectX::XMFLOAT3 Position; // 位置座標
		DirectX::XMFLOAT3 Normal; // 法線
		DirectX::XMFLOAT2 UV; // uv座標
		DirectX::XMFLOAT3 Tangent; // 接空間
		DirectX::XMFLOAT4 Color; // 頂点色

	};
	struct Vertex2D
	{
		DirectX::XMFLOAT3 Position; // 位置座標
		DirectX::XMFLOAT2 UV; // uv座標
	};

	struct Mesh
	{
		std::vector<Vertex3D> Vertices; // 頂点データの配列
		std::vector <ym::u32> Indices; // インデックスの配列
		std::wstring DiffuseMap; // テクスチャのファイルパス
		std::wstring SpecularMap;// スペキュラーマップのファイルパス
		std::wstring MaskMap; // マスクマップのファイルパス
	};

	enum ModelSetting
	{
		None = 0,
		InverseU = (int)(1 << 0),   // 0ビット目
		InverseV = (int)(1 << 1),   // 1ビット目
		InverseUV = (int)(InverseU | InverseV), // 合成フラグ
		AdjustScale = (int)(1 << 2),   // 2ビット目
		AdjustCenter = (int)(1 << 3),   // 3ビット目
		AdjustScaleAndCenter = (int)(AdjustScale | AdjustCenter) // 合成フラグ
	};

	struct ImportSettings // インポートするときのパラメータ
	{
		const wchar_t *filename = nullptr; // ファイルパス
		int flags = ModelSetting::None; // フラグ
	};

}