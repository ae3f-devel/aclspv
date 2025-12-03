#include <aclspv.h>
#include <clang-c/Index.h>
#include <llvm-c/Core.h>
#include <llvm-c/Target.h>

ae2f_extern ACLSPV_ABI_IMPL
e_aclspv_init_global	aclspv_init_global(void) {
	/** TODO: implement this */

	LLVMInitializeAllTargets();
	LLVMInitializeAllTargetMCs();
	LLVMInitializeAllAsmParsers();
	LLVMInitializeAllAsmPrinters();

	return ACLSPV_INIT_GLOBAL_OK;
}

ae2f_extern ACLSPV_ABI_IMPL
void	aclspv_stop_global(void) {
	/** TODO: implement this */
}
