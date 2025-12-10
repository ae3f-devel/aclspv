#include "./ctx.h"
#include <llvm-c/Core.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define GENERIC_ADDR_SPACE 4

#define		is_generic_pointer_type(c_ty)		\
	(LLVMGetTypeKind(c_ty) == LLVMPointerTypeKind	\
	&& LLVMGetPointerAddressSpace(c_ty))

IMPL_PASS_RET aclspv_pass_fix_mem_access(
    const LLVMModuleRef M,
    const h_aclspv_pass_ctx CTX
    )
{
	LLVMBuilderRef builder = LLVMCreateBuilder();
	LLVMValueRef F;
	const LLVMContextRef C = LLVMGetModuleContext(M);

	IGNORE(CTX);

	for (F = LLVMGetFirstFunction(M); F; F = LLVMGetNextFunction(F)) {
		LLVMBasicBlockRef BB;
		if (LLVMGetFunctionCallConv(F) != LLVMSPIRKERNELCallConv)
			continue;

		for (BB = LLVMGetFirstBasicBlock(F); BB; BB = LLVMGetNextBasicBlock(BB)) {
			LLVMValueRef I;

			for (I = LLVMGetFirstInstruction(BB); I; I = LLVMGetNextInstruction(I)) {
				const LLVMOpcode op = LLVMGetInstructionOpcode(I);
				LLVMValueRef	ptr_op;
				LLVMTypeRef	ptr_ty, expected_ty;
				unsigned	ptr_as, expected_as;


				if (op != LLVMLoad && op != LLVMStore && op != LLVMGetElementPtr && op != LLVMBitCast)
					continue;

				ptr_op = LLVMGetOperand(I, 0);
				ptr_ty = LLVMTypeOf(ptr_op);
				ptr_as = LLVMGetPointerAddressSpace(ptr_ty);

				/* Does this instruction expect a generic pointer? */
				expected_ty = LLVMTypeOf(I);
				expected_as = LLVMGetPointerAddressSpace(expected_ty);

				if (expected_as != GENERIC_ADDR_SPACE)
					continue;

				if (ptr_as == GENERIC_ADDR_SPACE)
					continue; /* Already generic — nothing to do */

				/* Need to convert physical pointer to generic */

				/* In Vulkan, generic = StorageBuffer for global, Workgroup for local */
#define		target_sc	((ptr_as == 1) ? 12  /* StorageBuffer */ : 3) /* Workgroup */
#define		phys_ty		LLVMPointerTypeInContext(C, target_sc)
#define		phys_ptr	LLVMBuildBitCast(builder, ptr_op, phys_ty, "")

				LLVMPositionBuilderBefore(builder, I);

				LLVMSetOperand(I, 0, phys_ptr);
			}
		}
	}

	LLVMDisposeBuilder(builder);
	return FN_ACLSPV_PASS_OK;
}
