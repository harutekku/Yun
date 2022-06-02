#include "../include/Containers.hpp"
#include <cstdio>
#include <exception>
#include <iterator>
#include <memory>
#include <string>

namespace Yun::VM::Containers {

RegisterArray::RegisterArray(size_t count)
    :_index{ 0 }, _registers(count) {
}

[[nodiscard]] auto RegisterArray::At(size_t index) -> Primitives::Value& {
    if (_index < index)
        throw std::out_of_range{ "Register index out of range" };
    return _registers[index];
}

auto RegisterArray::Allocate(size_t count) -> void {
    if (_index + count > _registers.size())
        _registers.resize(_index * 2);
    _index += count;
}

auto RegisterArray::Deallocate(size_t count) -> void {
    if (_index < count)
        throw Error::AllocationError{ "Can't deallocate more than `currentCount` registers" };
    _index -= count;
}

auto RegisterArray::Copy(std::size_t base, std::size_t count) -> void {
    for (size_t i = 0; i != count; ++i)
        _registers.at(_index - base + i) = _registers.at(_index - base - count + i);
}

auto RegisterArray::SaveReturnValue(std::size_t currentFrameCount) -> void {
    _registers.at(_index - currentFrameCount - 1) = _registers.at(_index - currentFrameCount);
}

auto RegisterArray::Print() -> void {
    for (size_t i = 0; i != _index; ++i)
        printf("  0x%zx -> %s\n", i, _registers[i].ToString().data());
}

[[nodiscard]] auto ConstantPool::Read(size_t index) -> Primitives::Value {
    return _constants.at(index);
}

[[nodiscard]] auto ConstantPool::Has(size_t index) -> bool {
    return index < _constants.size();
}

auto ConstantPool::Print() -> void {
    for (size_t i = 0; i != _constants.size(); ++i)
        printf("  $0x%zx -> %s\n", i, _constants[i].ToString().data());
}

[[nodiscard]] auto ConstantPool::Add(Primitives::Value value) -> size_t {
    auto oldSize = _constants.size();
    _constants.push_back(value);
    return oldSize;
}

InstructionBuffer::InstructionBuffer(size_t size) {
    _data = std::make_unique<uint32_t[]>(size);
    _last = &_data[size];
}

[[nodiscard]] auto InstructionBuffer::begin() noexcept -> uint32_t* {
    return &_data[0];    
}

[[nodiscard]] auto InstructionBuffer::end() noexcept -> uint32_t* {
    return _last;
}

[[nodiscard]] auto begin(InstructionBuffer& buffer) noexcept -> uint32_t* {
    return buffer.begin();
}

[[nodiscard]] auto end(InstructionBuffer& buffer) noexcept -> uint32_t* {
    return buffer.end();
}

[[nodiscard]] auto Symbol::ToString() -> std::string {
    std::string retVal{ std::string("@0x") + std::to_string(Start) + std::string(" -> ") };
    retVal.append(Name);
    retVal.append("\n    Registers: ");
    retVal.append(std::to_string(Registers));
    retVal.append("\n    Arguments: ");
    retVal.append(std::to_string(Arguments));
    retVal.append("\n    Returns: ");
    retVal.append(DoesReturn? "Value" : "void");
    retVal.append("\n    End: ");
    retVal.append(std::to_string(End) + "\n");
    return retVal;
}

auto Symbol::PrettyFunctionSignature() const -> std::string {
    std::string retVal{ DoesReturn? "Value " : "void " };
    retVal.append(Name);
    retVal.append("(");
    int i = 0;
    for (; i < Arguments - 1; ++i)
        retVal.append("Value, ");
    if (i < Arguments)
        retVal.append("Value");
    retVal.append("):");
    return retVal;
}

[[nodiscard]] auto SymbolTable::FindByName(const std::string_view name) -> const Symbol& {
    for (size_t i = 0; i != _symbols.size(); ++i)
        if (_symbols[i].Name == name)
            return _symbols[i];
    throw Error::VMError{ "No such symbol exists in symbol table" };
}

[[nodiscard]] auto SymbolTable::FindByLocation(uint32_t location) -> const Symbol& {
    for (size_t i = 0; i != _symbols.size(); ++i)
        if (_symbols[i].Start == location)
            return _symbols[i];
    throw Error::VMError{ "No such symbol exists in symbol table" };
}

[[nodiscard]] auto SymbolTable::At(size_t index) -> const Symbol& {
    return _symbols.at(index);
}

[[nodiscard]] auto SymbolTable::Count() -> size_t {
    return _symbols.size();
}

auto SymbolTable::Print() -> void {
    for (size_t i = 0; i != _symbols.size(); ++i)
        printf("  %s", _symbols[i].ToString().data());
}
    
auto SymbolTable::Add(Symbol symbol) -> void {
    _size += symbol.Name.size() + sizeof(symbol.Start) + sizeof(symbol.End) + sizeof(symbol.Registers) + sizeof(symbol.Arguments) + sizeof(symbol.DoesReturn); 
    _symbols.emplace_back(std::move(symbol));
}

CallStack::CallStack(size_t count)
    :_count{ 0 }, _relativeOffset{ 0 }, _frames(count) {
}

auto CallStack::Push(Frame frame) -> void {
    _relativeOffset += _count? frame.RegisterCount : 0;
    _frames[_count++] = frame;
}

[[nodiscard]] auto CallStack::Pop() -> Frame {
    auto returnValue = _frames[--_count];
    _relativeOffset -= _count? _frames[_count].RegisterCount : 0;
    return returnValue;
}

[[nodiscard]] auto CallStack::RelativeOffset() -> size_t {
    return _relativeOffset;
}

[[nodiscard]] auto CallStack::IsEmpty() -> bool {
    return _count == 0;
}

}