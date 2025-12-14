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

	/** 
	 * @var		m_internal_name
	 * @brief	internal declaration name for ocl.
	 * @details	must have a prefix of `ACLSPV_PREFIX_BLTIN`
	 * */
	const char* ae2f_restrict m_internal_name;
} aclspv_ocl_bltin_map;

/**
 * @def		ACLSPV_PREFIX_BLTIN
 * @brief	prefix for `aclspv_ocl_bltin_map::m_internal_name`
 * */
#define	ACLSPV_PREFIX_BLTIN	"aclspv.bltin."

/**
 * @var		g_aclspv_ocl_bltin_map
 * @brief	the actual map for opencl builtins
 * @see 	lib/pass/emit_bltin_decl.c 
 * */
ACLSPV_ABI_DECL ae2f_extern const aclspv_ocl_bltin_map g_aclspv_ocl_bltin_map[];

/** 
 * @def		ACLSPV_OCL_BLTIN_MAP_COUNT
 * @brief	count of `g_aclspv_ocl_bltin_map`
 * */
#define	ACLSPV_OCL_BLTIN_MAP_COUNT	67

#endif
