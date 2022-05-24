// C header files
#include <cstdio>
// My header files
#include "../include/Emit.hpp"
#include "../include/Instructions.hpp"
#include "../include/Assembler.hpp"
// L O L : ^ O -->
#define Slap Patch

auto main(void) -> int try {
    using namespace Yun::VM::Instructions;
    using namespace Yun::ASM;
    using enum Yun::VM::Instructions::Opcode;
    
    Assembler ass{  };

    ass.AddJump(jmp, "Label-1");
    ass.AddVoid(nop);
    ass.AddVoid(nop);
    ass.AddBinary(add, 1, 2);
    ass.AddBinary(add, 3, 4);
    ass.AddLabel("Label-1");
    ass.AddVoid(hlt);

    auto res = ass.Slap();

    res.Disassemble();

    return 0;
} catch (std::exception& e) {
    puts(e.what());
}
