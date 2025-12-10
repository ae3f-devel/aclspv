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
 * */ 
ACLSPV_ABI_IMPL const aclspv_ocl_bltin_map g_aclspv_ocl_bltin_map[] = {
    /* === Work-item builtins === */
    {"_Z13get_global_idj",      "aclspv.get_global_id"},
    {"get_global_id",           "aclspv.get_global_id"},

    {"_Z12get_local_idj",       "aclspv.get_local_id"},
    {"get_local_id",            "aclspv.get_local_id"},

    {"_Z12get_group_idj",       "aclspv.get_group_id"},
    {"get_group_id",            "aclspv.get_group_id"},

    {"_Z14get_global_sizej",    "aclspv.get_global_size"},
    {"get_global_size",         "aclspv.get_global_size"},

    {"_Z13get_local_sizej",     "aclspv.get_local_size"},
    {"get_local_size",          "aclspv.get_local_size"},

    {"_Z15get_global_offsetj",  "aclspv.get_global_offset"},
    {"get_global_offset",       "aclspv.get_global_offset"},

    {"_Z12get_work_dimv",       "aclspv.get_work_dim"},
    {"get_work_dim",            "aclspv.get_work_dim"},

    {"_Z16get_num_groupsj",     "aclspv.get_num_groups"},
    {"get_num_groups",          "aclspv.get_num_groups"},

    /* === Barrier === */
    {"_Z7barrierj",             "aclspv.barrier"},
    {"barrier",                 "aclspv.barrier"},

    /* === Memory fences === */
    {"_Z14mem_fencej",          "aclspv.mem_fence"},
    {"mem_fence",               "aclspv.mem_fence"},

    {"_Z18read_mem_fencej",     "aclspv.read_mem_fence"},
    {"read_mem_fence",          "aclspv.read_mem_fence"},

    {"_Z19write_mem_fencej",    "aclspv.write_mem_fence"},
    {"write_mem_fence",         "aclspv.write_mem_fence"},

    /* === 32-bit Integer Atomics === */
    {"_Z10atomic_addPU3AS1Vii", "aclspv.atomic_add"},
    {"_Z10atomic_addPU3AS3Vii", "aclspv.atomic_add"},
    {"_Z10atomic_subPU3AS1Vii", "aclspv.atomic_sub"},
    {"_Z10atomic_subPU3AS3Vii", "aclspv.atomic_sub"},

    {"_Z11atomic_xchgPU3AS1Vii", "aclspv.atomic_xchg"},
    {"_Z11atomic_xchgPU3AS3Vii", "aclspv.atomic_xchg"},

    {"_Z9atomic_incPU3AS1Vi",   "aclspv.atomic_inc"},
    {"_Z9atomic_incPU3AS3Vi",   "aclspv.atomic_inc"},
    {"_Z9atomic_decPU3AS1Vi",   "aclspv.atomic_dec"},
    {"_Z9atomic_decPU3AS3Vi",   "aclspv.atomic_dec"},

    {"_Z14atomic_cmpxchgPU3AS1Viii", "aclspv.atomic_cmpxchg"},
    {"_Z14atomic_cmpxchgPU3AS3Viii", "aclspv.atomic_cmpxchg"},

    {"_Z10atomic_minPU3AS1Vii", "aclspv.atomic_min"},
    {"_Z10atomic_minPU3AS3Vii", "aclspv.atomic_min"},
    {"_Z10atomic_maxPU3AS1Vii", "aclspv.atomic_max"},
    {"_Z10atomic_maxPU3AS3Vii", "aclspv.atomic_max"},

    {"_Z10atomic_minPU3AS1Vjj", "aclspv.atomic_umin"},
    {"_Z10atomic_minPU3AS3Vjj", "aclspv.atomic_umin"},
    {"_Z10atomic_maxPU3AS1Vjj", "aclspv.atomic_umax"},
    {"_Z10atomic_maxPU3AS3Vjj", "aclspv.atomic_umax"},

    {"_Z10atomic_andPU3AS1Vii", "aclspv.atomic_and"},
    {"_Z10atomic_andPU3AS3Vii", "aclspv.atomic_and"},
    {"_Z10atomic_orPU3AS1Vii",  "aclspv.atomic_or"},
    {"_Z10atomic_orPU3AS3Vii",  "aclspv.atomic_or"},
    {"_Z10atomic_xorPU3AS1Vii", "aclspv.atomic_xor"},
    {"_Z10atomic_xorPU3AS3Vii", "aclspv.atomic_xor"},

    /* === Math builtins (common ones) === */
    {"_Z3sinf", "aclspv.sin"},   {"sin",  "aclspv.sin"},
    {"_Z3cosf", "aclspv.cos"},   {"cos",  "aclspv.cos"},
    {"_Z4fabsf", "aclspv.fabs"}, {"fabs", "aclspv.fabs"},
    {"_Z5sqrtf", "aclspv.sqrt"}, {"sqrt", "aclspv.sqrt"},
    {"_Z3logf", "aclspv.log"},   {"log",  "aclspv.log"},
    {"_Z4expf", "aclspv.exp"},   {"exp",  "aclspv.exp"},
    {"_Z5powff", "aclspv.pow"},  {"pow",  "aclspv.pow"},

    /* === Sub-group (optional but common) === */
    {"_Z18get_sub_group_sizev", "aclspv.sub_group_size"},
    {"_Z19get_sub_group_idv",   "aclspv.sub_group_id"},
    {"_Z22get_sub_group_local_idv", "aclspv.sub_group_local_id"},
};

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


