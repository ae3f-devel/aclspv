#include "./ctx.h"
#include <llvm-c/Core.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


/** Standard generic address space for OpenCL in LLVM */
#define GENERIC_ADDR_SPACE 4

/**  macro to check if a type is a generic pointer */
#define		is_generic_pointer_type(c_ty)		\
	(LLVMGetTypeKind(c_ty) == LLVMPointerTypeKind	\
	 && LLVMGetPointerAddressSpace(c_ty) == GENERIC_ADDR_SPACE)


typedef struct {
	LLVMValueRef	m_phyptr;
	LLVMTypeRef	m_elty;
} phy_src_t;

/**
 * Helper to find the non-generic source of a generic pointer,
 * reconstructing intermediate instructions if necessary.
 * Returns the physical pointer, or NULL if not found/handled
 * This function is recursive and needs to handle various instruction types.
 * */
ae2f_inline static phy_src_t get_phy_src(
		LLVMValueRef generic_ptr,
		LLVMBuilderRef builder,
		x_aclspv_vec* ae2f_restrict V
		)
{
	phy_src_t	RET = { ae2f_NIL, ae2f_NIL };

	/** If the pointer is not generic, it's already physical. */
	unless (is_generic_pointer_type(LLVMTypeOf(generic_ptr))) {
		RET.m_phyptr	= generic_ptr;
		RET.m_elty	= LLVMGetElementType(LLVMTypeOf(generic_ptr));
		return RET;
	}

	/**
	 * If it's not an instruction (e.g., a function argument or global variable),
	 * and it's generic, we can't find a physical source this way.
	 */
	unless (LLVMIsAInstruction(generic_ptr)) {
		return RET;
	}

	/** If it's an instruction, we need to trace its definition. */
	if (LLVMIsAAddrSpaceCastInst(generic_ptr)) {
		const phy_src_t
			srres = get_phy_src(
				LLVMGetOperand(generic_ptr, 0)
				, builder
				, V
				);

		return srres.m_phyptr ? srres : RET /** NULL */;

	}

	if (LLVMIsAGetElementPtrInst(generic_ptr)) {
		const phy_src_t phy_base =
			get_phy_src(LLVMGetOperand(generic_ptr, 0), builder, V);

		LLVMTypeRef	gep_source_type;
		int num_indices;
		unsigned i;

		unless(phy_base.m_elty && phy_base.m_phyptr)
			return RET;


		/** Reconstruct GEP with physical base */
		gep_source_type = LLVMGetGEPSourceElementType(generic_ptr);
		num_indices = LLVMGetNumOperands(generic_ptr) - 1;

		if (num_indices < 0) return RET;

#define	indices		((LLVMValueRef* ae2f_restrict)(V->m_p))
#define num_indices	((unsigned)num_indices)

		_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, *V, (size_t)(num_indices * sizeof(LLVMValueRef)));
		unless(indices) return RET;

		for (i = num_indices; i--; ) {
			indices[i] = LLVMGetOperand(generic_ptr, i + 1);
		}

		RET.m_phyptr	= LLVMBuildGEP2(
				builder, gep_source_type
				, phy_base.m_phyptr
				, indices, num_indices, ""
				);
		RET.m_elty	= LLVMGetElementType(gep_source_type);
		return RET;
	}

	if (LLVMIsABitCastInst(generic_ptr)) {
#define source_val		LLVMGetOperand(generic_ptr, 0)
		const phy_src_t physical_source = get_phy_src(source_val, builder, V);

		unless (physical_source.m_elty && physical_source.m_phyptr)
			return RET;

		/** Reconstruct BitCast with physical source */
#define	old_dest_type		LLVMTypeOf(generic_ptr) /** This is the generic pointer type */
#define	physical_addr_space	LLVMGetPointerAddressSpace(LLVMTypeOf(physical_source.m_phyptr))
#define	element_type		LLVMGetElementType(old_dest_type) /** Assuming typed pointers */
#define	new_dest_type		LLVMPointerType(element_type, physical_addr_space)

		RET.m_phyptr	= LLVMBuildBitCast(builder, physical_source.m_phyptr, new_dest_type, "");
		RET.m_elty	= element_type;

		return RET;
	}
	/**
	 * TODO: Handle PHI nodes, Select instructions, etc. for more complex cases.
	 * For now, we only handle direct chains from addrspacecast, GEP, BitCast.
	 */

	return RET;
}

#undef	old_dest_type
#undef	physical_addr_space
#undef	element_type
#undef	new_dest_type
#undef	indices
#undef	num_indices


IMPL_PASS_RET aclspv_pass_fix_mem_access(
		const LLVMModuleRef M,
		const h_aclspv_pass_ctx CTX)
{

	LLVMBuilderRef builder = LLVMCreateBuilder();

	/** Struct to hold modification details: instruction, operand index, and new operand value */
	typedef struct { LLVMValueRef inst; unsigned op_idx; LLVMValueRef new_op; } Modification;
	size_t mod_count = 0;
	size_t	to_remove_count = 0;

#define	mod_capacity	((size_t)((CTX)->m_v1.m_sz / ((size_t)sizeof(Modification))))
#define	modifications	((Modification* ae2f_restrict)(CTX->m_v1.m_p))

	LLVMValueRef	F;

	/** First pass: Identify all generic pointer operands and their physical replacements */
	for (F = LLVMGetFirstFunction(M); F; F = LLVMGetNextFunction(F)) {
		LLVMBasicBlockRef	B;

		if (LLVMIsDeclaration(F)) continue; /* Skip function declarations */

		for (B = LLVMGetFirstBasicBlock(F); B; B = LLVMGetNextBasicBlock(B)) {
			LLVMValueRef	I;
			for (I = LLVMGetFirstInstruction(B); I; I = LLVMGetNextInstruction(I)) {
				unsigned op_idx;

				/** Position builder before current instruction for any new instructions created by get_phy_src */
				LLVMPositionBuilderBefore(builder, I);

#define num_operands 	LLVMGetNumOperands(I)
				if(num_operands < 0) return FN_ACLSPV_PASS_MET_INVAL;
				for (op_idx = (unsigned)num_operands; op_idx--; ) {
#undef	num_operands
					LLVMValueRef operand = LLVMGetOperand(I, op_idx);
					LLVMTypeRef operand_type = LLVMTypeOf(operand);

					if (is_generic_pointer_type(operand_type)) {
						/** Found a generic pointer operand, try to get its physical source */
						phy_src_t phy_rep = get_phy_src(
								operand, builder, &CTX->m_v0
								);

						if (phy_rep.m_phyptr && phy_rep.m_elty) {

#define	src_as		LLVMGetPointerAddressSpace(LLVMTypeOf(phy_rep.m_phyptr))

							/*
							 * 3 as workgroupm 12 as storagebuffer, 6 as fallback.
							 * TODO: document this or somewhere in header.
							 * */
#define	sc		((src_as == 1) ? 12 : (src_as == 3) ? 3 : 6)
#define	new_ptr_ty	LLVMPointerType(phy_rep.m_elty, sc)
#define	final_ptr	LLVMBuildBitCast(builder, phy_rep.m_phyptr, new_ptr_ty, "")

							/* Store modification to apply later */
							if (mod_count >= mod_capacity) {
								_aclspv_grow_vec_with_copy(
										_aclspv_malloc, _aclspv_free, _aclspv_memcpy
										, L_new, CTX->m_v1
										, ((mod_capacity == 0)
											? 16
											: mod_capacity << 1)
										);

								unless(modifications) return FN_ACLSPV_PASS_ALLOC_FAILED;
							}

							modifications[mod_count].inst = I;
							modifications[mod_count].op_idx = op_idx;
							modifications[mod_count].new_op = final_ptr /** phy_rep.m_phyptr */;

							++mod_count;
						}
					}
				}
			}
		}
	}

	/* Second pass: Apply all collected modifications */
	{
		size_t i = mod_count;
		while (i--) {
			LLVMSetOperand(modifications[i].inst, modifications[i].op_idx, modifications[i].new_op);
		}
	}

#undef modifications
#undef mod_capacity


	/** Third pass: Clean up dead addrspacecast instructions (and other dead instructions)
	 * We iterate through all instructions and remove those that are addrspacecasts
	 * and have no remaining uses.
	 */
#define	to_remove		((LLVMValueRef* ae2f_restrict)CTX->m_v0.m_p)
#define to_remove_capacity	((size_t)(CTX->m_v0.m_sz / sizeof(LLVMValueRef)))

	for (F = LLVMGetFirstFunction(M); F; F = LLVMGetNextFunction(F)) {
		LLVMBasicBlockRef	B;
		if (LLVMIsDeclaration(F)) continue;
		for (B = LLVMGetFirstBasicBlock(F); B; B = LLVMGetNextBasicBlock(B)) {
			LLVMValueRef I = LLVMGetFirstInstruction(B);
			while (I) {

				/* Get next instruction before potentially erasing I */
				const LLVMValueRef next_I = LLVMGetNextInstruction(I);
				if (LLVMIsAAddrSpaceCastInst(I) && LLVMGetFirstUse(I) == NULL) {
					_aclspv_grow_vec_with_copy(_aclspv_malloc, _aclspv_free, _aclspv_memcpy
							, L_new, CTX->m_v0, to_remove_count + 1);
					unless(to_remove) return FN_ACLSPV_PASS_ALLOC_FAILED;

					to_remove[to_remove_count++] = I;
				}
				I = next_I;
			}
		}
	}

	{

		size_t i;
		for (i = 0; i < to_remove_count; ++i) {
			if (LLVMGetInstructionParent(to_remove[i])) { /** Check if not already removed */
				LLVMInstructionEraseFromParent(to_remove[i]);
			}
		}
	}


	LLVMDisposeBuilder(builder); /* Dispose the builder */
	return FN_ACLSPV_PASS_OK;
}
