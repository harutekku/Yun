#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <string_view>
#include <vector>

namespace Yun::Interpreter {
    enum class TokenType : uint8_t {
        LeftParen,
        RightParen,
        LeftBrace,
        RightBrace,
        LeftBracket,
        RightBracket,
        Comma,
        Equals,
        Newline,

        FloatLiteral,
        SignedLiteral,
        UnsignedLiteral,
        True,
        False,
        Id,
        
        Label,
        Function,
        Register,
        Instruction,

        RegistersAttribute,
        ReturnsAttribute,
        ParametersAttribute,
        EndOfFile
    };

    [[nodiscard]] constexpr auto TokenTypeToString(TokenType type) -> std::string_view {
        switch (type) {
        case TokenType::LeftParen:
            return "LeftParen";
        case TokenType::RightParen:
            return "RightParen";
        case TokenType::LeftBrace:
            return "LeftBrace";
        case TokenType::RightBrace:
            return "RightBrace";
        case TokenType::LeftBracket:
            return "LeftBracket";
        case TokenType::RightBracket:
            return "RightBracket";
        case TokenType::Comma:
            return "Comma";
        case TokenType::Equals:
            return "Equals";
        case TokenType::Newline:
            return "Newline";
        case TokenType::FloatLiteral:
            return "FloatLiteral";
        case TokenType::SignedLiteral:
            return "SignedLiteral";
        case TokenType::UnsignedLiteral:
            return "UnsignedLiteral";
        case TokenType::True:
            return "True";
        case TokenType::False:
            return "False";
        case TokenType::Id:
            return "Id";
        case TokenType::Label:
            return "Label";
        case TokenType::Function:
            return "Function";
        case TokenType::Register:
            return "Register";
        case TokenType::Instruction:
            return "Instruction";
        case TokenType::RegistersAttribute:
            return "RegistersAttribute";
        case TokenType::ReturnsAttribute:
            return "ReturnsAttribute";
        case TokenType::ParametersAttribute:
            return "ParametersAttribute";
        case TokenType::EndOfFile:
            return "EndOfFile";
        default:
            return "<err>";
        }
    }

    class Token {
        public:
            Token() = default;

            template<typename T>
            Token(TokenType type, std::string_view lexeme, T value, uint32_t line)
                :Type{ type }, Lexeme{ lexeme }, Line{ line } {
                if constexpr (std::is_same_v<T, int64_t>)
                    SignedLiteral = value;
                else if constexpr (std::is_same_v<T, uint64_t>)
                    UnsignedLitaral = value;
                else
                    FloatingPointLiteral = value;
            }


            Token(TokenType, uint32_t);

        public:
            [[nodiscard]] auto ToString() const -> std::string;

        public:
            TokenType        Type;
            std::string_view Lexeme;
            union {
                uint64_t     UnsignedLitaral;
                int64_t      SignedLiteral;
                double       FloatingPointLiteral;
            };
            uint32_t         Line;
    };

    class Lexer {
        public:
            Lexer(std::string);

        public:
            [[nodiscard]] auto Scan() -> std::vector<Token>&;
            [[nodiscard]] auto HadError() -> bool;

        private:

            [[nodiscard]] auto HasNext() -> bool;
        
            auto Next() -> void;
            [[nodiscard]] auto Peek() -> char;
            [[nodiscard]] auto PeekNext() -> char;
            [[nodiscard]] auto NextCharacter() -> char;

            template<typename T = uint64_t>
            auto AddToken(TokenType type, T literal = T{  }) -> void {
                std::string_view lexeme{ _src.begin() + _start, _src.begin() + _current };
                _tokenBuffer.push_back({ type, lexeme, literal, _line });
            }
            auto ReportError([[maybe_unused]] std::string_view) -> void;

            auto Number() -> void;
            auto Identifier() -> void;

        private:
            std::string        _src;
            uint32_t           _start;
            uint32_t           _current;
            uint32_t           _line;
            std::vector<Token> _tokenBuffer;
            bool               _hadError;
    };
}

#endif