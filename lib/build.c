#include "./ctx.h"
#include "./build.h"
#include "./build/scale.h"

#include <string.h>

#include <aclspv/pass.h>
#include <aclspv/build.h>


ACLSPV_ABI_IMPL ae2f_retnew aclspv_wrd_t* aclspv_build(
		h_aclspv_lnker_t			h_lnk,
		e_fn_aclspv_pass* ae2f_restrict	const	wr_opt,
		aclspv_proc_t* ae2f_restrict const	wr_pass_wh_opt,
		uintptr_t* ae2f_restrict const		rwr_ret_count
		) 
{
	x_aclspv_build_ctx contxt;

	aclspv_proc_t		r_wh = ACLSPV_PASSES_OK;
	e_fn_aclspv_pass	r_fn = FN_ACLSPV_PASS_OK;


	memset(&contxt, 0, sizeof(contxt));

	unless(h_lnk) {
		r_fn = FN_ACLSPV_PASS_MET_INVAL;
		r_wh = ACLSPV_PASSES_EDENUM;
		goto END;
	}


	r_wh = aclspv_runall_module_passes(
			h_lnk->m_module
			, &contxt.m_pass_ctx
			, &r_fn
			);

	unless (init_scale(&contxt.m_scale_vars, 0)) {
		r_fn = FN_ACLSPV_PASS_ALLOC_FAILED;
		goto END;
	}

	if(r_wh || r_fn) {
		goto END;
	}

	if((r_fn = aclspv_build_conf(h_lnk->m_module, &contxt))) {
		r_wh = ACLSPV_BUILDS_CONF;
		goto END;
	}

	if((r_fn = aclspv_build_fniter(h_lnk->m_module, &contxt))) {
		r_wh = ACLSPV_BUILDS_FNITER;
		goto END;
	}

	if((r_fn = aclspv_build_decl_entp(h_lnk->m_module, &contxt))) {
		r_wh = ACLSPV_BUILDS_DECL_ENTP;
		goto END;
	}

	if((r_fn = aclspv_build_decl_fn(h_lnk->m_module, &contxt))) {
		r_wh = ACLSPV_BUILDS_DECL_FN;
		goto END;
	}

	if((r_fn = aclspv_build_fetch_bind(h_lnk->m_module, &contxt))) {
		r_wh = ACLSPV_BUILDS_FETCH_BIND;
		goto END;
	}

	if((r_fn = aclspv_build_decl_bind(h_lnk->m_module, &contxt))) {
		r_wh = ACLSPV_BUILDS_DECL_BIND;
		goto END;
	}

	if((r_fn = aclspv_build_decl_wrkgroup(h_lnk->m_module, &contxt))) {
		r_wh = ACLSPV_BUILDS_DECL_WRKGROUP;
		goto END;
	}

	if((r_fn = aclspv_build_def_entp(h_lnk->m_module, &contxt))) {
		r_wh = ACLSPV_BUILDS_DEF_ENTP;
		goto END;
	}

	if((r_fn = aclspv_build_asm(h_lnk->m_module, &contxt))) {
		r_wh = ACLSPV_BUILDS_ASM;
		goto END;
	}

END:
	_aclspv_stop_vec(_aclspv_free, contxt.m_scale_vars);
	_aclspv_stop_vec(_aclspv_free, contxt.m_vecid_vars);
	_aclspv_stop_vec(_aclspv_free, contxt.m_constant_cache);

	_aclspv_stop_vec(_aclspv_free, contxt.m_pass_ctx.m_v0);
	_aclspv_stop_vec(_aclspv_free, contxt.m_pass_ctx.m_v1);
	_aclspv_stop_vec(_aclspv_free, contxt.m_pass_ctx.m_v2);

	_aclspv_stop_vec(_aclspv_free, contxt.m_fnlist.m_entp);
	_aclspv_stop_vec(_aclspv_free, contxt.m_fnlist.m_fn);

	_aclspv_stop_vec(_aclspv_free, contxt.m_section.m_decorate);
	_aclspv_stop_vec(_aclspv_free, contxt.m_section.m_entp);
	_aclspv_stop_vec(_aclspv_free, contxt.m_section.m_entpdef);
	_aclspv_stop_vec(_aclspv_free, contxt.m_section.m_name);
	_aclspv_stop_vec(_aclspv_free, contxt.m_section.m_vars);
	_aclspv_stop_vec(_aclspv_free, contxt.m_section.m_execmode);
	_aclspv_stop_vec(_aclspv_free, contxt.m_section.m_types);

	_aclspv_stop_vec(_aclspv_free, contxt.m_tmp.m_v0);

	if(wr_pass_wh_opt) {
		*wr_pass_wh_opt = r_wh;
	}

	if(wr_opt) {
		*wr_opt = r_fn;
	}

	if(rwr_ret_count) {
		*rwr_ret_count = (uintptr_t)(contxt).m_retcount;
	}

	return contxt.m_ret.m_p;
}
