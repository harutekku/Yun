/**
 * @file
 *   Assembler.cpp
 * @author
 *   Harutekku @link https://github.com/harutekku @endlink
 * @brief 
 *   Assembler routines implementation
 * @version
 *   0.0.1
 * @date
 *   2022-05-24
 * @copyright
 *   Copyright (c) 2022
 */
// My header files
#include "../include/Assembler.hpp"

namespace Yun::ASM {

auto Assembler::AddLabel(std::string label) -> void {
    // First try adding a label to the map
    auto [it, success] = _labels.try_emplace(label, _emitter.Size());

    // If failed, report a redefinition error
    if (!success)
        throw Error::AssemblerError{ "Label redefinition" };
}

auto Assembler::AddJump(VM::Instructions::Opcode opcode, std::string label) -> void {
    if (!VM::Instructions::IsJump(opcode))
        throw Error::InstructionError{ "Opcode isn't a jump" };
    
    // Before emitting a new instruction, 
    // get the current relative and absolute offset
    // of the first instruction in the buffer
    JumpOffset offsets{ static_cast<int32_t>(_emitter.Count()), static_cast<int32_t>(_emitter.Size()) };

    // Will patch this later
    _emitter.Emit(opcode, 0);

    // No need for error checking, since instructions
    // are ordered
    _jumps.try_emplace(offsets, label);
}

auto Assembler::AddBinary(VM::Instructions::Opcode opcode, uint32_t dest, uint32_t src) -> void {
    _emitter.Emit(opcode, dest, src);
}

auto Assembler::AddVoid(VM::Instructions::Opcode opcode) -> void {
    _emitter.Emit(opcode);
}

auto Assembler::Patch() -> VM::Instructions::Buffer try {
    for (const auto& [jmpOffst, label] : _jumps) {
        auto [jmpRelOffst, jmpAbsOffst] = jmpOffst;

        // First, try finding a label.
        // This will throw exception if label doesn't exist
        // which is perfectly fine.
        auto labelAbsOffst = _labels.at(label);

        // If label is ahead of current jump -> jump forward
        // Otherwise jump backward
        auto trueOffset = labelAbsOffst - jmpAbsOffst;

        // Patch jump
        _emitter.At(jmpRelOffst).PatchJump(trueOffset);
    }

    // Get the final instruction buffer
    return _emitter.Serialize();
} catch (std::out_of_range& e) {
    throw Error::AssemblerError{ "No label found" };
}

}