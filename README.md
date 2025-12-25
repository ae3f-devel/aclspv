# aclspv
> it is incomplete project. please keep your expectation of this application low.

- OpenCL C -> SPIR-V compiler application & library for Vulkan.
- Uses llvm-c, clang-c to avoid memory corruption caused by static fiasco 
- `lib` for library source to be compiled
- `inc` for library header

## language
- C90 for llvm-c
- C++ for clang/llvm

# What is this library to be expected
- it compiles opencl c into `oclc flavoured llvm ir`.
- it converts `oclc flavoured llvm ir` to `custom llvm ir`.
- it emits the `final spir-v` based on `custom llvm ir` in for vulkan 1.0.

# What is not done
- proper library linking configuration on cmake
- normal function declaration & definition
- entry kernel definition
- storage buffer structure is hard-coded
- uniform buffer structure is hard-coded
- workgroup buffer structure is hard-coded.
