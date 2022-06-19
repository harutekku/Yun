#include <algorithm>
#include <cstdio>
#include <memory>
#include <exception>
#include <cstdarg>
#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"
#include "../include/VM.hpp"

static auto ReportErrorAndExit(std::string_view format, ...) -> void {
    va_list argList;
    va_start(argList, format);
    vfprintf(stderr, format.data(), argList);
    va_end(argList);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}

static auto PrintHelp() -> void {
    puts("Yun Virtual Machine\n"
         "Usage: yvm [options] INPUT\n"
         "Assemble and run a YASN file\n\n"
         "Options:\n"
         "  -h    Print this message and exit\n"
         "  -d    Disassemble current file\n"
         "  -t    Print tokens\n"
         "Author: Harutekku"
         );
}

[[nodiscard]] static auto GetRawSource(const char* filename) -> std::string {
    std::unique_ptr<FILE, int(*)(FILE*)> file{ fopen(filename, "r"), fclose };
    if (!file)
        ReportErrorAndExit("Error: Invalid file");
    fseek(file.get(), 0, SEEK_END);
    size_t size = ftell(file.get());

    std::string buffer{  };
    buffer.resize(size);

    rewind(file.get());
    if (auto read = fread(buffer.data(), 1, size, file.get()); read != size)
        ReportErrorAndExit("Error: file too big");
    return buffer;
}

struct ProgramOptions {
    constexpr ProgramOptions() noexcept
        :Filename{ nullptr }, Disassemble{ false }, PrintTokens{ false }, ShowHelp{ false } {
    }
    const char* Filename;
    bool        Disassemble;
    bool        PrintTokens;
    bool        ShowHelp;
};

[[nodiscard]] static auto ParseOptions(const int argc, const char* argv[]) noexcept -> ProgramOptions {
    ProgramOptions options{  };
    if (argc == 1)
        ReportErrorAndExit("Error: no input files");
    else if (argc == 2)
        if (!strcmp(argv[1], "-h"))
            options.ShowHelp = true;
        else
            options.Filename = argv[1];
    else if (argc == 3) {
        if (argv[1][0] != '-')
            ReportErrorAndExit("Error: invalid options format\n"
                               "Usage: yvm [-dht] INPUT");
        auto len = strlen(argv[1]);
        size_t i = 1;
        for (; i < len; ++i) {
            switch (argv[1][i]) {
            case 'h':
                options.ShowHelp = true;
                break;
            case 'd':
                options.Disassemble = true;
                break;
            case 't':
                options.PrintTokens = true;
                break;
            default:
                ReportErrorAndExit("Error: unrecognized option - '%c'", argv[1][i]);
                break;
            }
        }
        if (i != len)
            ReportErrorAndExit("Error: Failed to parse arguments");
        options.Filename = argv[2];
    } else
        ReportErrorAndExit("Error: unrecognized trailing options\n"
                           "Usage: yvm [-dht] INPUT");

    return options;
}

auto main(const int argc, const char* argv[]) -> int try {
    auto options = ParseOptions(argc, argv);
    if (options.ShowHelp) {
        PrintHelp();
        exit(EXIT_SUCCESS);
    }

    Yun::Interpreter::Lexer l{ GetRawSource(options.Filename) };
    auto& tokens = l.Scan();

    if (l.HadError())
        exit(EXIT_FAILURE);
    else if (options.PrintTokens)
        for (auto& token : tokens)
            puts(token.ToString().c_str());

    Yun::Interpreter::Parser p{ std::move(tokens) };
    auto executionUnit = p.Parse();

    if (options.Disassemble)
        executionUnit.Disassemble();

    Yun::VM::VM v{ std::move(executionUnit) };

    v.Run();
    return EXIT_SUCCESS;
} catch (Yun::Error::ParseError&) {
    return EXIT_FAILURE;
} catch (std::exception& e) {
    puts(e.what());
    return EXIT_FAILURE;
}
