#pragma once

namespace ym {
    template <typename T>
    class VectorBase {
    public:
        T x, y;

        VectorBase() : x(0), y(0) {}
        VectorBase(T x, T y) : x(x), y(y) {}

        // ���Z
        VectorBase operator+(const VectorBase &other) const {
            return VectorBase(x + other.x, y + other.y);
        }

        VectorBase &operator+=(const VectorBase &other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        // ���Z
        VectorBase operator-(const VectorBase &other) const {
            return VectorBase(x - other.x, y - other.y);
        }

        VectorBase &operator-=(const VectorBase &other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        // �X�J���[��Z
        VectorBase operator*(T scalar) const {
            return VectorBase(x * scalar, y * scalar);
        }

        VectorBase &operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        // �X�J���[���Z
        VectorBase operator/(T scalar) const {
            if (scalar == 0) throw std::runtime_error("Division by zero");
            return VectorBase(x / scalar, y / scalar);
        }

        VectorBase &operator/=(T scalar) {
            if (scalar == 0) throw std::runtime_error("Division by zero");
            x /= scalar;
            y /= scalar;
            return *this;
        }

        // ��r���Z�q
        bool operator==(const VectorBase &other) const {
            return x == other.x && y == other.y;
        }

        bool operator!=(const VectorBase &other) const {
            return !(*this == other);
        }

        // �����i�����������g�p�j
        T Length() const {
            return 1.0f / FastInverseSqrt(x * x + y * y);
        }

        // ���K��
        VectorBase Normalize() const {
            T len = Length();
            if (len == 0) throw std::runtime_error("Zero vector cannot be normalized.");
            return *this * (1 / len);
        }

        // ����
        T Dot(const VectorBase &other) const {
            return x * other.x + y * other.y;
        }

        // ���`���
        VectorBase Lerp(const VectorBase &target, T t) const {
            return *this * (1 - t) + target * t;
        }

        // �ˉe
        VectorBase Project(const VectorBase &onto) const {
            T dot = this->Dot(onto);
            T magSq = onto.Dot(onto);
            if (magSq == 0) throw std::runtime_error("Cannot project onto a zero vector.");
            return onto * (dot / magSq);
        }

        // �p�x�v�Z
        T AngleBetween(const VectorBase &other) const {
            T dotProduct = this->Dot(other);
            T lengths = this->Length() * other.Length();
            if (lengths == 0) throw std::runtime_error("Cannot calculate angle with zero vector.");
            return std::acos(dotProduct / lengths);
        }

        // ����
        VectorBase Reflect(const VectorBase &normal) const {
            T dot = this->Dot(normal);
            return *this - normal * (2 * dot);
        }

        // �������� (Clamp)
        VectorBase ClampMagnitude(T maxLength) const {
            T len = Length();
            if (len > maxLength) {
                return Normalize() * maxLength;
            }
            return *this;
        }

        // �ÓI�֐�: �����������̋t��
        static T FastInverseSqrt(T number) {
            T x2 = number * 0.5f;
            T y = number;
            long i = *(long *)&y;
            i = 0x5f3759df - (i >> 1);
            y = *(T *)&i;
            y = y * (1.5f - (x2 * y * y));
            return y;
        }

        // �f�o�b�O�p������
        virtual std::string ToString() const {
            std::ostringstream oss;
            oss << "(" << x << ", " << y << ")";
            return oss.str();
        }
    };
}