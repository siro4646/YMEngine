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

	// ���_�\����
	struct Vertex3D
	{
		DirectX::XMFLOAT3 Position; // �ʒu���W
		DirectX::XMFLOAT3 Normal; // �@��
		DirectX::XMFLOAT2 UV; // uv���W
		DirectX::XMFLOAT3 Tangent; // �ڋ��
		DirectX::XMFLOAT4 Color; // ���_�F

	};
	struct Vertex2D
	{
		DirectX::XMFLOAT3 Position; // �ʒu���W
		DirectX::XMFLOAT2 UV; // uv���W
	};

	struct Mesh
	{
		std::vector<Vertex3D> Vertices; // ���_�f�[�^�̔z��
		std::vector <ym::u32> Indices; // �C���f�b�N�X�̔z��
		std::wstring DiffuseMap; // �e�N�X�`���̃t�@�C���p�X
		std::wstring SpecularMap;// �X�y�L�����[�}�b�v�̃t�@�C���p�X
		std::wstring MaskMap; // �}�X�N�}�b�v�̃t�@�C���p�X
	};

	enum ModelSetting
	{
		None = 0,
		InverseU = (int)(1 << 0),   // 0�r�b�g��
		InverseV = (int)(1 << 1),   // 1�r�b�g��
		InverseUV = (int)(InverseU | InverseV), // �����t���O
		AdjustScale = (int)(1 << 2),   // 2�r�b�g��
		AdjustCenter = (int)(1 << 3),   // 3�r�b�g��
		AdjustScaleAndCenter = (int)(AdjustScale | AdjustCenter) // �����t���O
	};

	struct ImportSettings // �C���|�[�g����Ƃ��̃p�����[�^
	{
		const wchar_t *filename = nullptr; // �t�@�C���p�X
		int flags = ModelSetting::None; // �t���O
	};

}