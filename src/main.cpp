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

    // test(): Value
    as.BeginFunction("test", 4, 1, true);
    {
        as.LoadConstant(1, 3);      // R1    := $3
        as.AddBinary(icmp, 0, 1);   // FLAGS := R0 - R1
        as.AddJump(jge, "end");     // FLAGS >= 1? goto end

        as.LoadConstant(2, 1);      // R2     := $1
        as.AddBinary(i32add, 0, 2); // R0     := R0 + R2
        as.AddBinary(mov, 3, 0);    // R3     := R0
        as.AddCall("test");         // test(R3);
        as.AddLabel("end");
        as.AddVoid(ret);
    }
    as.EndFunction();

    as.BeginFunction("main", 1, 0, false);
    {
        as.LoadConstant(0, 1);
        as.AddCall("test");
        as.AddVoid(ret);
    }
    as.EndFunction();

    auto e = as.Patch("Test");

    e.Disassemble();

    VM v{ std::move(e) };
    v.Run();

    return 0;
} catch (std::exception& e) {
    puts(e.what());
}
