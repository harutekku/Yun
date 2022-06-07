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

    as.BeginFunction("main", 1, 0, false);
    {
        as.LoadConstant(0, 40ul);
        as.AddCall("fib");
        as.AddVoid(ret);
    }
    as.EndFunction();

    // Fibonacci(): Value
    as.BeginFunction("fib", 4, 1, true);
    {
        as.LoadConstant(1, 1ul);
        as.AddBinary(cmp, 0, 1);
        as.AddJump(jgt, "end");

        as.AddVoid(ret);

        as.AddLabel("end");

        as.AddBinary(mov, 3, 0);
        as.AddBinary(u64sub, 3, 1);
        
        as.AddCall("fib");

        as.AddBinary(mov, 2, 3);

        as.LoadConstant(1, 2ul);
        as.AddBinary(mov, 3, 0);
        as.AddBinary(u64sub, 3, 1);
        as.AddCall("fib");

        as.AddBinary(u64add, 2, 3);

        as.AddBinary(mov, 0, 2);

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
