#include <algorithm>
#include <cstdio>
#include <memory>
#include <exception>
#include "../include/Lexer.hpp"

auto main(void) -> int try {
    std::unique_ptr<FILE, int(*)(FILE*)> file{ fopen("Test.txt", "r"), fclose };
    fseek(file.get(), 0, SEEK_END);
    size_t size = ftell(file.get());

    std::string buffer{  };
    buffer.resize(size);

    rewind(file.get());
    fread(buffer.data(), 1, size, file.get());

    Yun::Interpreter::Lexer l{ std::move(buffer) };
    auto& res = l.Scan();

    if (l.HadError())
        return 1;

    for (const auto& tok : res)
        std::puts(tok.ToString().c_str());

    return 0;
} catch (std::exception& e) {
    puts(e.what());
}
