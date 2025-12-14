#include <aclspv.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

ae2f_extern ACLSPV_ABI_IMPL
e_aclspv_init_global	aclspv_init_global(void) {
	LLVMInitializeAllTargets();
	LLVMInitializeAllTargetMCs();
	LLVMInitializeAllAsmParsers();
	LLVMInitializeAllAsmPrinters();
	LLVMInitializeAllTargetInfos();

	LLVMInitializeSPIRVTargetInfo();
	LLVMInitializeSPIRVTarget();
	LLVMInitializeSPIRVTargetMC();

	return ACLSPV_INIT_GLOBAL_OK;
}

ae2f_extern ACLSPV_ABI_IMPL
void	aclspv_stop_global(void) {

}

