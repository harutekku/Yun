# Command Line Interface

YVM has a couple of option you can use to alter its behavior. As of now, 
all of them are specifically for debuggin purposes, but you may occassionally
find them useful.

## Options

- `-d` - Print disassembly of the `yun` file, as seen by YVM. That means
  no fancy label names, no fancy attribute syntax etc. The output is 
  in a raw machine format
- `-t` - Print tokens generated from the `yun` file by lexer. Note that
  a lexer may skip some of the tokens to facilitate the job of the parser:
  for instance lexer automatically discards comments (lines with `#`) and
  trailing line feeds
- `h` - Print usage information
