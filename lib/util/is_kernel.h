/** @file is_kernel.h @brief find if function is entry point or not */

#ifndef util_is_kernel_h
#define util_is_kernel_h

#include <clang-c/Index.h>
#include <ae2f/Keys.h>
#include <attr/execmodel.h>
#include <ae2f/c90/StdInt.h>


ae2f_inline static u32_least util_is_kernel(const CXCursor h_fndecl) {
	enum SpvExecutionModel_ MODEL = SpvExecutionModelMax;
	clang_visitChildren(h_fndecl, attr_execmodel, &MODEL);
	return (u32_least)(MODEL ^ SpvExecutionModelMax);
}

#endif
