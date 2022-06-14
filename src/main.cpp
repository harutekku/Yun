#include <algorithm>
#include <cstdio>
#include <memory>
#include <exception>
#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"
#include "../include/VM.hpp"

[[nodiscard]] auto GetRawSource(const char* filename) -> std::string {
    std::unique_ptr<FILE, int(*)(FILE*)> file{ fopen(filename, "r"), fclose };
    fseek(file.get(), 0, SEEK_END);
    size_t size = ftell(file.get());

    std::string buffer{  };
    buffer.resize(size);

    rewind(file.get());
    fread(buffer.data(), 1, size, file.get());

    buffer.erase(buffer.find_last_not_of("\n") + 1);

    return buffer;
}

auto main(void) -> int try {

    Yun::Interpreter::Lexer l{ GetRawSource("Test.txt") };
    auto& res = l.Scan();

    if (l.HadError())
        return 1;

    Yun::Interpreter::Parser p{ std::move(res) };
    auto e = p.Parse();

    Yun::VM::VM v{ std::move(e) };

    v.Run();
    return 0;
} catch (Yun::Error::ParseError&) {
} catch (std::exception& e) {
    puts(e.what());
}
