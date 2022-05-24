/**
 * @file
 *   Exceptions.hpp
 * @author
 *   Harutekku @link https://github.com/harutekku @endlink
 * @brief
 *   Exceptions used in project 
 * @version
 *   0.0.1
 * @date
 *   2022-05-24
 * @copyright
 *   Copyright (c) 2022
 * 
 */
#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

// C++ header files
#include <stdexcept>

/**
 * @brief 
 *   Namespace Yun::Error provides exception classes used for
 *   error handling by all other classes
 */
namespace Yun::Error {

/**
 * @brief 
 *   An exception class used internally by Emitter and Assembler.
 *   Used to indicate some problem with processed instruction
 */
class InstructionError : public std::runtime_error {
    public:
        /**
         * @brief
         *   Construct a new InstructionError object
         * @param message
         *   An information to pass down the stack
         */
        InstructionError(const char* message);

        /**
         * @brief
         *   Destroy the Instruction Error object
         */
        ~InstructionError() noexcept = default;
};

/**
 * @brief 
 *   An exception class used internally by the Yun virtual machine.
 *   Used to indicate operand type mismatch
 */
class TypeError : public std::runtime_error {
    public:
        /**
         * @brief
         *   Construct a new TypError object
         * @param message
         *   An information to pass down the stack
         */
        TypeError(const char* message);

        /**
         * @brief
         *   Destroy the Type Error object
         */
        ~TypeError() noexcept = default;
};

/**
 * @brief 
 *   An exception class used internally by the Yun virtual machine.
 *   Used to indicate integer arithmetic error, namely division by zero.
 */
class IntegerArithmeticError : public std::runtime_error {
    public:
        /**
         * @brief
         *   Construct a new IntegerArithmeticError object
         * @param message 
         *   An information to pass down the stack
         */
        IntegerArithmeticError(const char* message);

        /**
         * @brief
         *   Destroy the Integer Arithmetic Error object
         */
        ~IntegerArithmeticError() noexcept = default;
};

/**
 * @brief 
 *   An exception class used internally by the Assembler.
 *   Used to indicate label and consants-related errors.
 */
class AssemblerError : public std::runtime_error {
    public:
        /**
         * @brief
         *   Construct a new AssemblerError object
         * @param message 
         *   An information to pass down the stack
         */
        AssemblerError(const char* message);

        /**
         * @brief
         *   Destroy the Assembler Error object
         */
        ~AssemblerError() noexcept = default;
};

}

#endif
