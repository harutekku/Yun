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
#include <utility>

namespace Yun::VM::Instructions {

enum class Opcode : uint8_t {
    // Arithmetic
    i32neg, i32add, i32sub, i32mul, i32div, i32rem, i32and, i32or, i32xor, i32shl, i32shr,
    i64neg, i64add, i64sub, i64mul, i64div, i64rem, i64and, i64or, i64xor, i64shl, i64shr,

    u32add, u32sub, u32mul, u32div, u32rem, u32and, u32or, u32xor, u32shl, u32shr,
    u64add, u64sub, u64mul, u64div, u64rem, u64and, u64or, u64xor, u64shl, u64shr,

    f32neg, f32add, f32sub, f32mul, f32div, f32rem,
    f64neg, f64add, f64sub, f64mul, f64div, f64rem,

    bnot,

    // Conversion
    convi32toi8,  convi32toi16,
    convu32tou8,  convu32tou16,
    convi32toi64, convi32tou64, convi32tou32, convi32tof32, convi32tof64,
    convi64toi32, convi64tou32, convi64tou64, convi64tof32, convi64tof64,
    convu32toi64, convu32tou64, convu32toi32, convu32tof32, convu32tof64,
    convu64toi64, convu64tou32, convu64toi32, convu64tof32, convu64tof64,
    convf32toi32, convf32toi64, convf32tou32, convf32tof64, convf32tou64,
    convf64toi32, convf64toi64, convf64tou32, convf64tou64, convf64tof32,

    // Logic
    cmp, icmp, fcmp,
    land, lor, lnot,

    // Jumps
    jmp,
    je, jne,
    jgt, jge, jlt, jle,

    // Routine calls
    call,
    ret,

    // Constants - for now, only numbers
    ldconst,

    // Misc
    nop,
    hlt
};

[[nodiscard]] constexpr auto OpcodeCount(Opcode op) -> int {
    using enum Opcode;
    switch (op) {
    case i32add:
    case i32sub:
    case i32mul:
    case i32div:
    case i32rem:
    case i32and:
    case i32or:
    case i32xor:
    case i32shl:
    case i32shr:
    case i64add:
    case i64sub:
    case i64mul:
    case i64div:
    case i64rem:
    case i64and:
    case i64or:
    case i64xor:
    case i64shl:
    case i64shr:
    case u32add:
    case u32sub:
    case u32mul:
    case u32div:
    case u32rem:
    case u32and:
    case u32or:
    case u32xor:
    case u32shl:
    case u32shr:
    case u64add:
    case u64sub:
    case u64mul:
    case u64div:
    case u64rem:
    case u64and:
    case u64or:
    case u64xor:
    case u64shl:
    case u64shr:
    case f32add:
    case f32sub:
    case f32mul:
    case f32div:
    case f32rem:
    case f64add:
    case f64sub:
    case f64mul:
    case f64div:
    case f64rem:
    case cmp:
    case icmp:
    case fcmp:
    case land:
    case lor:
    case ldconst:
        return 2;
    case bnot:
    case i32neg:
    case i64neg:
    case f32neg:
    case f64neg:
    case lnot:
    case convi32toi8:
    case convi32toi16:
    case convu32tou8:
    case convu32tou16:
    case convi32toi64:
    case convi32tou64:
    case convi32tou32:
    case convi32tof32:
    case convi32tof64:
    case convi64toi32:
    case convi64tou32:
    case convi64tou64:
    case convi64tof32:
    case convi64tof64:
    case convu32toi64:
    case convu32tou64:
    case convu32toi32:
    case convu32tof32:
    case convu32tof64:
    case convu64toi64:
    case convu64tou32:
    case convu64toi32:
    case convu64tof32:
    case convu64tof64:
    case convf32toi32:
    case convf32toi64:
    case convf32tou32:
    case convf32tof64:
    case convf32tou64:
    case convf64toi32:
    case convf64toi64:
    case convf64tou32:
    case convf64tou64:
    case convf64tof32:
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
    case i32add:
    case i32sub:
    case i32mul:
    case i32div:
    case i32rem:
    case i32and:
    case i32or:
    case i32xor:
    case i32shl:
    case i32shr:
    case i64add:
    case i64sub:
    case i64mul:
    case i64div:
    case i64rem:
    case i64and:
    case i64or:
    case i64xor:
    case i64shl:
    case i64shr:
    case u32add:
    case u32sub:
    case u32mul:
    case u32div:
    case u32rem:
    case u32and:
    case u32or:
    case u32xor:
    case u32shl:
    case u32shr:
    case u64add:
    case u64sub:
    case u64mul:
    case u64div:
    case u64rem:
    case u64and:
    case u64or:
    case u64xor:
    case u64shl:
    case u64shr:
    case f32add:
    case f32sub:
    case f32mul:
    case f32div:
    case f32rem:
    case f64add:
    case f64sub:
    case f64mul:
    case f64div:
    case f64rem:
    case cmp:
    case icmp:
    case fcmp:
    case land:
    case lor:
    case ldconst:
        return 5;
    case i32neg:
    case i64neg:
    case f32neg:
    case f64neg:
    case bnot:
    case lnot:
    case convi32toi8:
    case convi32toi16:
    case convu32tou8:
    case convu32tou16:
    case convi32toi64:
    case convi32tou64:
    case convi32tou32:
    case convi32tof32:
    case convi32tof64:
    case convi64toi32:
    case convi64tou32:
    case convi64tou64:
    case convi64tof32:
    case convi64tof64:
    case convu32toi64:
    case convu32tou64:
    case convu32toi32:
    case convu32tof32:
    case convu32tof64:
    case convu64toi64:
    case convu64tou32:
    case convu64toi32:
    case convu64tof32:
    case convu64tof64:
    case convf32toi32:
    case convf32toi64:
    case convf32tou32:
    case convf32tof64:
    case convf32tou64:
    case convf64toi32:
    case convf64toi64:
    case convf64tou32:
    case convf64tou64:
    case convf64tof32:
        return 3;
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

[[nodiscard]] constexpr auto OpcodeCountAndSize(Opcode op) -> std::pair<int, int> {
    using enum Opcode;
    switch (op) {
    case i32add:
    case i32sub:
    case i32mul:
    case i32div:
    case i32rem:
    case i32and:
    case i32or:
    case i32xor:
    case i32shl:
    case i32shr:
    case i64add:
    case i64sub:
    case i64mul:
    case i64div:
    case i64rem:
    case i64and:
    case i64or:
    case i64xor:
    case i64shl:
    case i64shr:
    case u32add:
    case u32sub:
    case u32mul:
    case u32div:
    case u32rem:
    case u32and:
    case u32or:
    case u32xor:
    case u32shl:
    case u32shr:
    case u64add:
    case u64sub:
    case u64mul:
    case u64div:
    case u64rem:
    case u64and:
    case u64or:
    case u64xor:
    case u64shl:
    case u64shr:
    case f32add:
    case f32sub:
    case f32mul:
    case f32div:
    case f32rem:
    case f64add:
    case f64sub:
    case f64mul:
    case f64div:
    case f64rem:
    case cmp:
    case icmp:
    case fcmp:
    case land:
    case lor:
    case ldconst:
        return { 2, 5 };
    case i32neg:
    case i64neg:
    case f32neg:
    case f64neg:
    case lnot:
    case bnot:
    case convi32toi8:
    case convi32toi16:
    case convu32tou8:
    case convu32tou16:
    case convi32toi64:
    case convi32tou64:
    case convi32tou32:
    case convi32tof32:
    case convi32tof64:
    case convi64toi32:
    case convi64tou32:
    case convi64tou64:
    case convi64tof32:
    case convi64tof64:
    case convu32toi64:
    case convu32tou64:
    case convu32toi32:
    case convu32tof32:
    case convu32tof64:
    case convu64toi64:
    case convu64tou32:
    case convu64toi32:
    case convu64tof32:
    case convu64tof64:
    case convf32toi32:
    case convf32toi64:
    case convf32tou32:
    case convf32tof64:
    case convf32tou64:
    case convf64toi32:
    case convf64toi64:
    case convf64tou32:
    case convf64tou64:
    case convf64tof32:
        return { 1, 3 };
    case jmp:
    case je:
    case jne:
    case jgt:
    case jge:
    case jlt:
    case jle:
        return { 1, 5 };
    case nop:
    case hlt:
        return { 0, 1 };
    default:
        return { -1, -1 };
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
    case i32neg:
        return "i32neg";
    case i32add:
        return "i32add";
    case i32sub:
        return "i32sub";
    case i32mul:
        return "i32mul";
    case i32div:
        return "i32div";
    case i32rem:
        return "i32rem";
    case i32and:
        return "i32and";
    case i32or:
        return "i32or";
    case i32xor:
        return "i32xor";
    case i32shl:
        return "i32shl";
    case i32shr:
        return "i32shr";
    case i64neg:
        return "i64neg";
    case i64add:
        return "i64add";
    case i64sub:
        return "i64sub";
    case i64mul:
        return "i64mul";
    case i64div:
        return "i64div";
    case i64rem:
        return "i64rem";
    case i64and:
        return "i64and";
    case i64or:
        return "i64or";
    case i64xor:
        return "i64xor";
    case i64shl:
        return "i64shl";
    case i64shr:
        return "i64shr";
    case u32add:
        return "u32add";
    case u32sub:
        return "u32sub";
    case u32mul:
        return "u32mul";
    case u32div:
        return "u32div";
    case u32rem:
        return "u32rem";
    case u32and:
        return "u32and";
    case u32or:
        return "u32or";
    case u32xor:
        return "u32xor";
    case u32shl:
        return "u32shl";
    case u32shr:
        return "u32shr";
    case u64add:
        return "u64add";
    case u64sub:
        return "u64sub";
    case u64mul:
        return "u64mul";
    case u64div:
        return "u64div";
    case u64rem:
        return "u64rem";
    case u64and:
        return "u64and";
    case u64or:
        return "u64or";
    case u64xor:
        return "u64xor";
    case u64shl:
        return "u64shl";
    case u64shr:
        return "u64shr";
    case f32neg:
        return "f32neg";
    case f32add:
        return "f32add";
    case f32sub:
        return "f32sub";
    case f32mul:
        return "f32mul";
    case f32div:
        return "f32div";
    case f32rem:
        return "f32rem";
    case f64neg:
        return "f64neg";
    case f64add:
        return "f64add";
    case f64sub:
        return "f64sub";
    case f64mul:
        return "f64mul";
    case f64div:
        return "f64div";
    case f64rem:
        return "f64rem";
    case bnot:
        return "bnot";
    case convi32toi8:
        return "convi32toi8";
    case convi32toi16:
        return "convi32toi16";
    case convu32tou8:
        return "convu32tou8";
    case convu32tou16:
        return "convu32tou16";
    case convi32toi64:
        return "convi32toi64";
    case convi32tou64:
        return "convi32tou64";
    case convi32tou32:
        return "convi32tou32";
    case convi32tof32:
        return "convi32tof32";
    case convi32tof64:
        return "convi32tof64";
    case convi64toi32:
        return "convi64toi32";
    case convi64tou32:
        return "convi64tou32";
    case convi64tou64:
        return "convi64tou64";
    case convi64tof32:
        return "convi64tof32";
    case convi64tof64:
        return "convi64tof64";
    case convu32toi64:
        return "convu32toi64";
    case convu32tou64:
        return "convu32tou64";
    case convu32toi32:
        return "convu32toi32";
    case convu32tof32:
        return "convu32tof32";
    case convu32tof64:
        return "convu32tof64";
    case convu64toi64:
        return "convu64toi64";
    case convu64tou32:
        return "convu64tou32";
    case convu64toi32:
        return "convu64toi32";
    case convu64tof32:
        return "convu64tof32";
    case convu64tof64:
        return "convu64tof64";
    case convf32toi32:
        return "convf32toi32";
    case convf32toi64:
        return "convf32toi64";
    case convf32tou32:
        return "convf32tou32";
    case convf32tof64:
        return "convf32tof64";
    case convf32tou64:
        return "convf32tou64";
    case convf64toi32:
        return "convf64toi32";
    case convf64toi64:
        return "convf64toi64";
    case convf64tou32:
        return "convf64tou32";
    case convf64tou64:
        return "convf64tou64";
    case convf64tof32:
        return "convf64tof32";
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
    case call:
        return "call";
    case ret:
        return "ret";
    case ldconst:
        return "ldconst";
    case nop:
        return "nop";
    case hlt:
        return "hlt";
    default:
        return "<err>";
    }
}

}

#endif