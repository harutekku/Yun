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

    as.BeginFunction("main", 2, 0, false);
    {
        as.LoadConstant(0, 16u);
        as.LoadConstant(1, 1u);
        as.AddBinary(newarray, 0, 1);
        as.LoadConstant(0, 2);
        as.AddVoid(ret);
    }
    as.EndFunction();

    auto e = as.Patch("Test");
    
    VM v{ std::move(e) };
    v.Run();
    
    return 0;
} catch (std::exception& e) {
    puts(e.what());
}
