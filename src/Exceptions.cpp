// C++ header files
#include <cmath>
#include <cstddef>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
// My header files
#include "../include/Exceptions.hpp"
#include "../include/Value.hpp"

namespace Yun::Error {

InstructionError::InstructionError(std::string message)
    :_message{ std::move(message) } {
}

InstructionError::InstructionError(std::string message, VM::Instructions::Opcode opcode, int args)
    :_message{ std::move(message) } {
    _message += std::string("[") + VM::Instructions::OpcodeToString(opcode);
    if (args == 0)
        _message += "]";
    else if (args == 1)
        _message += " dest]";
    else
        _message += " dest, src]";
}

[[nodiscard]] auto InstructionError::what() const noexcept -> const char* {
    return _message.c_str();
}

IntegerArithmeticError::IntegerArithmeticError(const char* message)
    :std::runtime_error(message) {
}

TypeError::TypeError(std::string message, uint32_t id)
    :_message{ std::move(message) } {
    _message += std::string("[") + std::to_string(id) + "]";
}

TypeError::TypeError(std::string message, VM::Primitives::Type dest)
    :_message{ std::move(message) } {
    _message += std::string("[") + VM::Primitives::TypeToString(dest) + "]";
}

TypeError::TypeError(std::string message, VM::Primitives::Type dest, VM::Primitives::Type src)
    :_message{ std::move(message) } {
    _message += std::string("[") + VM::Primitives::TypeToString(dest) + ", " + VM::Primitives::TypeToString(src) + "]";
}


[[nodiscard]] auto TypeError::what() const noexcept -> const char* {
    return _message.c_str();
}

AssemblerError::AssemblerError(std::string message)
    :_message{ std::move(message) } {
}

AssemblerError::AssemblerError(std::string message, int location)
    :_message{ std::move(message) } {
    _message += std::string("[at ") + std::to_string(location) + "]";
}

AssemblerError::AssemblerError(std::string message, int index, int range)
    :_message{ std::move(message) } {
    _message += std::string("[") + std::to_string(index) + " != " + std::to_string(range) + "]";
}

[[nodiscard]] auto AssemblerError::what() const noexcept -> const char* {
    return _message.c_str();
}

AllocationError::AllocationError(const char* message)
    :std::runtime_error(message) {
}

VMError::VMError(const char* message)
    :std::runtime_error(message) {
}

RangeError::RangeError(std::string message, size_t index, size_t count)
    :_message{ std::move(message) } {
    _message += std::string("[") + std::to_string(index) + " > " + std::to_string(count) + "]";
}

RangeError::RangeError(std::string message)
    :_message{ std::move(message) } {
}
    
[[nodiscard]] auto RangeError::what() const noexcept -> const char* {
    return _message.c_str();
}

}