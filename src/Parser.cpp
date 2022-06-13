#include "../include/Parser.hpp"
#include <vector>

namespace Yun::Interpreter {

Parser::Parser(std::vector<Token> tokens)
    :_assembler{  }, _tokens{ std::move(tokens) }, _hadError{ false },
     _buffer{  },    _isEmpty{ false } {
}

[[nodiscard]] auto Parser::HadError() -> bool {
    return _hadError;
}

[[nodiscard]] auto Parser::Parse() -> VM::ExecutionUnit {
    while (HasNext())
        Function();
    return _assembler.Patch("Unit");
}

[[nodiscard]] auto Parser::Next() -> Token& {
    if (_isEmpty) {
        _isEmpty = false;
        return _buffer;
    }
    return _tokens[_current++];
}

[[nodiscard]] auto Parser::HasNext() -> bool {
    return _current < _tokens.size() || !_isEmpty;
}

auto Parser::Function() -> void {
    // TODO: Implement
}

}