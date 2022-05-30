// C++ header files
#include <string>
// My header files
#include "../include/Value.hpp"

namespace Yun::VM::Primitives {
    template<>
    [[nodiscard]] auto Value::As() noexcept -> int8_t& {
        return int8;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> int16_t& {
        return int16;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> int32_t& {
        return int32;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> int64_t& {
        return int64;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> uint8_t& {
        return uint8;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> uint16_t& {
        return uint16;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> uint32_t& {
        return uint32;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> uint64_t& {
        return uint64;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> float& {
        return float32;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> double& {
        return float64;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const int8_t& {
        return int8;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const int16_t& {
        return int16;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const int32_t& {
        return int32;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const int64_t& {
        return int64;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const uint8_t& {
        return uint8;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const uint16_t& {
        return uint16;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const uint32_t& {
        return uint32;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const uint64_t& {
        return uint64;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const float& {
        return float32;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const double& {
        return float64;
    }

    [[nodiscard]] auto Value::ToString() const noexcept -> std::string {
        using enum Type;
        std::string retVal{ "{ value: " };

        switch (_type) {
        case Uninit:
            retVal.append("null");
            break;
        case Int8:
            retVal.append(std::to_string(int8));
            break;
        case Int16:
            retVal.append(std::to_string(int16));
            break;
        case Int32:
            retVal.append(std::to_string(int32));
            break;
        case Int64:
            retVal.append(std::to_string(int64));
            break;
        case Uint8:
            retVal.append(std::to_string(uint8));
            break;
        case Uint16:
            retVal.append(std::to_string(uint16));
            break;
        case Uint32:
            retVal.append(std::to_string(uint32));
            break;
        case Uint64:
            retVal.append(std::to_string(uint64));
            break;
        case Float32:
            retVal.append(std::to_string(float32));
            break;
        case Float64:
            retVal.append(std::to_string(float64));
            break;
        }
        retVal.append(", type: ");
        retVal.append(TypeToString(_type));
        retVal.append(" }");
        return retVal;
    }
}