/**
 * @file 
 *   Assembler.hpp
 * @author
 *   Harutekku @link https://github.com/harutekku @endlink
 * @brief 
 *   Assembler for the Yun virtual machine
 * @version
 *   0.0.1
 * @date
 *   2022-05-24
 * @copyright
 *   Copyright (c) 2022
 * 
 */
#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

// C header files
#include <cstdint>
// C++ header files
#include <string>
#include <map>
// My header files
#include "Containers.hpp"
#include "Exceptions.hpp"
#include "VM.hpp"
#include "Emit.hpp"
#include "Instructions.hpp"
#include "Value.hpp"

/**
 * @brief 
 *   Yun::ASM namespace holds classes used by the assembler
 */
namespace Yun::ASM {

/**
 * @brief 
 *   Stores information about relative and absolute
 *   jump offsets
 * @note 
 *   JumpOffset allows us to store together information
 *   about offsets of the `j*` instructions relative to
 *   the beginning of some array of some Ts and their
 *   absolute position in bytes
 */
class JumpOffset {
    public:
        /**
         * @brief 
         *   Construct a new jump offset
         * @param rel 
         *   Represents relative offset, in units
         * @param abs
         *   Represents absolute offset, in bytes
         */
        constexpr JumpOffset(int32_t rel, int32_t abs)
            :RelativeOffset{ rel }, AbsoluteOffset{ abs } {
        }

    public:
        /**
         * @brief 
         *   Relative offset of the jump
         */
        int32_t RelativeOffset;
        /**
         * @brief 
         *   Absolute offset of the jump
         */
        int32_t AbsoluteOffset;
};

/**
 * @brief 
 *   Allow for "less-than" ordering of `JumpOffset` 
 * @param first 
 *   Left-hand value to compare
 * @param second 
 *   Right-hand value to compare
 * @return 
 *   true if first.AbsoluteOffset < second.AbsoluteOffset
 * @return
 *   false otherwise
 */
[[nodiscard]] constexpr auto operator<(JumpOffset first, JumpOffset second) noexcept -> bool {
    return first.AbsoluteOffset < second.AbsoluteOffset;
}

/**
 * @brief 
 *   Main assembler class, responsible for code generation
 *   and label patching
 */
class Assembler {
    public:
        /**
         * @brief 
         *   Default construct a new Assembler object
         */
        Assembler() = default;
    
    public:
        /**
         * @brief 
         *   Add a new label to the current file
         * @param label
         *   Label name
         * @throw Error::AssemblerError
         *   If there was an attempt to redefine label
         */
        auto AddLabel(std::string) -> void;

        /**
         * @brief 
         *   Add jump instruction with label name
         * @param opcode
         *   A valid jump instruction
         * @param label
         *   A valid label name, that should be patched later
         * @throws Error::InstructionError
         *   If the instruction wasn't from jump family
         */
        auto AddJump(VM::Instructions::Opcode, std::string) -> void;

    public:
        /**
         * @brief 
         *   Add a binary opcode
         * @param opcode
         *   A valid instruction that accepts two operands
         * @param dest
         *   Destination register
         * @param src
         *   Source register
         * @throws Error::InstructionError
         *   If the instruction doesn't accept two operands
         */
        auto AddBinary(VM::Instructions::Opcode, uint32_t, uint32_t) -> void;

        /**
         * @brief 
         *   Add a void instruction (i.e. one without any operands)
         * @param opcode
         *   A valid instruction without operands
         * @throws Error::InstructionError
         *   If instruction has a non-zero number of operands
         */
        auto AddVoid(VM::Instructions::Opcode) -> void;

        template<typename T>
        auto AddLoadConstant(VM::Instructions::Opcode opcode, uint16_t destination, T&& value) -> void {
            if (opcode != VM::Instructions::Opcode::ldconst)
                throw Error::AssemblerError{ "Invalid instruction: expected ldcnst" };

            auto index = _constants.FindOrAdd<T>(VM::Primitives::Value(value));
            _emitter.Emit(opcode, destination, (uint16_t)index);
        }
        
    public:
        /**
         * @brief 
         *   Patch the jumps and emit the InstructionBuffer,
         *   ready for interpretation
         * @param name
         *   A name for the execution unit
         * @return 
         *   A valid Instructions::Buffer if patching succeeds
         */
        auto Patch(std::string) -> VM::ExecutionUnit;
    
    private:
        /**
         * @brief 
         *   Emitter object responsible for producing the Instructions::Buffer
         */
        VM::Emit::Emitter                 _emitter;
        /**
         * @brief 
         *   A container of constants
         */
        VM::Containers::ConstantPool      _constants;
        /**
         * @brief 
         *   A map of jumps - where they occur -> where they are branching
         */
        std::map<JumpOffset, std::string> _jumps;
        /**
         * @brief 
         *   A map of labels and their absolute positions
         */
        std::map<std::string, int32_t>    _labels;
};

}

#endif