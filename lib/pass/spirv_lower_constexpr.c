/** 
 * @file 	spirv_lower_constexpr.c
 * @brief	is an implementation for an interface
 * */

#include <aclspv/pass.h>

#include <ae2f/c90/StdBool.h>
#include <ae2f/c90/Limits.h>
#include <ae2f/c90/StdInt.h>


/** 
 * @see <aclspv/pass.h> 
 * TODO: implement this 
 * */
ACLSPV_ABI_IMPL e_fn_aclspv_pass aclspv_pass_spirv_lower_constexpr(LLVMModuleRef _) {
	(void)_;
	return FN_ACLSPV_PASS_OK;
}
