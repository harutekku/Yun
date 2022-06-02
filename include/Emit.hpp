/**
 * @file
 *   Emit.hpp
 * @author 
 *   Harutekku @link https://github.com/harutekku @endlink
 * @brief 
 *   Contains helper classes for code generation and
 *   instruction binary conversion
 * @version
 *   0.0.1
 * @date
 *   2022-05-24
 * @copyright
 *   Copyright (c) 2022
 * 
 */
#ifndef EMITTER_HPP
#define EMITTER_HPP

// C header files
#include <cstdint>
// C++ header files
#include <span>
// My header files
#include "Instructions.hpp"
#include "Exceptions.hpp"
#include "Containers.hpp"

/**
 * @brief 
 *   Yun::VM::Emit namespace holds classes used by the assembler and
 *   Yun virtual machine
 */
namespace Yun::VM::Emit {

/**
 * @brief 
 *   A class representing a single instruction with its operands
 */
class Instruction {
    public:
        /**
         * @brief 
         *   Construct a new binary Instruction object
         * @param opcode
         *   A valid instruction that accepts two operands
         * @param dest
         *   Destination register
         * @param src
         *   Source register
         * @throws Error::InstructionError
         *   If the instruction doesn't accept two operands
         */
        constexpr Instruction(Instructions::Opcode opcode, uint32_t dest, uint32_t src)
            :_opcode{ opcode }, _dest{ static_cast<int32_t>(dest) }, _src{ static_cast<int32_t>(src) } {
            if (Instructions::OpcodeCount(opcode) != 2)
                throw Error::InstructionError{ "Invalid arg count" };
        }
        /**
         * @brief 
         *   Construct a new unary Instruction object
         * @param opcode
         *   A valid instruction that accepts one operand
         * @param offset
         *   Either an address of a label or index into the constant pool
         * @throws Error::InstructionError
         *   If the instruction doesn't accept one operand
         */
        constexpr Instruction(Instructions::Opcode opcode, int32_t offset)
            :_opcode{ opcode }, _dest{ offset }, _src{ 0 } {
            if (Instructions::OpcodeCount(opcode) != 1)
                throw Error::InstructionError{ "Invalid arg count" };
        }

        /**
         * @brief 
         *   Construct a new void Instruction object
         * @param opcode
         *   A valid instruction without operands
         * @throws Error::InstructionError
         *   If instruction has a non-zero number of operands
         */
        constexpr Instruction(Instructions::Opcode opcode)
            :_opcode{ opcode }, _dest{ 0 }, _src{ 0 } {
            if (Instructions::OpcodeCount(opcode) != 0)
                throw Error::InstructionError{ "Invalid arg count" };        
        }

    public:
        /**
         * @brief 
         *   Serialize the instruction into the buffer
         * @param[in] buffer
         *   Buffer to write to
         * @return
         *   Number of bytes written
         */
        auto Serialize(uint32_t*) -> void;

        /**
         * @brief 
         *   Get the instruction opcode
         * @return
         *   Instruction opcode
         */
        [[nodiscard]] constexpr auto Opcode() -> Instructions::Opcode {
            return _opcode;
        }
        /**
         * @brief 
         *   Get the instruction destination operand
         * @return
         *   Instruction destination operand
         */
        [[nodiscard]] constexpr auto Destination() -> int32_t {
            return _dest;
        }
        /**
         * @brief 
         *   Get the instruction source operand
         * @return 
         *   Instruction source operand
         */
        [[nodiscard]] constexpr auto Source() -> int32_t {
            return _src;
        }

        /**
         * @brief 
         *   Assign `offset` to the instruction destination operand if 
         *   it's from j* family
         * @param offset
         *   Jump offset
         */
        constexpr auto PatchOffset(int32_t offset) -> void {
            if (!Instructions::IsJump(_opcode) && _opcode != Instructions::Opcode::call)
                throw Error::InstructionError{ "Isn't a jump" };
            _dest = offset;
        }

    private:
        /**
         * @brief 
         *   Instruction opcode
         */
        Instructions::Opcode _opcode;
        /**
         * @brief 
         *   Instruction destination operand
         */
        int32_t              _dest;
        /**
         * @brief 
         *   Instruction source operand
         */
        int32_t              _src;
};

/**
 * @brief 
 *   Helper class for serializing and deserializing instructions
 *   into and from the binary format
 */
class Emitter {
    public:
        /**
         * @brief 
         *   Construct a new Emitter object
         */
        Emitter() = default;
    public:
        /**
         * @brief 
         *   Emit a single instruction
         * @param instruction 
         *   A valid instruction to write
         */
        auto Emit(Instruction) -> void;

        /**
         * @brief 
         *   A helper method provided for convenience
         * @tparam T 
         *   Types compatible with Instruction constructors
         * @param ts 
         *   Arguments to be forwarded
         */
        template<typename ... T>
        auto Emit(T&& ... ts) -> void {
            Emit(Instruction{ std::forward<T>(ts) ... });
        }

    public:
        /**
         * @brief 
         *   Return an instruction at index
         * @param index
         *   A valid instruction index
         * @return
         *   A reference to the instruction at `index` 
         */
        [[nodiscard]] auto At(size_t) -> Instruction&;

        /**
         * @brief 
         *   Get the relative size (in units) of the buffer
         * @return
         *   The number of elements inside the buffer
         */
        [[nodiscard]] auto Count() noexcept -> size_t;

        /**
         * @brief 
         *   Serialize instructions into binary form
         * @return
         *   A buffer of serialized instructions
         */
        auto Serialize() -> Containers::InstructionBuffer;

        [[nodiscard]] auto Serialize(uint32_t*) -> size_t;

        auto Clear() -> void;

    private:
        /**
         * @brief 
         *   Absoulte size of the `_instructions` buffer
         */
        std::size_t              _size;
        /**
         * @brief 
         *   Buffer of the instructions to be serialized
         */
        std::vector<Instruction> _instructions;
};

}

#endif