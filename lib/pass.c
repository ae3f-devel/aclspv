#include <aclspv/pass.h>
#include <assert.h>
#include "./pass/ctx.h"


#define aclspv_free(a, b)	free(a)

#define Z(a)	0

ae2f_noexcept ACLSPV_ABI_IMPL e_aclspv_passes aclspv_runall_module_passes(
		const LLVMModuleRef			h_module, 
		const h_aclspv_pass_ctx			hrdwr_ctx_opt,
		e_fn_aclspv_pass* ae2f_restrict const	wr_res_opt
		)
{
	e_fn_aclspv_pass	codepass	= FN_ACLSPV_PASS_OK;
	e_aclspv_passes		code		= ACLSPV_PASSES_OK;
	struct x_aclspv_pass_ctx	ctx;

	if(hrdwr_ctx_opt) {
		ctx = *hrdwr_ctx_opt;
	} else {
		_aclspv_init_vec(ctx.m_v0);
		_aclspv_init_vec(ctx.m_v1);
		_aclspv_init_vec(ctx.m_v2);
	}

	unless(h_module) {
		assert(Z(FN_ACLSPV_PASS_MODULE_NIL));
		code		= ACLSPV_PASSES_ADD_KERN_METADATA;
		codepass	= FN_ACLSPV_PASS_MODULE_NIL;
		goto LBL_RET;
	}

	if((codepass = aclspv_pass_add_kern_metadata(h_module, &ctx))) {
		code = ACLSPV_PASSES_ADD_KERN_METADATA;
		goto LBL_RET;
	}

	if((codepass = aclspv_pass_arg_anal(h_module, &ctx))) {
		code = ACLSPV_PASSES_ARG_ANAL;
		goto LBL_RET;
	}

#if 1
	if((codepass = aclspv_pass_assgn_pipelayout(h_module, &ctx))) {
		code = ACLSPV_PASSES_ASSGN_PIPELAYOUT;
		goto LBL_RET;
	}
#endif

	if((codepass = aclspv_pass_check_mem_access(h_module, &ctx))) {
		code = ACLSPV_PASSES_CHECK_MEM_ACCESS;
		goto LBL_RET;
	}

	if((codepass = aclspv_pass_cluster_pod_args(h_module, &ctx))) {
		code = ACLSPV_PASSES_CLUSTER_POD_ARGS;
		goto LBL_RET;
	}

	if((codepass = aclspv_pass_default_layout(h_module, &ctx))) {
		code = ACLSPV_PASSES_DEFAULT_LAYOUT;
		goto LBL_RET;
	}

	if((codepass = aclspv_pass_emit_bltin_decl(h_module, &ctx))) {
		code = ACLSPV_PASSES_EMIT_BLTIN_DECL;
		goto LBL_RET;
	}

	if((codepass = aclspv_pass_entp_abi_strip(h_module, &ctx))) {
		code = ACLSPV_PASSES_ENTP_ABI_STRIP;
		goto LBL_RET;
	}

	if((codepass = aclspv_pass_fix_mem_access(h_module, &ctx))) {
		code = ACLSPV_PASSES_FIX_MEM_ACCESS;
		goto LBL_RET;
	}

	if((codepass = aclspv_pass_inline_entp(h_module, &ctx))) {
		code = ACLSPV_PASSES_INLINE_ENTP;
		goto LBL_RET;
	}

	if((codepass = aclspv_pass_lower_ocl_intel_subgrps(h_module, &ctx))) {
		code = ACLSPV_PASSES_LOWER_OCL_INTEL_SUBGRPS;
		goto LBL_RET;
	}

	if((codepass = aclspv_pass_ocl_bltin_lower(h_module, &ctx))) {
		code = ACLSPV_PASSES_OCL_BLTIN_LOWER;
		goto LBL_RET;
	}

	if((codepass = aclspv_pass_alloc_descriptor(h_module, &ctx))) {
		code = ACLSPV_PASSES_ALLOC_DESCRIPTOR;
		goto LBL_RET;
	}

	if((codepass = aclspv_pass_rep_ptr_bitcast(h_module, &ctx))) {
		code = ACLSPV_PASSES_REP_PTR_BITCAST;
		goto LBL_RET;
	}

	if((codepass = aclspv_pass_loc_mem(h_module, &ctx))) {
		code = ACLSPV_PASSES_LOC_MEM;
		goto LBL_RET;
	}

	if((codepass = aclspv_pass_rewr_loc_ptr(h_module, &ctx))) {
		code = ACLSPV_PASSES_REWR_LOC_PTR;
		goto LBL_RET;
	}

	if(hrdwr_ctx_opt && (codepass = aclspv_pass_mkspv(h_module, &ctx))) {
		code = ACLSPV_PASSES_MKSPV;
		goto LBL_RET;
	}

LBL_RET:
	if(wr_res_opt) *wr_res_opt = codepass;
	if(hrdwr_ctx_opt) {
		*hrdwr_ctx_opt = ctx;
	} else {
		_aclspv_stop_vec(_aclspv_free, ctx.m_v0);
		_aclspv_stop_vec(_aclspv_free, ctx.m_v1);
		_aclspv_stop_vec(_aclspv_free, ctx.m_v2);
	}

	return code;
}
