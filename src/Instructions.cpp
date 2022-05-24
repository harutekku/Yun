// C header files
#include <cstdio>
#include <cstring>
// C++ header files
#include <memory>
// My header files
#include "../include/Instructions.hpp"

namespace Yun::VM::Instructions {

static auto DisassembleInstruction(uint8_t* ptr, size_t offset) -> size_t {
    using enum Instructions::Opcode;
    printf("0x%04zx | ", offset);

    auto byte = ptr[offset++];
    if (byte > static_cast<uint8_t>(hlt)) {
        puts("<err>");
        return offset;
    }
    
    auto opcode = static_cast<Opcode>(byte);
    int args = OpcodeArgumentCount(opcode);

    if (args == 1) {
        int32_t dest = 0;
        std::memcpy(&dest, &ptr[offset], sizeof(dest));
        offset += 4;
        printf(" %s  %x\n", OpcodeToString(opcode), dest);
    } else if (args == 2) {
        int16_t dest;
        int16_t src;
        std::memcpy(&dest, &ptr[offset], 2);
        std::memcpy(&src, &ptr[offset + 2], 2);
        offset += 4;
        printf(" %s  R%d, R%d\n", OpcodeToString(opcode), (int16_t)dest, (int16_t)src);
    } else {
        printf(" %s\n", OpcodeToString(opcode));
    }
    return offset;
}

Buffer::Buffer(size_t size) {
    _data  = std::make_unique<uint8_t[]>(size);
    _last = &_data[size];
}

[[nodiscard]] auto Buffer::begin() noexcept -> uint8_t* {
    return &_data[0];    
}

[[nodiscard]] auto Buffer::end() noexcept -> uint8_t* {
    return _last;
}

[[nodiscard]] auto begin(Buffer& buffer) noexcept -> uint8_t* {
    return buffer.begin();
}

[[nodiscard]] auto end(Buffer& buffer) noexcept -> uint8_t* {
    return buffer.end();
}

auto Buffer::Disassemble() -> void {
    const size_t range = _last - &_data[0];
    for (size_t offset = 0; offset < range;)
        offset = DisassembleInstruction(&_data[0], offset);
}

}