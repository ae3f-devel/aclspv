#include <aclspv/build.h>
#include <assert.h>

ACLSPV_ABI_IMPL ae2f_noexcept e_fn_aclspv_pass aclspv_build_decl_fn(
		const LLVMModuleRef		M,
		const h_aclspv_build_ctx_t	CTX)
{
	assert(M);
	assert(CTX);

	return FN_ACLSPV_PASS_OK;
}
