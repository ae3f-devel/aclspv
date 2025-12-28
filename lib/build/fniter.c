#include <pass/md.h>
#include <build.h>

#include <aclspv/pass.h>
#include <assert.h>

#include "./entp.h"
#include "./wrdemit.h"
#include "./fn.h"

ACLSPV_ABI_IMPL ae2f_noexcept e_fn_aclspv_pass	aclspv_build_fniter(
		const LLVMModuleRef		M,
		const h_aclspv_build_ctx_t	CTX
		)
{
	LLVMValueRef	func;
	aclspv_id_t	
		i_kernel = 0
		, i_func = 0
		, num_kernels = 0
		, num_func = 0
		;

	for (func = LLVMGetFirstFunction(M); func; func = LLVMGetNextFunction(func)) {
		if (LLVMGetMetadata(func, LLVMGetMDKindID(
						ACLSPV_MD_PIPELINE_LAYOUT
						, sizeof(ACLSPV_MD_PIPELINE_LAYOUT) - 1))

				&& LLVMGetFunctionCallConv(func) == LLVMSPIRKERNELCallConv
		   )
		{
			++num_kernels;
		} else if(!LLVMIsDeclaration(func)) {
			++num_func;
		}
	}

	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free
			, CTX->m_fnlist.m_entp, (size_t)(sizeof(lib_build_entp_t) * num_kernels)
			);
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free
			, CTX->m_fnlist.m_fn, (size_t)(sizeof(lib_build_fn_t) * num_kernels)
			);

	unless(CTX->m_fnlist.m_entp.m_p || !num_kernels) return FN_ACLSPV_PASS_ALLOC_FAILED;
	unless(CTX->m_fnlist.m_fn.m_p || !num_func)	return FN_ACLSPV_PASS_ALLOC_FAILED;

	CTX->m_fnlist.m_num_entp = num_kernels;
	CTX->m_fnlist.m_num_fn = num_func;


	for (func = LLVMGetFirstFunction(M); func; func = LLVMGetNextFunction(func)) {
		if (LLVMGetMetadata(func, LLVMGetMDKindID(
						ACLSPV_MD_PIPELINE_LAYOUT
						, sizeof(ACLSPV_MD_PIPELINE_LAYOUT) - 1))

				&& LLVMGetFunctionCallConv(func) == LLVMSPIRKERNELCallConv
		   )
		{
			assert(i_kernel < num_kernels);
			((lib_build_entp_t* ae2f_restrict)(CTX)->m_fnlist.m_entp.m_p)[i_kernel].m_fn = func;
			((lib_build_entp_t* ae2f_restrict)(CTX)->m_fnlist.m_entp.m_p)[i_kernel].m_id = CTX->m_id + i_kernel;
			++i_kernel;
		} else if(!LLVMIsDeclaration(func)) {
			((lib_build_fn_t* ae2f_restrict)(CTX->m_fnlist.m_fn.m_p))[i_func].m_fn = func;
			((lib_build_fn_t* ae2f_restrict)(CTX->m_fnlist.m_fn.m_p))[i_func].m_id = CTX->m_id + num_kernels + i_func;
			((lib_build_fn_t* ae2f_restrict)(CTX->m_fnlist.m_fn.m_p))[i_func].m_type_id = 0; /** not made */
			++i_func;
		}
	}

	assert(i_kernel == num_kernels);
	assert(i_func == num_func);

	CTX->m_id += num_kernels + num_func;

	return FN_ACLSPV_PASS_OK;
}
