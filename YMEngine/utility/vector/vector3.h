#pragma once
#include "vectorbase.h"

namespace ym {

    class Vector2;
    class Vector3 : public VectorBase<float> {
    public:
        float z;

        // �����o�ϐ�
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

        // �C���f�b�N�X�A�N�Z�X���Z�q
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

        // ���Z���Z�q (Vector3 + Vector3)
        inline Vector3 operator+(const Vector3 &other) const {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }

        // ���Z���Z�q (Vector3 - Vector3)
        inline Vector3 operator-(const Vector3 &other) const {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }
        // �P���}�C�i�X���Z�q (-Vector3)
        inline Vector3 operator-() const {
            return Vector3(-x, -y, -z);
        }
        //��Z���Z�q(Vector3 * Vector3)
        inline Vector3 operator*(const Vector3 &other) const {
            return Vector3(x * other.x, y * other.y, z * other.z);
        }
        //���Z���Z�q(Vector3 / Vector3)
        inline Vector3 operator/(const Vector3 &other) const {
            return Vector3(x / other.x, y / other.y, z / other.z);
        }

        // �X�J���[��Z (Vector3 * float)
        inline Vector3 operator*(float scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }
        // �X�J���[��Z (float * Vector3)
        inline friend Vector3 operator*(float scalar, const Vector3 &v) {
            return v * scalar;
        }

        // �X�J���[���Z (Vector3 / float)
        inline Vector3 operator/(float scalar) const {
            return Vector3(x / scalar, y / scalar, z / scalar);
        }
        // �X�J���[���Z (float / Vector3)
        inline friend Vector3 operator/(float scalar, const Vector3 &v) {
            return Vector3(scalar / v.x, scalar / v.y, scalar / v.z);
        }

        // ���Z��� (Vector3 += Vector3)
        inline Vector3 &operator+=(const Vector3 &other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        // ���Z��� (Vector3 -= Vector3)
        inline Vector3 &operator-=(const Vector3 &other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        // �X�J���[��Z��� (Vector3 *= float)
        inline Vector3 &operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        // �X�J���[���Z��� (Vector3 /= float)
        inline Vector3 &operator/=(float scalar) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

        // �������Z�q (Vector3 == Vector3)
        inline bool operator==(const Vector3 &other) const {
            return x == other.x && y == other.y && z == other.z;
        }

        // �񓙉����Z�q (Vector3 != Vector3)
        inline bool operator!=(const Vector3 &other) const {
            return !(*this == other);
        }
        inline float Dot(const Vector3 &v) const {
            return x * v.x + y * v.y + z * v.z;
        }

        // �O��
        inline Vector3 Cross(const Vector3 &other) const {
            return Vector3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }

        // �����i�����������g�p�j
        inline float Length() const {
            return 1.0f / FastInverseSqrt(x * x + y * y + z * z);
        }

        // �����̓����v�Z
       inline float LengthSquared() const {
            return x * x + y * y + z * z;
        }

        // ���K��
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


        // DirectXMath�݊�
        DirectX::XMFLOAT3 ToXMFLOAT3() const {
            return DirectX::XMFLOAT3(x, y, z);
        }

        // Vector2�ɕϊ�
        Vector2 ToVector2() const;

        // �f�o�b�O�p������
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
                // �n�b�V���v�Z�̗�FX, Y, Z��g�ݍ��킹��
                std::size_t hx = std::hash<float>()(vec.x);
                std::size_t hy = std::hash<float>()(vec.y);
                std::size_t hz = std::hash<float>()(vec.z);

                // �n�b�V���l��g�ݍ��킹��
                return hx ^ (hy << 1) ^ (hz << 2);
            }
        };
    }