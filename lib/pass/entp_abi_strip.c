#include "./ctx.h"
#include "./md.h"
#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>

IMPL_PASS_RET aclspv_pass_entp_abi_strip(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		) {
	const unsigned num_kernels = LLVMGetNamedMetadataNumOperands(M, ACLSPV_MD_OCL_KERNELS);
	const unsigned byval_kind = LLVMGetEnumAttributeKindForName("byval", 5);
	const unsigned sret_kind = LLVMGetEnumAttributeKindForName("sret", 4);

	unsigned i = num_kernels;
	unsigned j;


	IGNORE(CTX);

#define	kernel_md_nodes ae2f_static_cast(LLVMValueRef* ae2f_restrict, CTX->m_v0.m_p)
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_v0
			, (size_t)(sizeof(LLVMValueRef) * num_kernels));

	LLVMGetNamedMetadataOperands(M, ACLSPV_MD_OCL_KERNELS, kernel_md_nodes);

	while (i--) {
		const LLVMValueRef	kernel_md_node = kernel_md_nodes[i];
		const unsigned		func_md_len = LLVMIsAMDNode(kernel_md_node) 
			? LLVMGetMDNodeNumOperands(kernel_md_node) : 
			0
			;

		unsigned param_count;

		LLVMValueRef func_md;

		if (func_md_len != 1) {
			continue;
		}
#define	func_mds ae2f_static_cast(LLVMValueRef* ae2f_restrict, CTX->m_v1.m_p)
		_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_v1, (size_t)(sizeof(LLVMValueRef) * func_md_len));
		LLVMGetMDNodeOperands(kernel_md_node, func_mds);

		func_md = func_mds[0]; 
		unless (func_md) {
			continue;
		}

#define kernel_func	func_md

		if (!kernel_func || LLVMIsAFunction(kernel_func) == 0) {
			continue;
		}

		/** Strip 'byval' and 'sret' from all parameters */
		param_count = LLVMCountParams(kernel_func);
		for (j = param_count; j--; ) {
			const LLVMAttributeIndex attr_idx = j + 1;
			if (LLVMGetEnumAttributeAtIndex(kernel_func, attr_idx,
						       byval_kind)) {
				LLVMRemoveEnumAttributeAtIndex(
						kernel_func, attr_idx, byval_kind);
			}

			if (LLVMGetEnumAttributeAtIndex(kernel_func, attr_idx,
						       sret_kind)) {
				LLVMRemoveEnumAttributeAtIndex(
						kernel_func, attr_idx, sret_kind);
			}
		}
	}

	return FN_ACLSPV_PASS_OK;
}
