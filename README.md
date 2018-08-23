# MuTeC: Mutation Testing for C

## Introduction

MuTeC is developed on top of LLVM/Clang compiler infrastructure to generate mutations for programs written in C and C++.

## Requirement

Make sure you have the following:

1. Latest CMake and your favourite build system (recommand to use Ninja).
2. Latest LLVM. Instuctions can be found [here](http://clang.llvm.org/docs/LibASTMatchersTutorial.html).

## Build

1. Go to /path/to/your/llvm-source-tree/tools/clang/tools/extra/
2. Clone this project.
3. Add the following line to CMakeLists.txt:
```
add_subdirectory(mutec)
```
4. Go to /path/to/your/llvm/build
5. Run ninja mutec
6. Run ninja mutec-template

## Usage

Two executables are provided. 

1. mutec: generate mutants for C/C++ source files.

    Usage: mutec source_filenames [options] -o output_directory

    Options:

    -r: randomly generate one mutants. Default settings generate all possible mutants.
        
    -t: only generate MuTeC code template for the source files.

2. mutec-template: generate mutants from the intermediate form (MuTeC code template) produced by mutec command.

    Usage: mutec-template template_filenames [options]

    Options:
    
    -o output_directory: provide the path to store generated mutants.
    
    -d: show details of the code templates.
    
    -r: randomly generate one mutants. Default settings generate all possible mutants.
Note: either -o or -d or both should be provided. 

