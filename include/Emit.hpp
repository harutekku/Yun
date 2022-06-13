#ifndef EMITTER_HPP
#define EMITTER_HPP

// C header files
#include <cstdint>
// C++ header files
#include <span>
#include <string>
// My header files
#include "Instructions.hpp"
#include "Exceptions.hpp"
#include "Containers.hpp"

namespace Yun::VM::Emit {

class Instruction {
    public:
        constexpr Instruction(Instructions::Opcode opcode, uint32_t dest, uint32_t src)
            :_opcode{ opcode }, _dest{ static_cast<int32_t>(dest) }, _src{ static_cast<int32_t>(src) } {
            if (auto res = Instructions::OpcodeCount(opcode); res != 2)
                throw Error::InstructionError{ "Ivalid argument count: ", opcode, res };
        }
        constexpr Instruction(Instructions::Opcode opcode, int32_t offset)
            :_opcode{ opcode }, _dest{ offset }, _src{ 0 } {
            if (auto res = Instructions::OpcodeCount(opcode); res != 1)
                throw Error::InstructionError{ "Invalid argument count: ", opcode, res };
        }

        constexpr Instruction(Instructions::Opcode opcode)
            :_opcode{ opcode }, _dest{ 0 }, _src{ 0 } {
            if (auto res = Instructions::OpcodeCount(opcode); res != 0)
                throw Error::InstructionError{ "Invalid argument count", opcode, res };        
        }

    public:
        auto Serialize(uint32_t*) -> void;

        [[nodiscard]] constexpr auto Opcode() -> Instructions::Opcode {
            return _opcode;
        }
        [[nodiscard]] constexpr auto Destination() -> int32_t {
            return _dest;
        }
        [[nodiscard]] constexpr auto Source() -> int32_t {
            return _src;
        }

        constexpr auto PatchOffset(int32_t offset) -> void {
            if (!Instructions::IsJump(_opcode) && _opcode != Instructions::Opcode::call)
                throw Error::InstructionError{ "Opcode isn't a jump or a call: ", _opcode };
            _dest = offset;
        }

    private:
        Instructions::Opcode _opcode;
        int32_t              _dest;
        int32_t              _src;
};

class Emitter {
    public:
        Emitter() = default;
    public:
        auto Emit(Instruction) -> void;

        template<typename ... T>
        auto Emit(T&& ... ts) -> void {
            Emit(Instruction{ std::forward<T>(ts) ... });
        }

    public:
        [[nodiscard]] auto At(size_t) -> Instruction&;

        [[nodiscard]] auto Count() noexcept -> size_t;

        auto Serialize() -> Containers::InstructionBuffer;

        [[nodiscard]] auto Serialize(uint32_t*) -> size_t;

        auto Clear() -> void;

    private:
        std::size_t              _size;
        std::vector<Instruction> _instructions;
};

}

#endif