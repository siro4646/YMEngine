#pragma once
#include "vectorbase.h"
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

    // ���Z���Z�q (Vector3 + Vector3)
    Vector3 operator+(const Vector3 &other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    // ���Z���Z�q (Vector3 - Vector3)
    Vector3 operator-(const Vector3 &other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
	// �P���}�C�i�X���Z�q (-Vector3)
    Vector3 operator-() const {
		return Vector3(-x, -y, -z);
    }
	//��Z���Z�q(Vector3 * Vector3)
    Vector3 operator*(const Vector3 &other) const {
		return Vector3(x * other.x, y * other.y, z * other.z);
    }
	//���Z���Z�q(Vector3 / Vector3)
    Vector3 operator/(const Vector3 &other) const {
		return Vector3(x / other.x, y / other.y, z / other.z);
    }

    // �X�J���[��Z (Vector3 * float)
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
	// �X�J���[��Z (float * Vector3)
	friend Vector3 operator*(float scalar, const Vector3 &v) {
		return v * scalar;
	}

    // �X�J���[���Z (Vector3 / float)
    Vector3 operator/(float scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }
	// �X�J���[���Z (float / Vector3)
    friend Vector3 operator/(float scalar, const Vector3 &v) {
		return Vector3(scalar / v.x, scalar / v.y, scalar / v.z);
    }

    // ���Z��� (Vector3 += Vector3)
    Vector3 &operator+=(const Vector3 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    // ���Z��� (Vector3 -= Vector3)
    Vector3 &operator-=(const Vector3 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    // �X�J���[��Z��� (Vector3 *= float)
    Vector3 &operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    // �X�J���[���Z��� (Vector3 /= float)
    Vector3 &operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    // �������Z�q (Vector3 == Vector3)
    bool operator==(const Vector3 &other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    // �񓙉����Z�q (Vector3 != Vector3)
    bool operator!=(const Vector3 &other) const {
        return !(*this == other);
    }
    float Dot(const Vector3 &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    // �O��
    Vector3 Cross(const Vector3 &other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // �����i�����������g�p�j
    float Length() const {
        return 1.0f / FastInverseSqrt(x * x + y * y + z * z);
    }

    // ���K��
    Vector3 Normalize() const {
        float len = Length();
        if (len == 0) throw std::runtime_error("Zero vector cannot be normalized.");
		Vector3 result = *this;
        return (*this) * (1 / len);
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
};
