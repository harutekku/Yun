#ifndef PARSER_HPP
#define PARSER_HPP

#include "Containers.hpp"
#include "Lexer.hpp"
#include "Assembler.hpp"
#include "VM.hpp"
#include <vector>

namespace Yun::Interpreter {
    struct State {
        State() noexcept
            :KeepReturnValue{ 0 }, RegisterCount{ 0 }, ArgCount{ 0 }, Name{ "" } {
        }

        bool             KeepReturnValue;
        int16_t          RegisterCount;
        int16_t          ArgCount;
        std::string      Name;
    };

    class Parser {
        public:
            Parser(std::vector<Token>) noexcept;
        
        public:
            [[nodiscard]] constexpr auto HadError() const noexcept -> bool {
                return _hadError;
            }
            [[nodiscard]] auto Parse() -> VM::ExecutionUnit;
        
        private:
            [[nodiscard]] auto Next() noexcept -> Token&;
            [[nodiscard]] auto NextPrintable() noexcept -> Token&;
            [[nodiscard]] auto HasNext() const noexcept -> bool;
            auto Putback(Token) -> void;

            auto Function() -> void;
            auto FunctionDeclaration() -> void;
            auto Attributes() -> void;
            auto RegistersAttribute() -> void;
            auto ParametersAttribute() -> void;
            auto ReturnsAttribute() -> void;
            auto Block() -> void;
            auto Line() -> void;

        private:
            auto ReportError(std::string_view, ...) -> void;

        private:
            ASM::Assembler     _assembler;
            std::vector<Token> _tokens;
            size_t             _current;
            bool               _hadError;
            Token              _buffer;
            bool               _isFull;
            State              _state;

    };
}

#endif
