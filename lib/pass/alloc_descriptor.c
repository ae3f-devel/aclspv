/** 
 * @file	alloc_descriptor.c
 * @brief	implementation for `aclspv_pass_alloc_descriptor`.
 * */

#include "./ctx.h"
#include "./md.h"
#include "./argknd.h"
#include <llvm-c/Core.h>
#include <ae2f/c90/StdBool.h>
#include <string.h>
#include <assert.h>

static bool needs_descriptor(const char* ae2f_restrict kind) {
    if (!kind) return false;
    return strcmp(kind, ACLSPV_ARGKND_BUFF) == 0 ||
           strcmp(kind, ACLSPV_ARGKND_BUFF_UBO) == 0 ||
           strcmp(kind, ACLSPV_ARGKND_RO_IMG) == 0 ||
           strcmp(kind, ACLSPV_ARGKND_WO_IMG) == 0 ||
           strcmp(kind, ACLSPV_ARGKND_SMPLR) == 0 ||
           strcmp(kind, ACLSPV_ARGKND_POD_UBO) == 0;
}

IMPL_PASS_RET aclspv_pass_alloc_descriptor(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		)
{

	LLVMContextRef C = LLVMGetModuleContext(M);
	const unsigned args_md_id = LLVMGetMDKindIDInContext(C, ACLSPV_MD_ARGS, sizeof(ACLSPV_MD_ARGS) - 1);
	const unsigned layout_md_id = LLVMGetMDKindIDInContext(C, ACLSPV_MD_PIPELINE_LAYOUT, sizeof(ACLSPV_MD_PIPELINE_LAYOUT) - 1);
	const unsigned num_kernels = LLVMGetNamedMetadataNumOperands(M, ACLSPV_MD_OCL_KERNELS);
	unsigned i;

	unless (num_kernels) {
		return FN_ACLSPV_PASS_OK; 
	}

	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_v0, (size_t)(sizeof(LLVMValueRef) * num_kernels));
#define	kernel_md_nodes	((LLVMValueRef* ae2f_restrict)CTX->m_v0.m_p)
	unless(kernel_md_nodes) return FN_ACLSPV_PASS_ALLOC_FAILED;
	LLVMGetNamedMetadataOperands(M, ACLSPV_MD_OCL_KERNELS, kernel_md_nodes);

	for (i = num_kernels; i-- ;) {
		const LLVMValueRef kernel_md_node = kernel_md_nodes[i];
		LLVMValueRef	kernel_func, args_md;
		int	num_args;

		unsigned layout_count = 0;
		unsigned j;


		if (!kernel_md_node || LLVMGetNumOperands(kernel_md_node) == 0) {
			continue;
		}

		kernel_func = LLVMGetOperand(kernel_md_node, 0);

		if (!kernel_func || LLVMIsAFunction(kernel_func) == NULL) {
			continue;
		}

		args_md = LLVMGetMetadata(kernel_func, args_md_id);
		if (!args_md) {
			continue;
		}

		num_args = LLVMGetNumOperands(args_md);
		if(num_args == 0) goto LBL_SKIP_NARGS_FOR;
		if(num_args < 0) return FN_ACLSPV_PASS_MET_INVAL;

		_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_v1, (size_t)(sizeof(LLVMValueRef) * ((unsigned)num_args)));
#define layout_operands	((LLVMValueRef* ae2f_restrict)CTX->m_v1.m_p)
		unless(layout_operands) return FN_ACLSPV_PASS_ALLOC_FAILED;

		for (j = (unsigned)num_args; j-- ;) {
			const LLVMValueRef arg_info_md = LLVMGetOperand(args_md, j);
			const char* ae2f_restrict  kind_str = NULL;
			LLVMValueRef kind_md = NULL;
			unsigned k;
			int num_arg_info_ops;

			if (!arg_info_md || !LLVMIsAMDNode(arg_info_md)) continue;
			num_arg_info_ops = LLVMGetNumOperands(arg_info_md);
			if(num_arg_info_ops == 0) goto LBL_SKIP_FOR_K;
			if(num_arg_info_ops < 0) return FN_ACLSPV_PASS_MET_INVAL;

			for (k = 0; k < (unsigned)num_arg_info_ops; k += 2) {
				LLVMValueRef	key_md;
				const char* ae2f_restrict key_str;

				if (k + 1 >= (unsigned)num_arg_info_ops) break;
				key_md = LLVMGetOperand(arg_info_md, k);
				if (!key_md || !LLVMIsAMDString(key_md)) continue;
				
				key_str = LLVMGetMDString((key_md), NULL);
				if (key_str && strcmp(key_str, ACLSPV_MD_ARGKND) == 0) {
					const LLVMValueRef val_md = LLVMGetOperand(arg_info_md, k + 1);
					if (val_md && LLVMIsAMDString(val_md)) {
						kind_md = val_md;
						kind_str = LLVMGetMDString((val_md), NULL);
					}
					break;
				}
			}

LBL_SKIP_FOR_K:

			if (!kind_str) {
				return FN_ACLSPV_PASS_FAILED_FND_ARGKND;
			}

			if (needs_descriptor(kind_str)) {
				/**  set, binding, kind_md, argidx */
				LLVMValueRef values[4];

				values[0] = LLVMConstInt(LLVMInt32TypeInContext(C), 0, false);
				values[1] = LLVMConstInt(LLVMInt32TypeInContext(C), j, false);
				values[2] = kind_md;
				values[3] = LLVMConstInt(LLVMInt32TypeInContext(C), j, false); 

				layout_operands[layout_count++] = LLVMMDNodeInContext(C, values, 4);
			}
		}

LBL_SKIP_NARGS_FOR:

		if (layout_count > 0) {
			LLVMValueRef pipeline_layout_md = LLVMMDNodeInContext(C, layout_operands, layout_count);
			LLVMSetMetadata(kernel_func, layout_md_id, pipeline_layout_md);
		}

	}

	return FN_ACLSPV_PASS_OK;
}

