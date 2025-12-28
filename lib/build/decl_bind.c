#include <build.h>
#include <assert.h>
#include <stdio.h>

#include <pass/md.h>

#include <spirv/unified1/spirv.h>

#include "./wrdemit.h"
#include "./entp.h"
#include "./bind.h"
#include "./scale.h"

#define var_infos	((lib_build_bind* ae2f_restrict)CTX->m_vecid_vars.m_p)
#define interface_ids_mgr	get_last_scale_from_vec(&CTX->m_scale_vars)
#define	interface_ids		((aclspv_wrd_t* ae2f_restrict)get_buf_from_scale(&CTX->m_scale_vars, *interface_ids_mgr))

#define	__vec			((lib_build_entp_t * ae2f_restrict)CTX->m_fnlist.m_entp.m_p)

ACLSPV_ABI_IMPL ae2f_noexcept e_fn_aclspv_pass	aclspv_build_decl_bind(
		const LLVMModuleRef		M,
		const h_aclspv_build_ctx_t	CTX
		)
{
	aclspv_wrd_t	num_vars = sz_to_count(CTX->m_vecid_vars.m_sz), i;

	IGNORE(M);

	/** OpName for Decorators */
#if	!defined(NDEBUG) || !NDEBUG
	for(i = (unsigned)num_vars; i--; ) {
		char	var_name[64];

		aclspv_wrdcount_t	name_pos;

		snprintf(
				var_name, sizeof(var_name), "%s.arg%u"
				, LLVMGetValueName(__vec[var_infos[i].m_entp_idx].m_fn), var_infos[i].m_arg_idx
				);

#define		ret_count	CTX->m_count.m_name
#define		m_ret		m_section.m_name
		name_pos = ret_count;
		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpName, 0))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, var_infos[i].m_var_id)))
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_str(&CTX->m_ret, ret_count, var_name))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_ret)[name_pos], ret_count - name_pos - 1);
	}
#endif

	/* Emit decorations */
	for (i = num_vars; i--; ) {
#define	info	(&var_infos[i])

		/** Decorations::Descriptorsets */
		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpDecorate, 3))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->m_var_id))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvDecorationDescriptorSet))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->m_set))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;

		/** Decorations::Bind */
		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpDecorate, 3))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->m_var_id))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvDecorationBinding))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->m_binding))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;

#undef		ret_count
#undef		m_ret

#define		ret_count	CTX->m_count.m_vars
#define		m_ret		m_section.m_vars
		/* OpVariable */
		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpVariable, 3))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->m_ptr_struct_id))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->m_var_id))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->m_storage_class)))
			return FN_ACLSPV_PASS_ALLOC_FAILED;
#undef	info
	}

	return FN_ACLSPV_PASS_OK;
}
