/** 
 * @file 	lower_opq_ptr.c
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
ACLSPV_ABI_IMPL e_fn_aclspv_pass aclspv_pass_lower_opq_ptr(LLVMModuleRef _) {
	(void)_;
	return FN_ACLSPV_PASS_OK;
}
