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
    Float64,
    Reference,
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
    case Reference:
        return "Reference";
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

class Reference {
    public:
        [[nodiscard]] auto ToString() const -> std::string;
    public:
        uint32_t HeapID;
        uint32_t ArrayIndex;
};


class Value {
    public:  // Special member functions
        constexpr Value()
            :_as{  }, _type{ Type::Uninit } {
        }

        template<typename T>
        constexpr Value(T value)
            :_type{ TAsEnum<T>() } {
            As<T>() = value;
        }

        constexpr Value(Type type)
            :_as{  }, _type{ type } {
        }

    public:  // As<T> and Is()
        [[nodiscard]] constexpr auto Is() const noexcept -> Type {
            return _type;
        }

        template<typename T>
        [[nodiscard]] auto As() noexcept -> T&;

        template<typename T>
        [[nodiscard]] auto As() const noexcept -> const T&;

        [[nodiscard]] auto AsPtr() -> void* {
            return &_as;
        }

        auto Assign(const Value& value) noexcept -> void {
            if (this == &value)
                return;
            _type = value._type;
            std::memcpy(&_as, &value._as, sizeof(_as));
        }

        auto Assign(const Reference& ref) noexcept -> void {
            _type = Primitives::Type::Reference;
            _as.ref = ref;
        }
    
    public:  // Arithmetic operations
        template<typename T, typename = typename std::enable_if_t<std::is_signed_v<T> || std::is_floating_point_v<T>, T>>
        constexpr auto Neg() -> void {
            if (_type != TAsEnum<T>())
                throw Error::TypeError{ "Value of this type can't be negated: ", _type };
            As<T>() = -As<T>();
        } 

        template<typename T>
        constexpr auto Add(const Value& value) -> void {
            if (_type != value._type || _type != TAsEnum<T>() )
                throw Error::TypeError{ "Incompatible types: ", _type, value._type };
            As<T>() += value.As<T>();
        }

        template<typename T>
        constexpr auto Sub(const Value& value) -> void {
            if (_type != value._type || _type != TAsEnum<T>() )
                throw Error::TypeError{ "Incompatible types: ", _type, value._type };
            As<T>() -= value.As<T>();
        }

        template<typename T>
        constexpr auto Mul(const Value& value) -> void {
            if (_type != value._type || _type != TAsEnum<T>() )
                throw Error::TypeError{ "Incompatible types: ", _type, value._type };
            As<T>() *= value.As<T>();
        }

        template<typename T>
        constexpr auto Div(const Value& value) -> void {
            if (_type != value._type || _type != TAsEnum<T>() )
                throw Error::TypeError{ "Incompatible types: ", _type, value._type };
            if constexpr (std::is_integral_v<T>)
                if (value.As<T>() == 0)
                    throw Error::IntegerArithmeticError{ "Division by zero" };
            As<T>() /= value.As<T>();
        }

        template<typename T>
        constexpr auto Rem(const Value& value) -> void {
            if (_type != value._type || _type != TAsEnum<T>() )
                throw Error::TypeError{ "Incompatible types: ", _type, value._type };
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
            if (_type != value._type || _type != TAsEnum<T>() )
                throw Error::TypeError{ "Incompatible types: ", _type, value._type };
            As<T>() &= value.As<T>();
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<T>, T>>
        constexpr auto Or(const Value& value) -> void {
            if (_type != value._type || _type != TAsEnum<T>() )
                throw Error::TypeError{ "Incompatible types: ", _type, value._type };
            As<T>() |= value.As<T>();
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<T>, T>>
        constexpr auto Xor(const Value& value) -> void {
            if (_type != value._type || _type != TAsEnum<T>() )
                throw Error::TypeError{ "Incompatible types: ", _type, value._type };
            As<T>() ^= value.As<T>();
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<T>, T>>
        constexpr auto ShiftLeft(const Value& value) -> void {
            if (value._type != Type::Uint32 || _type != TAsEnum<T>() )
                throw Error::TypeError{ "Incompatible types: ", _type, value._type };
            As<T>() <<= value._as.uint32;
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<T>, T>>
        constexpr auto ShiftRight(const Value& value) -> void {
            if (value._type != Type::Uint32 || _type != TAsEnum<T>() )
                throw Error::TypeError{ "Incompatible types: ", _type, value._type };
            As<T>() >>= value._as.uint32;
        }

        constexpr auto Not() -> void {
            if (!IsIntegral())
                throw Error::TypeError{ "Non-intergral type: ", _type };
            _as.uint64 = ~_as.uint64;
        }

        template<typename T, typename = typename std::enable_if_t<std::is_arithmetic_v<T>, T>>
        [[nodiscard]] auto Compare(const Value& value) -> int32_t {
            if (_type != value._type)
                throw Error::TypeError{ "Incompatible types for comparison: ", _type, value._type };

            if constexpr (std::is_signed_v<T>) {
                if (_type == Type::Int32) {
                    if (_as.int32 < value._as.int32)
                        return -1;
                    else if (_as.int32 > value._as.int32)
                        return 1;
                    else
                        return 0;
                } else if (_type == Type::Int64) {
                    if (_as.int64 < value._as.int64)
                        return -1;
                    else if (_as.int64 > value._as.int64)
                        return 1;
                    else
                        return 0;
                } else
                    throw Error::TypeError{ "Can't compare i8 or i16: ", _type };
            } else if constexpr (std::is_unsigned_v<T>) {
                if (_type == Type::Uint32) {
                    if (_as.uint32 < value._as.uint32)
                        return -1;
                    else if (_as.uint32 > value._as.uint32)
                        return 1;
                    else
                        return 0;
                } else if (_type == Type::Uint64) {
                    if (_as.uint64 < value._as.uint64)
                        return -1;
                    else if (_as.uint64 > value._as.uint64)
                        return 1;
                    else
                        return 0;
                } else
                    throw Error::TypeError{ "Can't compare u8 or u16: ", _type };
            } else if constexpr (std::is_floating_point_v<T>) {
                if (_type == Type::Float32) {
                    if (_as.float32 < value._as.float32)
                        return -1;
                    else if (_as.float32 > value._as.float32)
                        return 1;
                    else
                        return 0;
                } else {
                    if (_as.float64 < value._as.float64)
                        return -1;
                    else if (_as.float64 > value._as.float64)
                        return 1;
                    else
                        return 0;
                }
            }
        }

        template<typename From, typename To>
        constexpr auto Convert() -> void {
            if (_type != TAsEnum<From>())
                throw Error::TypeError{ "Invalid type for conversion", _type };
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
            int8_t    int8;
            int16_t   int16;
            int32_t   int32;
            int64_t   int64;   // These are the defaults
            uint8_t   uint8;
            uint16_t  uint16;
            uint32_t  uint32;
            uint64_t  uint64;  // These are the defaults
            float     float32;
            double    float64; // These are the defaults
            Reference ref;
        } _as;
        Type       _type;
};

}

#endif