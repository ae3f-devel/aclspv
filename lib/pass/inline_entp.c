#include "./ctx.h"
#include "./md.h"
#include <llvm-c/Core.h>

#define	ATTR_ALWAYS_INLINE	"alwaysinline"

IMPL_PASS_RET aclspv_pass_inline_entp(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		)
{
#define kernel_md_node_len	\
	LLVMGetNamedMetadataNumOperands(M, ACLSPV_MD_OCL_KERNELS)

	LLVMValueRef	kernel_md_node, func_md_val;

	IGNORE(CTX);

	if (kernel_md_node_len != 1) {
		/** if non-single kernel, this pass is unnecessary */
		return FN_ACLSPV_PASS_OK;
	}

	LLVMGetNamedMetadataOperands(M, ACLSPV_MD_OCL_KERNELS, &kernel_md_node);

	unless (kernel_md_node) {
		return FN_ACLSPV_PASS_MET_INVAL;
	}

#define kernel_md_val	kernel_md_node

	if (LLVMGetNumOperands(kernel_md_val) == 0) {
		return FN_ACLSPV_PASS_MET_INVAL;
	}

	 func_md_val = LLVMGetOperand(kernel_md_val, 0);
	unless (func_md_val) {
		return FN_ACLSPV_PASS_MET_INVAL;
	}

#define	kernel_func	kernel_md_val

	if (LLVMGetValueKind(kernel_func) != LLVMFunctionValueKind) {
		return FN_ACLSPV_PASS_MET_INVAL;
	}

#define	always_inline_kind	LLVMGetEnumAttributeKindForName(ATTR_ALWAYS_INLINE, sizeof(ATTR_ALWAYS_INLINE) - 1)
#define always_inline_attr	LLVMCreateEnumAttribute(LLVMGetGlobalContext(), always_inline_kind, 0)
	LLVMAddAttributeAtIndex(kernel_func, (LLVMAttributeIndex)LLVMAttributeFunctionIndex, always_inline_attr);

	LLVMSetLinkage(kernel_func, LLVMInternalLinkage);

	return FN_ACLSPV_PASS_OK;
}
