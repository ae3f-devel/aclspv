#include "./ctx.h"

IMPL_PASS_RET aclspv_pass_lower_ocl_intel_subgrps(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		) 
{
	/**
	 * TODO:
	 * Following is the stub for lower opencl intel subgroups.
	 * Modern OpenCL does not utilise it, so I am not implementing it now.
	 * This will be implemented after the passes works finely.
	 * */

	IGNORE(M); IGNORE(CTX);
	return FN_ACLSPV_PASS_OK;
}
