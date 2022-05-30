/**
 * @file
 *   Exceptions.cpp
 * @author
 *   Harutekku @link https://github.com/harutekku @endlink
 * @brief 
 *   Exception methods implementation
 * @version
 *   0.0.1
 * @date
 *   2022-05-24
 * @copyright
 *   Copyright (c) 2022
 */
// C++ header files
#include <stdexcept>
// My header files
#include "../include/Exceptions.hpp"

namespace Yun::Error {

IntegerArithmeticError::IntegerArithmeticError(const char* message)
    :std::runtime_error(message) {
}

TypeError::TypeError(const char* message)
    :std::runtime_error(message) {
}

InstructionError::InstructionError(const char* message)
    :std::runtime_error(message) {
}

AssemblerError::AssemblerError(const char* message)
    :std::runtime_error(message) {
}

AllocationError::AllocationError(const char* message)
    :std::runtime_error(message) {
}

VMError::VMError(const char* message)
    :std::runtime_error(message) {
}

}