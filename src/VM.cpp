// My header files
#include "../include/VM.hpp"
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace Yun::VM {

ExecutionUnit::ExecutionUnit(std::string name, Containers::SymbolTable symbols, Containers::ConstantPool constants, Containers::InstructionBuffer instructions)
    :_name{ std::move(name) }, _symbols{ std::move(symbols) }, _constants{ std::move(constants) }, _buffer{ std::move(instructions) } {
}
    
[[nodiscard]] auto ExecutionUnit::Name() const noexcept -> std::string_view {
    return _name;
}

[[nodiscard]] auto ExecutionUnit::StartPC() const noexcept -> const uint32_t* {
    return _buffer.begin();
}

[[nodiscard]] auto ExecutionUnit::StopPC() const noexcept -> const uint32_t* {
    return _buffer.end();
}

[[nodiscard]] auto ExecutionUnit::ConstantLookup(size_t index) const -> Primitives::Value {
    return _constants.Read(index);
}

[[nodiscard]] auto ExecutionUnit::SymbolLookup(size_t index) const -> const Containers::Symbol& {
    return _symbols.FindByLocation(index);
}

[[nodiscard]] auto ExecutionUnit::SymbolLookup(const std::string& string) const -> const Containers::Symbol& {
    return _symbols.FindByName(string);
}

[[nodiscard]] auto ExecutionUnit::DisassembleInstruction(size_t offset) const noexcept -> size_t {
    printf("    0x%04zx | ", offset * 4);

    auto instruction = _buffer.begin()[offset++];
    auto op = (instruction >> 24) & 0xFF;
    auto dest = (instruction >> 12) & 0xFFF;
    auto src = (instruction) & 0xFFF;

    if (op > static_cast<uint8_t>(Instructions::Opcode::hlt)) {
        puts("<err>");
        return offset;
    }

    auto opcode = static_cast<Yun::VM::Instructions::Opcode>(op);
    

    int args = OpcodeCount(opcode);
    if (args == 1)
        if (opcode == Instructions::Opcode::call)
            printf(" %-12s @%s\n", OpcodeToString(opcode), _symbols.FindByLocation(instruction & 0xFFFFFF).Name.c_str());
        else if (Instructions::IsJump(opcode))
            printf(" %-12s 0x%x\n", OpcodeToString(opcode), instruction & 0xFFFFFF);
        else
            printf(" %-12s R%d\n", OpcodeToString(opcode), dest);
    else if (args == 2)
        if (opcode == Instructions::Opcode::ldconst)
            printf(" %-12s R%d, $0x%d\n", OpcodeToString(opcode), dest, src);
        else
            printf(" %-12s R%d, R%d\n", OpcodeToString(opcode), dest, src);
    else
        printf(" %-12s\n", OpcodeToString(opcode));

    return offset;
}

auto ExecutionUnit::Disassemble() const noexcept -> void {
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

VM::VM(ExecutionUnit unit) noexcept
    :_unit{ std::move(unit) }, _registers{  }, _callStack{  }, _heap{  }, _flags{ 0 } {
}

auto VM::Run() -> void {
    auto pc = _unit.StartPC();

    const auto& entryPoint = _unit.SymbolLookup("main");

    if (entryPoint.Start > (_unit.StopPC() - pc))
        ReportError("Entry point offset outside of instructions segment");
    else if (entryPoint.DoesReturn || entryPoint.Arguments != 0)
        ReportError("Invalid main signature");
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
            ReportError("Invalid instruction");

        op = static_cast<Instructions::Opcode>(rawOp);

        if (auto res = Instructions::OpcodeCount(op); res == 1) {
            if (Instructions::IsJump(op) || op == Instructions::Opcode::call) {
                if (auto int24 = *pc & 0x00FFFFFF; int24 & 0x800000)
                    destIndex = int24 | 0xFF000000;
                else
                    destIndex = int24;
                destIndex >>= 2;
            } else
                destIndex = (*pc & 0x00FFF000) >> 12;
            srcIndex  = 0;
        } else if (res == 2) {
            destIndex = ((*pc & 0x00FFF000) >> 12) + _callStack.RelativeOffset();
            if (op == Instructions::Opcode::ldconst)
                srcIndex  = (*pc & 0x00000FFF);
            else
                srcIndex  = (*pc & 0x00000FFF) + _callStack.RelativeOffset();
        } else {
            destIndex = 0;
            srcIndex  = 0;
        }

        size = 1;

        switch (op) {
        case Instructions::Opcode::i32neg: {
            auto& dest = _registers[destIndex];
            dest.Negate<int32_t>();
            break;
        }
        case Instructions::Opcode::i32add: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Add<int32_t>(src);
            break;
        }
        case Instructions::Opcode::i32sub: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Subtract<int32_t>(src);
            break;
        }
        case Instructions::Opcode::i32mul: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Multiply<int32_t>(src);
            break;
        }
        case Instructions::Opcode::i32div: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Divide<int32_t>(src);
            break;
        }
        case Instructions::Opcode::i32rem: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Remainder<int32_t>(src);
            break;
        }
        case Instructions::Opcode::i32and: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.AND<int32_t>(src);
            break;
        }
        case Instructions::Opcode::i32or: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.OR<int32_t>(src);
            break;
        }
        case Instructions::Opcode::i32xor: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.XOR<int32_t>(src);
            break;
        }
        case Instructions::Opcode::i32shl: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.ShiftLeft<int32_t>(src);
            break;
        }
        case Instructions::Opcode::i32shr: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.ShiftRight<int32_t>(src);
            break;
        }
        case Instructions::Opcode::i64neg: {
            auto& dest = _registers[destIndex];
            dest.Negate<int64_t>();
            break;
        }
        case Instructions::Opcode::i64add: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Add<int64_t>(src);
            break;
        }
        case Instructions::Opcode::i64sub: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Subtract<int64_t>(src);
            break;
        }
        case Instructions::Opcode::i64mul: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Multiply<int64_t>(src);
            break;
        }
        case Instructions::Opcode::i64div: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Divide<int64_t>(src);
            break;
        }
        case Instructions::Opcode::i64rem: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Remainder<int64_t>(src);
            break;
        }
        case Instructions::Opcode::i64and: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.AND<int64_t>(src);
            break;
        }
        case Instructions::Opcode::i64or: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.OR<int64_t>(src);
            break;
        }
        case Instructions::Opcode::i64xor: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.XOR<int64_t>(src);
            break;
        }
        case Instructions::Opcode::i64shl: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.ShiftLeft<int64_t>(src);
            break;
        }
        case Instructions::Opcode::i64shr: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.ShiftRight<int64_t>(src);
            break;
        }

        case Instructions::Opcode::u32add: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Add<uint32_t>(src);
            break;
        }
        case Instructions::Opcode::u32sub: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Subtract<uint32_t>(src);
            break;
        }
        case Instructions::Opcode::u32mul: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Multiply<uint32_t>(src);
            break;
        }
        case Instructions::Opcode::u32div: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Divide<uint32_t>(src);
            break;
        }
        case Instructions::Opcode::u32rem: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Remainder<uint32_t>(src);
            break;
        }
        case Instructions::Opcode::u32and: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.AND<uint32_t>(src);
            break;
        }
        case Instructions::Opcode::u32or: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.OR<uint32_t>(src);
            break;
        }
        case Instructions::Opcode::u32xor: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.XOR<uint32_t>(src);
            break;
        }
        case Instructions::Opcode::u32shl: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.ShiftLeft<uint32_t>(src);
            break;
        }
        case Instructions::Opcode::u32shr: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.ShiftRight<uint32_t>(src);
            break;
        }
        case Instructions::Opcode::u64add: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Add<uint64_t>(src);
            break;
        }
        case Instructions::Opcode::u64sub: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Subtract<uint64_t>(src);
            break;
        }
        case Instructions::Opcode::u64mul: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Multiply<uint64_t>(src);
            break;
        }
        case Instructions::Opcode::u64div: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Divide<uint64_t>(src);
            break;
        }
        case Instructions::Opcode::u64rem: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Remainder<uint64_t>(src);
            break;
        }
        case Instructions::Opcode::u64and: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.AND<uint64_t>(src);
            break;
        }
        case Instructions::Opcode::u64or: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.OR<uint64_t>(src);
            break;
        }
        case Instructions::Opcode::u64xor: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.XOR<uint64_t>(src);
            break;
        }
        case Instructions::Opcode::u64shl: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.ShiftLeft<uint64_t>(src);
            break;
        }
        case Instructions::Opcode::u64shr: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.ShiftRight<uint64_t>(src);
            break;
        }

        case Instructions::Opcode::f32neg: {
            auto& dest = _registers[destIndex];
            dest.Negate<float>();
            break;
        }
        case Instructions::Opcode::f32add: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Add<float>(src);
            break;
        }
        case Instructions::Opcode::f32sub: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Subtract<float>(src);
            break;
        }
        case Instructions::Opcode::f32mul: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Multiply<float>(src);
            break;
        }
        case Instructions::Opcode::f32div: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Divide<float>(src);
            break;
        }
        case Instructions::Opcode::f32rem: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Remainder<float>(src);
            break;
        }
        case Instructions::Opcode::f64neg: {
            auto& dest = _registers[destIndex];
            dest.Negate<double>();
            break;
        }
        case Instructions::Opcode::f64add: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Add<double>(src);
            break;
        }
        case Instructions::Opcode::f64sub: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Subtract<double>(src);
            break;
        }
        case Instructions::Opcode::f64mul: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Multiply<double>(src);
            break;
        }
        case Instructions::Opcode::f64div: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Divide<double>(src);
            break;
        }
        case Instructions::Opcode::f64rem: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            dest.Remainder<double>(src);
            break;
        }
        case Instructions::Opcode::bnot: {
            auto& dest = _registers[destIndex];
            dest.NOT();
            break;
        }
        case Instructions::Opcode::convi32toi8: {
            auto& dest = _registers[destIndex];
            dest.Convert<int32_t, int8_t>();
            break;
        }
        case Instructions::Opcode::convi32toi16: {
            auto& dest = _registers[destIndex];
            dest.Convert<int32_t, int16_t>();
            break;
        }
        case Instructions::Opcode::convu32tou8: {
            auto& dest = _registers[destIndex];
            dest.Convert<uint32_t, uint8_t>();
            break;
        }
        case Instructions::Opcode::convu32tou16: {
            auto& dest = _registers[destIndex];
            dest.Convert<uint32_t, uint16_t>();
            break;
        }
        case Instructions::Opcode::convi32toi64: {
            auto& dest = _registers[destIndex];
            dest.Convert<int32_t, int64_t>();
            break;
        }
        case Instructions::Opcode::convi32tou64: {
            auto& dest = _registers[destIndex];
            dest.Convert<int32_t, uint64_t>();
            break;
        }
        case Instructions::Opcode::convi32tou32: {
            auto& dest = _registers[destIndex];
            dest.Convert<int32_t, uint32_t>();
            break;
        }
        case Instructions::Opcode::convi32tof32: {
            auto& dest = _registers[destIndex];
            dest.Convert<int32_t, float>();
            break;
        }
        case Instructions::Opcode::convi32tof64: {
            auto& dest = _registers[destIndex];
            dest.Convert<int32_t, double>();
            break;
        }
        case Instructions::Opcode::convi64toi32: {
            auto& dest = _registers[destIndex];
            dest.Convert<int64_t, int32_t>();
            break;
        }
        case Instructions::Opcode::convi64tou32: {
            auto& dest = _registers[destIndex];
            dest.Convert<int64_t, uint32_t>();
            break;
        }
        case Instructions::Opcode::convi64tou64: {
            auto& dest = _registers[destIndex];
            dest.Convert<int64_t, uint64_t>();
            break;
        }
        case Instructions::Opcode::convi64tof32: {
            auto& dest = _registers[destIndex];
            dest.Convert<int64_t, float>();
            break;
        }
        case Instructions::Opcode::convi64tof64: {
            auto& dest = _registers[destIndex];
            dest.Convert<int64_t, double>();
            break;
        }
        case Instructions::Opcode::convu32toi64: {
            auto& dest = _registers[destIndex];
            dest.Convert<uint32_t, int64_t>();
            break;
        }
        case Instructions::Opcode::convu32tou64: {
            auto& dest = _registers[destIndex];
            dest.Convert<uint32_t, uint64_t>();
            break;
        }
        case Instructions::Opcode::convu32toi32: {
            auto& dest = _registers[destIndex];
            dest.Convert<uint32_t, int32_t>();
            break;
        }
        case Instructions::Opcode::convu32tof32: {
            auto& dest = _registers[destIndex];
            dest.Convert<uint32_t, float>();
            break;
        }
        case Instructions::Opcode::convu32tof64: {
            auto& dest = _registers[destIndex];
            dest.Convert<uint32_t, double>();
            break;
        }
        case Instructions::Opcode::convu64toi64: {
            auto& dest = _registers[destIndex];
            dest.Convert<uint64_t, int64_t>();
            break;
        }
        case Instructions::Opcode::convu64tou32: {
            auto& dest = _registers[destIndex];
            dest.Convert<uint64_t, uint32_t>();
            break;
        }
        case Instructions::Opcode::convu64toi32: {
            auto& dest = _registers[destIndex];
            dest.Convert<uint64_t, int32_t>();
            break;
        }
        case Instructions::Opcode::convu64tof32: {
            auto& dest = _registers[destIndex];
            dest.Convert<uint64_t, float>();
            break;
        }
        case Instructions::Opcode::convu64tof64: {
            auto& dest = _registers[destIndex];
            dest.Convert<uint64_t, double>();
            break;
        }
        case Instructions::Opcode::convf32toi32: {
            auto& dest = _registers[destIndex];
            dest.Convert<float, int32_t>();
            break;
        }
        case Instructions::Opcode::convf32toi64: {
            auto& dest = _registers[destIndex];
            dest.Convert<float, int64_t>();
            break;
        }
        case Instructions::Opcode::convf32tou32: {
            auto& dest = _registers[destIndex];
            dest.Convert<float, uint32_t>();
            break;
        }
        case Instructions::Opcode::convf32tof64: {
            auto& dest = _registers[destIndex];
            dest.Convert<float, double>();
            break;
        }
        case Instructions::Opcode::convf32tou64: {
            auto& dest = _registers[destIndex];
            dest.Convert<float, uint64_t>();
            break;
        }
        case Instructions::Opcode::convf64toi32: {
            auto& dest = _registers[destIndex];
            dest.Convert<double, int32_t>();
            break;
        }
        case Instructions::Opcode::convf64toi64: {
            auto& dest = _registers[destIndex];
            dest.Convert<double, int64_t>();
            break;
        }
        case Instructions::Opcode::convf64tou32: {
            auto& dest = _registers[destIndex];
            dest.Convert<double, uint32_t>();
            break;
        }
        case Instructions::Opcode::convf64tou64: {
            auto& dest = _registers[destIndex];
            dest.Convert<double, uint64_t>();
            break;
        }
        case Instructions::Opcode::convf64tof32: {
            auto& dest = _registers[destIndex];
            dest.Convert<double, float>();
            break;
        }
        case Instructions::Opcode::cmp: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            _flags = dest.Compare<unsigned>(src);
            break;
        }
        case Instructions::Opcode::icmp: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            _flags = dest.Compare<signed>(src);
            break;
        }
        case Instructions::Opcode::fcmp: {
            auto& dest = _registers[destIndex];
            const auto& src  = _registers[srcIndex];
            _flags = dest.Compare<float>(src);
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
            
            if (symbol.Arguments != 0)
                _registers.Copy(symbol.Registers, symbol.Arguments, _heap);

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
                _registers.SaveReturnValue(oldFrame.RegisterCount, _heap);

            _registers.Deallocate(oldFrame.RegisterCount, _heap);

            size = 0;
            pc = _unit.StartPC() + currentFrame.ReturnAddress;
            break;
        }
        case Instructions::Opcode::ldconst: {
            auto& destRegister = _registers[destIndex];
            if (destRegister.Typeof() == Primitives::Type::Reference)
                _heap.Notify(destRegister.As<Primitives::Reference>().HeapID, false);
            destRegister.Assign(_unit.ConstantLookup(srcIndex));
            break;
        }
        case Instructions::Opcode::mov: {
            auto& destRegister = _registers[destIndex];
            const auto& srcRegister  = _registers[srcIndex];

            if (destRegister.Typeof() == Primitives::Type::Reference)
                _heap.Notify(destRegister.As<Primitives::Reference>().HeapID, false);
            if (srcRegister.Typeof() == Primitives::Type::Reference)
                _heap.Notify(srcRegister.As<Primitives::Reference>().HeapID, true);

            destRegister.Assign(srcRegister);
            break;
        }
        case Instructions::Opcode::newarray: {
            auto& destRegister = _registers[destIndex];
            const auto& srcRegister  = _registers[srcIndex];
            if (destRegister.Typeof() !=  Primitives::Type::Uint32)
                ReportError("Invalid type for array size");
            else if (srcRegister.Typeof() != Primitives::Type::Uint32)
                ReportError("Invalid type for array type");

            destRegister.Assign(_heap.NewArray(destRegister.As<uint32_t>(), srcRegister.As<uint32_t>()));
            break;
        }
        case Instructions::Opcode::arraycount: {
            auto& destRegister = _registers[destIndex];
            const auto& srcRegister = _registers[srcIndex];
            if (srcRegister.Typeof() != Primitives::Type::Reference)
                ReportError("Invalid type for arraycount");
            else if (destRegister.Typeof() == Primitives::Type::Reference)
                _heap.Notify(srcRegister.As<Primitives::Reference>().HeapID, false);


            auto arrayPtr = _heap.GetArray(srcRegister.As<Primitives::Reference>().HeapID);
            destRegister.Assign(arrayPtr->Count());
            break;
        }
        case Instructions::Opcode::load: {
            auto& destRegister = _registers[destIndex];
            const auto& srcRegister = _registers[srcIndex];

            if (destRegister.Typeof() != Primitives::Type::Reference)
                ReportError("Invalid type for load (expected a reference)");
            else if (srcRegister.Typeof() != Primitives::Type::Uint32)
                ReportError("Invalid type for load (expected uint32)");
            else if (srcRegister.Typeof() == Primitives::Type::Reference)
                _heap.Notify(srcRegister.As<Primitives::Reference>().HeapID, false);

            auto arrayPtr = _heap.GetArray(destRegister.As<Primitives::Reference>().HeapID);
            destRegister.Assign(arrayPtr->Load(srcRegister.As<uint32_t>()));
            break;
        }
        case Instructions::Opcode::store: {
            auto& destRegister = _registers[destIndex];
            const auto& srcRegister = _registers[srcIndex];
            if (destRegister.Typeof() != Primitives::Type::Reference)
                ReportError("Invalid type for store (expected a reference)");
            else if (srcRegister.Typeof() != Primitives::Type::Uint32)
                ReportError("Invalid type for store (expected uint32)");
            auto arrayPtr = _heap.GetArray(destRegister.As<Primitives::Reference>().HeapID);
            arrayPtr->Store(destRegister.As<Primitives::Reference>().ArrayIndex, srcRegister);
            break;
        }
        case Instructions::Opcode::advance: {
            auto& destRegister = _registers[destIndex];
            const auto& srcRegister = _registers[srcIndex];

            if (destRegister.Typeof() != Primitives::Type::Reference)
                ReportError("Invalid type for advance (expected a reference)");
            else if (srcRegister.Typeof() != Primitives::Type::Uint32)
                ReportError("Invalid type for advance (expected int32)");

            auto arrayPtr = _heap.GetArray(destRegister.As<Primitives::Reference>().HeapID);

            arrayPtr->Advance(destRegister.As<Primitives::Reference>(), srcRegister.As<uint32_t>());
            break;
        }
        case Instructions::Opcode::printreg: {
            const auto& dest = _registers[destIndex];

            puts(dest.ToString(false).c_str());
        }
        case Instructions::Opcode::nop:
            break;
        case Instructions::Opcode::hlt:
            getchar();
            break;
        }

        pc += size;

    } while (!_callStack.IsEmpty());
}

auto VM::ReportError(std::string_view message) const -> void {
    std::puts(message.data());

    exit(EXIT_FAILURE);
}

}
