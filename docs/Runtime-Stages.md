# Runtime Stages

## Parsing the command line arguments

When YVM first starts, it parses the command line arguments that were
specified by the program invoker. Since YVM is written in pure C++, it 
uses its own command line arguments parser, written similarly to C ones.

```cpp
[[nodiscard]] static auto ParseOptions(int argc, const char* argv[]) noexcept -> ProgramOptions;
```

First, `ParseOptions` is called from `main`. Here, the program first checks
if the number of arguments is appropriate (it can't be one, since
YVM doesn't support REPL and can't be bigger than 3, since it would
make no sense). If it's two, then the argument either specifies a file to run or
is an option to print help (`-h`). If it's three, then there are both options and
an input file.

The result of parsed commands is returned in `struct ProgramOptions` for
the further program to process.

```cpp
struct ProgramOptions {
    // ...
    const char* Filename;    // File to run
    bool        Disassemble; // Print disassembly?
    bool        PrintTokens; // Print tokens?
    bool        ShowHelp;    // Show help and terminate?
};
```

## Lexing

After successfully processing the command line arguments, YVM runs a 
lexer (lexical analyzer) to create tokens from the input file. 


```cpp
[[nodiscard]] static auto GetRawSource(const char* filename) -> std::string {
    // ...
    if (auto read = fread(buffer.data(), 1, size, file.get()); read != size)
        ReportErrorAndExit("Error: file too big");
    // ...
}
```

The file is loaded in its entirety to the memory by `GetRawSource`
function, which avoids slower disk IO. If the read fails, then we terminate.

After successfully reading the contents of the input file into the memory,
the program instantiates a lexer and starts to produce tokens.

A lexer (lexical analyzer) is a program that's responsible for reading small chunks of text
and turning them into sequence of lexical tokens (strings with some additional data, for instance
their abstract type, position in the file, if they represent literals or not etc.).
A stub of token class in YASN looks like this:

```cpp
class Token {
    // Constructors and printing functions
    public:
        TokenType                    Type;
        std::string                  Lexeme;
        union {
            uint64_t                 UnsignedLitaral;
            int64_t                  SignedLiteral;
            double                   FloatingPointLiteral;
            VM::Instructions::Opcode InstrLiteral;
        };
        uint32_t                     Line;
};
```

`Token` in YASN stores its type, it's textual representation in the source file,
it's literal value if it has one and a line number it was scanned on. Notice the
use of the union to save some extra bytes. 

The lexing starts when the `Lexer::Scan()` method is invoked.

```cpp
[[nodiscard]] auto Lexer::Scan() -> std::vector<Token>& {
    while (HasNext()) {
        _start = _current;
        Next();
    }
    TrimNewlines();
    // ...
}
```

Here, the lexer enters a loop where it creates tokens as long as it doesn't reach
the last character in the string. 

```cpp
auto Lexer::Next() -> void {
    auto c = NextCharacter();

    switch (c) {
    case '(':
        AddToken(TokenType::LeftParen);
        break;
    case ')':
        AddToken(TokenType::RightParen);
        break;
    // ...
    
    }
}
```

The `Next()` function contains a `switch` statement that handles simple
tokens, like '=' in separate cases and more complex ones in the `default` one.

```cpp
    // ...
    default:
        if (std::isdigit(c) || c == '-')
            Number();
        else if (std::isalpha(c))
            Identifier();
        else
            ReportError("Unexpected character");
        break;
    // ...
```

If we hit the end of the `switch` statement, we either have a number literal,
an identifier or some unexpected characters, like ':' or '?'.
If the current character `c` is a digit, then there's a chance it belongs
to a bigger string making a number literal. The `Number()` function iterates
through the string characters and tries to figure out if it's a valid literal or not.
On the other hand, if the current character is a letter, the `Identifier()` function
tries to determine whether or not that's a register descriptor (`R\d*`), a label declaration
(`\D[a-zA-Z0-9]*:`), one of the function attributes (`registers` etc.), an instruction
or a function identifier. `Identifier()` uses a `static const std::map` for determining these.

```cpp
const static std::map<std::string, MappedValues> Keywords{
    { "true",           TokenType::True },
    { "false",          TokenType::False },
    { "function",       TokenType::Function },
    // ...
};

// ...

auto Lexer::Identifier() -> void {
    // ...

    std::string key{ _src.begin() + _start, _src.begin() + _current };
    if (mightBeRegister) {
        // ...
    } else if (const auto it = Keywords.find(key); it != Keywords.end()) {
        // ...
    }
    // ...
}
```

All tokens are added to the internal `std::vector<Token>` by a call to `AddToken()` method,
that takes optional argument representing the token's literal value, if it has one.

```cpp
template<typename T = uint64_t>
auto Lexer::AddToken(TokenType type, T literal = T{  }) -> void {
    std::string_view lexeme{ _src.c_str() + _start, _current - _start };
    _tokenBuffer.push_back({ type, lexeme, literal, _line });
}
```

After there are no more characters left in file, the `Scan()` loop terminates. Before
returning a reference to the internal `_tokenBuffer`, lexer trims the trailing line feeds
and inserts one last token - the `EOF` indicator. This way, the parser will have a slightly 
easier job.

## Parsing

After the check if lexer succeeded, the program instantiates the `Parser` object.
YASN uses a recursive-descent parser written by hand. It's important to note
that the parser doesn't construct the AST as it parses, but generates assembly 
_as it parses_. This **greately** simplifies its design, but unfortunately makes
code more difficult to understand. For the purpose of this explanation, I will describe
these two processes separately, but bear in mind that in reality, they happen at the same time.

The parser is based on the YASN grammar that you can find in `YASN.ebnf`. First, a `Parse()` method
checks if `EOF` indicator was hit. If not, then it calls `Function()` in a loop (a YASN file is a collection
of functions after all). On invocation, `Function()` calls `FunctionDeclaration()` in order to get necessary data
about itself - its name, number of required registers, taken parameters etc. With this information, it can later set up
a `FunctionBuilder` object in the `Assembler` object that's internal to the parser.

```cpp
auto Parser::Function() -> void {
    _state = State();
    FunctionDeclaration();
    _assembler.BeginFunction(_state.Name, _state.RegisterCount, _state.ArgCount, _state.KeepReturnValue);
    Block();
    _assembler.EndFunction();
}
```

`FunctionDeclaration()` first calls `Attributes()`, which resolve some basic function characteristics, namely
argument count (specified by attribute `parameters`), register count (`registers`) and whether or not a function returns
a value (`returns`). Then, it tries to parse the rest of the function information - its name and some "declaration prettifiers", 
like `()` to make it look a bit nicer.

With these information, the rest of the `Function()` code sets up a `FunctionBuilder` object that's responsible 
for proper function code generation by a call to `Assembler::BeginFunction()`. 
The parser then proceeds to parse the instructions block.

An instruction block is a collection of lines that take the form:

```ebnf
[ identifier ':' ] [ instruction [ operand [ ',' operand ] ] ] '\n'
```

That is:
- A line can have a label declaration at the beginning. The label can have the same name
  as the function, but must be _localy-unqiue_, i.e label redefinition is an error. It can
  be followed by a newline character terminating the string
- After a label, there must be an instruction with zero (a _void_ instruction), one (_unary_) or two
  (_binary_) operands separated by ','
- At the end of each line, there must be a newline token

As `Line()` parsers source lines, it also does call to the internal `Assembler` object, which we will discuss
in the next section.

After the last line, the execution returns to the `Function()` method. If the freshly parsed function is the last one
in the source code, then the initiative is passed down to the assembler.

## Assembler

As said in the previous section, the assembler does its work as parser parses the token stream.
Before its internals are described, it's important to understand its architecture.

```
 Resolves          Resolves                Generates
 `call`s           jumps                   opcodes
+-----------+     +-----------------+     +---------+
| Assembler | --> | FunctionBuilder | --> | Emitter |
+-----------+     +-----------------+     +---------+
```

An assembler have a member object of a type `FunctionBuilder`. Every function is built by a proxy
of this object. The `FunctionBuilder` class is responsible for patching the jumps in each function,
i.e converting the relative location of a label within a function to a signed offset that is added to
a program counter during execution, AND making sure that a function performs valid operations, for instance
its last instruction is a `ret`. This way assembler avoids mixing up symbols - labels
are fundamentally different from function names and are stored differently. However, `FunctionBuilder` 
is too high level to handle actual code generation. For this task, it uses an `Emitter` class to 
generate and then serialize the instruction stream.

The calls get resolved at assembler level. After `FunctionBuilder` successfully generates a `FunctionUnit`,
that holds function instruction buffer, a _call map_ and the information about its _symbol_, the assembler
in its final step first collects all the information about available `FunctionUnits` into one structure,
called a _symbol table_, and then, using that symbol table, patches the calls in each function. 
It also does some last checks to ensure that every call is valid - for instance it makes sure that a function 
doesn't try to call another one that requires more parameters than it has registers.

After that, the `ExecutionUnit`, is generated the VM can finally run.

## Runtime

The execution of the actual VM is fairly straightforward. First, VM tries to locate the entry point - 
the `main` function that takes no arguments and doesn't return a value. If that fails, the program terminates
with an error. Else, the frame of a newly-found function is pushed onto a _callstack_, its registers are allocated
by a call to `RegisterArray::Allocate()` and the program starts to run.

```cpp
rawOp = (*pc & 0xFF000000) >> 24;
if (rawOp > static_cast<uint8_t>(Instructions::Opcode::hlt))
    ReportError("Invalid instruction");

op = static_cast<Instructions::Opcode>(rawOp);

if (auto res = Instructions::OpcodeCount(op); res == 1) {
    if (Instructions::IsJump(op) || op == Instructions::Opcode::call) {
        if (auto int24 = *pc & 0x00FFFFFF; (int24 & 0x800000) && op != Instructions::Opcode::call)
            destIndex = int24 | 0xFF000000; // Sign extension of 24-bit value if a jump
        else
            destIndex = int24;              // Nothing to be done if a call
        destIndex >>= 2;
    } else
        destIndex = (*pc & 0x00FFF000) >> 12 + _callstack.RelativeOffset();
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
```

First, the 32-bit wide instruction is fetched from memory and its operands are decoded.
The number of operands is determined by a call to `Instructions::OpcodeCount()` function.
If the result is zero, that means that the instruction is a void one. Else, if the result
is one, then the interpreter needs to decode an one operand that can either be a register id,
a signed 24-bit offset or unsigned 24-bit function call displacement. Only in case of a jump
the interpreter needs to sign-extend the operand to ensure offset correctness. In case of the
operand being a register id, it must be incremented by the value of a _relative offset_ that is
an offset to registers in the current function frame from the beginning of the callstack.

The same goes for binary instructions, with one exception. `ldconst` instruction requires its
second operand to be an unsigned index into the constant pool.

Then, after decoding operands, the interpreter executes the instruction in a giant `switch` 
statement. The program eventually terminates when either the `main` returns or some error occurs.

## Instructions

Most of the instruction formats can be figured out easily from the VM instruction loop,
that is located at lines 157-830 in the `VM.cpp` or in the `Ideas.md` file.