#include <aclspv/pass.h>
#include <aclspv/argknd.h>

#include <assert.h>
#include "./pass/ctx.h"

#include <stdlib.h>

#define aclspv_free(a, b)	free(a)

#define Z(a)	0

ae2f_noexcept ACLSPV_ABI_IMPL e_aclspv_passes aclspv_runall_module_passes(
		const LLVMModuleRef			h_module, 
		e_fn_aclspv_pass* ae2f_restrict const	wr_res_opt
		)
{
	e_fn_aclspv_pass	codepass	= FN_ACLSPV_PASS_OK;
	e_aclspv_passes		code		= ACLSPV_PASSES_OK;
	a_aclspv_pass_ctx	ctx;

	_aclspv_init_vec(ctx.m_v0);
	_aclspv_init_vec(ctx.m_v1);

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

	if((codepass = aclspv_pass_assgn_pipelayout(h_module, &ctx))) {
		code = ACLSPV_PASSES_ASSGN_PIPELAYOUT;
		goto LBL_RET;
	}

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

LBL_RET:
	_aclspv_stop_vec(aclspv_free, ctx.m_v0);
	_aclspv_stop_vec(aclspv_free, ctx.m_v1);

	if(wr_res_opt) *wr_res_opt = codepass;
	return code;
}
