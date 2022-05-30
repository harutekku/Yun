#include "../include/Containers.hpp"
#include <cstdio>

namespace Yun::VM::Containers {

RegisterArray::RegisterArray(size_t count)
    :_index{ 0 }, _registers(count) {
}

[[nodiscard]] auto RegisterArray::At(size_t index) -> Primitives::Value& {
    if (_index < index)
        throw std::out_of_range{ "Register index out of range" };
    return _registers.at(index);
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
    _data  = std::make_unique<uint8_t[]>(size);
    _last = &_data[size];
}

[[nodiscard]] auto InstructionBuffer::begin() noexcept -> uint8_t* {
    return &_data[0];    
}

[[nodiscard]] auto InstructionBuffer::end() noexcept -> uint8_t* {
    return _last;
}

[[nodiscard]] auto begin(InstructionBuffer& buffer) noexcept -> uint8_t* {
    return buffer.begin();
}

[[nodiscard]] auto end(InstructionBuffer& buffer) noexcept -> uint8_t* {
    return buffer.end();
}


}