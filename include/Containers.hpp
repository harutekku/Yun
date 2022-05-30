#ifndef CONTAINERS_HPP
#define CONTAINERS_HPP

#include <vector>
#include <cstddef>
#include <stack>
#include <memory>
#include "Value.hpp"
#include "Instructions.hpp"
#include "Exceptions.hpp"

namespace Yun::ASM {
class Assembler;
}

namespace Yun::VM::Containers {

class RegisterArray {
    public:
        RegisterArray(size_t count = 1024);

    public:
        [[nodiscard]] auto At(std::size_t) -> Primitives::Value&;
        auto Allocate(std::size_t) -> void;
        auto Deallocate(std::size_t) -> void;

    public:
        auto Print() -> void;

    private:
        std::size_t                    _index;
        std::vector<Primitives::Value> _registers;
};

class ConstantPool {
    public:
        ConstantPool() = default;
    
    public:
        [[nodiscard]] auto Read(size_t) -> Primitives::Value;
        [[nodiscard]] auto Has(size_t) -> bool;

    public:
        auto Print() -> void;

    private:
        friend ASM::Assembler;
        [[nodiscard]] auto Add(Primitives::Value) -> size_t;

        template<typename T>
        [[nodiscard]] auto FindOrAdd(Primitives::Value value) -> size_t {
            for (size_t i = 0; i != _constants.size(); ++i)
                if (_constants[i].Is() == value.Is() && _constants[i].As<T>() == value.As<T>())
                    return i;
            return Add(value);
        }

    private:
        std::vector<Primitives::Value> _constants;

};

struct Frame {
    size_t PCBegin;
    size_t RegisterCount;
    size_t PCEnd;
};

class FrameInfo {
    public:

    private:
        std::stack<Frame, std::vector<Frame>> _frames;
};

class InstructionBuffer {
    public:
        InstructionBuffer(size_t data);

    public:
        [[nodiscard]] auto begin() noexcept -> uint8_t*;
        [[nodiscard]] auto end() noexcept -> uint8_t*;
    
    private:
        std::unique_ptr<std::uint8_t[]> _data;
        std::uint8_t*                   _last;
};

[[nodiscard]] auto begin(InstructionBuffer& buffer) noexcept -> uint8_t*;

[[nodiscard]] auto end(InstructionBuffer& buffer) noexcept -> uint8_t*;


}

#endif