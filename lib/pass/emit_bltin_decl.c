/**
 * @file emit_bltin_decl.c
 * @brief Implementation of the aclspv_pass_emit_bltin_decl pass.
 *
 * This pass finds declarations of OpenCL builtin functions (which have
 * mangled names in LLVM IR) and replaces them with declarations of
 * internal, backend-specific functions. This is a key step in lowering
 * OpenCL builtins to SPIR-V operations.
 */
#include "./ctx.h"
#include <llvm-c/Core.h>
#include <llvm-c/Types.h>
#include <llvm-c/DataTypes.h>

#include <string.h>
#include <assert.h>

#include "./bltin_lowered.h"

#define builtin_map g_aclspv_ocl_bltin_map

/**
 * A list of builtins to handle.
 * This list can be extended to support more OpenCL builtins.
 *
 * TODO: document this
 * TODO: have prefix macro
 * */ 
ACLSPV_ABI_IMPL const aclspv_ocl_bltin_map g_aclspv_ocl_bltin_map[] = {
    /* === Work-item builtins === */
    {"_Z13get_global_idj",      ACLSPV_PREFIX_BLTIN "get_global_id"},
    {"get_global_id",           ACLSPV_PREFIX_BLTIN "get_global_id"},

    {"_Z12get_local_idj",       ACLSPV_PREFIX_BLTIN "get_local_id"},
    {"get_local_id",            ACLSPV_PREFIX_BLTIN "get_local_id"},

    {"_Z12get_group_idj",       ACLSPV_PREFIX_BLTIN "get_group_id"},
    {"get_group_id",            ACLSPV_PREFIX_BLTIN "get_group_id"},

    {"_Z14get_global_sizej",    ACLSPV_PREFIX_BLTIN "get_global_size"},
    {"get_global_size",         ACLSPV_PREFIX_BLTIN "get_global_size"},

    {"_Z13get_local_sizej",     ACLSPV_PREFIX_BLTIN "get_local_size"},
    {"get_local_size",          ACLSPV_PREFIX_BLTIN "get_local_size"},

    {"_Z15get_global_offsetj",  ACLSPV_PREFIX_BLTIN "get_global_offset"},
    {"get_global_offset",       ACLSPV_PREFIX_BLTIN "get_global_offset"},

    {"_Z12get_work_dimv",       ACLSPV_PREFIX_BLTIN "get_work_dim"},
    {"get_work_dim",            ACLSPV_PREFIX_BLTIN "get_work_dim"},

    {"_Z16get_num_groupsj",     ACLSPV_PREFIX_BLTIN "get_num_groups"},
    {"get_num_groups",          ACLSPV_PREFIX_BLTIN "get_num_groups"},

    /* === Barrier === */
    {"_Z7barrierj",             ACLSPV_PREFIX_BLTIN "barrier"},
    {"barrier",                 ACLSPV_PREFIX_BLTIN "barrier"},

    /* === Memory fences === */
    {"_Z14mem_fencej",          ACLSPV_PREFIX_BLTIN "mem_fence"},
    {"mem_fence",               ACLSPV_PREFIX_BLTIN "mem_fence"},

    {"_Z18read_mem_fencej",     ACLSPV_PREFIX_BLTIN "read_mem_fence"},
    {"read_mem_fence",          ACLSPV_PREFIX_BLTIN "read_mem_fence"},

    {"_Z19write_mem_fencej",    ACLSPV_PREFIX_BLTIN "write_mem_fence"},
    {"write_mem_fence",         ACLSPV_PREFIX_BLTIN "write_mem_fence"},

    /* === 32-bit Integer Atomics === */
    {"_Z10atomic_addPU3AS1Vii", ACLSPV_PREFIX_BLTIN "atomic_add"},
    {"_Z10atomic_addPU3AS3Vii", ACLSPV_PREFIX_BLTIN "atomic_add"},
    {"_Z10atomic_subPU3AS1Vii", ACLSPV_PREFIX_BLTIN "atomic_sub"},
    {"_Z10atomic_subPU3AS3Vii", ACLSPV_PREFIX_BLTIN "atomic_sub"},

    {"_Z11atomic_xchgPU3AS1Vii", ACLSPV_PREFIX_BLTIN "atomic_xchg"},
    {"_Z11atomic_xchgPU3AS3Vii", ACLSPV_PREFIX_BLTIN "atomic_xchg"},

    {"_Z9atomic_incPU3AS1Vi",   ACLSPV_PREFIX_BLTIN "atomic_inc"},
    {"_Z9atomic_incPU3AS3Vi",   ACLSPV_PREFIX_BLTIN "atomic_inc"},
    {"_Z9atomic_decPU3AS1Vi",   ACLSPV_PREFIX_BLTIN "atomic_dec"},
    {"_Z9atomic_decPU3AS3Vi",   ACLSPV_PREFIX_BLTIN "atomic_dec"},

    {"_Z14atomic_cmpxchgPU3AS1Viii", ACLSPV_PREFIX_BLTIN "atomic_cmpxchg"},
    {"_Z14atomic_cmpxchgPU3AS3Viii", ACLSPV_PREFIX_BLTIN "atomic_cmpxchg"},

    {"_Z10atomic_minPU3AS1Vii", ACLSPV_PREFIX_BLTIN "atomic_min"},
    {"_Z10atomic_minPU3AS3Vii", ACLSPV_PREFIX_BLTIN "atomic_min"},
    {"_Z10atomic_maxPU3AS1Vii", ACLSPV_PREFIX_BLTIN "atomic_max"},
    {"_Z10atomic_maxPU3AS3Vii", ACLSPV_PREFIX_BLTIN "atomic_max"},

    {"_Z10atomic_minPU3AS1Vjj", ACLSPV_PREFIX_BLTIN "atomic_umin"},
    {"_Z10atomic_minPU3AS3Vjj", ACLSPV_PREFIX_BLTIN "atomic_umin"},
    {"_Z10atomic_maxPU3AS1Vjj", ACLSPV_PREFIX_BLTIN "atomic_umax"},
    {"_Z10atomic_maxPU3AS3Vjj", ACLSPV_PREFIX_BLTIN "atomic_umax"},

    {"_Z10atomic_andPU3AS1Vii", ACLSPV_PREFIX_BLTIN "atomic_and"},
    {"_Z10atomic_andPU3AS3Vii", ACLSPV_PREFIX_BLTIN "atomic_and"},
    {"_Z10atomic_orPU3AS1Vii",  ACLSPV_PREFIX_BLTIN "atomic_or"},
    {"_Z10atomic_orPU3AS3Vii",  ACLSPV_PREFIX_BLTIN "atomic_or"},
    {"_Z10atomic_xorPU3AS1Vii", ACLSPV_PREFIX_BLTIN "atomic_xor"},
    {"_Z10atomic_xorPU3AS3Vii", ACLSPV_PREFIX_BLTIN "atomic_xor"},

    /* === Math builtins (common ones) === */
    {"_Z3sinf", ACLSPV_PREFIX_BLTIN "sin"},   {"sin",  ACLSPV_PREFIX_BLTIN "sin"},
    {"_Z3cosf", ACLSPV_PREFIX_BLTIN "cos"},   {"cos",  ACLSPV_PREFIX_BLTIN "cos"},
    {"_Z4fabsf", ACLSPV_PREFIX_BLTIN "fabs"}, {"fabs", ACLSPV_PREFIX_BLTIN "fabs"},
    {"_Z5sqrtf", ACLSPV_PREFIX_BLTIN "sqrt"}, {"sqrt", ACLSPV_PREFIX_BLTIN "sqrt"},
    {"_Z3logf", ACLSPV_PREFIX_BLTIN "log"},   {"log",  ACLSPV_PREFIX_BLTIN "log"},
    {"_Z4expf", ACLSPV_PREFIX_BLTIN "exp"},   {"exp",  ACLSPV_PREFIX_BLTIN "exp"},
    {"_Z5powff", ACLSPV_PREFIX_BLTIN "pow"},  {"pow",  ACLSPV_PREFIX_BLTIN "pow"},

    /* === Sub-group (optional but common) === */
    {"_Z18get_sub_group_sizev", ACLSPV_PREFIX_BLTIN "sub_group_size"},
    {"_Z19get_sub_group_idv",   ACLSPV_PREFIX_BLTIN "sub_group_id"},
    {"_Z22get_sub_group_local_idv", ACLSPV_PREFIX_BLTIN "sub_group_local_id"},
};


extern char A[sizeof(g_aclspv_ocl_bltin_map) / sizeof(g_aclspv_ocl_bltin_map[0]) == ACLSPV_OCL_BLTIN_MAP_COUNT ? 1 : -1];

IMPL_PASS_RET aclspv_pass_emit_bltin_decl(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		)
{
	const size_t num_builtins = sizeof(builtin_map) / sizeof(builtin_map[0]);
	size_t i = num_builtins;

	assert(M);
	IGNORE(CTX);

	while (i--) {
		const char* ae2f_restrict const ocl_name = builtin_map[i].m_ocl_name;
		const char* ae2f_restrict const m_internal_name = builtin_map[i].m_internal_name;
		const LLVMValueRef ocl_func = LLVMGetNamedFunction(M, ocl_name);
		LLVMTypeRef	func_type;
		LLVMValueRef	internal_func;

		if (!ocl_func || !LLVMIsDeclaration(ocl_func) || !LLVMIsAFunction(ocl_func) || LLVMGetFirstUse(ocl_func) == NULL) {
			/**
			 * The builtin is not declared, has a body, or is not used.
			 * In any case, there's nothing for us to do for this one.
			 * */
			continue;
		}

		assert(LLVMIsAFunction(ocl_func));

		func_type = (LLVMGlobalGetValueType(LLVMIsAFunction(ocl_func)));
		assert(LLVMGetTypeKind(func_type) == LLVMFunctionTypeKind);

		/** Get or insert the internal representation of the builtin function. */
		internal_func = LLVMGetNamedFunction(M, m_internal_name);

		if (!internal_func) {
			internal_func = LLVMAddFunction(M, m_internal_name, func_type);
		} else {

			/** Function already exists, ensure type matches. */
			LLVMTypeRef existing_type = LLVMGlobalGetValueType(internal_func);
			assert(LLVMGetTypeKind(func_type) == LLVMFunctionTypeKind);

			if (existing_type != func_type) {
				/**
				 * Type mismatch, which indicates a problem in the IR or the pass logic.
				 * For now, we skip this to avoid creating invalid IR.
				 * */
				return FN_ACLSPV_PASS_MET_INVAL;
			}
		}

		/** Replace all uses of the OpenCL builtin with our internal one. */
		LLVMReplaceAllUsesWith(ocl_func, internal_func);

		/** The old function declaration is now unused and can be removed. */
		LLVMDeleteFunction(ocl_func);
	}

	return FN_ACLSPV_PASS_OK;
}


