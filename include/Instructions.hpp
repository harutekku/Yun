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

    // Jumps
    jmp,
    je, jne,
    jgt, jge, jlt, jle,

    // Routine calls
    call,
    ret,

    // Constants - for now, only numbers
    ldconst,
    mov,

    // Array instructions
    newarray,
    arraycount,
    load,
    store,
    advance,

    // Misc
    printreg,
    nop,
    hlt
};

[[nodiscard]] constexpr auto OpcodeCount(Opcode op) noexcept -> int {
    switch (op) {
    case Opcode::i32add:
    case Opcode::i32sub:
    case Opcode::i32mul:
    case Opcode::i32div:
    case Opcode::i32rem:
    case Opcode::i32and:
    case Opcode::i32or:
    case Opcode::i32xor:
    case Opcode::i32shl:
    case Opcode::i32shr:
    case Opcode::i64add:
    case Opcode::i64sub:
    case Opcode::i64mul:
    case Opcode::i64div:
    case Opcode::i64rem:
    case Opcode::i64and:
    case Opcode::i64or:
    case Opcode::i64xor:
    case Opcode::i64shl:
    case Opcode::i64shr:
    case Opcode::u32add:
    case Opcode::u32sub:
    case Opcode::u32mul:
    case Opcode::u32div:
    case Opcode::u32rem:
    case Opcode::u32and:
    case Opcode::u32or:
    case Opcode::u32xor:
    case Opcode::u32shl:
    case Opcode::u32shr:
    case Opcode::u64add:
    case Opcode::u64sub:
    case Opcode::u64mul:
    case Opcode::u64div:
    case Opcode::u64rem:
    case Opcode::u64and:
    case Opcode::u64or:
    case Opcode::u64xor:
    case Opcode::u64shl:
    case Opcode::u64shr:
    case Opcode::f32add:
    case Opcode::f32sub:
    case Opcode::f32mul:
    case Opcode::f32div:
    case Opcode::f32rem:
    case Opcode::f64add:
    case Opcode::f64sub:
    case Opcode::f64mul:
    case Opcode::f64div:
    case Opcode::f64rem:
    case Opcode::cmp:
    case Opcode::icmp:
    case Opcode::fcmp:
    case Opcode::ldconst:
    case Opcode::mov:
    case Opcode::newarray:
    case Opcode::load:
    case Opcode::store:
    case Opcode::advance:
    case Opcode::arraycount:
        return 2;
    case Opcode::bnot:
    case Opcode::i32neg:
    case Opcode::i64neg:
    case Opcode::f32neg:
    case Opcode::f64neg:
    case Opcode::convi32toi8:
    case Opcode::convi32toi16:
    case Opcode::convu32tou8:
    case Opcode::convu32tou16:
    case Opcode::convi32toi64:
    case Opcode::convi32tou64:
    case Opcode::convi32tou32:
    case Opcode::convi32tof32:
    case Opcode::convi32tof64:
    case Opcode::convi64toi32:
    case Opcode::convi64tou32:
    case Opcode::convi64tou64:
    case Opcode::convi64tof32:
    case Opcode::convi64tof64:
    case Opcode::convu32toi64:
    case Opcode::convu32tou64:
    case Opcode::convu32toi32:
    case Opcode::convu32tof32:
    case Opcode::convu32tof64:
    case Opcode::convu64toi64:
    case Opcode::convu64tou32:
    case Opcode::convu64toi32:
    case Opcode::convu64tof32:
    case Opcode::convu64tof64:
    case Opcode::convf32toi32:
    case Opcode::convf32toi64:
    case Opcode::convf32tou32:
    case Opcode::convf32tof64:
    case Opcode::convf32tou64:
    case Opcode::convf64toi32:
    case Opcode::convf64toi64:
    case Opcode::convf64tou32:
    case Opcode::convf64tou64:
    case Opcode::convf64tof32:
    case Opcode::jmp:
    case Opcode::je:
    case Opcode::jne:
    case Opcode::jgt:
    case Opcode::jge:
    case Opcode::jlt:
    case Opcode::jle:
    case Opcode::call:
    case Opcode::printreg:
        return 1;
    case Opcode::nop:
    case Opcode::hlt:
    case Opcode::ret:
        return 0;
    default:
        return -1;
    }
}

[[nodiscard]] constexpr auto IsJump(Opcode op) noexcept -> bool {
    switch (op) {
    case Opcode::jmp:
    case Opcode::je:
    case Opcode::jne:
    case Opcode::jgt:
    case Opcode::jge:
    case Opcode::jlt:
    case Opcode::jle:
        return true;
    default:
        return false;
    }
}

[[nodiscard]] constexpr auto OpcodeToString(Opcode op) noexcept -> const char* {
    switch (op) {
    case Opcode::i32neg:
        return "i32neg";
    case Opcode::i32add:
        return "i32add";
    case Opcode::i32sub:
        return "i32sub";
    case Opcode::i32mul:
        return "i32mul";
    case Opcode::i32div:
        return "i32div";
    case Opcode::i32rem:
        return "i32rem";
    case Opcode::i32and:
        return "i32and";
    case Opcode::i32or:
        return "i32or";
    case Opcode::i32xor:
        return "i32xor";
    case Opcode::i32shl:
        return "i32shl";
    case Opcode::i32shr:
        return "i32shr";
    case Opcode::i64neg:
        return "i64neg";
    case Opcode::i64add:
        return "i64add";
    case Opcode::i64sub:
        return "i64sub";
    case Opcode::i64mul:
        return "i64mul";
    case Opcode::i64div:
        return "i64div";
    case Opcode::i64rem:
        return "i64rem";
    case Opcode::i64and:
        return "i64and";
    case Opcode::i64or:
        return "i64or";
    case Opcode::i64xor:
        return "i64xor";
    case Opcode::i64shl:
        return "i64shl";
    case Opcode::i64shr:
        return "i64shr";
    case Opcode::u32add:
        return "u32add";
    case Opcode::u32sub:
        return "u32sub";
    case Opcode::u32mul:
        return "u32mul";
    case Opcode::u32div:
        return "u32div";
    case Opcode::u32rem:
        return "u32rem";
    case Opcode::u32and:
        return "u32and";
    case Opcode::u32or:
        return "u32or";
    case Opcode::u32xor:
        return "u32xor";
    case Opcode::u32shl:
        return "u32shl";
    case Opcode::u32shr:
        return "u32shr";
    case Opcode::u64add:
        return "u64add";
    case Opcode::u64sub:
        return "u64sub";
    case Opcode::u64mul:
        return "u64mul";
    case Opcode::u64div:
        return "u64div";
    case Opcode::u64rem:
        return "u64rem";
    case Opcode::u64and:
        return "u64and";
    case Opcode::u64or:
        return "u64or";
    case Opcode::u64xor:
        return "u64xor";
    case Opcode::u64shl:
        return "u64shl";
    case Opcode::u64shr:
        return "u64shr";
    case Opcode::f32neg:
        return "f32neg";
    case Opcode::f32add:
        return "f32add";
    case Opcode::f32sub:
        return "f32sub";
    case Opcode::f32mul:
        return "f32mul";
    case Opcode::f32div:
        return "f32div";
    case Opcode::f32rem:
        return "f32rem";
    case Opcode::f64neg:
        return "f64neg";
    case Opcode::f64add:
        return "f64add";
    case Opcode::f64sub:
        return "f64sub";
    case Opcode::f64mul:
        return "f64mul";
    case Opcode::f64div:
        return "f64div";
    case Opcode::f64rem:
        return "f64rem";
    case Opcode::bnot:
        return "bnot";
    case Opcode::convi32toi8:
        return "convi32toi8";
    case Opcode::convi32toi16:
        return "convi32toi16";
    case Opcode::convu32tou8:
        return "convu32tou8";
    case Opcode::convu32tou16:
        return "convu32tou16";
    case Opcode::convi32toi64:
        return "convi32toi64";
    case Opcode::convi32tou64:
        return "convi32tou64";
    case Opcode::convi32tou32:
        return "convi32tou32";
    case Opcode::convi32tof32:
        return "convi32tof32";
    case Opcode::convi32tof64:
        return "convi32tof64";
    case Opcode::convi64toi32:
        return "convi64toi32";
    case Opcode::convi64tou32:
        return "convi64tou32";
    case Opcode::convi64tou64:
        return "convi64tou64";
    case Opcode::convi64tof32:
        return "convi64tof32";
    case Opcode::convi64tof64:
        return "convi64tof64";
    case Opcode::convu32toi64:
        return "convu32toi64";
    case Opcode::convu32tou64:
        return "convu32tou64";
    case Opcode::convu32toi32:
        return "convu32toi32";
    case Opcode::convu32tof32:
        return "convu32tof32";
    case Opcode::convu32tof64:
        return "convu32tof64";
    case Opcode::convu64toi64:
        return "convu64toi64";
    case Opcode::convu64tou32:
        return "convu64tou32";
    case Opcode::convu64toi32:
        return "convu64toi32";
    case Opcode::convu64tof32:
        return "convu64tof32";
    case Opcode::convu64tof64:
        return "convu64tof64";
    case Opcode::convf32toi32:
        return "convf32toi32";
    case Opcode::convf32toi64:
        return "convf32toi64";
    case Opcode::convf32tou32:
        return "convf32tou32";
    case Opcode::convf32tof64:
        return "convf32tof64";
    case Opcode::convf32tou64:
        return "convf32tou64";
    case Opcode::convf64toi32:
        return "convf64toi32";
    case Opcode::convf64toi64:
        return "convf64toi64";
    case Opcode::convf64tou32:
        return "convf64tou32";
    case Opcode::convf64tou64:
        return "convf64tou64";
    case Opcode::convf64tof32:
        return "convf64tof32";
    case Opcode::cmp:
        return "cmp";
    case Opcode::icmp:
        return "icmp";
    case Opcode::fcmp:
        return "fcmp";
    case Opcode::jmp:
        return "jmp";
    case Opcode::je:
        return "je";
    case Opcode::jne:
        return "jne";
    case Opcode::jgt:
        return "jgt";
    case Opcode::jge:
        return "jge";
    case Opcode::jlt:
        return "jlt";
    case Opcode::jle:
        return "jle";
    case Opcode::call:
        return "call";
    case Opcode::ret:
        return "ret";
    case Opcode::ldconst:
        return "ldconst";
    case Opcode::mov:
        return "mov";
    case Opcode::newarray:
        return "newarray";
    case Opcode::arraycount:
        return "arraycount";
    case Opcode::load:
        return "load";
    case Opcode::store:
        return "store";
    case Opcode::advance:
        return "advance";
    case Opcode::printreg:
        return "printreg";
    case Opcode::nop:
        return "nop";
    case Opcode::hlt:
        return "hlt";
    default:
        return "<err>";
    }
}

}

#endif