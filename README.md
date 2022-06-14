# Yun Virtual Machine

**YVM** is a register-based, bytecode-based, type-checking virtual machine made 
in pure C++ for ***F U N*** and to learn how VMs work.

## Motivation

  Writing a virtual machine from scratch, without any 3rd party libraries, in pure C++
can considered meaningless given how many successful virtual machines are out there
written in way better languages :^). However, it is one of the greatest learning 
opportunities that a programmer can have - implementing a VM with its own instruction
set and assembler requires at least some understanding of science behind computer 
architecture, memory management, compilers, parsers etc. It's a great exercise to broaden
one's horizons.

## Features

- Runtime type checking
- Functions
- Arrays (unfortunately you can't have arrays of arrays)
- Garbage collection with reference counting

## Gist of YASN

**YASN** is an assembler language for YVM. As of now, it only supports functions, but more
features _may_ come in the future.

A simple program written in YASN looks like this

```
[registers=0]
function main() {
    ret
}
```

As in other languages, `main` function with no arguments and not returning any value is
an entry point.
Every function defintion starts with an attribute list. Attribute `registers` indicates
how many registers a function needs for its internal use and for storing its arguments.
For instance:

```
[registers=2, parameters=2, returns=true]
function sum() {
    u64add R0, R1
    ret
}
```

Declares a function that needs two registers for its internal use: two of them will be used
for storing actual parameters and a caller is expected to keep its return value. Declaring
a function returning a value or requiring arguments and not having any registers is an error
that will be caught during compilation. Notice that you always need to specify at least
one attribute.

A simple program that adds two numbers and prints their sum may look like this:

```
[registers=2]
function main() {
    ldconst  R0, 60
    ldconst  R1, 9
    call     sum
    printreg R1
    ret
}

[registers=2, parameters=2, returns=true]
function sum() {
    u64add R0, R1
    ret
}
```

All literals, by default, are 64-bit wide. YASN currently supports three types of literals:
unsigned, signed and floating-point literals. You have to convert them to
appropriate type using proper conversion instructions if you need to.

## Documentation 

Documentation is available in the `docs` directory. It's written in Markdown and 
explains some parts of VM in detail. Treat it like an explanation of how some of
its parts work.

## Future

The development of YVM might continue in the future - after all, this is a passion project
of mine and I enjoy working on it. One of the features I would love to add is a bytecode 
verifier - I hate type-checking at runtime at the cost of performance. However, adding
a verifier is a non-trivial task requiring some understanding of graph theory.


# Credits

- `Crafting Interpreters` by Robert Nystrom - A nice book about interpreters. YASN lexer
  takes some inspiration from the one that he crafted for Lox
- LuaVM 5.4 implementation - Reference

# Special thanks

- To my collegues for their patience and understanding
- To Yuno - the VM is named after her after all

# License

GNU GPL-3.0

# Author

[Harutekku](https://github.com/harutekku/Yun)
