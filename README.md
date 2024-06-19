BitVector
=========

Compile
-------

This is a simple CMake Project. Use CMake for compilation.
For example:

```
user@pc $ mkdir cmake-build-release
user@pc $ cd cmake-build-release
user@pc $ cmake -S .. -B . -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Release
-- Configuring done (0.0s)
-- Generating done (0.0s)
-- Build files have been written to: /home/user/workspace/bitvector/cmake-build-release
user@pc $ cmake --build .
[100%] Built target bitvector
user@pc $ ./bitvector ../bench/reference.txt /tmp/reference-const-result.txt
RESULT name=robert_fritsch time=0 space=8512
```

Usage
-----

```
bitvector <file_in> <file_out> [<variant>]

    <file_in>  : the input file
    <file_out> : the output file
    <variant>  : optional - one of [linear, const, pasta]
                 (pasta only if compiled with the INCLUDE_PASTA_BIT_VECTOR flag!)
```

Structure
---------

| Directory | Description                                                                      |
|-----------|----------------------------------------------------------------------------------|
| bench     | Contains sample input files for the cli                                          |
| cmake     | CMake code that was moved out of the CMakeLists.txt for readability.             |
| include   | The bitvector header only library implemented by this project.                   |
| scripts   | Some Python scripts for testing and plotting.                                    |
| src       | A cli for using the bitvector library on the command line. Entry point: main.cpp |
| test      | Some CTest test applications. See Tests.                                         |

Tests
-----

### Fuzz Tester

Simple python tool to generate random yet valid input instances.
These are than run for pasta (as reference) and the custom implementation.
The results of both runs are compared and if there are differences, these differences get printed.

### CTests

The C-Tests at the moment check only the consistency of some datastructure implementations.
There is no logic check for the right rank and select queries.
I extend these every time I face a bug in one of the datastructures.
