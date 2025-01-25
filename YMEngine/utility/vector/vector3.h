#pragma once
#include "vectorbase.h"

namespace ym {

    class Vector2;
    class Vector3 : public VectorBase<float> {
    public:
        float z;

        // メンバ変数
        static Vector3 back;
        static Vector3 forward;

        static Vector3 up;
        static Vector3 down;
        static Vector3 left;
        static Vector3 right;

        static Vector3 zero;
        static Vector3 one;

        Vector3() : VectorBase(), z(0) {}
        Vector3(float x, float y, float z) : VectorBase(x, y), z(z) {}

        // インデックスアクセス演算子
        inline float &operator[](size_t i) {
            if (i == 0) return x;
            if (i == 1) return y;
            if (i == 2) return z;
            throw std::out_of_range("Index out of range for Vector3");
        }

        inline const float &operator[](size_t i) const {
            if (i == 0) return x;
            if (i == 1) return y;
            if (i == 2) return z;
            throw std::out_of_range("Index out of range for Vector3");
        }

        // 加算演算子 (Vector3 + Vector3)
        inline Vector3 operator+(const Vector3 &other) const {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }

        // 減算演算子 (Vector3 - Vector3)
        inline Vector3 operator-(const Vector3 &other) const {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }
        // 単項マイナス演算子 (-Vector3)
        inline Vector3 operator-() const {
            return Vector3(-x, -y, -z);
        }
        //乗算演算子(Vector3 * Vector3)
        inline Vector3 operator*(const Vector3 &other) const {
            return Vector3(x * other.x, y * other.y, z * other.z);
        }
        //除算演算子(Vector3 / Vector3)
        inline Vector3 operator/(const Vector3 &other) const {
            return Vector3(x / other.x, y / other.y, z / other.z);
        }

        // スカラー乗算 (Vector3 * float)
        inline Vector3 operator*(float scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }
        // スカラー乗算 (float * Vector3)
        inline friend Vector3 operator*(float scalar, const Vector3 &v) {
            return v * scalar;
        }

        // スカラー除算 (Vector3 / float)
        inline Vector3 operator/(float scalar) const {
            return Vector3(x / scalar, y / scalar, z / scalar);
        }
        // スカラー除算 (float / Vector3)
        inline friend Vector3 operator/(float scalar, const Vector3 &v) {
            return Vector3(scalar / v.x, scalar / v.y, scalar / v.z);
        }

        // 加算代入 (Vector3 += Vector3)
        inline Vector3 &operator+=(const Vector3 &other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        // 減算代入 (Vector3 -= Vector3)
        inline Vector3 &operator-=(const Vector3 &other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        // スカラー乗算代入 (Vector3 *= float)
        inline Vector3 &operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        // スカラー除算代入 (Vector3 /= float)
        inline Vector3 &operator/=(float scalar) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

        // 等価演算子 (Vector3 == Vector3)
        inline bool operator==(const Vector3 &other) const {
            return x == other.x && y == other.y && z == other.z;
        }

        // 非等価演算子 (Vector3 != Vector3)
        inline bool operator!=(const Vector3 &other) const {
            return !(*this == other);
        }
        inline float Dot(const Vector3 &v) const {
            return x * v.x + y * v.y + z * v.z;
        }

        // 外積
        inline Vector3 Cross(const Vector3 &other) const {
            return Vector3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }

        // 長さ（高速平方根使用）
        inline float Length() const {
            return 1.0f / FastInverseSqrt(x * x + y * y + z * z);
        }

        // 長さの二乗を計算
       inline float LengthSquared() const {
            return x * x + y * y + z * z;
        }

        // 正規化
        inline Vector3 Normalize() const {
            float len = Length();
            if (len == 0) throw std::runtime_error("Zero vector cannot be normalized.");
            Vector3 result = *this;
            return (*this) * (1 / len);
        }

		inline Vector3 Min(const Vector3 &a, const Vector3 &b) {
			return Vector3(
				std::min(a.x, b.x),
				std::min(a.y, b.y),
				std::min(a.z, b.z)
			);
		}

        inline Vector3 Max(const Vector3 &a, const Vector3 &b) {

			return Vector3(
				std::max(a.x, b.x),
				std::max(a.y, b.y),
				std::max(a.z, b.z)
			);
        }

        inline float Distance(const Vector3 &other) const {
            return (*this - other).Length();
        }

       inline Vector3 ProjectOnto(const Vector3 &other) const {
            float scalar = this->Dot(other) / other.LengthSquared();
            return other * scalar;
        }

       inline Vector3 ProjectOntoPlane(const Vector3 &planeNormal) const {
           Vector3 projection = this->ProjectOnto(planeNormal);
           return *this - projection;
       }


        // DirectXMath互換
        DirectX::XMFLOAT3 ToXMFLOAT3() const {
            return DirectX::XMFLOAT3(x, y, z);
        }

        // Vector2に変換
        Vector2 ToVector2() const;

        // デバッグ用文字列
        std::string ToString() const override {
            std::ostringstream oss;
            oss << "(" << x << ", " << y << ", " << z << ")";
            return oss.str();
        }

        bool operator==(const Vector3 &rhs) {
            return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
        }
       

    };


}
    namespace std {
        template<>
        struct hash<ym::Vector3> {
            std::size_t operator()(const ym::Vector3 &vec) const noexcept {
                // ハッシュ計算の例：X, Y, Zを組み合わせる
                std::size_t hx = std::hash<float>()(vec.x);
                std::size_t hy = std::hash<float>()(vec.y);
                std::size_t hz = std::hash<float>()(vec.z);

                // ハッシュ値を組み合わせる
                return hx ^ (hy << 1) ^ (hz << 2);
            }
        };
    }