#include "../include/Lexer.hpp"
#include "../include/Instructions.hpp"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <map>
#include <string>

namespace Yun::Interpreter {

struct MappedValues {
    constexpr MappedValues(TokenType type)
        :Type{ type }, InstrValue{  } {
    }

    constexpr MappedValues(TokenType type, VM::Instructions::Opcode value)
        :Type{ type }, InstrValue{ value } {
    }

    TokenType Type;
    VM::Instructions::Opcode InstrValue;
};

const static std::map<std::string, MappedValues> Keywords{
    { "true",           TokenType::True },
    { "false",          TokenType::False },
    { "function",       TokenType::Function },
    { "registers",      TokenType::RegistersAttribute },
    { "returns",        TokenType::ReturnsAttribute },
    { "parameters",     TokenType::ParametersAttribute },
    { "i32neg",       { TokenType::Instruction, VM::Instructions::Opcode::i32neg } },
    { "i32add",       { TokenType::Instruction, VM::Instructions::Opcode::i32add } },
    { "i32sub",       { TokenType::Instruction, VM::Instructions::Opcode::i32sub } },
    { "i32mul",       { TokenType::Instruction, VM::Instructions::Opcode::i32mul } },
    { "i32div",       { TokenType::Instruction, VM::Instructions::Opcode::i32div } },
    { "i32rem",       { TokenType::Instruction, VM::Instructions::Opcode::i32rem } },
    { "i32and",       { TokenType::Instruction, VM::Instructions::Opcode::i32and } },
    { "i32or",        { TokenType::Instruction, VM::Instructions::Opcode::i32or  } },
    { "i32xor",       { TokenType::Instruction, VM::Instructions::Opcode::i32xor } },
    { "i32shl",       { TokenType::Instruction, VM::Instructions::Opcode::i32shl } },
    { "i32shr",       { TokenType::Instruction, VM::Instructions::Opcode::i32shr } },
    { "i64neg",       { TokenType::Instruction, VM::Instructions::Opcode::i64neg } },
    { "i64add",       { TokenType::Instruction, VM::Instructions::Opcode::i64add } },
    { "i64sub",       { TokenType::Instruction, VM::Instructions::Opcode::i64sub } },
    { "i64mul",       { TokenType::Instruction, VM::Instructions::Opcode::i64mul } },
    { "i64div",       { TokenType::Instruction, VM::Instructions::Opcode::i64div } },
    { "i64rem",       { TokenType::Instruction, VM::Instructions::Opcode::i64rem } },
    { "i64and",       { TokenType::Instruction, VM::Instructions::Opcode::i64and } },
    { "i64or",        { TokenType::Instruction, VM::Instructions::Opcode::i64or } },
    { "i64xor",       { TokenType::Instruction, VM::Instructions::Opcode::i64xor } },
    { "i64shl",       { TokenType::Instruction, VM::Instructions::Opcode::i64shl } },
    { "i64shr",       { TokenType::Instruction, VM::Instructions::Opcode::i64shr } },
    { "u32add",       { TokenType::Instruction, VM::Instructions::Opcode::u32add } },
    { "u32sub",       { TokenType::Instruction, VM::Instructions::Opcode::u32sub } },
    { "u32mul",       { TokenType::Instruction, VM::Instructions::Opcode::u32mul } },
    { "u32div",       { TokenType::Instruction, VM::Instructions::Opcode::u32div } },
    { "u32rem",       { TokenType::Instruction, VM::Instructions::Opcode::u32rem } },
    { "u32and",       { TokenType::Instruction, VM::Instructions::Opcode::u32and } },
    { "u32or",        { TokenType::Instruction, VM::Instructions::Opcode::u32or } },
    { "u32xor",       { TokenType::Instruction, VM::Instructions::Opcode::u32xor } },
    { "u32shl",       { TokenType::Instruction, VM::Instructions::Opcode::u32shl } },
    { "u32shr",       { TokenType::Instruction, VM::Instructions::Opcode::u32shr } },
    { "u64add",       { TokenType::Instruction, VM::Instructions::Opcode::u64add } },
    { "u64sub",       { TokenType::Instruction, VM::Instructions::Opcode::u64sub } },
    { "u64mul",       { TokenType::Instruction, VM::Instructions::Opcode::u64mul } },
    { "u64div",       { TokenType::Instruction, VM::Instructions::Opcode::u64div } },
    { "u64rem",       { TokenType::Instruction, VM::Instructions::Opcode::u64rem } },
    { "u64and",       { TokenType::Instruction, VM::Instructions::Opcode::u64and } },
    { "u64or",        { TokenType::Instruction, VM::Instructions::Opcode::u64or } },
    { "u64xor",       { TokenType::Instruction, VM::Instructions::Opcode::u64xor } },
    { "u64shl",       { TokenType::Instruction, VM::Instructions::Opcode::u64shl } },
    { "u64shr",       { TokenType::Instruction, VM::Instructions::Opcode::u64shr } },
    { "f32neg",       { TokenType::Instruction, VM::Instructions::Opcode::f32neg } },
    { "f32add",       { TokenType::Instruction, VM::Instructions::Opcode::f32add } },
    { "f32sub",       { TokenType::Instruction, VM::Instructions::Opcode::f32sub } },
    { "f32mul",       { TokenType::Instruction, VM::Instructions::Opcode::f32mul } },
    { "f32div",       { TokenType::Instruction, VM::Instructions::Opcode::f32div } },
    { "f32rem",       { TokenType::Instruction, VM::Instructions::Opcode::f32rem } },
    { "f64neg",       { TokenType::Instruction, VM::Instructions::Opcode::f64neg } },
    { "f64add",       { TokenType::Instruction, VM::Instructions::Opcode::f64add } },
    { "f64sub",       { TokenType::Instruction, VM::Instructions::Opcode::f64sub } },
    { "f64mul",       { TokenType::Instruction, VM::Instructions::Opcode::f64mul } },
    { "f64div",       { TokenType::Instruction, VM::Instructions::Opcode::f64div } },
    { "f64rem",       { TokenType::Instruction, VM::Instructions::Opcode::f64rem } },
    { "bnot",         { TokenType::Instruction, VM::Instructions::Opcode::bnot } },
    { "convi32toi8",  { TokenType::Instruction, VM::Instructions::Opcode::convi32toi8 } },
    { "convi32toi16", { TokenType::Instruction, VM::Instructions::Opcode::convi32toi16 } },
    { "convu32tou8",  { TokenType::Instruction, VM::Instructions::Opcode::convu32tou8 } },
    { "convu32tou16", { TokenType::Instruction, VM::Instructions::Opcode::convu32tou16 } },
    { "convi32toi64", { TokenType::Instruction, VM::Instructions::Opcode::convi32toi64 } },
    { "convi32tou64", { TokenType::Instruction, VM::Instructions::Opcode::convi32tou64 } },
    { "convi32tou32", { TokenType::Instruction, VM::Instructions::Opcode::convi32tou32 } },
    { "convi32tof32", { TokenType::Instruction, VM::Instructions::Opcode::convi32tof32 } },
    { "convi32tof64", { TokenType::Instruction, VM::Instructions::Opcode::convi32tof64 } },
    { "convi64toi32", { TokenType::Instruction, VM::Instructions::Opcode::convi64toi32 } },
    { "convi64tou32", { TokenType::Instruction, VM::Instructions::Opcode::convi64tou32 } },
    { "convi64tou64", { TokenType::Instruction, VM::Instructions::Opcode::convi64tou64 } },
    { "convi64tof32", { TokenType::Instruction, VM::Instructions::Opcode::convi64tof32 } },
    { "convi64tof64", { TokenType::Instruction, VM::Instructions::Opcode::convi64tof64 } },
    { "convu32toi64", { TokenType::Instruction, VM::Instructions::Opcode::convu32toi64 } },
    { "convu32tou64", { TokenType::Instruction, VM::Instructions::Opcode::convu32tou64 } },
    { "convu32toi32", { TokenType::Instruction, VM::Instructions::Opcode::convu32toi32 } },
    { "convu32tof32", { TokenType::Instruction, VM::Instructions::Opcode::convu32tof32 } },
    { "convu32tof64", { TokenType::Instruction, VM::Instructions::Opcode::convu32tof64 } },
    { "convu64toi64", { TokenType::Instruction, VM::Instructions::Opcode::convu64toi64 } },
    { "convu64tou32", { TokenType::Instruction, VM::Instructions::Opcode::convu64tou32 } },
    { "convu64toi32", { TokenType::Instruction, VM::Instructions::Opcode::convu64toi32 } },
    { "convu64tof32", { TokenType::Instruction, VM::Instructions::Opcode::convu64tof32 } },
    { "convu64tof64", { TokenType::Instruction, VM::Instructions::Opcode::convu64tof64 } },
    { "convf32toi32", { TokenType::Instruction, VM::Instructions::Opcode::convf32toi32 } },
    { "convf32toi64", { TokenType::Instruction, VM::Instructions::Opcode::convf32toi64 } },
    { "convf32tou32", { TokenType::Instruction, VM::Instructions::Opcode::convf32tou32 } },
    { "convf32tof64", { TokenType::Instruction, VM::Instructions::Opcode::convf32tof64 } },
    { "convf32tou64", { TokenType::Instruction, VM::Instructions::Opcode::convf32tou64 } },
    { "convf64toi32", { TokenType::Instruction, VM::Instructions::Opcode::convf64toi32 } },
    { "convf64toi64", { TokenType::Instruction, VM::Instructions::Opcode::convf64toi64 } },
    { "convf64tou32", { TokenType::Instruction, VM::Instructions::Opcode::convf64tou32 } },
    { "convf64tou64", { TokenType::Instruction, VM::Instructions::Opcode::convf64tou64 } },
    { "convf64tof32", { TokenType::Instruction, VM::Instructions::Opcode::convf64tof32 } },
    { "cmp",          { TokenType::Instruction, VM::Instructions::Opcode::cmp } },
    { "icmp",         { TokenType::Instruction, VM::Instructions::Opcode::icmp } },
    { "fcmp",         { TokenType::Instruction, VM::Instructions::Opcode::fcmp } },
    { "jmp",          { TokenType::Instruction, VM::Instructions::Opcode::jmp } },
    { "je",           { TokenType::Instruction, VM::Instructions::Opcode::je } },
    { "jne",          { TokenType::Instruction, VM::Instructions::Opcode::jne } },
    { "jgt",          { TokenType::Instruction, VM::Instructions::Opcode::jgt } },
    { "jge",          { TokenType::Instruction, VM::Instructions::Opcode::jge } },
    { "jlt",          { TokenType::Instruction, VM::Instructions::Opcode::jlt } },
    { "jle",          { TokenType::Instruction, VM::Instructions::Opcode::jle } },
    { "call",         { TokenType::Instruction, VM::Instructions::Opcode::call } },
    { "ret",          { TokenType::Instruction, VM::Instructions::Opcode::ret } },
    { "ldconst",      { TokenType::Instruction, VM::Instructions::Opcode::ldconst } },
    { "mov",          { TokenType::Instruction, VM::Instructions::Opcode::mov } },
    { "newarray",     { TokenType::Instruction, VM::Instructions::Opcode::newarray } },
    { "arraycount",   { TokenType::Instruction, VM::Instructions::Opcode::arraycount } },
    { "load",         { TokenType::Instruction, VM::Instructions::Opcode::load } },
    { "store",        { TokenType::Instruction, VM::Instructions::Opcode::store } },
    { "advance",      { TokenType::Instruction, VM::Instructions::Opcode::advance } },
    { "dbgprintreg",  { TokenType::Instruction, VM::Instructions::Opcode::dbgprintreg } },
    { "nop",          { TokenType::Instruction, VM::Instructions::Opcode::nop } },
    { "hlt",          { TokenType::Instruction, VM::Instructions::Opcode::hlt } }
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
    case '@':
        // TODO: Implement
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

    std::string res{ _src.data() + _start + 1, _src.data() + _current };
    try {

    if (isFloat)
        AddToken(TokenType::FloatLiteral, std::stod(res));
    else if (isSigned)
        AddToken(TokenType::SignedLiteral, std::stol(res));
    else
        AddToken(TokenType::UnsignedLiteral, std::stoul(res));
    } catch (std::exception&) {
        ReportError("Invalid literal");
    }
}

auto Lexer::Identifier() -> void {
    bool mightBeRegister = false;    
    char c = _src[_current - 1];
    if (c == 'R' && std::isdigit(Peek()))
        mightBeRegister = true;

    for (c = NextCharacter(); std::isalpha(c) || std::isdigit(c); c = NextCharacter()) {
        if (!std::isdigit(c))
            mightBeRegister = false;
    }

    if (c == ':' && !mightBeRegister) {
        AddToken(TokenType::Label);
        return;
    }

    --_current;

    std::string key{ _src.begin() + _start, _src.begin() + _current };
    if (mightBeRegister) {
        AddToken(TokenType::Register);
        return;
    } else if (auto it = Keywords.find(key); it != Keywords.end()) {
        if (auto type = it->second.Type; type == TokenType::Instruction)
            AddToken(it->second.Type, it->second.InstrValue);
        else
            AddToken(it->second.Type);
        return;
    }

    AddToken(TokenType::Id);
}

}