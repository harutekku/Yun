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
        [[nodiscard]] auto Name() const noexcept -> std::string_view;
        [[nodiscard]] auto StartPC() const noexcept -> const uint32_t*;
        [[nodiscard]] auto StopPC() const noexcept -> const uint32_t*;
        [[nodiscard]] auto ConstantLookup(size_t) const -> Primitives::Value;
        [[nodiscard]] auto SymbolLookup(size_t) const -> const Containers::Symbol&;
        [[nodiscard]] auto SymbolLookup(const std::string&) const -> const Containers::Symbol&;
        
    public:
        auto Disassemble() const noexcept -> void;
        [[nodiscard]] auto DisassembleInstruction(size_t) const noexcept -> size_t;

    private:
        std::string                   _name;
        Containers::SymbolTable       _symbols;
        Containers::ConstantPool      _constants;
        Containers::InstructionBuffer _buffer;
};

class VM final {
    public:
        VM(ExecutionUnit) noexcept;
    
    public:
        auto Run() -> void;
    
    private:
        auto ReportError(std::string_view) const -> void;

    private:
        ExecutionUnit             _unit;
        Containers::RegisterArray _registers;
        Containers::CallStack     _callStack;
        Containers::ArrayHeap     _heap;
        int32_t                   _flags;
        bool                      _hadError;
};


}

#endif
