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

auto Instruction::Serialize(uint8_t* buffer) -> size_t {
    size_t offset    = 0;

    buffer[offset++] = static_cast<uint8_t>(_opcode);
    
    // Switch on the number of operands
    // and copy the data into the buffer
    // in a safe way.
    switch (Instructions::OpcodeArgumentCount(_opcode)) {
    case 1:
        std::memcpy(&buffer[offset], &_dest, sizeof(_dest));
        return offset + sizeof(_dest);
    case 2: {
        int16_t dest = _dest;
        int16_t src  = _src;

        std::memcpy(&buffer[offset], &dest, sizeof(dest));
        offset += sizeof(dest);

        std::memcpy(&buffer[offset], &src, sizeof(src));
        offset += sizeof(src);

        return offset;
    }
    default:
        return offset;
    }
}

auto Emitter::Emit(Instruction instruction) -> void {
    _instructions.push_back(instruction);

    // Add the current instruction size to the total absolute size
    _size += Instructions::OpcodeSize(instruction.Opcode());
}

auto Emitter::Serialize() -> Instructions::Buffer {
    // Allocate buffer using the absolute size
    Instructions::Buffer buffer{ _size };

    size_t index = 0;
    for (auto instruction : _instructions)
        index += instruction.Serialize(buffer.begin() + index);
    return buffer;
}

[[nodiscard]] auto Emitter::At(size_t index) -> Instruction& {
    return _instructions.at(index);
}

[[nodiscard]] auto Emitter::Count() noexcept -> size_t {
    return _instructions.size();
}

}