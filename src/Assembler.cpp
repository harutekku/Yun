// My header files
#include "../include/Assembler.hpp"
#include <cmath>
#include <cstdint>
#include <string>

namespace Yun::ASM {

FunctionUnit::FunctionUnit(VM::Containers::Symbol symbol, VM::Emit::Emitter emitter, std::map<uint32_t, std::string> calls)
    :_symbol{ std::move(symbol) }, _emitter{ std::move(emitter) }, _calls{ std::move(calls) } {
}

[[nodiscard]] auto FunctionUnit::Size() -> size_t {
    return _emitter.Count() * 4;
}

[[nodiscard]] auto FunctionUnit::Serialize(uint32_t* buffer) -> size_t {
    return _emitter.Serialize(buffer);
}

[[nodiscard]] auto FunctionUnit::Serialize() -> VM::Containers::InstructionBuffer {
    return _emitter.Serialize();
}

[[nodiscard]] auto FunctionUnit::At(size_t index) -> VM::Emit::Instruction& {
    return _emitter.At(index);
}

[[nodiscard]] auto FunctionUnit::Symbol() -> VM::Containers::Symbol& {
    return _symbol;
}
        
[[nodiscard]] auto FunctionUnit::CallMap() -> std::map<uint32_t, std::string>& {
    return _calls;
}

auto FunctionBuilder::NewFunction(std::string name, uint16_t registerCount, uint16_t argumentCount, bool doesReturn) -> void {
    if (argumentCount > registerCount || (registerCount == 0 && doesReturn))
        throw Error::AssemblerError{ "Argument count bigger than register count: ", argumentCount, registerCount };
    _name          = std::move(name);
    _registerCount = registerCount;
    _argumentCount = argumentCount;
    _doesReturn    = doesReturn;
    _emitter.Clear();
    _jumps.clear();
    _calls.clear();
    _labels.clear();
}

auto FunctionBuilder::AddLabel(std::string label) -> void {
    // First try adding a label to the map
    auto [it, success] = _labels.try_emplace(label, _emitter.Count() * 4);

    // If failed, report a redefinition error
    if (!success)
        throw Error::AssemblerError{ "Label redefinition: found " + label + " at ", it->second };
}

auto FunctionBuilder::AddJump(VM::Instructions::Opcode opcode, std::string label) -> void {
    if (!VM::Instructions::IsJump(opcode))
        throw Error::InstructionError{ "Opcode isn't a jump: ", opcode };
    
    // No need for error checking, since instructions
    // are ordered
    _jumps.try_emplace(_emitter.Count(), label);
    
    // Will patch this later
    _emitter.Emit(opcode, 0);
}

auto FunctionBuilder::AddCall(std::string function) -> void {
    auto offset = _emitter.Count();
    
    _emitter.Emit(VM::Instructions::Opcode::call, 0);

    _calls.try_emplace(offset, function);
}

auto FunctionBuilder::AddBinary(VM::Instructions::Opcode opcode, uint32_t dest, uint32_t src) -> void {
    if (dest >= _registerCount || (opcode != VM::Instructions::Opcode::ldconst && src >= _registerCount))
        throw Error::AssemblerError{ "Register index out of range: ", static_cast<int>(dest), static_cast<int>(src) };
    _emitter.Emit(opcode, dest, src);
}

auto FunctionBuilder::AddVoid(VM::Instructions::Opcode opcode) -> void {
    _emitter.Emit(opcode);
}

auto FunctionBuilder::Finalize() -> FunctionUnit {
    CheckIfReturns();

    for (const auto& [jmpOffst, label] : _jumps) {
        try {
        // First, try finding a label.
        // This will throw exception if label doesn't exist
        // which is perfectly fine.
        auto labelOffst = _labels.at(label);

        // If label is ahead of current jump -> jump forward
        // Otherwise jump backward
        auto trueOffset = labelOffst - jmpOffst * 4;

        // Patch jump
        _emitter.At(jmpOffst).PatchOffset(trueOffset);

        } catch (std::out_of_range& e) {
            throw Error::AssemblerError{ "No " + label + " found inside function " + _name };
        }
    }

    VM::Containers::Symbol s{ std::move(_name), _registerCount, _argumentCount, 0, static_cast<uint32_t>(_emitter.Count() * 4), _doesReturn };


    return { s, _emitter, _calls };
}

[[nodiscard]] auto FunctionBuilder::FunctionName() const -> const std::string& {
    return _name;
}

auto FunctionBuilder::CheckIfReturns() -> void {
    auto last = _emitter.Count();

    if (last == 0 || _emitter.At(last - 1).Opcode() != VM::Instructions::Opcode::ret)
        throw Error::AssemblerError{ "Function" + _name + "must contain `ret` instruction" };
}

auto FunctionBuilder::AddUnary(VM::Instructions::Opcode opcode, int32_t source) -> void {
    if (source >= _registerCount)
        throw Error::AssemblerError{ "Register index out of range: ", static_cast<int>(source) };
    else if (VM::Instructions::IsJump(opcode) || VM::Instructions::Opcode::call == opcode)
        throw Error::AssemblerError{ "Can't add jump or a call directly" };
    _emitter.Emit(opcode, source);
}

auto Assembler::BeginFunction(std::string name, uint16_t registerCount, uint16_t argumentCount, bool doesReturn) -> void {
    if (!_isBuildingAFunction) {
        _builder.NewFunction(name, registerCount, argumentCount, doesReturn);
        _isBuildingAFunction = true;
    } else
        throw Error::AssemblerError{ "Unfinished build of a function: " + _builder.FunctionName() };
}

auto Assembler::EndFunction() -> void {
    if (_isBuildingAFunction) {
        _functions.emplace_back(_builder.Finalize());
        _isBuildingAFunction = false;
    } else
        throw Error::AssemblerError{ "Can't end a build of a function that doesn't exist" };
}

auto Assembler::AddLabel(std::string label) -> void {
    if (!_isBuildingAFunction)
        throw Error::AssemblerError{ "Can't add an instruction when not in build mode" };
    _builder.AddLabel(label);
}

auto Assembler::AddJump(VM::Instructions::Opcode opcode, std::string label) -> void {
    if (!_isBuildingAFunction)
        throw Error::AssemblerError{ "Can't add an instruction when not in build mode" };
    _builder.AddJump(opcode, label);
}

auto Assembler::AddCall(std::string function) -> void {
    if (!_isBuildingAFunction)
        throw Error::AssemblerError{ "Can't add an instruction when not in build mode" };
    _builder.AddCall(function);
}

auto Assembler::AddUnary(VM::Instructions::Opcode opcode, int32_t source) -> void {
    if (!_isBuildingAFunction)
        throw Error::AssemblerError{ "Can't add an instruction when not in build mode" };
    _builder.AddUnary(opcode, source);
}

auto Assembler::AddBinary(VM::Instructions::Opcode opcode, uint32_t dest, uint32_t src) -> void {
    if (!_isBuildingAFunction)
        throw Error::AssemblerError{ "Can't add an instruction when not in build mode" };
    else if (opcode == VM::Instructions::Opcode::ldconst && !_constants.Has(src)) 
        throw Error::AssemblerError{ "Can't add a new `ldconst` with invalid index" };
    _builder.AddBinary(opcode, dest, src);
}

auto Assembler::AddVoid(VM::Instructions::Opcode opcode) -> void {
    if (!_isBuildingAFunction)
        throw Error::AssemblerError{ "Can't add an instruction when not in build mode" };
    _builder.AddVoid(opcode);
}

[[nodiscard]] auto Assembler::Patch(std::string name) -> VM::ExecutionUnit {
    size_t codeSegmentSize = 0;
    
    // First, calculate the code segment size
    for (auto& function : _functions) {
        auto symbol = std::move(function.Symbol());

        // Fill the symbol table with declared functions
        symbol.Start = codeSegmentSize;
        symbol.End   = codeSegmentSize + function.Size();

        _symbolTable.Add(symbol);

        codeSegmentSize += function.Size();
    }

    VM::Containers::InstructionBuffer buffer{ codeSegmentSize };

    size_t index = 0;
    for (auto& function : _functions) {
        const auto& callMap = function.CallMap();

        for (const auto& [relOffst, string] : callMap) {
            const auto& symbol = _symbolTable.FindByName(string);

            function.At(relOffst).PatchOffset(symbol.Start);
        }
        index += function.Serialize(buffer.begin() + index);
    }
    return { std::move(name), std::move(_symbolTable), std::move(_constants), std::move(buffer) };
}

}