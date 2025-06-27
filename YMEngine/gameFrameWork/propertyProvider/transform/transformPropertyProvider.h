#pragma once
#include "../propertyProvider.h"
namespace ym
{
    // TransformPropertyProvider.h
    class TransformPropertyProvider : public IPropertyProvider {
    public:
        TransformPropertyProvider(Transform *transform) : transform_(transform) {}

        std::string GetProviderName() const override { return "Transform"; }

        std::vector<PropertyInfo> GetProperties() override {
            std::vector<PropertyInfo> props;
            props.push_back({ "Position.x", &transform_->Position.x });
            props.push_back({ "Position.y", &transform_->Position.y });
            props.push_back({ "Position.z", &transform_->Position.z });
            props.push_back({ "Rotation.x", &transform_->Rotation.x });
            props.push_back({ "Rotation.y", &transform_->Rotation.y });
            props.push_back({ "Rotation.z", &transform_->Rotation.z });
            props.push_back({ "Scale.x", &transform_->Scale.x });
            props.push_back({ "Scale.y", &transform_->Scale.y });
            props.push_back({ "Scale.z", &transform_->Scale.z });
            return props;
        }

    private:
        Transform *transform_;
    };

}