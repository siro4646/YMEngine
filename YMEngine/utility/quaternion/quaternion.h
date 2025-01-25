#pragma once
#include <cmath>
namespace ym
{
    class Quaternion {
    public:
        float x, y, z, w;

        // �R���X�g���N�^
        Quaternion() : x(0), y(0), z(0), w(1) {}
        Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

        // �N�H�[�^�j�I���̐��K��
        void Normalize() {
            float magnitude = std::sqrt(x * x + y * y + z * z + w * w);
            if (magnitude > 0.0f) {
                float invMag = 1.0f / magnitude;
                x *= invMag;
                y *= invMag;
                z *= invMag;
                w *= invMag;
            }
        }

        // �N�H�[�^�j�I�����m�̏�Z
        Quaternion operator*(const Quaternion &q) const {
            return Quaternion(
                w * q.x + x * q.w + y * q.z - z * q.y,
                w * q.y - x * q.z + y * q.w + z * q.x,
                w * q.z + x * q.y - y * q.x + z * q.w,
                w * q.w - x * q.x - y * q.y - z * q.z
            );
        }

        // �x�N�g�����N�H�[�^�j�I���ŉ�]
        Vector3 Rotate(const Vector3 &v) const {
            Quaternion vecQuat(v.x, v.y, v.z, 0);
            Quaternion result = (*this) * vecQuat * Inverse();
            return Vector3(result.x, result.y, result.z);
        }

        // �N�H�[�^�j�I���̋t�i���� / �m�����Ŋ���j
        Quaternion Inverse() const {
            return Quaternion(-x, -y, -z, w);
        }

        // ��]�p�Ǝ�����N�H�[�^�j�I���𐶐�
        static Quaternion FromAxisAngle(const Vector3 &axis, float angleDegrees) {
            float angleRadians = angleDegrees * (3.14159265358979323846f / 180.0f);
            float halfAngle = angleRadians * 0.5f;
            float sinHalfAngle = std::sin(halfAngle);

            return Quaternion(
                axis.x * sinHalfAngle,
                axis.y * sinHalfAngle,
                axis.z * sinHalfAngle,
                std::cos(halfAngle)
            );
        }
		static Quaternion FromEulerAngles(const Vector3 &eulerAngles) {
			float c1 = std::cos(eulerAngles.y * 0.5f);
			float c2 = std::cos(eulerAngles.x * 0.5f);
			float c3 = std::cos(eulerAngles.z * 0.5f);
			float s1 = std::sin(eulerAngles.y * 0.5f);
			float s2 = std::sin(eulerAngles.x * 0.5f);
			float s3 = std::sin(eulerAngles.z * 0.5f);

			return Quaternion(
				s1 * c2 * c3 + c1 * s2 * s3,
				c1 * s2 * c3 - s1 * c2 * s3,
				c1 * c2 * s3 + s1 * s2 * c3,
				c1 * c2 * c3 - s1 * s2 * s3
			);
		}

        // ��]�s�񂩂�N�H�[�^�j�I���𐶐�
        static Quaternion FromRotationMatrix(const XMMATRIX &matrix) {
            float trace = matrix.r[0].m128_f32[0] + matrix.r[1].m128_f32[1] + matrix.r[2].m128_f32[2];

            if (trace > 0.0f) {
                float s = std::sqrt(trace + 1.0f) * 2.0f;
                return Quaternion(
                    (matrix.r[2].m128_f32[1] - matrix.r[1].m128_f32[2]) / s,
                    (matrix.r[0].m128_f32[2] - matrix.r[2].m128_f32[0]) / s,
                    (matrix.r[1].m128_f32[0] - matrix.r[0].m128_f32[1]) / s,
                    0.25f * s
                );
            }
            else if (matrix.r[0].m128_f32[0] > matrix.r[1].m128_f32[1] && matrix.r[0].m128_f32[0] > matrix.r[2].m128_f32[2]) {
                float s = std::sqrt(1.0f + matrix.r[0].m128_f32[0] - matrix.r[1].m128_f32[1] - matrix.r[2].m128_f32[2]) * 2.0f;
                return Quaternion(
                    0.25f * s,
                    (matrix.r[0].m128_f32[1] + matrix.r[1].m128_f32[0]) / s,
                    (matrix.r[0].m128_f32[2] + matrix.r[2].m128_f32[0]) / s,
                    (matrix.r[2].m128_f32[1] - matrix.r[1].m128_f32[2]) / s
                );
            }
            else if (matrix.r[1].m128_f32[1] > matrix.r[2].m128_f32[2]) {
                float s = std::sqrt(1.0f + matrix.r[1].m128_f32[1] - matrix.r[0].m128_f32[0] - matrix.r[2].m128_f32[2]) * 2.0f;
                return Quaternion(
                    (matrix.r[0].m128_f32[1] + matrix.r[1].m128_f32[0]) / s,
                    0.25f * s,
                    (matrix.r[1].m128_f32[2] + matrix.r[2].m128_f32[1]) / s,
                    (matrix.r[0].m128_f32[2] - matrix.r[2].m128_f32[0]) / s
                );
            }
            else {
                float s = std::sqrt(1.0f + matrix.r[2].m128_f32[2] - matrix.r[0].m128_f32[0] - matrix.r[1].m128_f32[1]) * 2.0f;
                return Quaternion(
                    (matrix.r[0].m128_f32[2] + matrix.r[2].m128_f32[0]) / s,
                    (matrix.r[1].m128_f32[2] + matrix.r[2].m128_f32[1]) / s,
                    0.25f * s,
                    (matrix.r[1].m128_f32[0] - matrix.r[0].m128_f32[1]) / s
                );
            }
        }

        // �N�H�[�^�j�I������]�s��ɕϊ�
        XMMATRIX ToRotationMatrix() const {
            float xx = x * x;
            float yy = y * y;
            float zz = z * z;
            float xy = x * y;
            float xz = x * z;
            float yz = y * z;
            float wx = w * x;
            float wy = w * y;
            float wz = w * z;

            return XMMATRIX(
                XMVECTOR{ 1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz),       2.0f * (xz + wy),       0.0f },
                XMVECTOR{ 2.0f * (xy + wz),       1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx),       0.0f },
                XMVECTOR{ 2.0f * (xz - wy),       2.0f * (yz + wx),       1.0f - 2.0f * (xx + yy), 0.0f },
                XMVECTOR{ 0.0f,                   0.0f,                   0.0f,                   1.0f }
            );
        }
		XMVECTOR ToXMVECTOR() const {
			return XMVectorSet(x, y, z, w);
		}
        // �I�C���[�p���擾 (Pitch, Yaw, Roll)
        Vector3 ToEulerAngles() const {
            Vector3 euler;

            // �s�b�`�iX����]�j
            float sinPitch = 2.0f * (w * x + y * z);
            float cosPitch = 1.0f - 2.0f * (x * x + y * y);
            euler.x = std::atan2(sinPitch, cosPitch);

            // ���[�iY����]�j
            float sinYaw = 2.0f * (w * y - z * x);
            if (std::abs(sinYaw) >= 1) {
                euler.y = std::copysign(3.14159265358979323846f / 2.0f, sinYaw); // �}90�x�i�N�����v�j
            }
            else {
                euler.y = std::asin(sinYaw);
            }

            // ���[���iZ����]�j
            float sinRoll = 2.0f * (w * z + x * y);
            float cosRoll = 1.0f - 2.0f * (y * y + z * z);
            euler.z = std::atan2(sinRoll, cosRoll);

            // ���W�A����x�ɕϊ�
            euler.x = XMConvertToDegrees(euler.x);
            euler.y = XMConvertToDegrees(euler.y);
            euler.z = XMConvertToDegrees(euler.z);

            return euler;
        }
    };
}