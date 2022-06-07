/**
 * @file
 *   VM.hpp
 * @author
 *   Harutekku @link https://github.com/harutekku @endlink
 * @brief
 *   The core machinery behind the Yun virtual machine
 * @version
 *   0.0.1
 * @date
 *   2022-05-24
 * @copyright
 *   Copyright (c) 2022
 * 
 */
// My header files
#include "../include/VM.hpp"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <exception>
#include <stdexcept>

namespace Yun::VM {

ExecutionUnit::ExecutionUnit(std::string name, Containers::SymbolTable symbols, Containers::ConstantPool constants, Containers::InstructionBuffer instructions)
    :_name{ std::move(name) }, _symbols{ std::move(symbols) }, _constants{ std::move(constants) }, _buffer{ std::move(instructions) } {
}
    
[[nodiscard]] auto ExecutionUnit::Name() -> std::string_view {
    return _name;
}

[[nodiscard]] auto ExecutionUnit::StartPC() -> uint32_t* {
    return _buffer.begin();
}

[[nodiscard]] auto ExecutionUnit::StopPC() -> uint32_t* {
    return _buffer.end();
}

[[nodiscard]] auto ExecutionUnit::ConstantLookup(size_t index) -> Primitives::Value {
    return _constants.Read(index);
}

[[nodiscard]] auto ExecutionUnit::SymbolLookup(size_t index) -> const Containers::Symbol& {
    return _symbols.FindByLocation(index);
}

[[nodiscard]] auto ExecutionUnit::SymbolLookup(const std::string& string) -> const Containers::Symbol& {
    return _symbols.FindByName(string);
}

[[nodiscard]] auto ExecutionUnit::DisassembleInstruction(size_t offset) -> size_t {
    using enum Instructions::Opcode;
    printf("    0x%04zx | ", offset * 4);

    auto instruction = _buffer.begin()[offset++];
    auto op = (instruction >> 24) & 0xFF;
    auto dest = (instruction >> 12) & 0xFFF;
    auto src = (instruction) & 0xFFF;

    if (op > static_cast<uint8_t>(hlt)) {
        puts("<err>");
        return offset;
    }

    auto opcode = static_cast<Yun::VM::Instructions::Opcode>(op);
    

    int args = OpcodeCount(opcode);
    if (args == 1)
        if (opcode == call)
            printf(" %-12s @%s\n", OpcodeToString(opcode), _symbols.FindByLocation(instruction & 0xFFFFFF).Name.c_str());
        else if (Instructions::IsJump(opcode))
            printf(" %-12s 0x%x\n", OpcodeToString(opcode), instruction & 0xFFFFFF);
        else
            printf(" %-12s %x\n", OpcodeToString(opcode), dest);
    else if (args == 2)
        if (opcode == ldconst)
            printf(" %-12s R%d, $0x%d\n", OpcodeToString(opcode), dest, src);
        else
            printf(" %-12s R%d, R%d\n", OpcodeToString(opcode), dest, src);
    else
        printf(" %-12s\n", OpcodeToString(opcode));

    return offset;
}

auto ExecutionUnit::Disassemble() -> void {
    printf("===== Disassembly of the Execution Unit: %s =====\n\n", _name.data());

    puts("Symbol table:");
    _symbols.Print();

    puts("\nConstant pool:");
    _constants.Print();

    puts("\nInstructions:");

    const size_t range = (_buffer.end() - _buffer.begin()) / 4;
    size_t stIndex = 0;
    for (size_t offset = 0; offset != range;) {
        if (stIndex < _symbols.Count() && _symbols.At(stIndex).Start == offset * 4) {
                printf("  %s\n", _symbols.At(stIndex).PrettyFunctionSignature().c_str());
                ++stIndex;
        }
        offset = DisassembleInstruction(offset);
    }
}

VM::VM(ExecutionUnit unit)
    :_unit{ std::move(unit) }, _registers{  }, _flags{ 0 } {
}

[[nodiscard]] auto VM::GetRegisters(uint16_t destIndex, uint16_t srcIndex) -> std::pair<Primitives::Value&, Primitives::Value&> {
    return { _registers.At(_callStack.RelativeOffset() + destIndex), _registers.At(_callStack.RelativeOffset() + srcIndex) };
}

[[nodiscard]] auto VM::GetRegister(uint16_t destIndex) -> Primitives::Value& {
    return _registers.At(_callStack.RelativeOffset() + destIndex);
}

auto VM::Run() -> void {
    auto pc = _unit.StartPC();

    const auto& entryPoint = _unit.SymbolLookup("main");

    if (entryPoint.Start > (_unit.StopPC() - pc))
       throw Error::VMError{ "Invalid entry point offset" };
    else
        pc += entryPoint.Start / 4;
    
    Containers::Frame currentFrame{ entryPoint.End - 1, entryPoint.Registers, entryPoint.DoesReturn, entryPoint.End };
    _callStack.Push(currentFrame);
    _registers.Allocate(currentFrame.RegisterCount);

    int32_t destIndex;
    int32_t srcIndex;
    int32_t size;

    uint8_t              rawOp;
    Instructions::Opcode op;

    do {
        rawOp = (*pc & 0xFF000000) >> 24;
        if (rawOp > static_cast<uint8_t>(Instructions::Opcode::hlt))
            throw Error::InstructionError{ "Invalid instruction" };

        op = static_cast<Instructions::Opcode>(rawOp);

        if (auto res = Instructions::OpcodeCount(op); res == 1) {
            if (Instructions::IsJump(op) || op == Instructions::Opcode::call)
                destIndex = (*pc & 0x00FFFFFF) >> 2;
            else
                destIndex = (*pc & 0x00FFF000) >> 12;
            srcIndex  = 0;
        } else if (res == 2) {
            destIndex = (*pc & 0x00FFF000) >> 12;
            srcIndex  = *pc & 0x00000FFF;
        } else {
            destIndex = 0;
            srcIndex  = 0;
        }

        size = 1;

        #pragma region Interpreter

        switch (op) {
        case Instructions::Opcode::i32neg: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Neg<int32_t>();
            break;
        }
        case Instructions::Opcode::i32add: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Add<int32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i32sub: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Sub<int32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i32mul: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Mul<int32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i32div: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Div<int32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i32rem: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Rem<int32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i32and: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.And<int32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i32or: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Or<int32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i32xor: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Xor<int32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i32shl: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.ShiftLeft<int32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i32shr: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.ShiftRight<int32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i64neg: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Neg<int64_t>();
            break;
        }
        case Instructions::Opcode::i64add: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Add<int64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i64sub: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Sub<int64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i64mul: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Mul<int64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i64div: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Div<int64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i64rem: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Rem<int64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i64and: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.And<int64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i64or: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Or<int64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i64xor: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Xor<int64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i64shl: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.ShiftLeft<int64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::i64shr: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.ShiftRight<int64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u32add: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Add<uint32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u32sub: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Sub<uint32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u32mul: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Mul<uint32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u32div: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Div<uint32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u32rem: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Rem<uint32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u32and: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.And<uint32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u32or: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Or<uint32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u32xor: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Xor<uint32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u32shl: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.ShiftLeft<uint32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u32shr: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.ShiftRight<uint32_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u64add: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Add<uint64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u64sub: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Sub<uint64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u64mul: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Mul<uint64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u64div: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Div<uint64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u64rem: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Rem<uint64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u64and: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.And<uint64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u64or: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Or<uint64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u64xor: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Xor<uint64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u64shl: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.ShiftLeft<uint64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::u64shr: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.ShiftRight<uint64_t>(srcRegister);
            break;
        }
        case Instructions::Opcode::f32neg: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Neg<float>();
            break;
        }
        case Instructions::Opcode::f32add: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Add<float>(srcRegister);
            break;
        }
        case Instructions::Opcode::f32sub: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Sub<float>(srcRegister);
            break;
        }
        case Instructions::Opcode::f32mul: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Mul<float>(srcRegister);
            break;
        }
        case Instructions::Opcode::f32div: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Div<float>(srcRegister);
            break;
        }
        case Instructions::Opcode::f32rem: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Rem<float>(srcRegister);
            break;
        }
        case Instructions::Opcode::f64neg: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Neg<double>();
            break;
        }
        case Instructions::Opcode::f64add: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Add<double>(srcRegister);
            break;
        }
        case Instructions::Opcode::f64sub: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Sub<double>(srcRegister);
            break;
        }
        case Instructions::Opcode::f64mul: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Mul<double>(srcRegister);
            break;
        }
        case Instructions::Opcode::f64div: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Div<double>(srcRegister);
            break;
        }
        case Instructions::Opcode::f64rem: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Rem<double>(srcRegister);
            break;
        }
        case Instructions::Opcode::bnot: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Not();
            break;
        }
        case Instructions::Opcode::convi32toi8: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<int32_t, int8_t>();
            break;
        }
        case Instructions::Opcode::convi32toi16: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<int32_t, int16_t>();
            break;
        }
        case Instructions::Opcode::convu32tou8: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<uint32_t, uint8_t>();
            break;
        }
        case Instructions::Opcode::convu32tou16: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<uint32_t, uint16_t>();
            break;
        }
        case Instructions::Opcode::convi32toi64: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<int32_t, int64_t>();
            break;
        }
        case Instructions::Opcode::convi32tou64: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<int32_t, uint64_t>();
            break;
        }
        case Instructions::Opcode::convi32tou32: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<int32_t, uint32_t>();
            break;
        }
        case Instructions::Opcode::convi32tof32: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<int32_t, float>();
            break;
        }
        case Instructions::Opcode::convi32tof64: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<int32_t, double>();
            break;
        }
        case Instructions::Opcode::convi64toi32: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<int64_t, int32_t>();
            break;
        }
        case Instructions::Opcode::convi64tou32: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<int64_t, uint32_t>();
            break;
        }
        case Instructions::Opcode::convi64tou64: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<int64_t, uint64_t>();
            break;
        }
        case Instructions::Opcode::convi64tof32: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<int64_t, float>();
            break;
        }
        case Instructions::Opcode::convi64tof64: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<int64_t, double>();
            break;
        }
        case Instructions::Opcode::convu32toi64: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<uint32_t, int64_t>();
            break;
        }
        case Instructions::Opcode::convu32tou64: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<uint32_t, uint64_t>();
            break;
        }
        case Instructions::Opcode::convu32toi32: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<uint32_t, int32_t>();
            break;
        }
        case Instructions::Opcode::convu32tof32: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<uint32_t, float>();
            break;
        }
        case Instructions::Opcode::convu32tof64: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<uint32_t, double>();
            break;
        }
        case Instructions::Opcode::convu64toi64: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<uint64_t, int64_t>();
            break;
        }
        case Instructions::Opcode::convu64tou32: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<uint64_t, uint32_t>();
            break;
        }
        case Instructions::Opcode::convu64toi32: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<uint64_t, int32_t>();
            break;
        }
        case Instructions::Opcode::convu64tof32: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<uint64_t, float>();
            break;
        }
        case Instructions::Opcode::convu64tof64: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<uint64_t, double>();
            break;
        }
        case Instructions::Opcode::convf32toi32: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<float, int32_t>();
            break;
        }
        case Instructions::Opcode::convf32toi64: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<float, int64_t>();
            break;
        }
        case Instructions::Opcode::convf32tou32: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<float, uint32_t>();
            break;
        }
        case Instructions::Opcode::convf32tof64: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<float, double>();
            break;
        }
        case Instructions::Opcode::convf32tou64: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<float, uint64_t>();
            break;
        }
        case Instructions::Opcode::convf64toi32: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<double, int32_t>();
            break;
        }
        case Instructions::Opcode::convf64toi64: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<double, int64_t>();
            break;
        }
        case Instructions::Opcode::convf64tou32: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<double, uint32_t>();
            break;
        }
        case Instructions::Opcode::convf64tou64: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<double, uint64_t>();
            break;
        }
        case Instructions::Opcode::convf64tof32: {
            auto destRegister = GetRegister(destIndex);
            destRegister.Convert<double, float>();
            break;
        }
        case Instructions::Opcode::cmp: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex,srcIndex);
            _flags = destRegister.Compare<unsigned>(srcRegister);
            break;
        }
        case Instructions::Opcode::icmp: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex,srcIndex);
            _flags = destRegister.Compare<signed>(srcRegister);
            break;
        }
        case Instructions::Opcode::fcmp: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex,srcIndex);
            _flags = destRegister.Compare<float>(srcRegister);
            break;
        }
        case Instructions::Opcode::jmp: {
            size = destIndex;
            break;
        }
        case Instructions::Opcode::je: {
            if (_flags == 0)
                size = destIndex;
            break;
        }
        case Instructions::Opcode::jne: {
            if (_flags != 0)
                size = destIndex;
            break;
        }
        case Instructions::Opcode::jlt: {
            if (_flags < 0)
                size = destIndex;
            break;
        }
        case Instructions::Opcode::jle: {
            if (_flags <= 0)
                size = destIndex;
            break;
        }
        case Instructions::Opcode::jgt: {
            if (_flags > 0)
                size = destIndex;
            break;
        }
        case Instructions::Opcode::jge: {
            if (_flags >= 0)
                size = destIndex;
            break;
        }

        case Instructions::Opcode::call: {
            currentFrame.ReturnAddress = pc - _unit.StartPC() + size;
            _callStack.Push(currentFrame);

            const auto& symbol = _unit.SymbolLookup(destIndex << 2);

            // Allocate new registers
            _registers.Allocate(symbol.Registers);
            
            // Copy last `count` registers to a new frame
            if (symbol.Arguments != 0)
                _registers.Copy(symbol.Registers, symbol.Arguments);

            currentFrame.ReturnAddress   = 0;
            currentFrame.End             = symbol.End;
            currentFrame.RegisterCount   = symbol.Registers;
            currentFrame.KeepReturnValue = symbol.DoesReturn;
            
            size = 0;
            pc = _unit.StartPC() + destIndex;
            break;
        }

        case Instructions::Opcode::ret: {
            auto oldFrame = currentFrame;
            currentFrame = _callStack.Pop();

            if (oldFrame.KeepReturnValue && oldFrame.RegisterCount != 0)
                _registers.SaveReturnValue(oldFrame.RegisterCount);

            _registers.Deallocate(oldFrame.RegisterCount);

            size = 0;
            pc = _unit.StartPC() + currentFrame.ReturnAddress;
            break;
        }

        case Instructions::Opcode::ldconst: {
            auto& destRegister = GetRegister(destIndex);
            destRegister.Assign(_unit.ConstantLookup(srcIndex));
            break;
        }
        
        case Instructions::Opcode::mov: {
            auto [destRegister, srcRegister] = GetRegisters(destIndex, srcIndex);
            destRegister.Assign(srcRegister);
            break;
        }

        case Instructions::Opcode::nop:
            break;
        case Instructions::Opcode::hlt:
            getchar();
            break;
        }

        #pragma endregion

        pc += size;

        /*
        puts("Registers:");
        _registers.Print();
        printf("Flags: %d\n", _flags);
        putchar('\n');
        */
    } while (!_callStack.IsEmpty());
}

}
