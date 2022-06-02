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
#include <cmath>
// C++ header files
#include <string>
#include <type_traits>
// My header files
#include "Exceptions.hpp"

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
        [[nodiscard]] constexpr auto Is() const noexcept -> Type {
            return _type;
        }

        template<typename T>
        [[nodiscard]] auto As() noexcept -> T&;

        template<typename T>
        [[nodiscard]] auto As() const noexcept -> const T&;

        auto Assign(const Value& value) noexcept -> void {
            _type = value._type;
            std::memcpy(&uint64, &value.uint64, sizeof(uint64));
        }
    
    public:  // Arithmetic operations
        template<typename T, typename = typename std::enable_if_t<std::is_signed_v<T> || std::is_floating_point_v<T>, T>>
        constexpr auto Neg() -> void {
            if (_type != TAsEnum<T>())
                throw Error::TypeError{ "Bad type" };
            As<T>() = -As<T>();
        } 

        template<typename T>
        constexpr auto Add(const Value& value) -> void {
            if (Is() != value.Is() || _type != TAsEnum<T>() )
                throw Error::TypeError{ "Incompatible types" };
            As<T>() += value.As<T>();
        }

        template<typename T>
        constexpr auto Sub(const Value& value) -> void {
            if (Is() != value.Is() || _type != TAsEnum<T>())
                throw Error::TypeError{ "Incompatible types" };
            As<T>() -= value.As<T>();
        }

        template<typename T>
        constexpr auto Mul(const Value& value) -> void {
            if (Is() != value.Is() || _type != TAsEnum<T>())
                throw Error::TypeError{ "Incompatible types" };
            As<T>() *= value.As<T>();
        }

        template<typename T>
        constexpr auto Div(const Value& value) -> void {
            if (Is() != value.Is() || _type != TAsEnum<T>())
                throw Error::TypeError{ "Incompatible types" };
            if constexpr (std::is_integral_v<T>)
                if (value.As<T>() == 0)
                    throw Error::IntegerArithmeticError{ "Division by zero" };
            As<T>() /= value.As<T>();
        }

        template<typename T>
        constexpr auto Rem(const Value& value) -> void {
            if (Is() != value.Is() || _type != TAsEnum<T>())
                throw Error::TypeError{ "Incompatible types" };
            if constexpr (std::is_integral_v<T>) {
                if (value.As<T>() == 0)
                    throw Error::IntegerArithmeticError{ "Remainder by zero" };
                As<T>() %= value.As<T>();
                return;
            }
            As<T>() = std::remainder(As<T>(), value.As<T>());
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<T>, T>>
        constexpr auto And(const Value& value) -> void {
            if (Is() != value.Is() || _type != TAsEnum<T>())
                throw Error::TypeError{ "Incompatible types" };
            As<T>() &= value.As<T>();
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<T>, T>>
        constexpr auto Or(const Value& value) -> void {
            if (Is() != value.Is() || _type != TAsEnum<T>())
                throw Error::TypeError{ "Incompatible types" };
            As<T>() |= value.As<T>();
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<T>, T>>
        constexpr auto Xor(const Value& value) -> void {
            if (Is() != value.Is() || _type != TAsEnum<T>())
                throw Error::TypeError{ "Incompatible types" };
            As<T>() ^= value.As<T>();
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<T>, T>>
        constexpr auto ShiftLeft(const Value& value) -> void {
            if (value.Is() != Type::Uint32 || _type != TAsEnum<T>())
                throw Error::TypeError{ "Incompatible types" };
            As<T>() <<= value.uint32;
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<T>, T>>
        constexpr auto ShiftRight(const Value& value) -> void {
            if (value.Is() != Type::Uint32 || _type == TAsEnum<T>())
                throw Error::TypeError{ "Incompatible types" };
            As<T>() >>= value.uint32;
        }

        constexpr auto Not() -> void {
            if (!IsIntegral())
                throw Error::TypeError{ "Incompatible type" };
            uint64 = ~uint64;
        }

        template<typename T, typename = typename std::enable_if_t<std::is_arithmetic_v<T>, T>>
        [[nodiscard]] auto Compare(const Value& value) -> int32_t {
            if (Is() != value.Is())
                throw Error::TypeError{ "Incompatible types" };

            if constexpr (std::is_signed_v<T>) {
                if (_type == Type::Int32) {
                    if (int32 < value.int32)
                        return -1;
                    else if (int32 > value.int32)
                        return 1;
                    else
                        return 0;
                } else if (_type == Type::Int64) {
                    if (int64 < value.int64)
                        return -1;
                    else if (int64 > value.int64)
                        return 1;
                    else
                        return 0;
                } else
                    throw Error::TypeError{ "Invalid signed type" };
            } else if constexpr (std::is_unsigned_v<T>) {
                if (_type == Type::Uint32) {
                    if (uint32 < value.uint32)
                        return -1;
                    else if (uint32 > value.uint32)
                        return 1;
                    else
                        return 0;
                } else if (_type == Type::Uint64) {
                    if (uint64 < value.uint64)
                        return -1;
                    else if (uint64 > value.uint64)
                        return 1;
                    else
                        return 0;
                } else
                    throw Error::TypeError{ "Invalid unsigned type" };
            } else if constexpr (std::is_floating_point_v<T>) {
                if (_type == Type::Float32) {
                    if (float32 < value.float32)
                        return -1;
                    else if (float32 > value.float32)
                        return 1;
                    else
                        return 0;
                } else {
                    if (float64 < value.float64)
                        return -1;
                    else if (float64 > value.float64)
                        return 1;
                    else
                        return 0;
                }
            }
        }

        template<typename From, typename To>
        constexpr auto Convert() -> void {
            if (_type != TAsEnum<From>())
                throw Error::TypeError{ "Bad cast" };
            _type       = TAsEnum<To>();
            As<From>()  = (To)As<From>();
        }

    private:
        [[nodiscard]] constexpr auto IsSigned() const -> bool {
            return _type == Type::Int32 || _type == Type::Int64;
        }
        [[nodiscard]] constexpr auto IsIntegral() const -> bool {
            return _type == Type::Uint32 ||
                   _type == Type::Uint64 ||
                   _type == Type::Int32  || 
                   _type == Type::Int64;
        } 

        [[nodiscard]] constexpr auto IsFloatingPoint() const -> bool {
            return _type == Type::Float32 ||
                   _type == Type::Float64;
        }

    public:
        [[nodiscard]] auto ToString() const noexcept -> std::string;
    
    private: // Member values
        union {
            int8_t   int8;
            int16_t  int16;
            int32_t  int32;
            int64_t  int64;   // These are the defaults
            uint8_t  uint8;
            uint16_t uint16;
            uint32_t uint32;
            uint64_t uint64;  // These are the defaults
            float    float32;
            double   float64; // These are the defaults
        };
        Type       _type;
};

}

#endif