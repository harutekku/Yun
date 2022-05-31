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

    // oldValue
    as.AddLoadConstant(ldconst, 0, (uint64_t)0ull);

    // currentValue
    as.AddLoadConstant(ldconst, 1, (uint64_t)1ull);

    // i
    as.AddBinary(mov, 2, 1);

    // increment
    as.AddBinary(mov, 3, 1);

    // range
    as.AddLoadConstant(ldconst, 4, (uint64_t)20ull);


    as.AddLabel("begin");
    as.AddBinary(cmp, 2, 4);
    as.AddJump(je, "end");

    // temp
    as.AddBinary(mov, 5, 1);
    as.AddBinary(u64add, 1, 0);
    as.AddBinary(mov, 0, 5);

    as.AddBinary(u64add, 2, 3);
    as.AddJump(jmp, "begin");

    as.AddLabel("end");
    as.AddVoid(hlt);

    auto res = as.Patch("Test");

    VM vm{ std::move(res) };
    vm.Run();

    return 0;
} catch (std::exception& e) {
    puts(e.what());
}
