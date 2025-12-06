#include <aclspv/pass.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stddef.h>

#include <ae2f/c90/Limits.h>

#define get_kern_from_md(md)	LLVMGetOperand(md, 0)
#define ZERO(a)			0

/**
 * @function	aclspv_pass_rewr_kern_fn
 * @details	\n
 * 	- Rewrites kernel signatures into SPIR-V compatible form	\n
 * 	- Lowers kernel parameters into buffers or POD arguments.
 */
ACLSPV_ABI_IMPL
e_fn_aclspv_pass aclspv_pass_rewr_kern_fn(LLVMModuleRef M) {
	unsigned num_kernels, i;
	LLVMValueRef* ae2f_restrict V_md = ae2f_NIL;
	e_fn_aclspv_pass	ret;

	num_kernels = LLVMGetNamedMetadataNumOperands(M, "opencl.kernels");
	V_md = malloc((size_t)(sizeof(LLVMValueRef) * num_kernels));
	ret = FN_ACLSPV_PASS_OK;

	unless(V_md) {
		ret = FN_ACLSPV_PASS_ALLOC_FAILED;
	}

	LLVMGetNamedMetadataOperands(M, "opencl.kernels", V_md);

	for (i = num_kernels; i--; ) {
		LLVMValueRef	old_fn, new_fn, md;

		LLVMBasicBlockRef	block;

		LLVMTypeRef 
			ret_void, new_fn_ty, old_fn_ty
			, * new_params = ae2f_NIL
			;
		const char*	old_name = ae2f_NIL;
		char*		new_name = ae2f_NIL;
		size_t		name_len;

		size_t nparams, p;


		old_fn = get_kern_from_md(V_md[i]);
		unless (old_fn) continue;

		/* ---- Step 1: get original signature ---- */

		old_fn_ty = LLVMGetElementType(LLVMTypeOf(old_fn));
		nparams = (size_t)LLVMCountParamTypes(old_fn_ty);

		/* build new param types */
		new_params = malloc((((size_t)sizeof(LLVMTypeRef)) * ((size_t)nparams)));
		unless(new_params) {
			assert(ZERO("FN_ACLSPV_PASS_ALLOC_FAILED"));
			ret = FN_ACLSPV_PASS_ALLOC_FAILED;
			goto LBL_LOOP;
		}

		/** 0xfffff.... */
		p = nparams;
		for (p = nparams ; p-- ;) {
			/* pointer to i8 addrspace(1) */
			LLVMTypeRef i8 = LLVMInt8TypeInContext(LLVMGetModuleContext(M));
			LLVMTypeRef ptr = LLVMPointerType(i8, 1);

			new_params[p] = ptr;
		}

		ret_void = LLVMVoidTypeInContext(LLVMGetModuleContext(M));
		new_fn_ty = LLVMFunctionType(ret_void, new_params, (unsigned)nparams, 0);

		/* ---- Step 2: create new function ---- */

		old_name = LLVMGetValueName(old_fn);
		name_len = strlen(old_name);

		/** I could not understand why sizeof(".rewrite") must be hard-coded as 16 */
		new_name = malloc((size_t)(name_len + (size_t)sizeof(".rewrite")));
		unless(new_name) goto LBL_LOOP;

		/** was char* new_name */
		strcpy(new_name, old_name);
		strcat(new_name, ".rewrite");

		new_fn = LLVMAddFunction(M, new_name, new_fn_ty);

		/* ---- Step 3: copy attributes ---- */

		LLVMSetLinkage(new_fn, LLVMGetLinkage(old_fn));
		LLVMSetFunctionCallConv(new_fn, LLVMGetFunctionCallConv(old_fn));

		/* ---- Step 4: move body ---- */

		block = LLVMGetFirstBasicBlock(old_fn);
		while (block) {
			LLVMBasicBlockRef next = LLVMGetNextBasicBlock(block);
			LLVMMoveBasicBlockAfter(block
					, (LLVMBasicBlockRef)LLVMGetBasicBlockTerminator(
						LLVMGetLastBasicBlock(new_fn)
						)
					);
			block = next;
		}

		/* ---- Step 5: replace old function in callers ---- */

		LLVMReplaceAllUsesWith(old_fn, new_fn);

		/* ---- Step 6: rename old to garbage and delete ---- */

		LLVMSetValueName(old_fn, ".dead_kernel");
		LLVMDeleteFunction(old_fn);

		/* ---- Step 7: update metadata entry to point to new_fn ---- */

		/** LLVMGetNamedMetadataOperand(M, "opencl.kernels", i); */
		md = get_kern_from_md(V_md[i]);

		if (md) {
			LLVMSetOperand(md, 0, new_fn);
		}

		free(new_params);
		free(new_name);
		continue;

LBL_LOOP:
		free(new_params);
		free(new_name);
		break;
	}


	free(V_md);
	return ret;
}
