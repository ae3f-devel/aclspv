#include <aclspv/build.h>

#include "./id.h"
#include "./entp.h"
#include "./wrdemit.h"
#include "./iddef.h"

#include "aclspv/pass.h"

#include <build.h>
#include <spirv/unified1/spirv.h>

ACLSPV_ABI_IMPL ae2f_noexcept e_fn_aclspv_pass	aclspv_build_def_entp(
		const LLVMModuleRef		M,
		const h_aclspv_build_ctx_t	CTX
		)
{
	aclspv_wrdcount_t	i;

	assert(M); assert(CTX);

#define		ret_count	CTX->m_count.m_entpdef
#define		m_ret		m_section.m_entpdef
#define		num_kernels	CTX->m_fnlist.m_num_entp
#define		kernel_nodes	((lib_build_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)

	/* Emit function stubs */
	for (i = num_kernels; i--; ) {
		aclspv_wrd_t func_id = kernel_nodes[i].m_id; 
		aclspv_wrd_t lbl_id = CTX->m_id++;

		unless(lib_build_get_default_id(ID_DEFAULT_VOID, CTX))
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless(lib_build_get_default_id(ID_DEFAULT_FN_VOID, CTX))
			return FN_ACLSPV_PASS_ALLOC_FAILED;

		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpFunction, 4))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_VOID))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, func_id))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvFunctionControlMaskNone))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_FN_VOID))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;

		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpLabel, 1))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, lbl_id))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;

		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpReturn, 0))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpFunctionEnd, 0))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
	}

#undef	m_ret
#undef	ret_count
#undef	num_kernels
#undef	kernel_nodes

	return FN_ACLSPV_PASS_OK;
}
