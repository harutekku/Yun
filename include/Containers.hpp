#ifndef CONTAINERS_HPP
#define CONTAINERS_HPP

#include <memory>
#include <queue>
#include "Value.hpp"
#include "Instructions.hpp"
#include "Exceptions.hpp"

namespace Yun::ASM {
class Assembler;
}

namespace Yun::VM::Containers {
class ArrayHeap;

class RegisterArray {
    public:
        RegisterArray(size_t count = 1024) noexcept;

    public:
        auto Allocate(std::size_t) -> void;
        auto Deallocate(std::size_t, ArrayHeap&) noexcept -> void;
        auto Copy(std::size_t, std::size_t, ArrayHeap&) noexcept -> void;
        auto SaveReturnValue(std::size_t, ArrayHeap&) noexcept -> void;

        [[nodiscard]] 
        #if __has_cpp_attribute(__cpp_lib_constexpr_vector)
        constexpr
        #else
        inline
        #endif
        auto operator[](size_t index) noexcept -> Primitives::Value& {
            return _registers[index];
        }

    public:
        auto Print() const -> void;

    private:
        std::size_t                    _index;
        std::vector<Primitives::Value> _registers;
};

class ConstantPool {
    public:
        ConstantPool() = default;
    
    public:
        [[nodiscard]] auto Read(size_t) const -> Primitives::Value;
        [[nodiscard]] auto Has(size_t) const noexcept -> bool;

    public:
        auto Print() const noexcept -> void;

    private:
        friend ASM::Assembler;
        [[nodiscard]] auto Add(Primitives::Value) -> size_t;

        template<typename T>
        [[nodiscard]] auto FindOrAdd(Primitives::Value value) -> size_t {
            for (size_t i = 0; i != _constants.size(); ++i)
                if (_constants[i].Typeof() == value.Typeof() && _constants[i].As<T>() == value.As<T>())
                    return i;
            return Add(value);
        }

    private:
        std::vector<Primitives::Value> _constants;
};

class InstructionBuffer {
    public:
        InstructionBuffer(size_t data) noexcept;

    public:
        [[nodiscard]] auto begin() noexcept -> uint32_t*;
        [[nodiscard]] auto end() noexcept -> uint32_t*;
        [[nodiscard]] auto begin() const noexcept -> const uint32_t*;
        [[nodiscard]] auto end() const noexcept -> const uint32_t*;
    
    private:
        std::unique_ptr<std::uint32_t[]> _data;
        std::uint32_t*                   _last;
};

[[nodiscard]] auto begin(InstructionBuffer& buffer) noexcept -> uint32_t*;
[[nodiscard]] auto end(InstructionBuffer& buffer) noexcept -> uint32_t*;

class Symbol {
    public:
        [[nodiscard]] auto ToString() const -> std::string;
        auto PrettyFunctionSignature() const -> std::string;

    public:
        std::string Name; 
        uint16_t    Registers;
        uint16_t    Arguments;
        uint32_t    Start;
        uint32_t    End;
        bool        DoesReturn;
};

class SymbolTable {
    public:
        SymbolTable() noexcept = default;

    public:
        [[nodiscard]] auto FindByName(const std::string_view) const -> const Symbol&;
        [[nodiscard]] auto FindByLocation(uint32_t) const -> const Symbol&;
        [[nodiscard]] auto At(size_t) const -> const Symbol&;
        [[nodiscard]] auto Count() const noexcept -> size_t;
    
    public:
        auto Print() const -> void;

    private:
        friend class ASM::Assembler;
        auto Add(Symbol) -> void;

    private:
        std::vector<Symbol> _symbols;
};

class Frame {
    public:
        constexpr Frame() noexcept
            :ReturnAddress{ 0 }, RegisterCount{ 0 }, KeepReturnValue{ 0 }, End{ 0 } {
        }

        constexpr Frame(uint32_t returnAddress, uint16_t registerCount, bool keepReturnValue, uint32_t end) noexcept
            :ReturnAddress{ returnAddress }, RegisterCount{ registerCount }, KeepReturnValue{ keepReturnValue }, End{ end } {
        }

    public:
        uint32_t ReturnAddress;
        uint16_t RegisterCount;
        bool     KeepReturnValue;
        uint32_t End;
};

class CallStack {
    public:
        CallStack(size_t count = 1024) noexcept;

    public:
        auto Push(Frame) -> void;
        [[nodiscard]] auto Pop() -> Frame;
        [[nodiscard]] constexpr auto Count() const noexcept -> size_t {
            return _count;
        }

    public:
        [[nodiscard]] constexpr auto RelativeOffset() const noexcept -> size_t {
            return _relativeOffset;
        }

    public:
        [[nodiscard]] constexpr auto IsEmpty() const noexcept -> bool {
            return _count == 0;
        }
    
    private:
        size_t             _count;
        size_t             _relativeOffset;
        std::vector<Frame> _frames;
};

class Array {
    using Value = Primitives::Value;
    public:
        Array(Primitives::Type, size_t) noexcept;

    public:
        [[nodiscard]] constexpr auto Count() const noexcept -> size_t {
            return _count;
        }
        [[nodiscard]] auto Load(size_t) -> Primitives::Value;
        auto Store(size_t, Primitives::Value) -> void;
        auto Advance(Primitives::Reference&, uint32_t) -> void;

    private:
        Primitives::Type            _elementType;
        size_t                      _count;
        std::unique_ptr<uint64_t[]> _elements;
};

struct HeapRecord {
    uint32_t               Id;
    uint32_t               RefCount;
    std::unique_ptr<Array> Pointer;
};

class ArrayHeap {
    public:
        ArrayHeap(size_t initialSize = 1024);

    public:
        [[nodiscard]] auto NewArray(uint32_t, uint32_t) -> Primitives::Reference;
        auto Notify(uint32_t, bool) noexcept -> void;
        [[nodiscard]] auto GetArray(uint32_t) noexcept -> Array*;

    private:
        size_t                  _index;
        std::vector<HeapRecord> _heapArrays;
        std::queue<uint32_t>    _idsForReuse;
};

}

#endif