/**
 * @file	abi.h
 * @brief	function & variable ABI
 * */
#ifndef aclspv_abi_h
#define aclspv_abi_h

#include <ae2f/cc.h>
#include <ae2f/.Call.h>

/**
 * @def ACLSPV_ABI_IMPL	
 * @brief Implementation as ABI
 *
 * @def ACLSPV_ABI_DECL	
 * @brief Declaration as ABI
 * */

#if ae2f_IS_SHARED
#define ACLSPV_ABI_DECL ae2f_decl
#define ACLSPV_ABI_IMPL	ae2f_impl
#else
#define ACLSPV_ABI_DECL
#define ACLSPV_ABI_IMPL
#endif

#endif
