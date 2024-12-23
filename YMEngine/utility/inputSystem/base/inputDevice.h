#pragma once
#include <string>
#include <unordered_map>
namespace ym {

    class InputDevice {
    public:
        virtual ~InputDevice() = default;

        virtual bool GetKey(const std::string &key) = 0;      // キーが押されている
        virtual bool GetKeyDown(const std::string &key) = 0;  // キーが押された瞬間
        virtual bool GetKeyUp(const std::string &key) = 0;    // キーが離された瞬間
    };

} // namespace ym