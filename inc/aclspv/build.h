#ifndef	aclspv_build_h
#define	aclspv_build_h

#include "./pass.h"
#include "./lnker.h"
#include "./spvty.h"

typedef struct a_aclspv_build_ctx			a_aclspv_build_ctx;
typedef struct a_aclspv_build_ctx* ae2f_restrict	h_aclspv_build_ctx_t;
typedef	e_fn_aclspv_pass	fn_aclspv_build_t(const LLVMModuleRef, const h_aclspv_build_ctx_t);

/** 
 * @fn		aclspv_build_conf	
 * @brief	configurates the initial state 
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_build_t aclspv_build_conf;

/**
 * @fn		aclspv_build_fniter	
 * @brief	iterates the function and extracts the entry points 
 * @related	aclspv_build_conf
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_build_t aclspv_build_fniter;

/** 
 * @fn		aclspv_build_decl_entp	
 * @brief	emits the declaration of entry points 
 * @related	aclspv_build_fniter
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_build_t aclspv_build_decl_entp;

/**
 * @fn		aclspv_build_decl_fn
 * @brief	emits the declaration of normal functions
 * @related	aclspv_build_fniter
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_build_t aclspv_build_decl_fn;

/** 
 * @fn		aclspv_build_fetch_bind
 * @brief	extracts the binds from each entry points 
 * @related	aclspv_build_fniter
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_build_t aclspv_build_fetch_bind;

/**
 * @fn		aclspv_build_decl_bind
 * @brief	emits the declaration of bindings for entry points
 * @related	aclspv_build_fetch_bind
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_build_t aclspv_build_decl_bind;

/**
 * @fn		aclspv_bulid_decl_wrkgroup
 * @brief	emits the declaration of bindings as workgroup
 * @related	aclspv_build_fniter
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_build_t aclspv_build_decl_wrkgroup;

/**
 * @fn		aclspv_build_def_entp
 * @brief	emits the definition (body) of entry points
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_build_t aclspv_build_def_entp;

/** 
 * @fn		aclspv_build_asm
 * @brief	assemble the fianl header
 * @related	aclspv_build_decl_entp
 * @related	aclspv_build_decl_bind
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_build_t aclspv_build_asm;

typedef enum {
	/** @brief nothing is wrong. */
	ACLSPV_BUILDS_OK,
	ACLSPV_BUILDS_CONF	= ACLSPV_PASSES_EDENUM,
	ACLSPV_BUILDS_FNITER,
	ACLSPV_BUILDS_DECL_ENTP,
	ACLSPV_BUILDS_DECL_FN,
	ACLSPV_BUILDS_FETCH_BIND,
	ACLSPV_BUILDS_DECL_BIND,
	ACLSPV_BUILDS_DECL_WRKGROUP,
	ACLSPV_BUILDS_DEF_ENTP,
	ACLSPV_BUILDS_ASM,
	ACLSPV_BUILDS_EDENUM
} e_aclspv_builds;

/**
 * @fn		aclspv_pass_mkspv
 * @brief	make final spir-v for vulkan
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_pass_t aclspv_build_mkspv;


ae2f_extern ACLSPV_ABI_DECL ae2f_retnew
aclspv_wrd_t* aclspv_build(
		h_aclspv_lnker_t			h_lnk,
		e_fn_aclspv_pass* ae2f_restrict	const	wr_opt,
		aclspv_proc_t* ae2f_restrict const	wr_pass_wh_opt,
		uintptr_t* ae2f_restrict const		rwr_ret_count
		);

#endif
