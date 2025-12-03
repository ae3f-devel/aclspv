/** @file compiler.h  @brief compiles */

#ifndef aclspv_compiler_h
#define aclspv_compiler_h

#include <ae2f/Keys.h>
#include <stddef.h>
#include "./abi.h"
#include <clang-c/CXFile.h>
#include <clang-c/Index.h>
#include "./obj.h"

/**
 * @class	x_aclspv_compiler
 * @brief	this will be defined only when it is compiled on C++
 * */

/**
 * @class	h_aclspv_compiler_t
 * @brief	handle for a compiler
 * */
typedef struct x_aclspv_compiler* ae2f_restrict h_aclspv_compiler_t;

/**
 * @enum	e_aclspv_init_compiler
 * @brief	result for `aclspv_init_compiler`
 * */
typedef enum {
	/** @brief good */
	ACLSPV_INIT_COMPILER_OK,

	/** argument `i_compiler` was null */
	ACLSPV_INIT_COMPILER_ARG_WAS_NULL
} e_aclspv_init_compiler;

/**
 * @method	aclspv_compiler_init
 * @memberof	h_aclspv_compiler_t
 * @brief	Initialise the instance of the compiler
 *
 * @returns	<NEW:aclspv_stop_compiler>	\n
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_retnew
h_aclspv_compiler_t	aclspv_mk_compiler(void);

/**
 * @method	aclspv_compiler_stop
 * @memberof	h_aclspv_compiler_t
 * @brief	Deinitialise the instance of the compiler
 *
 * @param	d_compiler	<FREE>				\n
 * a valid compiler object to turn off
 * */
ae2f_extern ACLSPV_ABI_DECL
void	aclspv_stop_compiler(h_aclspv_compiler_t d_compiler);

/**
 * @method	aclspv_compile
 * @memberof	h_aclspv_compiler_t
 * @brief	compile files and generate an object
 * @returns	<INIT:aclspv_obj_stop>
 * */
ae2f_extern ACLSPV_ABI_DECL
h_aclspv_obj_t	aclspv_compile(
		h_aclspv_compiler_t	h_compiler,
		const char* const					rd_srcpath,
		const  struct CXUnsavedFile* const ae2f_restrict	rd_unsaved,
		const size_t						c_unsaved_len,
		const char* ae2f_restrict const * ae2f_restrict const	rd_argv_opt,
		const size_t						c_argc
		);

#endif
