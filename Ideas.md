# Ideas

## Design

- Register-based virtual machine
  - We need to be able to allocate registers

## What I would like to have

- Support for arrays
  - `Tag-Size-Values`
- Activation record
- Call-Ret mechanism

## Instructions

- Let's take Java approach, i.e the operand types must match the
  opcode type 
- Format
```

  opcode    dest16    src16
  opcode    offset32

```

### Opcodes

- Arithmetic [56 instructions]
  - Signed [24 instructions]
    - i8neg,  i8add,  i8sub,  i8mul,  i8div,  i8rem
    - i16neg, i16add, i16sub, i16mul, i16div, i16rem
    - i32neg, i32add, i32sub, i32mul, i32div, i32rem
    - i64neg, i64add, i64sub, i64mul, i64div, i64rem
  - Unsigned [20 instructions]
    - u8add,  u8sub,  u8mul,  u8div,  u8rem
    - u16add, u16sub, u16mul, u16div, u16rem
    - u32add, u32sub, u32mul, u32div, u32rem
    - u64add, u64sub, u64mul, u64div, u64rem
  - Floating-point [12 instructions]
    - f32neg, f32add, f32sub, f32mul, f32div, f32rem
    - f64neg, f64add, f64sub, f64mul, f64div, f64rem
- Bitwise [4 instructions]
  - band, bor, bxor, bnot
- Logic [6 instructions]
  - cmp, icmp, fcmp
  - land, lor, lnot
- Jumps [7 instructions]
  - jmp
  - je, jne
  - jgt, jge, jlt, jle
- Calls [2 instructions]
  - call
  - ret
- Misc [2 instructions]
  - nop
  - hlt

## Instruction size

- Right now, it varies from 1 to 5 bytes
- Wouldn't it be cool if we could have all instruction 4-bytes wide?
  - 8 bits for opcode
  - 12 bits for first operand
  - 12 bits for second operand
  - 24 bits for jump offset
- Turns out, this was a great idea
  - From 48s on `Fib(40)` we went down to 33s
  - We're officialy faster than Python

## TODOs

[x] Implement VM

[x] Implement functions

[ ] Optimize for speed

[ ] Add string validation - can't be empty

[ ] Implement arrays
