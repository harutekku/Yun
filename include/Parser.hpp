#ifndef PARSER_HPP
#define PARSER_HPP

#include "Lexer.hpp"
#include "Assembler.hpp"
#include "VM.hpp"
#include <vector>

namespace Yun::Interpreter {
    class Parser {
        public:
            Parser(std::vector<Token>);
        
        public:
            [[nodiscard]] auto HadError() -> bool;
            [[nodiscard]] auto Parse() -> VM::ExecutionUnit;
        
        private:
            [[nodiscard]] auto Next() -> Token&;
            [[nodiscard]] auto HasNext() -> bool;
            auto Putback(Token) -> void;

            auto Function() -> void;
            auto FunctionDeclaration() -> void;
            auto Attributes() -> void;
            auto Block() -> void;
            auto Line() -> void;
            auto Value() -> void;
            auto Operand() -> void;

        private:
            ASM::Assembler     _assembler;
            std::vector<Token> _tokens;
            size_t             _current;
            bool               _hadError;
            Token              _buffer;
            bool               _isEmpty;

    };
}

#endif