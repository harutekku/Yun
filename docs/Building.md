# Building

## Dependencies

- A C++ compiler with ability to generate dependency information
  in a makefile format

## Linux

On Linux, in the project's directory simply type:

```bash
make
```

If you want to use a different compiler (`g++` is the default), you will need
to change `g++` in this line:

```make
export CXX       := g++
```

To whatever compiler your heart desires (it still needs to be able to generate dep files though).

## Windows

On Windows, you will need to instal MinGW or clang in order to build YVM. Additionally, in the makefile,
you will need to change `g++` to the appropriate path to your compiler executable.
