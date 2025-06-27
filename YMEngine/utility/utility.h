#pragma once

#define NOMINMAX

#include "transform/transform.h"
#include "quaternion/quaternion.h"
#include "stringUtility.h"

#include "debug/logSystem.h"

namespace ym
{
	struct Color
	{
		float r, g, b, a;
	};

	enum class ColorSpaceType
	{
		Rec709,
		Rec2020,
	};	// enum class ColorSpaceType

	template <typename T>
	void SafeRelease(T &p)
	{
		if (p != nullptr)
		{
			p->Release();
			p = nullptr;
		}
	}

	template <typename T>
	void SafeDelete(T &p)
	{
		if (p != nullptr)
		{
			delete p;
			p = nullptr;
		}
	}

	template <typename T>
	void SafeDeleteArray(T &p)
	{
		if (p != nullptr)
		{
			delete[] p;
			p = nullptr;
		}
	}

	inline void ConsoleLog(const char *format, ...)
	{
#ifdef _DEBUG
		va_list arg;

		char tsv[4096];
		va_start(arg, format);
		vsprintf_s(tsv, format, arg);
		va_end(arg);

		OutputDebugStringA(tsv);
#endif // _DEBUG

	}
	inline void ConsoleLogRelease(const char *format, ...)
	{
		va_list arg;

		char tsv[4096];
		va_start(arg, format);
		vsprintf_s(tsv, format, arg);
		va_end(arg);

		OutputDebugStringA(tsv);
	}
	inline void DebugLog(const char *format, ...)
	{
		va_list arg;

		char tsv[4096];
		va_start(arg, format);
		vsprintf_s(tsv, format, arg);
		va_end(arg);

		LogSystem::Instance().AddLog(tsv);

	}

	static const u32 kFnv1aPrime32 = 16777619;
	static const u32 kFnv1aSeed32 = 0x811c9dc5;
	static const u64 kFnv1aPrime64 = 1099511628211L;
	static const u64 kFnv1aSeed64 = 0xcbf29ce484222325;

	inline u32 CalcFnv1a32(u8 oneByte, u32 hash = kFnv1aSeed32)
	{
		return (oneByte ^ hash) * kFnv1aPrime32;
	}
	inline u32 CalcFnv1a32(const void *data, size_t numBytes, u32 hash = kFnv1aSeed32)
	{
		assert(data);
		const u8 *ptr = reinterpret_cast<const u8 *>(data);
		while (numBytes--)
			hash = CalcFnv1a32(*ptr++, hash);
		return hash;
	}
	inline u64 CalcFnv1a64(u8 oneByte, u64 hash = kFnv1aSeed64)
	{
		return (oneByte ^ hash) * kFnv1aPrime32;
	}
	inline u64 CalcFnv1a64(const void *data, size_t numBytes, u64 hash = kFnv1aSeed64)
	{
		assert(data);
		const u8 *ptr = reinterpret_cast<const u8 *>(data);
		while (numBytes--)
			hash = CalcFnv1a64(*ptr++, hash);
		return hash;
	}

	constexpr u32 GetAlignedSize(const u32 size, const u32 align)
	{
		return ((size + align - 1) / align) * align;
	}
	constexpr size_t GetAlignedSize(const size_t size, const size_t align)
	{
		return ((size + align - 1) / align) * align;
	}

	class HashString
	{
	public:
		HashString()
		{}
		HashString(const char *str)
			: str_(str)
		{
			hash_ = CalcFnv1a32(str_.c_str(), str_.size());
		}

		bool operator==(const HashString &rhs) const
		{
			if (hash_ != rhs.hash_) return false;
			return str_ == rhs.str_;
		}
		bool operator<(const HashString &rhs) const
		{
			if (hash_ != rhs.hash_) return hash_ < rhs.hash_;
			return str_ < rhs.str_;
		}
		bool operator>(const HashString &rhs) const
		{
			if (hash_ != rhs.hash_) return hash_ > rhs.hash_;
			return str_ > rhs.str_;
		}

		const std::string &GetString() const
		{
			return str_;
		}
		u32 GetHash() const
		{
			return hash_;
		}

	private:
		std::string	str_;
		u32			hash_ = 0;
	};	// class HashString

	class HashWString
	{
	public:
		HashWString()
		{}
		HashWString(const wchar_t *str)
			: str_(str)
		{
			hash_ = CalcFnv1a32(str_.c_str(), str_.size());
		}

		bool operator==(const HashWString &rhs) const
		{
			if (hash_ != rhs.hash_) return false;
			return str_ == rhs.str_;
		}
		bool operator<(const HashWString &rhs) const
		{
			if (hash_ != rhs.hash_) return hash_ < rhs.hash_;
			return str_ < rhs.str_;
		}
		bool operator>(const HashWString &rhs) const
		{
			if (hash_ != rhs.hash_) return hash_ > rhs.hash_;
			return str_ > rhs.str_;
		}

		const std::wstring &GetString() const
		{
			return str_;
		}
		u32 GetHash() const
		{
			return hash_;
		}

	private:
		std::wstring	str_;
		u32				hash_ = 0;
	};	// class HashWString

	class CpuTimer
	{
	public:
		static void Initialize()
		{
			QueryPerformanceFrequency(&frequency_);
		}

		static CpuTimer CurrentTime()
		{
			CpuTimer r;
			QueryPerformanceCounter(&r.time_);
			return r;
		}

		CpuTimer &operator=(const CpuTimer &v)
		{
			time_ = v.time_;
			return *this;
		}

		CpuTimer &operator+=(const CpuTimer &v)
		{
			time_.QuadPart += v.time_.QuadPart;
			return *this;
		}

		CpuTimer &operator-=(const CpuTimer &v)
		{
			time_.QuadPart -= v.time_.QuadPart;
			return *this;
		}

		CpuTimer operator+(const CpuTimer &v) const
		{
			CpuTimer r;
			r.time_.QuadPart = time_.QuadPart + v.time_.QuadPart;
			return r;
		}

		CpuTimer operator-(const CpuTimer &v) const
		{
			CpuTimer r;
			r.time_.QuadPart = time_.QuadPart - v.time_.QuadPart;
			return r;
		}

		float ToSecond() const
		{
			return (float)time_.QuadPart / (float)frequency_.QuadPart;
		}
		float ToMilliSecond() const
		{
			return (float)(time_.QuadPart * 1000) / (float)frequency_.QuadPart;
		}
		float ToMicroSecond() const
		{
			return (float)(time_.QuadPart * 1000 * 1000) / (float)frequency_.QuadPart;
		}
		float ToNanoSecond() const
		{
			return (float)(time_.QuadPart * 1000 * 1000 * 1000) / (float)frequency_.QuadPart;
		}

	private:
		static LARGE_INTEGER	frequency_;
		LARGE_INTEGER			time_ = { 0 };
	};	// class CpuTimer

	class Random
	{
	public:

		Random()
		{
			Random::Random((u32)timeGetTime());
		}
		Random(u32 seed)
		{
			//ym::ConsoleLogRelease("aaaaaaaaa");
			x_ = seed = 1812433253 * (seed ^ (seed >> 30));
			y_ = seed = 1812433253 * (seed ^ (seed >> 30)) + 1;
			z_ = seed = 1812433253 * (seed ^ (seed >> 30)) + 2;
			w_ = seed = 1812433253 * (seed ^ (seed >> 30)) + 3;
		}

		void SetSeed(u32 seed)
		{
			x_ = seed = 1812433253 * (seed ^ (seed >> 30));
			y_ = seed = 1812433253 * (seed ^ (seed >> 30)) + 1;
			z_ = seed = 1812433253 * (seed ^ (seed >> 30)) + 2;
			w_ = seed = 1812433253 * (seed ^ (seed >> 30)) + 3;
		}

		u32 GetValue()
		{
			u32 t = x_ ^ (x_ << 11);
			x_ = y_;
			y_ = z_;
			z_ = w_;
			return w_ = (w_ ^ (w_ >> 19)) ^ (t ^ (t >> 8));
		}

		float GetFValue()
		{
			return (float)GetValue() / (float)UINT_MAX;
		}

		float GetFValueRange(float minV, float maxV)
		{
			return minV + (maxV - minV) * GetFValue();
		}

	private:
		u32		x_ = 123456789;
		u32		y_ = 362436069;
		u32		z_ = 521288629;
		u32		w_ = 88675123;
	};	// class Random

	struct BoundingSphere
	{
		DirectX::XMFLOAT3	center;
		float				radius;

		BoundingSphere()
			: center(0.0f, 0.0f, 0.0f), radius(0.0f)
		{}
		BoundingSphere(float x, float y, float z, float r)
			: center(x, y, z), radius(r)
		{}
		BoundingSphere(const DirectX::XMFLOAT3 &c, float r)
			: center(c), radius(r)
		{}
	};	// struct BoundingSphere

	struct BoundingBox
	{
		DirectX::XMFLOAT3	boxMin;
		DirectX::XMFLOAT3	boxMax;

		BoundingBox()
			: boxMin(0.0f, 0.0f, 0.0f), boxMax(0.0f, 0.0f, 0.0f)
		{}
		BoundingBox(float x0, float y0, float z0, float x1, float y1, float z1)
			: boxMin(x0, y0, z0), boxMax(x1, y1, z1)
		{}
		BoundingBox(const DirectX::XMFLOAT3 &pmin, const DirectX::XMFLOAT3 &pmax)
			: boxMin(pmin), boxMax(pmax)
		{}
	};	// struct BoundingBox

	// inverse z perspective matrix.
	inline DirectX::XMMATRIX MatrixPerspectiveInverseFovRH(float FovYRad, float Aspect, float Zn, float Zf)
	{
		float YScale = 1.0f / tanf(FovYRad * 0.5f);
		float XScale = YScale / Aspect;
		float DivZ = 1.0f / (Zf - Zn);
		DirectX::XMFLOAT4X4 m(
			XScale, 0.0f, 0.0f, 0.0f,
			0.0f, YScale, 0.0f, 0.0f,
			0.0f, 0.0f, Zn * DivZ, -1.0f,
			0.0f, 0.0f, Zn * Zf * DivZ, 0.0f);
		return DirectX::XMLoadFloat4x4(&m);
	}
	inline DirectX::XMMATRIX MatrixPerspectiveInverseFovLH(float FovYRad, float Aspect, float Zn, float Zf)
	{
		float YScale = 1.0f / tanf(FovYRad * 0.5f);
		float XScale = YScale / Aspect;
		float DivZ = 1.0f / (Zf - Zn);
		DirectX::XMFLOAT4X4 m(
			XScale, 0.0f, 0.0f, 0.0f,
			0.0f, YScale, 0.0f, 0.0f,
			0.0f, 0.0f, -Zn * DivZ, 1.0f,
			0.0f, 0.0f, Zn * Zf * DivZ, 0.0f);
		return DirectX::XMLoadFloat4x4(&m);
	}

	// infinite perspective matrix.
	inline DirectX::XMMATRIX MatrixPerspectiveInfiniteFovRH(float FovYRad, float Aspect, float Zn)
	{
		float YScale = 1.0f / tanf(FovYRad * 0.5f);
		float XScale = YScale / Aspect;
		DirectX::XMFLOAT4X4 m(
			XScale, 0.0f, 0.0f, 0.0f,
			0.0f, YScale, 0.0f, 0.0f,
			0.0f, 0.0f, -1.0f, -1.0f,
			0.0f, 0.0f, -Zn, 0.0f);
		return DirectX::XMLoadFloat4x4(&m);
	}
	inline DirectX::XMMATRIX MatrixPerspectiveInfiniteFovLH(float FovYRad, float Aspect, float Zn)
	{
		float YScale = 1.0f / tanf(FovYRad * 0.5f);
		float XScale = YScale / Aspect;
		DirectX::XMFLOAT4X4 m(
			XScale, 0.0f, 0.0f, 0.0f,
			0.0f, YScale, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 0.0f, -Zn, 0.0f);
		return DirectX::XMLoadFloat4x4(&m);
	}

	// infinite inverse z perspective matrix.
	inline DirectX::XMMATRIX MatrixPerspectiveInfiniteInverseFovRH(float FovYRad, float Aspect, float Zn)
	{
		float YScale = 1.0f / tanf(FovYRad * 0.5f);
		float XScale = YScale / Aspect;
		DirectX::XMFLOAT4X4 m(
			XScale, 0.0f, 0.0f, 0.0f,
			0.0f, YScale, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, Zn, 0.0f);
		return DirectX::XMLoadFloat4x4(&m);
	}
	inline DirectX::XMMATRIX MatrixPerspectiveInfiniteInverseFovLH(float FovYRad, float Aspect, float Zn)
	{
		float YScale = 1.0f / tanf(FovYRad * 0.5f);
		float XScale = YScale / Aspect;
		DirectX::XMFLOAT4X4 m(
			XScale, 0.0f, 0.0f, 0.0f,
			0.0f, YScale, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, Zn, 0.0f);
		return DirectX::XMLoadFloat4x4(&m);
	}

	// View Z from perspective matrix.
	inline float ViewZFromPerspective(const DirectX::XMMATRIX &Persp, float DeviceZ)
	{
		float a = Persp.r[2].m128_f32[2];
		float b = Persp.r[3].m128_f32[2];
		float c = Persp.r[2].m128_f32[3];
		return -b / (a - c * DeviceZ);
	}

	//スケール行列の作成
	static std::array<std::array<float, 4>, 4> CreateScaleMatrix(float x, float y, float z)
	{
		return{ {
			{x,0,0,0},
			{0,y,0,0},
			{0,0,z,0},
			{0,0,0,1}
			} };
	}

	static std::array<std::array<float, 4>, 4> CreateScaleMatrix(Vector3 v)
	{
		return CreateScaleMatrix(v.x, v.y, v.z);
	}
	//回転行列を作成
	static std::array<std::array<float, 4>, 4> CreateRotationMatrix(float x, float y, float z)
	{
		float radX = XMConvertToRadians(x);
		float radY = XMConvertToRadians(y);
		float radZ = XMConvertToRadians(z);

		float cosX = cosf(radX);
		float sinX = sinf(radX);
		float cosY = cosf(radY);
		float sinY = sinf(radY);
		float cosZ = cosf(radZ);
		float sinZ = sinf(radZ);

		return{ {
			{cosY * cosZ,cosY * sinZ,0,0},
			{sinX * sinY * cosZ - cosX * sinZ, sinX * sinY * sinZ + cosX * cosZ,sinX * cosY,0},
			{cosX * sinY * cosZ + sinX * sinZ, cosX * sinY * sinZ - sinX * cosZ,cosX * cosY,0},
			{0,0,0,1}
		} };
	}
	static std::array<std::array<float, 4>, 4> CreateRotationMatrix(Vector3 v)
	{
		return CreateRotationMatrix(v.x, v.y, v.z);
	}

	//平行移動行列を作成
	static std::array<std::array<float, 4>, 4> CreateTranslationMatrix(float x, float y, float z)
	{
		return{ {
			{1,0,0,x},
			{0,1,0,y},
			{0,0,1,z},
			{0,0,0,1}
		} };
	}
	static std::array<std::array<float, 4>, 4> CreateTranslationMatrix(Vector3 v)
	{
		return CreateTranslationMatrix(v.x, v.y, v.z);
	}
	//行列の掛け算
	static std::array<std::array<float, 4>, 4> MultiplyMatrix(const std::array<std::array<float, 4>, 4> &m1, const std::array<std::array<float, 4>, 4> &m2)
	{
		std::array<std::array<float, 4>, 4> ans = { 0 };
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				ans[i][j] =
					m1[i][0] * m2[0][j] +
					m1[i][1] * m2[1][j] +
					m1[i][2] * m2[2][j] +
					m1[i][3] * m2[3][j];
			}
		}
		return ans;
	}
	//変換行列を作成
	static std::array<std::array<float, 4>, 4> CreateTransformMatrix(Vector3 scale, Vector3 rot, Vector3 trans)
	{
		auto scaleMatrix = CreateScaleMatrix(scale);
		auto rotMatrix = CreateRotationMatrix(rot);
		auto transMatrix = CreateTranslationMatrix(trans);

		auto ans = MultiplyMatrix(MultiplyMatrix(scaleMatrix, rotMatrix), transMatrix);

		return ans;
	}
	static std::array<std::array<float, 4>, 4> CreateTransformMatrix(Transform t)
	{
		
		return CreateTransformMatrix(t.Scale, t.Rotation, t.Position);
	}
	// 座標を変換するための関数
	static Vector3 TransformPoint(const std::array<std::array<float, 4>, 4> &matrix, const std::array<float, 3> &point) {
		std::array<float, 3> ans = {
			matrix[0][0] * point[0] + matrix[0][1] * point[1] + matrix[0][2] * point[2] + matrix[0][3],
			matrix[1][0] * point[0] + matrix[1][1] * point[1] + matrix[1][2] * point[2] + matrix[1][3],
			matrix[2][0] * point[0] + matrix[2][1] * point[1] + matrix[2][2] * point[2] + matrix[2][3]
		};
		return Vector3(ans[0], ans[1], ans[2]);
	}



	
	extern Random GlobalRandom;
}	// namespace ym