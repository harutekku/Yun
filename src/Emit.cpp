// C header files
#include <cstring>
// My header files
#include "../include/Emit.hpp"
#include <type_traits>

namespace Yun::VM::Emit {

auto Instruction::Serialize(uint32_t* buffer) const noexcept -> void {
    uint32_t instruction = 0;

    instruction |= static_cast<uint8_t>(_opcode) << 24;

    // Switch on the number of operands
    // and copy the data into the buffer
    // in a safe way.
    if (auto count = Instructions::OpcodeCount(_opcode); Instructions::IsJump(_opcode) || _opcode == Instructions::Opcode::call)
        instruction |= _dest & 0x00FFFFFF;
    else if (count == 1) {
        instruction |= (_dest & 0xFFF) << 12;
    } else {
        instruction |= (_dest & 0xFFF) << 12;
        instruction |= (_src & 0xFFF);
    }
    *buffer = instruction;
}

auto Emitter::Emit(Instruction instruction) -> void {
    _instructions.push_back(instruction);
    _size += 4;
}

auto Emitter::Serialize() const -> Containers::InstructionBuffer {
    // Allocate buffer using the absolute size
    Containers::InstructionBuffer buffer{ _size };

    size_t index = 0;
    for (auto instruction : _instructions) {
        instruction.Serialize(buffer.begin() + index);
        index += 1;
    }
    return buffer;
}

[[nodiscard]] auto Emitter::Serialize(uint32_t* buffer) const -> size_t {
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

[[nodiscard]] auto Emitter::At(size_t index) const -> const Instruction& {
    return _instructions.at(index);
}

[[nodiscard]] auto Emitter::Count() const noexcept -> size_t {
    return _instructions.size();
}

auto Emitter::Clear() -> void {
    _instructions.clear();
    _size = 0;
}

}