/**
 * @file
 *   Value.hpp
 * @author
 *   Harutekku @link https://github.com/harutekku @endlink
 * @brief
 *   Provides a `Value` class, used internaly by Yun virtual machine
 * @version
 *   0.0.1
 * @date
 *   2022-05-24
 * @copyright
 *   Copyright (c) 2022
 * 
 */
#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP

// C header files
#include <cstdint>
#include <cstring>
// C++ header files
#include <string>
#include <type_traits>

namespace Yun::VM::Primitives {

enum class Type : uint8_t {
    Uninit,
    Int8,
    Int16,
    Int32,
    Int64,
    Uint8,
    Uint16,
    Uint32,
    Uint64,
    Float32,
    Float64
};

[[nodiscard]] constexpr auto TypeToString(Type type) noexcept -> const char* {
    using enum Type;
    switch (type) {
    case Uninit:
        return "<uninit>";
    case Int8:
        return "Int8";
    case Int16:
        return "Int16";
    case Int32:
        return "Int32";
    case Int64:
        return "Int64";
    case Uint8:
        return "Uint8";
    case Uint16:
        return "Uint16";
    case Uint32:
        return "Uint32";
    case Uint64:
        return "Uint64";
    case Float32:
        return "Float32";
    case Float64:
        return "Float64";
    default:
        return "<err>";
    }
}

template<typename T>
[[nodiscard]] constexpr auto TAsEnum() -> Type {
    if constexpr (std::is_same_v<T, int8_t>)
        return Type::Int8;
    else if constexpr (std::is_same_v<T, int16_t>)
        return Type::Int16;
    else if constexpr (std::is_same_v<T, int32_t>)
        return Type::Int32;
    else if constexpr (std::is_same_v<T, int64_t>)
        return Type::Int64;
    else if constexpr (std::is_same_v<T, uint8_t>)
        return Type::Uint8;
    else if constexpr (std::is_same_v<T, uint16_t>)
        return Type::Uint16;
    else if constexpr (std::is_same_v<T, uint32_t>)
        return Type::Uint32;
    else if constexpr (std::is_same_v<T, uint64_t>)
        return Type::Uint64;
    else if constexpr (std::is_same_v<T, float>)
        return Type::Float32;
    else if constexpr (std::is_same_v<T, double>)
        return Type::Float64;
}

class Value {
    public:  // Special member functions
        constexpr Value()
            :_type{ Type::Uninit } {
        }

        template<typename T>
        constexpr Value(T value)
            :_type{ TAsEnum<T>() } {
            As<T>() = value;
        }

    public:  // As<T> and Is()
        [[nodiscard]] constexpr auto Is() noexcept -> Type {
            return _type;
        }

        template<typename T>
        [[nodiscard]] auto As() noexcept -> T&;

        template<typename T>
        constexpr auto Assign(T value) noexcept -> void {
            As<T>() = value;
            _type   = TAsEnum<T>();
        }
    
    public:
        [[nodiscard]] auto ToString() noexcept -> std::string;
    
    private: // Member values
        union {
            int8_t   int8;
            int16_t  int16;
            int32_t  int32;
            int64_t  int64;
            uint8_t  uint8;
            uint16_t uint16;
            uint32_t uint32;
            uint64_t uint64;
            float    float32;
            double   float64;
        };
        Type       _type;
};

}

#endif