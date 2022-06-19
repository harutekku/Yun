#include "../include/Containers.hpp"
#include <bit>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>

namespace Yun::VM::Containers {

RegisterArray::RegisterArray(size_t count) noexcept
    :_index{ 0 }, _registers(count) {
}

auto RegisterArray::Allocate(size_t count) -> void {
    if (_index + count > _registers.size())
        _registers.resize(_index * 2);
    _index += count;
}

auto RegisterArray::Deallocate(size_t count, ArrayHeap& heap) noexcept -> void {
    for (size_t i = _index - count; i != _index; ++i)
        if (_registers[i].Typeof() == Primitives::Type::Reference)
            heap.Notify(_registers[i].As<Primitives::Reference>().HeapID, false);
    _index -= count;
}

auto RegisterArray::Copy(std::size_t base, std::size_t count, ArrayHeap& heap) noexcept -> void {
    for (size_t i = 0; i != count; ++i)
        if (const auto& ref = _registers[_index - base + i] = _registers[_index - base - count + i]; ref.Typeof() == Primitives::Type::Reference)
            heap.Notify(ref.As<Primitives::Reference>().HeapID, true);
}

auto RegisterArray::SaveReturnValue(std::size_t currentFrameCount, ArrayHeap& heap) noexcept -> void {
    auto& oldLast = _registers[_index - currentFrameCount - 1];
    const auto& newFirst = _registers[_index - currentFrameCount];
    if (oldLast.Typeof() == Primitives::Type::Reference)
        heap.Notify(oldLast.As<Primitives::Reference>().HeapID, false);
    oldLast = newFirst;
    if (oldLast.Typeof() == Primitives::Type::Reference)
        heap.Notify(oldLast.As<Primitives::Reference>().HeapID, true);
}

auto RegisterArray::Print() const -> void {
    for (size_t i = 0; i != _index; ++i)
        printf("  0x%zx -> %s\n", i, _registers[i].ToString().data());
}

[[nodiscard]] auto ConstantPool::Read(size_t index) const -> Primitives::Value {
    return _constants.at(index);
}

[[nodiscard]] auto ConstantPool::Has(size_t index) const noexcept -> bool {
    return index < _constants.size();
}

auto ConstantPool::Print() const noexcept -> void {
    for (size_t i = 0; i != _constants.size(); ++i)
        printf("  $0x%zx -> %s\n", i, _constants[i].ToString().data());
}

[[nodiscard]] auto ConstantPool::Add(Primitives::Value value) -> size_t {
    const auto oldSize = _constants.size();
    _constants.push_back(value);
    return oldSize;
}

InstructionBuffer::InstructionBuffer(size_t size) noexcept {
    _data = std::make_unique<uint32_t[]>(size);
    _last = &_data[size];
}

[[nodiscard]] auto InstructionBuffer::begin() noexcept -> uint32_t* {
    return &_data[0];    
}

[[nodiscard]] auto InstructionBuffer::begin() const noexcept -> const uint32_t* {
    return &_data[0];    
}

[[nodiscard]] auto InstructionBuffer::end() noexcept -> uint32_t* {
    return _last;
}
[[nodiscard]] auto InstructionBuffer::end() const noexcept -> const uint32_t* {
    return _last;
}

[[nodiscard]] auto begin(InstructionBuffer& buffer) noexcept -> uint32_t* {
    return buffer.begin();
}

[[nodiscard]] auto end(InstructionBuffer& buffer) noexcept -> uint32_t* {
    return buffer.end();
}

[[nodiscard]] auto Symbol::ToString() const -> std::string {
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

[[nodiscard]] auto SymbolTable::FindByName(const std::string_view name) const -> const Symbol& {
    for (size_t i = 0; i != _symbols.size(); ++i)
        if (_symbols[i].Name == name)
            return _symbols[i];
    throw Error::VMError{ std::string("Symbol '") + std::string(name.begin(), name.end()) + "' doesn't exist in the symbol table" };
}

[[nodiscard]] auto SymbolTable::FindByLocation(uint32_t location) const -> const Symbol& {
    for (size_t i = 0; i != _symbols.size(); ++i)
        if (_symbols[i].Start == location)
            return _symbols[i];
    throw Error::VMError{ std::string("Symbol at @") + std::to_string(location) + " doesn't exists in the symbol table" };
}

[[nodiscard]] auto SymbolTable::At(size_t index) const -> const Symbol& {
    return _symbols.at(index);
}

[[nodiscard]] auto SymbolTable::Count() const noexcept -> size_t {
    return _symbols.size();
}

auto SymbolTable::Print() const -> void {
    for (size_t i = 0; i != _symbols.size(); ++i)
        printf("  %s", _symbols[i].ToString().data());
}
    
auto SymbolTable::Add(Symbol symbol) -> void {
    _symbols.emplace_back(std::move(symbol));
}

CallStack::CallStack(size_t count) noexcept 
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

Array::Array(Primitives::Type type, size_t count) noexcept
    :_elementType{ type }, _count{ count }, _elements{ std::make_unique<uint64_t[]>(count) } {
}

[[nodiscard]] auto Array::Load(size_t index) -> Primitives::Value {
    if (index >= _count)
        throw Error::RangeError{ "Index was higher than count: ", index, _count };

    Primitives::Value retVal{ _elementType };
    std::memcpy(retVal.AsPtr(), &_elements[index], sizeof(uint64_t));
    return retVal;
}

auto Array::Store(size_t index, Primitives::Value value) -> void {
    if (index >= _count)
        throw Error::RangeError{ "Index was higher than element count: ", index, _count };
    else if (_elementType != value.Typeof())
        throw Error::TypeError{ "Store of value with incompatible type", value.Typeof(), _elementType };

    std::memcpy(&_elements[index], value.AsPtr(), sizeof(uint64_t));
}

auto Array::Advance(Primitives::Reference& reference, uint32_t offset) -> void {
    if (offset > _count) // Can cast this safely
        throw Error::RangeError{ "Index was outside range: " + std::to_string(offset) + " not in [0;" + std::to_string(_count) + "]" };
    else
        reference.ArrayIndex = offset;
}

ArrayHeap::ArrayHeap(size_t initialSize)
    :_index{ 0 }, _heapArrays(initialSize), _idsForReuse{  } {
}

[[nodiscard]] auto ArrayHeap::NewArray(uint32_t size, uint32_t type) -> Primitives::Reference {
    if (type > static_cast<uint8_t>(Primitives::Type::Float64) || type < 1)
        throw Error::TypeError{ "Unsupported type id: ", type };

    uint32_t id = 0;
    if (!_idsForReuse.empty()) {
        id = _idsForReuse.back();
        _idsForReuse.pop();
    } else
        id = _index++;
    if (id == _heapArrays.capacity())
        _heapArrays.resize(2 * id);
    
    _heapArrays[id] = HeapRecord{ id, 1, std::make_unique<Array>(static_cast<Primitives::Type>(type), size) }; 
    return { id, 0 };
}

auto ArrayHeap::Notify(uint32_t id, bool refAddElseSub) noexcept -> void {
    if (refAddElseSub)
        ++_heapArrays[id].RefCount;
    else
        --_heapArrays[id].RefCount;

    if (_heapArrays[id].RefCount == 0) {
        _heapArrays[id].Pointer = nullptr;
        _idsForReuse.push(id);
    }
}

[[nodiscard]] auto ArrayHeap::GetArray(uint32_t id) noexcept -> Array* {
    return _heapArrays[id].Pointer.get();
}

    
}
