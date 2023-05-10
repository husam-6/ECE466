# ECE466 Compilers - Husam Almanakly & Michael Bentivegna

## Overview 

This repo implements a C99 compiler. All the source code can be <br>
found in the src folder. It consists of the following files

<pre>
├── LICENSE
├── Makefile
├── README.md
├── build
├── ham
├── include
│   ├── assemble.h
│   ├── lexer.h
│   ├── parser.h
│   ├── quads.h
│   ├── sym.h
│   └── type.h
├── src
│   ├── die-util.c
│   ├── lexer
│   │   ├── lex_help.c
│   │   ├── lex_tester.c
│   │   └── lexer.l
│   ├── main.c
│   ├── parser
│   │   ├── ast.c
│   │   ├── parser.y
│   │   ├── sym.c
│   │   └── type.c
│   └── quads
│       ├── assemble.c
│       └── quads.c
└── t.out

</pre>

All the source code for each relevant section of the compiler can be found <br>
in the src folder, with header files in the include directory. <br>

## Support

Our compiler supports basic integer and pointer operations, including array <br>
and multi-dimensional arrays. <br>



## Build and run the executable

You must be on a machine that supports 32 bit x86 in order to run the compiler. <br>
Additionally, you can manually build the executable, by running 

<code>make</code>

while in the ECE466 directory. The executable will be in the generated build <br>
directory. 


## Example Run

There is additionally a shell script, <i>ham</i>, that will build the executable, 
and run it on one given test file. Below is an example run of our compiler.

<pre>

[husam.almanakly@hakvm target_tests]$ ham fib.c
make: `build/a.out' is up to date.
[husam.almanakly@hakvm target_tests]$ ./a.out
fib(1) = 1
fib(2) = 1
fib(3) = 2
fib(4) = 3
fib(5) = 5
fib(6) = 8
fib(7) = 13
fib(8) = 21
fib(9) = 34

</pre>


The above was run on a simple C program implementing the fibonacci sequence. <br>
The program will also output the generated x86 32bit assembly file in the same <br>
directory as the inputted file. 