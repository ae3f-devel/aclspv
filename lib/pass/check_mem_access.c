#include "./ctx.h"

#include <llvm-c/Core.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <aclspv/argknd.h>
#include <aclspv/md.h>

typedef enum {
	CHECK_WR_RECUR_GOOD,
	CHECK_WR_RECUR_ALLOC_FAILED,
	CHECK_WR_RECUR_ALLOC_ERR
} e_check_wr_recur_t;

/**
 * @brief Recursively traverses the uses of a value to find store instructions.
 * @param value The LLVM value to check (a pointer).
 * @param visited The set of already visited values to prevent infinite loops.
 */
ae2f_inline static e_check_wr_recur_t check_wr_recur(
		const LLVMValueRef c_val, 
		x_aclspv_vec* ae2f_restrict const rdwr_vec,
		const unsigned c_vcount 
		) {
	LLVMUseRef	L_use, N_use;

	unsigned	t_uint0;

	for((t_uint0) = (c_vcount); (t_uint0)--; ) {
		if((t_uint0)[ae2f_static_cast(const LLVMValueRef* ae2f_restrict, rdwr_vec->m_p)] == (c_val)) break;
	}

	if((t_uint0) >= (c_vcount)) {
		return CHECK_WR_RECUR_GOOD;
	}

	_aclspv_grow_vec_with_copy(
			_aclspv_malloc, _aclspv_free, _aclspv_memcpy
			, L_new, *rdwr_vec, (size_t)(((c_vcount) + 1) * sizeof(LLVMValueRef))
			);
	unless((rdwr_vec)->m_p) {
		return CHECK_WR_RECUR_ALLOC_FAILED;
	}

	(c_vcount)[ae2f_static_cast(LLVMValueRef* ae2f_restrict, rdwr_vec ->m_p)]
		= c_val;

	for (L_use = LLVMGetFirstUse(c_val); L_use; L_use = N_use) {
		const LLVMValueRef user = LLVMGetUser(L_use);

		N_use = LLVMGetNextUse(L_use);

		if (LLVMIsAStoreInst(user)) {
			/* The second operand of a store is the pointer being stored to. */
			if (LLVMGetOperand(user, 1) == c_val) {
				assert(0 && "Write to a read-only Uniform Buffer Object (UBO) detected!");
			}
		} else if (LLVMIsAInstruction(user)) {
			/* If the use is in an instruction that derives a new pointer, recurse. */
			switch ((unsigned long)LLVMGetInstructionOpcode(user)) {
				case LLVMGetElementPtr:
				case LLVMBitCast:
				case LLVMPtrToInt:
				case LLVMIntToPtr:
				case LLVMPHI: 
				case LLVMSelect:
					switch(check_wr_recur(user, rdwr_vec, c_vcount + 1)) {
						default: case CHECK_WR_RECUR_ALLOC_ERR:
							return CHECK_WR_RECUR_ALLOC_ERR;

						case CHECK_WR_RECUR_ALLOC_FAILED:
							return CHECK_WR_RECUR_ALLOC_FAILED;

						case CHECK_WR_RECUR_GOOD:
							break;
					}
					break;
				default:
					/** Other instructions are not expected to propagate the pointer for writing,
					 * or are complex cases (e.g., function calls) not handled here.
					 */
					break;
			}
		}
	}

	return CHECK_WR_RECUR_GOOD;
}

IMPL_PASS_RET aclspv_pass_check_mem_access(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		)
{
	const LLVMContextRef C = LLVMGetModuleContext(M);
	const unsigned arg_kind_md_id = LLVMGetMDKindIDInContext(C, ACLSPV_MD_ARGKND, sizeof(ACLSPV_MD_ARGKND) - 1);
	unsigned i;

	LLVMValueRef F, FNxt;
	for (F = LLVMGetFirstFunction(M); F; F = FNxt) {
		LLVMValueRef	arg_kinds_node;
		unsigned	num_params;

		FNxt = LLVMGetNextFunction(F);

		if (LLVMGetFunctionCallConv(F) != LLVMSPIRKERNELCallConv) {
			continue;
		}

		arg_kinds_node = LLVMGetMetadata(F, arg_kind_md_id);

		if (!arg_kinds_node) {
			continue;
		}

		num_params = LLVMCountParams(F);
		if (num_params != LLVMGetMDNodeNumOperands(arg_kinds_node)) {
			assert(0 && "Mismatch between function parameter count and argument kind metadata");
			continue;
		}

		_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_v0, (size_t)(num_params * sizeof(LLVMValueRef)));
#define	params	((LLVMValueRef* ae2f_restrict)CTX->m_v0.m_p)
		unless(params) return FN_ACLSPV_PASS_ALLOC_FAILED;
		LLVMGetParams(F, params);

		for (i = num_params; i--;) {
			const LLVMValueRef op = params[i];
			unsigned len;
			const char* kind_str;

			if (!op) continue;


			kind_str = LLVMGetMDString(op, &len);
			if (!kind_str) continue;

			if (strcmp(kind_str, ACLSPV_ARGKND_POD_UBO) == 0 ||
					strcmp(kind_str, ACLSPV_ARGKND_BUFF_UBO) == 0)
			{
				LLVMValueRef param = LLVMGetParam(F, i);
				check_wr_recur(param, &CTX->m_v1, 0);
			}
		}
	}

	return FN_ACLSPV_PASS_OK;
}


