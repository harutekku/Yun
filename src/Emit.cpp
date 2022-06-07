/**
 * @file
 *   Assembler.cpp
 * @author
 *   Harutekku @link https://github.com/harutekku @endlink
 * @brief 
 *   Emitter and Instruction routines implementation
 * @version
 *   0.0.1
 * @date
 *   2022-05-24
 * @copyright
 *   Copyright (c) 2022
 */
// C header files
#include <cstring>
// My header files
#include "../include/Emit.hpp"


namespace Yun::VM::Emit {

auto Instruction::Serialize(uint32_t* buffer) -> void {
    uint32_t instruction = 0;

    instruction |= static_cast<uint8_t>(_opcode) << 24;

    
    // Switch on the number of operands
    // and copy the data into the buffer
    // in a safe way.
    if (Instructions::OpcodeCount(_opcode) == 1)
        instruction |= _dest & 0xFFFFFF;
    else {
        instruction |= (_dest & 0xFFF) << 12;
        instruction |= (_src & 0xFFF);
    }
    *buffer = instruction;
}

auto Emitter::Emit(Instruction instruction) -> void {
    _instructions.push_back(instruction);

    _size += 4;
}

auto Emitter::Serialize() -> Containers::InstructionBuffer {
    // Allocate buffer using the absolute size
    Containers::InstructionBuffer buffer{ _size };

    size_t index = 0;
    for (auto instruction : _instructions) {
        instruction.Serialize(buffer.begin() + index);
        index += 1;
    }
    return buffer;
}

[[nodiscard]] auto Emitter::Serialize(uint32_t* buffer) -> size_t {
    size_t index = 0;
    for (auto instruction : _instructions) {
        instruction.Serialize(buffer + index);
        index += 1;
    }
    return index;
}

[[nodiscard]] auto Emitter::At(size_t index) -> Instruction& {
    return _instructions.at(index);
}

[[nodiscard]] auto Emitter::Count() noexcept -> size_t {
    return _instructions.size();
}

auto Emitter::Clear() -> void {
    _instructions.clear();
    _size = 0;
}

}