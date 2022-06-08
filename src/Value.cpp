// C++ header files
#include <string>
// My header files
#include "../include/Value.hpp"

namespace Yun::VM::Primitives {

    [[nodiscard]] auto Reference::ToString() const -> std::string {
        std::string retVal{ "(HeapID: " };
        retVal += std::to_string(HeapID) + ", ArrayIndex: ";
        retVal += std::to_string(ArrayIndex) + ")";
        return retVal;
    }

    template<>
    [[nodiscard]] auto Value::As() noexcept -> int8_t& {
        return _as.int8;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> int16_t& {
        return _as.int16;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> int32_t& {
        return _as.int32;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> int64_t& {
        return _as.int64;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> uint8_t& {
        return _as.uint8;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> uint16_t& {
        return _as.uint16;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> uint32_t& {
        return _as.uint32;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> uint64_t& {
        return _as.uint64;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> float& {
        return _as.float32;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> double& {
        return _as.float64;
    }
    template<>
    [[nodiscard]] auto Value::As() noexcept -> Reference& {
        return _as.ref;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const int8_t& {
        return _as.int8;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const int16_t& {
        return _as.int16;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const int32_t& {
        return _as.int32;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const int64_t& {
        return _as.int64;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const uint8_t& {
        return _as.uint8;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const uint16_t& {
        return _as.uint16;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const uint32_t& {
        return _as.uint32;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const uint64_t& {
        return _as.uint64;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const float& {
        return _as.float32;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const double& {
        return _as.float64;
    }
    template<>
    [[nodiscard]] auto Value::As() const noexcept -> const Reference& {
        return _as.ref;
    }


    [[nodiscard]] auto Value::ToString() const noexcept -> std::string {
        using enum Type;
        std::string retVal{ "(" };

        switch (_type) {
        case Uninit:
            retVal.append("null");
            break;
        case Int8:
            retVal.append(std::to_string(_as.int8));
            break;
        case Int16:
            retVal.append(std::to_string(_as.int16));
            break;
        case Int32:
            retVal.append(std::to_string(_as.int32));
            break;
        case Int64:
            retVal.append(std::to_string(_as.int64));
            break;
        case Uint8:
            retVal.append(std::to_string(_as.uint8));
            break;
        case Uint16:
            retVal.append(std::to_string(_as.uint16));
            break;
        case Uint32:
            retVal.append(std::to_string(_as.uint32));
            break;
        case Uint64:
            retVal.append(std::to_string(_as.uint64));
            break;
        case Float32:
            retVal.append(std::to_string(_as.float32));
            break;
        case Float64:
            retVal.append(std::to_string(_as.float64));
            break;
        case Reference:
            retVal.append(_as.ref.ToString());
            break;
        }
        retVal.append(": ");
        retVal.append(TypeToString(_type));
        retVal.append(")");
        return retVal;
    }
}