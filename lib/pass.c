#include <aclspv/pass.h>
#include <assert.h>

#define Z(a)	0

ACLSPV_ABI_IMPL
size_t	aclspv_runall_module_passes(
		LLVMModuleRef		h_module, 
		e_fn_aclspv_pass* ae2f_restrict	wr_res_opt
		)
{
	e_fn_aclspv_pass	ret = FN_ACLSPV_PASS_OK;
	size_t			progress = 0;

	unless(h_module) {
		assert(Z(FN_ACLSPV_PASS_MODULE_NIL));
		ret = FN_ACLSPV_PASS_MODULE_NIL;
		goto LBL_RET;
	}

	if((ret = aclspv_pass_rewr_kern_fn(h_module))) {
		goto LBL_RET;
	}

	progress++;


LBL_RET:
	if(wr_res_opt) *wr_res_opt = ret;
	return progress;
}
