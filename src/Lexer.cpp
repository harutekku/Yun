#include "../include/Lexer.hpp"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <map>

namespace Yun::Interpreter {

const static std::map<std::string, TokenType> Keywords{
    { "true", TokenType::True },
    { "false", TokenType::False },
    { "function", TokenType::Function },
    { "registers", TokenType::RegistersAttribute },
    { "returns", TokenType::ReturnsAttribute },
    { "parameters", TokenType::ParametersAttribute },
    { "i32neg", TokenType::Instruction },
    { "i32add", TokenType::Instruction },
    { "i32sub", TokenType::Instruction },
    { "i32mul", TokenType::Instruction },
    { "i32div", TokenType::Instruction },
    { "i32rem", TokenType::Instruction },
    { "i32and", TokenType::Instruction },
    { "i32or", TokenType::Instruction },
    { "i32xor", TokenType::Instruction },
    { "i32shl", TokenType::Instruction },
    { "i32shr", TokenType::Instruction },
    { "i64neg", TokenType::Instruction },
    { "i64add", TokenType::Instruction },
    { "i64sub", TokenType::Instruction },
    { "i64mul", TokenType::Instruction },
    { "i64div", TokenType::Instruction },
    { "i64rem", TokenType::Instruction },
    { "i64and", TokenType::Instruction },
    { "i64or", TokenType::Instruction },
    { "i64xor", TokenType::Instruction },
    { "i64shl", TokenType::Instruction },
    { "i64shr", TokenType::Instruction },
    { "u32add", TokenType::Instruction },
    { "u32sub", TokenType::Instruction },
    { "u32mul", TokenType::Instruction },
    { "u32div", TokenType::Instruction },
    { "u32rem", TokenType::Instruction },
    { "u32and", TokenType::Instruction },
    { "u32or", TokenType::Instruction },
    { "u32xor", TokenType::Instruction },
    { "u32shl", TokenType::Instruction },
    { "u32shr", TokenType::Instruction },
    { "u64add", TokenType::Instruction },
    { "u64sub", TokenType::Instruction },
    { "u64mul", TokenType::Instruction },
    { "u64div", TokenType::Instruction },
    { "u64rem", TokenType::Instruction },
    { "u64and", TokenType::Instruction },
    { "u64or", TokenType::Instruction },
    { "u64xor", TokenType::Instruction },
    { "u64shl", TokenType::Instruction },
    { "u64shr", TokenType::Instruction },
    { "f32neg", TokenType::Instruction },
    { "f32add", TokenType::Instruction },
    { "f32sub", TokenType::Instruction },
    { "f32mul", TokenType::Instruction },
    { "f32div", TokenType::Instruction },
    { "f32rem", TokenType::Instruction },
    { "f64neg", TokenType::Instruction },
    { "f64add", TokenType::Instruction },
    { "f64sub", TokenType::Instruction },
    { "f64mul", TokenType::Instruction },
    { "f64div", TokenType::Instruction },
    { "f64rem", TokenType::Instruction },
    { "bnot", TokenType::Instruction },
    { "convi32toi8", TokenType::Instruction },
    { "convi32toi16", TokenType::Instruction },
    { "convu32tou8", TokenType::Instruction },
    { "convu32tou16", TokenType::Instruction },
    { "convi32toi64", TokenType::Instruction },
    { "convi32tou64", TokenType::Instruction },
    { "convi32tou32", TokenType::Instruction },
    { "convi32tof32", TokenType::Instruction },
    { "convi32tof64", TokenType::Instruction },
    { "convi64toi32", TokenType::Instruction },
    { "convi64tou32", TokenType::Instruction },
    { "convi64tou64", TokenType::Instruction },
    { "convi64tof32", TokenType::Instruction },
    { "convi64tof64", TokenType::Instruction },
    { "convu32toi64", TokenType::Instruction },
    { "convu32tou64", TokenType::Instruction },
    { "convu32toi32", TokenType::Instruction },
    { "convu32tof32", TokenType::Instruction },
    { "convu32tof64", TokenType::Instruction },
    { "convu64toi64", TokenType::Instruction },
    { "convu64tou32", TokenType::Instruction },
    { "convu64toi32", TokenType::Instruction },
    { "convu64tof32", TokenType::Instruction },
    { "convu64tof64", TokenType::Instruction },
    { "convf32toi32", TokenType::Instruction },
    { "convf32toi64", TokenType::Instruction },
    { "convf32tou32", TokenType::Instruction },
    { "convf32tof64", TokenType::Instruction },
    { "convf32tou64", TokenType::Instruction },
    { "convf64toi32", TokenType::Instruction },
    { "convf64toi64", TokenType::Instruction },
    { "convf64tou32", TokenType::Instruction },
    { "convf64tou64", TokenType::Instruction },
    { "convf64tof32", TokenType::Instruction },
    { "cmp", TokenType::Instruction },
    { "icmp", TokenType::Instruction },
    { "fcmp", TokenType::Instruction },
    { "jmp", TokenType::Instruction },
    { "je", TokenType::Instruction },
    { "jne", TokenType::Instruction },
    { "jgt", TokenType::Instruction },
    { "jge", TokenType::Instruction },
    { "jlt", TokenType::Instruction },
    { "jle", TokenType::Instruction },
    { "call", TokenType::Instruction },
    { "ret", TokenType::Instruction },
    { "ldconst", TokenType::Instruction },
    { "mov", TokenType::Instruction },
    { "newarray", TokenType::Instruction },
    { "arraycount", TokenType::Instruction },
    { "load", TokenType::Instruction },
    { "store", TokenType::Instruction },
    { "advance", TokenType::Instruction },
    { "dbgprintreg", TokenType::Instruction },
    { "nop", TokenType::Instruction },
    { "hlt", TokenType::Instruction }
};

Token::Token(TokenType type, uint32_t line)
    :Type{ type }, Lexeme{  }, Line{ line } {
}

Lexer::Lexer(std::string source)
    :_src{ source }, _start{ 0 }, _current{ 0 }, 
     _line{ 1 },     _hadError{ false } {
}

[[nodiscard]] auto Token::ToString() const -> std::string {
    std::string retVal{ "(" };
    retVal.append(Type == TokenType::Newline? "\\n" : Lexeme);
    retVal.append(":");
    retVal.append(TokenTypeToString(Type));
    retVal.append(")");
    return retVal;
}

[[nodiscard]] auto Lexer::Scan() -> std::vector<Token>& {
    while (HasNext()) {
        _start = _current;
        Next();
    }
    _tokenBuffer.push_back({ TokenType::EndOfFile, _line });
    return _tokenBuffer;
}

[[nodiscard]] auto Lexer::HadError() -> bool {
    return _hadError;
}

[[nodiscard]] auto Lexer::HasNext() -> bool {
    return _current < _src.size();
}

auto Lexer::Next() -> void {
    auto c = NextCharacter();

    switch (c) {
    case '(':
        AddToken(TokenType::LeftParen);
        break;
    case ')':
        AddToken(TokenType::RightParen);
        break;
    case '{':
        AddToken(TokenType::LeftBrace);
        break;
    case '}':
        AddToken(TokenType::RightBrace);
        break;
    case '[':
        AddToken(TokenType::LeftBracket);
        break;
    case ']':
        AddToken(TokenType::RightBracket);
        break;
    case ',':
        AddToken(TokenType::Comma);
        break;
    case '=':
        AddToken(TokenType::Equals);
        break;
    case '\n':
        AddToken(TokenType::Newline);
        ++_line;
        break;
    case ' ':
        break;
    case '\t':
        break;
    case '\r':
        break;
    case '$':
        if (auto ch = NextCharacter(); std::isdigit(ch) || ch == '-')
            Number();
        else
            ReportError("Unexpected characters in immediate operand");
        break;
    default:
        if (std::isalpha(c))
            Identifier();
        else
            ReportError("Unexpected character");
        break;
    }
}

[[nodiscard]] auto Lexer::Peek() -> char {
    if (!HasNext())
        return '\0';
    return _src[_current];
}

[[nodiscard]] auto Lexer::PeekNext() -> char {
    if (_current + 1 >= _src.size())
        return '\0';
    return _src[_current + 1];
}

[[nodiscard]] auto Lexer::NextCharacter() -> char {
    return _src[_current++];
}

[[nodiscard]] static auto CountDigits(uint32_t n) -> int {
    int retVal = 0;
    while (n != 0) {
        n /= 10;
        ++retVal;
    }
    return retVal;
}

auto Lexer::ReportError([[maybe_unused]] std::string_view message) -> void {
    std::printf("error: %s\n\n    ", message.data());
    std::printf("%u | ", _line);

    uint64_t lineStart = 0;
    uint64_t lineEnd   = 0;
    uint64_t i = 0;
    int      digits = CountDigits(_line);

    for (i = _start; i > 0 && _src[i] != '\n'; --i);
    lineStart = i == 0? i : i + 1;
    for (i = _start; i < _src.size() && _src[i] != '\n'; ++i);
    lineEnd   = i;


    for (i = lineStart; i != lineEnd; ++i)
        std::putchar(_src[i]);
    std::printf("\n");
    for (i = 0; i < _start - lineStart + digits + 7; ++i)
        std::putchar(' ');

    std::putchar('^');

    for (i = _start; i < _current - 1; ++i)
        std::putchar('~');

    std::puts("\n");

    _hadError = true;
}

auto Lexer::Number() -> void {
    bool isSigned = false;
    bool isFloat  = false;
    [[maybe_unused]] char c = '\0';

    if (_src[_current - 1] == '-')
        isSigned = true;

    while (std::isdigit(Peek()))
        c = NextCharacter();
    
    if (Peek() == '.' && std::isdigit(PeekNext())) {
        isFloat = true;
        c = NextCharacter();
        while (std::isdigit(Peek()))
            c = NextCharacter();
    }

        auto end = _src.data() + _current;
    if (isFloat) {
        auto res = std::strtod(_src.data() + _start + 1, &end);
        if (errno == ERANGE || end != _src.data() + _current) {
            ReportError("Invalid literal");
            return;
        } else
            AddToken(TokenType::FloatLiteral, res);
    } else if (isSigned) {
        auto res = std::strtol(_src.data() + _start + 1, &end, 10);
        if (errno == ERANGE || end != _src.data() + _current) {
            ReportError("Invalid literal");
            return;
        } else
            AddToken(TokenType::SignedLiteral, res);
    } else {
        auto res = std::strtoul(_src.data() + _start + 1, &end, 10);
        if (errno == ERANGE || end != _src.data() + _current) {
            ReportError("Invalid literal");
            return;
        } else
            AddToken(TokenType::UnsignedLiteral, res);
    }
}

auto Lexer::Identifier() -> void {
    for (char c = Peek(); std::isalpha(c) || std::isdigit(c); c = Peek())
        c = NextCharacter();
    std::string key{ _src.begin() + _start, _src.begin() + _current };
    if (auto it = Keywords.find(key); it != Keywords.end()) {
        AddToken(it->second);
        return;
    }

    AddToken(TokenType::Id);
}

}