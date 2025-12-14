/**
 * @file	pass.h
 * @brief	pass utility function declarations for module
 * */

#ifndef aclspv_pass_h
#define aclspv_pass_h

#include <llvm-c/Core.h>
#include <aclspv/abi.h>
#include <ae2f/Keys.h>
#include <ae2f/cc.h>

/** 
 * @enum	e_fn_aclspv_pass
 * @brief	results of `fn_aclspv_pass_t` like functions
 * */
typedef enum {
	/** @brief	success */
	FN_ACLSPV_PASS_OK,

	/** @brief	passed module was null. */
	FN_ACLSPV_PASS_MODULE_NIL,

	/** @brief	allocation has failed. */
	FN_ACLSPV_PASS_ALLOC_FAILED,

	/** @brief	finding argument kind has been failed. */
	FN_ACLSPV_PASS_FAILED_FND_ARGKND,

	/** @brief	met unexpected situation */
	FN_ACLSPV_PASS_MET_INVAL,

	/** @brief	met the valid syntax but technically not supported. */
	FN_ACLSPV_PASS_NO_SUPPORT
} e_fn_aclspv_pass;

/**
 * @class	x_aclspv_pass_ctx
 * @typedef	a_aclspv_pass_ctx
 * @typedef	h_aclspv_pass_ctx
 * */
typedef struct x_aclspv_pass_ctx a_aclspv_pass_ctx, * ae2f_restrict h_aclspv_pass_ctx;

/**
 * @typedef	fn_aclspv_pass_t
 * @brief	pass function
 * */
typedef e_fn_aclspv_pass fn_aclspv_pass_t(const LLVMModuleRef, const h_aclspv_pass_ctx);

/**
 * @fn		aclspv_pass_add_kern_metadata
 * @brief	Adds metadata to kernel functions for argument analysis and entry point identification.
 * @details	Prepares OpenCL kernel metadata for Vulkan entry point mapping.
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_pass_t aclspv_pass_add_kern_metadata;

/**
 * @fn		aclspv_pass_arg_anal
 * @brief	Analyzes kernel argument kinds and assigns types.
 * @details	\n
 * kernel argument kinds e.g., POD, images, samplers	\n
 * assign types e.g.,  ArgKind::Pod or ArgKind::Image.
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_pass_t aclspv_pass_arg_anal;


/**
 * @fn		aclspv_pass_assgn_pipelayout
 * @brief	Assigns Vulkan pipeline layout information, including descriptor sets and push constant ranges.
 * @details	Ensures Vulkan-compliant layout for SPIR-V entry points.
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_pass_t aclspv_pass_assgn_pipelayout;

/**
 * @fn		aclspv_pass_check_mem_access
 * @brief	Verifies memory access patterns for Vulkan storage classes (e.g., StorageBuffer vs. Uniform).
 * @detials	Validates OpenCL memory semantics against Vulkan restrictions.
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_pass_t aclspv_pass_check_mem_access;

/**
 * @fn		aclspv_pass_cluster_pod_args
 * @brief	Groups plain-old-data (POD) kernel arguments into clusters for efficient Vulkan passing.
 * @details	Bundles scalar/vector args into composite structures for push constants or buffers.
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_pass_t aclspv_pass_cluster_pod_args;

/**
 * @fn		aclspv_pass_default_layout
 * @brief	Applies a default descriptor set layout if not specified.
 * @details	Provides fallback Vulkan layout for simple kernels.
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_pass_t aclspv_pass_default_layout;

/**
 * @fn		aclspv_pass_emit_bltin_decl
 * @brief	Declares OpenCL builtin functions as LLVM intrinsics or calls.
 * @details	Sets up lowering for OpenCL builtins to Vulkan-compatible ops.
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_pass_t aclspv_pass_emit_bltin_decl;

/**
 * @fn		aclspv_pass_entp_abi_strip
 * @brief	Strips ABI-specific attributes from entry points to match Vulkan calling conventions.
 * @details	Cleans up OpenCL ABI for Vulkan compute shader entry points.
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_pass_t aclspv_pass_entp_abi_strip;

/**
 * @fn		aclspv_pass_fix_mem_access
 * @brief	Adjusts memory accesses to use Vulkan storage classes (e.g., converting generic to physical addressing).
 * @details	Transforms OpenCL pointer accesses to Vulkan's opaque pointer model.
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_pass_t aclspv_pass_fix_mem_access;


/**
 * @fn		aclspv_pass_inline_entp
 * @brief	Inlines kernel entry points for single-kernel modules.
 * @details	Optimizes for Vulkan's flat module structure.
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_pass_t aclspv_pass_inline_entp;

/**
 * @fn		aclspv_pass_lower_ocl_intel_subgrps
 * @brief	Lowers OpenCL subgroup builtins to SPIR-V subgroup ops.
 * @details	Maps OpenCL subgroup extensions to Vulkan subgroup capabilities.
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_pass_t aclspv_pass_lower_ocl_intel_subgrps;

/**
 * @fn		aclspv_pass_ocl_bltin_lower
 * @brief	Lowers OpenCL builtins (e.g., `get_global_id`) to LLVM intrinsics or SPIR-V equivalents.
 * @detials	Replaces OpenCL-specific calls with Vulkan/SPIR-V compatible code.
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_pass_t aclspv_pass_ocl_bltin_lower;
/**
 * @fn		aclspv_pass_pod_pshconst_args
 * @brief	Rewrites POD arguments to use Vulkan push constants instead of descriptors.
 * @details	Routes small args via push constants for performance.
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_noexcept fn_aclspv_pass_t aclspv_pass_pod_pshconst_args;

/**
 * @enum	e_aclspv_passes
 * @brief	the given number for passes
 * */
typedef enum {
	ACLSPV_PASSES_OK,
	/** @brief error from `aclspv_pass_add_kern_metadata` */
	ACLSPV_PASSES_ADD_KERN_METADATA,

	/** @brief error from `aclspv_pass_arg_anal` */
	ACLSPV_PASSES_ARG_ANAL,
	
	/** @brief error from `aclspv_pass_assgn_pipelayout` */
	ACLSPV_PASSES_ASSGN_PIPELAYOUT,

	/** @brief error from `aclspv_pass_check_mem_access` */
	ACLSPV_PASSES_CHECK_MEM_ACCESS,

	/** @brief error from `aclspv_pass_cluster_pod_args` */
	ACLSPV_PASSES_CLUSTER_POD_ARGS,

	/** @brief error from `aclspv_pass_default_layout` */
	ACLSPV_PASSES_DEFAULT_LAYOUT,

	/** @brief error from `aclspv_pass_emit_bltin_decl` */
	ACLSPV_PASSES_EMIT_BLTIN_DECL,

	/** @brief error from `aclspv_pass_entp_abi_strip` */
	ACLSPV_PASSES_ENTP_ABI_STRIP,

	/** @brief error from `aclspv_pass_fix_mem_access` */
	ACLSPV_PASSES_FIX_MEM_ACCESS,

	/** @brief error from `aclspv_pass_inline_entp` */
	ACLSPV_PASSES_INLINE_ENTP,

	/** @brief error from `aclspv_pass_lower_ocl_intel_subgrps` */
	ACLSPV_PASSES_LOWER_OCL_INTEL_SUBGRPS,

	/** @brief error from `aclspv_pass_ocl_bltin_lower` */
	ACLSPV_PASSES_OCL_BLTIN_LOWER
} e_aclspv_passes;


/**
 * @fn		aclspv_runall_module_passes
 * @brief	run all passes for a module
 * @param	h_module	<HANDLE>	\n
 * a module to run passes
 *
 * @param	wr_res_opt	<WR> <OPT>	\n
 * When not null, 
 *
 * @return	\n
 * `ACLSPV_PASSES_OK` (0) on success.
 * When not, the given number for a function will be returned.
 * */
ae2f_extern ACLSPV_ABI_DECL
e_aclspv_passes	aclspv_runall_module_passes(
		const LLVMModuleRef		h_module, 
		e_fn_aclspv_pass* ae2f_restrict	const wr_res_opt
		);


#endif
