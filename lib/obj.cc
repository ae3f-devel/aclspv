#include <aclspv/obj.h>

ae2f_extern ACLSPV_ABI_IMPL ae2f_ccconst
LLVMModuleRef	aclspv_get_module_from_obj(x_aclspv_obj* ae2f_restrict const hs) {
	return reinterpret_cast<LLVMModuleRef>(hs->m_act->takeModule().get());
}

ae2f_extern ACLSPV_ABI_IMPL
void	aclspv_free_obj(const h_aclspv_obj_t hs) {
	delete hs;
}
