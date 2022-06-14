#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

// C header files
#include <cstdint>
// C++ header files
#include <string>
#include <map>
#include <vector>
// My header files
#include "Containers.hpp"
#include "Exceptions.hpp"
#include "VM.hpp"
#include "Emit.hpp"
#include "Instructions.hpp"
#include "Value.hpp"

namespace Yun::ASM {

class FunctionUnit {
    public:
        FunctionUnit(VM::Containers::Symbol, VM::Emit::Emitter, std::map<uint32_t, std::string> calls) noexcept;
    
    public:
        [[nodiscard]] auto Size() const noexcept -> size_t;

        auto Serialize() const -> VM::Containers::InstructionBuffer;
        [[nodiscard]] auto Serialize(uint32_t*) -> size_t;

        [[nodiscard]] auto At(size_t) -> VM::Emit::Instruction&;

        [[nodiscard]] auto Symbol() -> VM::Containers::Symbol&;
        
        [[nodiscard]] auto CallMap() const -> const std::map<uint32_t, std::string>&;
    
    private:
        VM::Containers::Symbol            _symbol;
        VM::Emit::Emitter                 _emitter;
        std::map<uint32_t, std::string>   _calls;
};

class FunctionBuilder {
    public:
        FunctionBuilder() = default;

    public:
        auto NewFunction(std::string, uint16_t, uint16_t, bool) -> void; 

    public:
        auto AddLabel(std::string) -> void;
        auto AddJump(VM::Instructions::Opcode, std::string) -> void;
        auto AddBinary(VM::Instructions::Opcode, uint32_t, uint32_t) -> void;
        auto AddVoid(VM::Instructions::Opcode) -> void;
        auto AddCall(std::string) -> void;
        auto AddUnary(VM::Instructions::Opcode, int32_t) -> void;

    public:
        [[nodiscard]] auto Finalize() -> FunctionUnit;

    public:
        [[nodiscard]] auto FunctionName() const noexcept -> const std::string&;
    
    private:
        auto CheckIfReturns() const -> void;

    private:
        std::string                     _name;
        uint16_t                        _registerCount;
        uint16_t                        _argumentCount;
        bool                            _doesReturn;
        VM::Emit::Emitter               _emitter;

        std::map<int32_t, std::string>  _jumps;
        std::map<std::string, int32_t>  _labels;
        std::map<uint32_t, std::string> _calls;
};

class Assembler {
    public:
        Assembler() noexcept = default;
    
    public:
        auto BeginFunction(std::string, uint16_t, uint16_t, bool) -> void;
        auto EndFunction() -> void;

    public:
        auto AddLabel(std::string) -> void;

        auto AddJump(VM::Instructions::Opcode, std::string) -> void;

        auto AddCall(std::string) -> void;

    public:
        auto AddBinary(VM::Instructions::Opcode, uint32_t, uint32_t) -> void;

        auto AddVoid(VM::Instructions::Opcode) -> void;

        template<typename T>
        auto LoadConstant(uint16_t destination, T&& value) -> void {
            auto index = _constants.FindOrAdd<T>(VM::Primitives::Value(std::forward<T>(value)));
            _builder.AddBinary(VM::Instructions::Opcode::ldconst, destination, index);
        }

        auto AddUnary(VM::Instructions::Opcode, int32_t) -> void;
        
    public:
        [[nodiscard]] auto Patch(std::string) -> VM::ExecutionUnit;
    
    private:
        auto CheckCall(const VM::Containers::Symbol&, const VM::Containers::Symbol&) const -> void;
    
    private:
        VM::Containers::SymbolTable       _symbolTable;
        VM::Containers::ConstantPool      _constants;
        FunctionBuilder                   _builder;
        std::vector<FunctionUnit>         _functions;
        bool                              _isBuildingAFunction;
};

}

#endif