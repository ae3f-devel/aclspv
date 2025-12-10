#include "./ctx.h"

#include <llvm-c/Core.h>
#include <string.h>
#include <assert.h>

#include <aclspv/argknd.h>
#include <aclspv/md.h>


#define md_kind_str ACLSPV_MD_ARGKND 

#define ZSTR(a)	0

typedef enum { 
	/** @brief __global */
	CL_ADDRSPACE_GLOB	= 1,

	/** @brief __constant */
	CL_ADDRSPACE_CONST	= 2,

	/** @brief __local */
	CL_ADDRSPACE_LOC	= 3
} e_cl_addrspace;

ae2f_noexcept static const char* get_arg_kind(
		const LLVMValueRef ae2f_restrict param, 
		const unsigned param_index
		) {
	const LLVMTypeRef type = LLVMTypeOf(param);
	const LLVMValueRef func = LLVMGetParamParent(param);

	/** 1-based in llvm */
	const LLVMAttributeIndex attr_index = param_index + 1;

	if (LLVMGetTypeKind(type) == LLVMPointerTypeKind) {
		const unsigned addr_space = LLVMGetPointerAddressSpace(type);
		LLVMTypeRef	pointee_type;

		/** __local */
		if (addr_space == CL_ADDRSPACE_LOC) {
			return ACLSPV_ARGKND_LOC;
		}

		if (LLVMPointerTypeIsOpaque(type)) {
			switch((e_cl_addrspace)addr_space) {
				default:
				case CL_ADDRSPACE_LOC:
					assert(ZSTR("Unexpected value"));
					return ACLSPV_ARGKND_LOC;

				case CL_ADDRSPACE_GLOB:
					return ACLSPV_ARGKND_BUFF;

				case CL_ADDRSPACE_CONST:
					return ACLSPV_ARGKND_BUFF_UBO;
			}
		}

		pointee_type = LLVMGetElementType(type);

		if (LLVMGetTypeKind(pointee_type) == LLVMStructTypeKind) {
			const char* ae2f_restrict const name = LLVMGetStructName(pointee_type);
			if (name) {
				if (strstr(name, "opencl.sampler_t")) {
					return ACLSPV_ARGKND_SMPLR;
				}
				if (strstr(name, "opencl.image")) {
					const unsigned writeonly_kind = LLVMGetEnumAttributeKindForName("writeonly", sizeof("writeonly") - 1);
					if (LLVMGetEnumAttributeAtIndex(func, attr_index, writeonly_kind)) {
						return ACLSPV_ARGKND_WO_IMG;
					}
					return ACLSPV_ARGKND_RO_IMG;
				}
			}
		}

		switch((e_cl_addrspace)addr_space) {
			case CL_ADDRSPACE_LOC:
			default:
				assert(0);
				return ae2f_NIL;

			case CL_ADDRSPACE_GLOB:
				if(LLVMGetEnumAttributeAtIndex(func, attr_index, LLVMGetEnumAttributeKindForName(
								"readonly"
								, sizeof("readonly") - 1)))
					return ACLSPV_ARGKND_BUFF_UBO;

				return ACLSPV_ARGKND_BUFF;

			case CL_ADDRSPACE_CONST: 
				return ACLSPV_ARGKND_BUFF_UBO;
		}
	}

	/** if non-pointer, i would think it to be push constant  */
	return ACLSPV_ARGKND_POD_PSHCONST;
}

IMPL_PASS_RET aclspv_pass_arg_anal(
		const LLVMModuleRef	M,
		h_aclspv_pass_ctx	CTX	
		)
{
	const LLVMContextRef C = LLVMGetModuleContext(M);
	const unsigned kind_id =
		LLVMGetMDKindIDInContext(C, md_kind_str, sizeof(md_kind_str) - 1);

	LLVMValueRef F, FNxt;

	for (F = LLVMGetFirstFunction(M); F; F = FNxt) {
		FNxt = LLVMGetNextFunction(F);

		if (LLVMGetFunctionCallConv(F) != LLVMSPIRKERNELCallConv) {
			continue;
		}

#define kern_fn F
		{
			const unsigned nprms = LLVMCountParams(kern_fn);
			unsigned j;

			unless (nprms) {
				LLVMGlobalSetMetadata(
						kern_fn,
						kind_id,
						LLVMMDNodeInContext2(C, ae2f_NIL, 0)
						);
				continue;
			}

			_aclspv_grow_vec(
					_aclspv_malloc,
					_aclspv_free,
					CTX->m_v0,
					(size_t)(sizeof(LLVMMetadataRef) * nprms)
					);

			_aclspv_grow_vec(
					_aclspv_malloc,
					_aclspv_free,
					CTX->m_v1,
					(size_t)(sizeof(LLVMValueRef) * nprms)
					);
#define arg_kind_mds	ae2f_static_cast(LLVMMetadataRef* ae2f_restrict, CTX->m_v0.m_p)
#define prms		ae2f_static_cast(LLVMValueRef* ae2f_restrict, CTX->m_v1.m_p)

			unless (arg_kind_mds && prms)
				return FN_ACLSPV_PASS_ALLOC_FAILED;

			LLVMGetParams(kern_fn, prms);


			for (j = nprms; j--; ) {
				const char* ae2f_restrict kind_str;
				assert(j < nprms);
				kind_str = get_arg_kind(prms[j], j);
				unless(kind_str)
					return FN_ACLSPV_PASS_FAILED_FND_ARGKND;

				arg_kind_mds[j] =
					LLVMMDStringInContext2(C, kind_str, strlen(kind_str));
			}

			LLVMGlobalSetMetadata(
					kern_fn,
					kind_id,
					LLVMMDNodeInContext2(C, arg_kind_mds, (size_t)nprms)
					);
		}
#undef kern_fn
#undef arg_kind_mds
#undef prms
	}

	return FN_ACLSPV_PASS_OK;
}
