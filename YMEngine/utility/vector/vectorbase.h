#pragma once

namespace ym {
    template <typename T>
    class VectorBase {
    public:
        T x, y;

        VectorBase() : x(0), y(0) {}
        VectorBase(T x, T y) : x(x), y(y) {}

        // ‰ÁŽZ
        VectorBase operator+(const VectorBase &other) const {
            return VectorBase(x + other.x, y + other.y);
        }

        VectorBase &operator+=(const VectorBase &other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        // Œ¸ŽZ
        VectorBase operator-(const VectorBase &other) const {
            return VectorBase(x - other.x, y - other.y);
        }

        VectorBase &operator-=(const VectorBase &other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        // ƒXƒJƒ‰[æŽZ
        VectorBase operator*(T scalar) const {
            return VectorBase(x * scalar, y * scalar);
        }

        VectorBase &operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        // ƒXƒJƒ‰[œŽZ
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

        // ”äŠr‰‰ŽZŽq
        bool operator==(const VectorBase &other) const {
            return x == other.x && y == other.y;
        }

        bool operator!=(const VectorBase &other) const {
            return !(*this == other);
        }

        // ’·‚³i‚‘¬•½•ûªŽg—pj
        T Length() const {
            return 1.0f / FastInverseSqrt(x * x + y * y);
        }

        // ³‹K‰»
        VectorBase Normalize() const {
            T len = Length();
            if (len == 0) throw std::runtime_error("Zero vector cannot be normalized.");
            return *this * (1 / len);
        }

        // “àÏ
        T Dot(const VectorBase &other) const {
            return x * other.x + y * other.y;
        }

        // üŒ`•âŠÔ
        VectorBase Lerp(const VectorBase &target, T t) const {
            return *this * (1 - t) + target * t;
        }

        // ŽË‰e
        VectorBase Project(const VectorBase &onto) const {
            T dot = this->Dot(onto);
            T magSq = onto.Dot(onto);
            if (magSq == 0) throw std::runtime_error("Cannot project onto a zero vector.");
            return onto * (dot / magSq);
        }

        // Šp“xŒvŽZ
        T AngleBetween(const VectorBase &other) const {
            T dotProduct = this->Dot(other);
            T lengths = this->Length() * other.Length();
            if (lengths == 0) throw std::runtime_error("Cannot calculate angle with zero vector.");
            return std::acos(dotProduct / lengths);
        }

        // ”½ŽË
        VectorBase Reflect(const VectorBase &normal) const {
            T dot = this->Dot(normal);
            return *this - normal * (2 * dot);
        }

        // ’·‚³§ŒÀ (Clamp)
        VectorBase ClampMagnitude(T maxLength) const {
            T len = Length();
            if (len > maxLength) {
                return Normalize() * maxLength;
            }
            return *this;
        }

        // Ã“IŠÖ”: ‚‘¬•½•ûª‚Ì‹t”
        static T FastInverseSqrt(T number) {
            T x2 = number * 0.5f;
            T y = number;
            long i = *(long *)&y;
            i = 0x5f3759df - (i >> 1);
            y = *(T *)&i;
            y = y * (1.5f - (x2 * y * y));
            return y;
        }

        // ƒfƒoƒbƒO—p•¶Žš—ñ
        virtual std::string ToString() const {
            std::ostringstream oss;
            oss << "(" << x << ", " << y << ")";
            return oss.str();
        }
    };
}