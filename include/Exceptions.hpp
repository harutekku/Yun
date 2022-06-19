#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

// C++ header files
#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>
// My header files
#include "Instructions.hpp"

namespace Yun::VM::Primitives {
    enum class Type : uint8_t;
}

namespace Yun::Error {

class InstructionError : public std::exception {
    public:
        InstructionError(std::string message);

        InstructionError(std::string message, VM::Instructions::Opcode opcode, int args = 0);

        ~InstructionError() noexcept = default;

    public:
        [[nodiscard]] auto what() const noexcept -> const char* override;

    private:
        std::string _message;
};

class TypeError : public std::exception {
    public:
        TypeError(std::string, uint32_t);
        TypeError(std::string, VM::Primitives::Type);
        TypeError(std::string, VM::Primitives::Type, VM::Primitives::Type);

        ~TypeError() noexcept = default;

    public:
        [[nodiscard]] auto what() const noexcept -> const char* override;

    private:
        std::string _message;
};

class IntegerArithmeticError : public std::runtime_error {
    public:
        IntegerArithmeticError(const char* message);

        ~IntegerArithmeticError() noexcept = default;
};

class AssemblerError : public std::exception {
    public:
        AssemblerError(std::string);           // General
        AssemblerError(std::string, int);      // For label redefinitions
        AssemblerError(std::string, int, int); // For out of range errors

        ~AssemblerError() noexcept = default;

    public:
        [[nodiscard]] auto what() const noexcept -> const char* override;

    private:
        std::string _message;
};

class AllocationError : public std::runtime_error {
    public:
        AllocationError(const char* message);

        ~AllocationError() noexcept = default;
};

class VMError : public std::exception {
    public:
        VMError(std::string);

        ~VMError() noexcept = default;
    public:
        [[nodiscard]] auto what() const noexcept -> const char* override;

    private:
        std::string _message;
};

class RangeError : public std::exception {
    public:
        RangeError(std::string, size_t, size_t);
        RangeError(std::string);
    
    public:
        [[nodiscard]] auto what() const noexcept -> const char* override;

    private:
        std::string _message;
};

class ParseError {

};

}

#endif
