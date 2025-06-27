#pragma once
#include <string>
#include <vector>
namespace ym
{
    // PropertyProvider.h

    struct PropertyInfo {
        std::string displayName;   // 表示名（例："Position.x"）
        float *valuePtr;      // 実際のデータポインタ（floatに固定）
    };

    class IPropertyProvider {
    public:
        virtual std::string GetProviderName() const = 0;          // 例："Transform"
        virtual std::vector<PropertyInfo> GetProperties() = 0;    // アニメーション可能プロパティ
        virtual ~IPropertyProvider() = default;
    };

}