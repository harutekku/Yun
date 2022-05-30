// C header files
#include <cstdio>
#include <iterator>
// My header files
#include "../include/VM.hpp"
#include "../include/Instructions.hpp"
#include "../include/Assembler.hpp"

auto main(void) -> int try {
    using namespace Yun::VM::Instructions;
    using namespace Yun::VM;
    using namespace Yun::ASM;
    using enum Yun::VM::Instructions::Opcode;
    
    Assembler as{  };

    as.AddLoadConstant(ldconst, 0, 2.0f);
    as.AddLoadConstant(ldconst, 1, 2.1f);
    as.AddBinary(fcmp, 0, 1);

    auto res = as.Patch("Test");
    res.Disassemble();

    return 0;
} catch (std::exception& e) {
    puts(e.what());
}
