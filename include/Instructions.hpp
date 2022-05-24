/**
 * @file
 *   Instructions.hpp
 * @author
 *   Harutekku @link https://github.com/harutekku @endlink
 * @brief
 *   Instructions used by other classes
 * @version
 *   0.0.1
 * @date
 *   2022-05-24
 * @copyright
 *   Copyright (c) 2022
 * 
 */
#ifndef INSTRUCTIONS_HPP
#define INSTRUCTIONS_HPP

// C header files
#include <cstddef>
#include <cstdint>
// C++ header files
#include <span>
#include <vector>
#include <memory>

namespace Yun::VM::Instructions {

enum class Opcode : uint8_t {
    add, iadd, fadd,
    sub, isub, fsub,
    mul, imul, fmul,
    div, idiv, fdiv,
    rem, irem, frem,

    cmp, icmp, fcmp,
    land, lor, lnot,

    jmp,
    je, jne,
    jgt, jge, jlt, jle,

    nop,
    hlt // Will always be the last one
};

[[nodiscard]] constexpr auto OpcodeArgumentCount(Opcode op) -> int {
    using enum Opcode;
    switch (op) {
    case add:
    case sub:
    case mul:
    case div:
    case rem:
    case iadd:
    case isub:
    case imul:
    case idiv:
    case irem:
    case fadd:
    case fsub:
    case fmul:
    case fdiv:
    case frem:
    case cmp:
    case icmp:
    case fcmp:
    case land:
    case lor:
        return 2;
    case lnot:
    case jmp:
    case je:
    case jne:
    case jgt:
    case jge:
    case jlt:
    case jle:
        return 1;
    case nop:
    case hlt:
        return 0;
    default:
        return -1;
    }
}

[[nodiscard]] constexpr auto OpcodeSize(Opcode op) -> int {
    using enum Opcode;
    switch (op) {
    case add:
    case sub:
    case mul:
    case div:
    case rem:
    case iadd:
    case isub:
    case imul:
    case idiv:
    case irem:
    case fadd:
    case fsub:
    case fmul:
    case fdiv:
    case frem:
    case cmp:
    case icmp:
    case fcmp:
    case land:
    case lor:
        return 5;
    case lnot:
    case jmp:
    case je:
    case jne:
    case jgt:
    case jge:
    case jlt:
    case jle:
        return 5;
    case nop:
    case hlt:
        return 1;
    default:
        return -1;
    }
}

[[nodiscard]] constexpr auto IsJump(Opcode op) noexcept -> bool {
    using enum Opcode;
    switch (op) {
    case jmp:
    case je:
    case jne:
    case jgt:
    case jge:
    case jlt:
    case jle:
        return true;
    default:
        return false;
    }
}

[[nodiscard]] constexpr auto OpcodeToString(Opcode op) noexcept -> const char* {
    using enum Opcode;
    switch (op) {
    case add:
        return "add";
    case sub:
        return "sub";
    case mul:
        return "mul";
    case div:
        return "div";
    case rem:
        return "rem";
    case iadd:
        return "iadd";
    case isub:
        return "isub";
    case imul:
        return "imul";
    case idiv:
        return "idiv";
    case irem:
        return "irem";
    case fadd:
        return "fadd";
    case fsub:
        return "fsub";
    case fmul:
        return "fmul";
    case fdiv:
        return "fdiv";
    case frem:
        return "frem";
    case cmp:
        return "cmp";
    case icmp:
        return "icmp";
    case fcmp:
        return "fcmp";
    case land:
        return "land";
    case lor:
        return "lor";
    case lnot:
        return "lnot";
    case jmp:
        return "jmp";
    case je:
        return "je";
    case jne:
        return "jne";
    case jgt:
        return "jgt";
    case jge:
        return "jge";
    case jlt:
        return "jlt";
    case jle:
        return "jle";
    case nop:
        return "nop";
    case hlt:
        return "hlt";
    default:
        return "<err>";
    }
}

class Buffer {
    public:
        Buffer(size_t data);

    public:
        [[nodiscard]] auto begin() noexcept -> uint8_t*;
        [[nodiscard]] auto end() noexcept -> uint8_t*;
    public:
        auto Disassemble() -> void;
    
    private:
        std::unique_ptr<std::uint8_t[]> _data;
        std::uint8_t*                   _last;
};

[[nodiscard]] auto begin(Buffer& buffer) noexcept -> uint8_t*;

[[nodiscard]] auto end(Buffer& buffer) noexcept -> uint8_t*;

}

#endif