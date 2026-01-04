/**
 * @file aclspv.h
 * Compiles OpenCL C kernel code to SPIR-V for Vulkan.
 * */

#ifndef aclspv_h
#define aclspv_h

#include <ae2f/Keys.h>
#include <ae2f/cc.h>

#include "./aclspv/abi.h"
#include "./aclspv/spvty.h"

#include <clang-c/Index.h>

typedef enum {
	ACLSPV_COMPILE_OK,
	ACLSPV_COMPILE_ALLOC_FAILED,
	ACLSPV_COMPILE_MET_INVAL,
	ACLSPV_COMPILE_ERR_CLANG,
	ACLSPV_COMPILE_TOO_BIG
} e_aclspv_compile_t;

/**
 * @fn	aclspv_compile
 * @brief	compile files and generate spir-v for vulkan
 * @returns	<INIT:aclspv_free_obj>
 * */
ae2f_extern ACLSPV_ABI_DECL e_aclspv_compile_t 
aclspv_compile(
		struct CXUnsavedFile* ae2f_restrict const 	rdwr_unsaved,
		const unsigned					c_unsaved_count,
		const char* ae2f_restrict const * ae2f_restrict const	rd_argv_opt,
		const int						c_argc,
		aclspv_wrdcount_t* ae2f_restrict			rwr_output_count_opt,
		aclspv_wrd_t* ae2f_restrict* ae2f_restrict		rwr_output,
		enum CXErrorCode* ae2f_restrict				rwr_cxerr_opt
	      );

#endif
