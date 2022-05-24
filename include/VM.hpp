/**
 * @file
 *   VM.hpp
 * @author
 *   Harutekku @link https://github.com/harutekku @endlink
 * @brief
 *   The core interface of the Yun virtual machine
 * @version
 *   0.0.1
 * @date
 *   2022-05-24
 * @copyright
 *   Copyright (c) 2022
 * 
 */
#ifndef VM_HPP
#define VM_HPP

// C++ header files
#include <vector>
// My header files
#include "Value.hpp"
#include "Instructions.hpp"

namespace Yun::VM {

class RegisterArray {
    public:
        RegisterArray(size_t size = 1024);

    public:
        [[nodiscard]] auto At(std::size_t) -> Primitives::Value&;
        auto AllocateFrame(std::size_t) -> void;
        auto DeallocateFrame(std::size_t) -> void;

    private:
        std::size_t                    _index;
        std::vector<Primitives::Value> _registers;
};

class ConstantPool {

};

class VM final {
    public:
        VM(Instructions::Buffer);
    
    public:
        auto Run() -> void;
        
    private:
        /**
         * @brief 
         *   For now, holds one and only one execution unit
         * @todo
         *   Add a possibility to split source code into multiple files
         */
        Instructions::Buffer _instructions;
        /**
         * @brief 
         *   Contains all the allocated registers
         */
        RegisterArray        _registers;
        /**
         * @brief 
         *   For now, holds one and only one constant pool
         *   for the current execution unit
         */
        ConstantPool         _constantPool;
};


}

#endif
