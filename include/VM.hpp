#ifndef VM_HPP
#define VM_HPP

// C++ header files
#include <vector>
// My header files
#include "Containers.hpp"
#include "Value.hpp"

namespace Yun::VM {

class ExecutionUnit {
    public:
        ExecutionUnit(std::string, Containers::SymbolTable, Containers::ConstantPool, Containers::InstructionBuffer);
    
    public:
        [[nodiscard]] auto Name() -> std::string_view;
        [[nodiscard]] auto StartPC() -> uint32_t*;
        [[nodiscard]] auto StopPC() -> uint32_t*;
        [[nodiscard]] auto ConstantLookup(size_t) -> Primitives::Value;
        [[nodiscard]] auto SymbolLookup(size_t) -> const Containers::Symbol&;
        [[nodiscard]] auto SymbolLookup(const std::string&) -> const Containers::Symbol&;
        
    public:
        auto Disassemble() -> void;

    private:
        [[nodiscard]] auto DisassembleInstruction(size_t) -> size_t;

    private:
        std::string                   _name;
        Containers::SymbolTable       _symbols;
        Containers::ConstantPool      _constants;
        Containers::InstructionBuffer _buffer;
};

class VM final {
    public:
        VM(ExecutionUnit);
    
    public:
        auto Run() -> void;
        auto PrintRegs() -> void;

    private:
        ExecutionUnit             _unit;
        Containers::RegisterArray _registers;
        Containers::CallStack     _callStack;
        Containers::ArrayHeap     _heap;
        int32_t                   _flags;
};


}

#endif
