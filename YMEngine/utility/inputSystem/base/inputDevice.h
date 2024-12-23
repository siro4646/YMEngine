#pragma once
#include <string>
#include <unordered_map>
namespace ym {

    class InputDevice {
    public:
        virtual ~InputDevice() = default;

        virtual bool GetKey(const std::string &key) = 0;      // �L�[��������Ă���
        virtual bool GetKeyDown(const std::string &key) = 0;  // �L�[�������ꂽ�u��
        virtual bool GetKeyUp(const std::string &key) = 0;    // �L�[�������ꂽ�u��
    };

} // namespace ym