#ifndef pass_bltin_lowered_h
#define pass_bltin_lowered_h

#include <ae2f/Keys.h>
#include <aclspv/abi.h>

/** 
 * @struct	builtin_map
 * @brief	A struct to map OpenCL mangled names to internal builtin names 
 * */
typedef struct {
	const char* ae2f_restrict m_ocl_name;
	const char* ae2f_restrict m_internal_name;
} aclspv_ocl_bltin_map;

/**
 * @var		g_aclspv_ocl_bltin_map
 * @brief	the actual map for opencl builtins
 * @see 	lib/pass/emit_bltin_decl.c 
 * */
ACLSPV_ABI_DECL ae2f_extern const aclspv_ocl_bltin_map g_aclspv_ocl_bltin_map[];

#endif
