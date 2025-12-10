#include "./ctx.h"
#include <aclspv/argknd.h>
#include <aclspv/md.h>
#include <llvm-c/Core.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MD_KERNELS "opencl.kernels"

IMPL_PASS_RET aclspv_pass_default_layout(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		)
{
	LLVMContextRef C = LLVMGetModuleContext(M);
	unsigned i;

	const unsigned layout_kind_id = LLVMGetMDKindIDInContext(
			C
			, ACLSPV_MD_PIPELINE_LAYOUT
			, sizeof(ACLSPV_MD_PIPELINE_LAYOUT) - 1 
			);
	const unsigned arg_kind_id = LLVMGetMDKindIDInContext(
			C
			, ACLSPV_MD_ARGKND
			, (sizeof(ACLSPV_MD_ARGKND) - 1)
			);

	const unsigned num_kernels = LLVMGetNamedMetadataNumOperands(M, MD_KERNELS);
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_v0, (size_t)(sizeof(LLVMValueRef) * num_kernels));
#define	kernel_nodes	ae2f_static_cast(LLVMValueRef* ae2f_restrict, CTX->m_v0.m_p)

	LLVMGetNamedMetadataOperands(M, MD_KERNELS, kernel_nodes);

	for (i = 0; i < num_kernels; ++i) {
		const LLVMValueRef	kernel_node = kernel_nodes[i];
		LLVMValueRef	F;
		unsigned num_params;
		size_t	layout_mds_count = 0;
		unsigned binding = 0;
		unsigned arg_idx = 0;

		LLVMValueRef	P, PNxt, arg_kind_md;

		if (LLVMGetNumOperands(kernel_node) == 0) {
			continue;
		}

		F = LLVMGetOperand(kernel_node, 0);
		if (!F || !LLVMIsAFunction(F)) {
			continue;
		}

		/** If layout metadata already exists, skip this kernel. */
		if (LLVMGetMetadata(F, layout_kind_id)) {
			continue;
		}

		num_params = LLVMCountParams(F);
		if (num_params == 0) {
			continue;
		}

		arg_kind_md = LLVMGetMetadata(F, arg_kind_id);

		_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_v1, (size_t)(sizeof(LLVMValueRef) * num_params));
#define		layout_mds	ae2f_static_cast(LLVMValueRef* ae2f_restrict, CTX->m_v1.m_p)
		unless (layout_mds) {
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		}

		for (P = LLVMGetFirstParam(F); P; P = PNxt) {
			LLVMValueRef	str_md_val;
			unsigned	len;
			const char* ae2f_restrict kind_str;

			PNxt = LLVMGetNextParam(P);

			if (!arg_kind_md) {
				arg_idx++;
				continue;
			}

			if (!LLVMIsAMDNode(arg_kind_md) || LLVMGetNumOperands(arg_kind_md) == 0) {
				arg_idx++;
				continue;
			}

			str_md_val = LLVMGetOperand(arg_kind_md, 0);
			if (!str_md_val || !LLVMIsAMDString(str_md_val)) {
				arg_idx++;
				continue;
			}
			
			kind_str = LLVMGetMDString(str_md_val, &len);
			unless (kind_str) {
				arg_idx++;
				continue;
			}

#define			is_resource							\
				(strcmp(kind_str, ACLSPV_ARGKND_BUFF) == 0) ||		\
				(strcmp(kind_str, ACLSPV_ARGKND_BUFF_UBO) == 0) ||	\
				(strcmp(kind_str, ACLSPV_ARGKND_RO_IMG) == 0) ||	\
				(strcmp(kind_str, ACLSPV_ARGKND_WO_IMG) == 0) ||	\
				(strcmp(kind_str, ACLSPV_ARGKND_SMPLR) == 0)

			if (is_resource) {
				LLVMValueRef	fin_vals[3];

				/** Layout metadata is a node: !{i32 <arg_idx>, i32 <set>, i32 <binding>} */
#define v_arg_idx	LLVMConstInt(LLVMInt32TypeInContext(C), arg_idx, 0)

				/** Default set 0 */
#define	v_set		LLVMConstInt(LLVMInt32TypeInContext(C), 0, 0)	
#define	v_binding	LLVMConstInt(LLVMInt32TypeInContext(C), binding++, 0)

				fin_vals[0] = v_arg_idx;
				fin_vals[1] = v_set;
				fin_vals[2] = v_binding;

				layout_mds[layout_mds_count++] = 
					LLVMMDNodeInContext(C, fin_vals, sizeof(fin_vals) / sizeof(fin_vals[0]));
			}
			arg_idx++;
		}

		if (layout_mds_count > 0) {
			const LLVMValueRef func_layout_md = LLVMMDNodeInContext(C, layout_mds, (unsigned)layout_mds_count);
			LLVMSetMetadata(F, layout_kind_id, func_layout_md);
		}
	}

	return FN_ACLSPV_PASS_OK;
}
