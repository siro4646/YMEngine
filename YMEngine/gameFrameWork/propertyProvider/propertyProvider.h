#pragma once
#include <string>
#include <vector>
namespace ym
{
    // PropertyProvider.h

    struct PropertyInfo {
        std::string displayName;   // �\�����i��F"Position.x"�j
        float *valuePtr;      // ���ۂ̃f�[�^�|�C���^�ifloat�ɌŒ�j
    };

    class IPropertyProvider {
    public:
        virtual std::string GetProviderName() const = 0;          // ��F"Transform"
        virtual std::vector<PropertyInfo> GetProperties() = 0;    // �A�j���[�V�����\�v���p�e�B
        virtual ~IPropertyProvider() = default;
    };

}