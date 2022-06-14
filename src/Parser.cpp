#include "../include/Parser.hpp"
#include <cmath>
#include <cstdio>
#include <iterator>
#include <math.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace Yun::Interpreter {

Parser::Parser(std::vector<Token> tokens)
    :_assembler{  },     _tokens{ std::move(tokens) }, _current{ 0 },
     _hadError{ false }, _buffer{  },                  _isFull{ false },
     _state{  } {
}

[[nodiscard]] auto Parser::HadError() -> bool {
    return _hadError;
}

auto Parser::Parse() -> VM::ExecutionUnit {
    while (HasNext()) {
        Function();
    }
    return _assembler.Patch("Main");
}

[[nodiscard]] auto Parser::Next() -> Token& {
    if (_isFull) {
        _isFull = false;
        return _buffer;
    }
    return _tokens[_current++];
}

[[nodiscard]] auto Parser::NextPrintable() -> Token& {
    auto& t = Next();
    while (t.Type == TokenType::Newline)
        t = Next();
    return t;
}

[[nodiscard]] auto Parser::HasNext() -> bool {
    return (_current < _tokens.size() && _tokens[_current].Type != TokenType::EndOfFile) || 
           (_isFull && _buffer.Type != TokenType::EndOfFile);
}

auto Parser::Putback(Token token) -> void {
    if (!_isFull) {
        _buffer = token;
        _isFull = true;
    } else
        throw std::runtime_error{ "[Parser:Internal] Buffer is full" };
}

auto Parser::Function() -> void {
    _state = State();
    FunctionDeclaration();
    _assembler.BeginFunction(_state.Name, _state.RegisterCount, _state.ArgCount, _state.KeepReturnValue);
    Block();
    _assembler.EndFunction();
}

auto Parser::FunctionDeclaration() -> void {
    Attributes();
    auto& t = Next();

    if (t.Type != TokenType::Function)
        ReportError("Token mismatch: expected 'function' keyword");

    t = NextPrintable();

    if (t.Type != TokenType::Id)
        ReportError("Token mismatch: expected identifier");
    _state.Name = t.Lexeme;

    t = Next();

    if (t.Type != TokenType::LeftParen)
        ReportError("Token mismatch: expected left paren");

    t = Next();

    if (t.Type != TokenType::RightParen)
        ReportError("Token mismatch: expected right paren");
}

auto Parser::Attributes() -> void {
    auto& t = NextPrintable();

    if (t.Type != TokenType::LeftBracket)
        ReportError("Token mismatch: expected left bracket");

    t = Next();

    for (auto& token = Next(); true; token = Next()) {
        if (t.Type == TokenType::RegistersAttribute) {
            if (token.Type != TokenType::Equals)
                ReportError("Token mismatch: expected '='");
            token = Next();
            if (token.Type != TokenType::UnsignedLiteral)
                ReportError("Token mismatch: expected unsigned literal");
            _state.RegisterCount = token.UnsignedLitaral;
        } else if (t.Type == TokenType::ParametersAttribute) {
            if (token.Type != TokenType::Equals)
                ReportError("Token mismatch: expected '='");
            token = Next();
            if (token.Type != TokenType::UnsignedLiteral)
                ReportError("Token mismatch: expected unsigned literal");
            _state.ArgCount = token.UnsignedLitaral;
        } else if (t.Type == TokenType::ReturnsAttribute) {
            if (token.Type != TokenType::Equals)
                ReportError("Token mismatch: expected '='");
            token = Next();
            if (token.Type != TokenType::False && token.Type != TokenType::True)
                ReportError("Token mismatch: expected boolean value");
            _state.KeepReturnValue = token.Type == TokenType::False? false : true;
        } else
            ReportError("Bad token");

        t = Next();

        if (t.Type == TokenType::Comma)
            t = Next();
        else {
            Putback(t);
            break;
        }
    }
    t = Next();

    if (t.Type != TokenType::RightBracket)
        ReportError("Token mismatch: expected right bracket");

    t = Next();

    if (t.Type != TokenType::Newline)
        ReportError("Token mismatch: expected newline");

    return;    
}

auto Parser::Block() -> void {
    auto& t = Next();

    if (t.Type != TokenType::LeftBrace)
        ReportError("Token mismatch: expected '{'");
    
    t = Next();

    if (t.Type != TokenType::Newline)
        ReportError("Token mismatch: expected '\\n'");
    
    while (true) {
        t = NextPrintable();
        if (t.Type == TokenType::RightBrace)
            break;
        else
            Putback(t);
        Line();
    }
}

auto Parser::Line() -> void {
    auto& t = NextPrintable();

    if (t.Type == TokenType::Label) {
        _assembler.AddLabel(t.Lexeme.substr(0, t.Lexeme.size() - 1));
        t = NextPrintable();
    }

    if (t.Type == TokenType::Newline)
        return;

    if (t.Type != TokenType::Instruction)
        ReportError("Bad token");

    if (auto operands = VM::Instructions::OpcodeCount(t.InstrLiteral); operands == 0) {
        _assembler.AddVoid(t.InstrLiteral);
    } else if (operands == 1) {
        auto& destination = Next();
        if (VM::Instructions::IsJump(t.InstrLiteral)) {
            if (destination.Type != TokenType::Id)
                ReportError("Token type mismatch: expected 'ID'");
            _assembler.AddJump(t.InstrLiteral, destination.Lexeme);
        } else if (t.InstrLiteral == VM::Instructions::Opcode::call) {
            if (destination.Type != TokenType::Id)
                ReportError("Token type mismatch: expected 'ID'");
            _assembler.AddCall(t.Lexeme);
        } else if (destination.Type == TokenType::Register) {
            _assembler.AddUnary(t.InstrLiteral, std::stoul(destination.Lexeme.c_str() + 1));
        } else
            ReportError("Operand mismatch");
    } else if (operands == 2) {

        auto& destination = Next();

        if (destination.Type != TokenType::Register)
            ReportError("Expected register");

        auto destIndex = std::stoul(destination.Lexeme.c_str() + 1);

        if (Next().Type != TokenType::Comma)
            ReportError("Expected comma");
        
        auto& source = Next();

        if (VM::Instructions::Opcode::ldconst == t.InstrLiteral) {
            if (source.Type == TokenType::FloatLiteral)
                _assembler.LoadConstant(destIndex, (double)source.FloatingPointLiteral);
            else if (source.Type == TokenType::UnsignedLiteral)
                _assembler.LoadConstant(destIndex, (uint64_t)source.UnsignedLitaral);
            else if (source.Type == TokenType::SignedLiteral)
                _assembler.LoadConstant(destIndex, (int64_t)source.SignedLiteral);
            else
                ReportError("Invalid constant");
        } else {
            auto srcIndex = std::stoul(source.Lexeme.c_str() + 1);
            _assembler.AddBinary(t.InstrLiteral, destIndex, srcIndex);
        }
    }

    t = Next();

    if (t.Type != TokenType::Newline)
        ReportError("Expected newline after void instruction");
}

auto Parser::ReportError(std::string_view message) -> void {
    puts(message.data());
    throw Error::ParseError{  };
}

}