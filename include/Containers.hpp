#ifndef CONTAINERS_HPP
#define CONTAINERS_HPP

#include <map>
#include <string>
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
        auto Copy(std::size_t, std::size_t) -> void;
        auto SaveReturnValue(std::size_t, std::size_t) -> void;

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

class Symbol {
    public:
        [[nodiscard]] auto ToString() -> std::string;
        auto PrettyFunctionSignature() const -> std::string;

    public:
        std::string Name; 
        uint16_t    Registers;
        uint16_t    Arguments;
        bool        DoesReturn;
        uint32_t    Start;
        uint32_t    End;
};

class SymbolTable {
    public:
        SymbolTable() = default;

    public:
        [[nodiscard]] auto FindByName(const std::string_view) -> const Symbol&;
        [[nodiscard]] auto FindByLocation(uint32_t) -> const Symbol&;
        [[nodiscard]] auto At(size_t) -> const Symbol&;
        [[nodiscard]] auto Count() -> size_t;
    
    public:
        auto Print() -> void;

    private:
        friend class ASM::Assembler;
        auto Add(Symbol) -> void;

    private:
        std::size_t         _size;
        std::vector<Symbol> _symbols;
};

class Frame {
    public:
        constexpr Frame() noexcept = default;
        constexpr Frame(uint32_t returnAddress, uint16_t registerCount, bool keepReturnValue, uint32_t end) noexcept
            :ReturnAddress{ returnAddress }, RegisterCount{ registerCount }, KeepReturnValue{ keepReturnValue }, End{ end } {
        }

    public:
        uint32_t ReturnAddress;
        uint16_t RegisterCount;
        bool     KeepReturnValue;
        uint32_t End;
};

// TODO: Implement
class CallStack {
    public:
        CallStack(size_t count = 1024);

    public:
        auto Push(Frame) -> void;
        [[nodiscard]] auto Pop() -> Frame;

    public:
        [[nodiscard]] auto RelativeOffset() -> size_t;

    public:
        [[nodiscard]] auto IsEmpty() -> bool;
    
    private:
        size_t             _count;
        size_t             _relativeOffset;
        std::vector<Frame> _frames;
};

}

#endif