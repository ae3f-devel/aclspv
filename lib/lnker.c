#include <assert.h>

#include <aclspv.h>
#include <aclspv/lnker.h>

#include <llvm-c/BitReader.h>
#include <llvm-c/Linker.h>

/** make string to zero */
#define ZSTR(a) 0

ACLSPV_ABI_IMPL 
e_aclspv_init_lnker
aclspv_init_lnker(h_aclspv_lnker_t i_lnk) {
	unless(i_lnk) {
		assert(ZSTR("ACLSPV_INIT_LNKER_ADDR_INVAL"));
		return ACLSPV_INIT_LNKER_ADDR_INVAL;
	}

	unless((i_lnk)->m_module = LLVMModuleCreateWithName("lnker")) {
		assert(ZSTR("ACLSPV_INIT_LNKER_FAILED"));
		return ACLSPV_INIT_LNKER_FAILED;	
	}

	LLVMSetTarget((i_lnk)->m_module, "spir64-unknown-unknown");

	/** Following clspv */
	LLVMSetDataLayout((i_lnk)->m_module,
			ACLSPV_OBJ_DATA_LAYOUT_DEFAULT
			);

	return ACLSPV_INIT_LNKER_OK;
}

ACLSPV_ABI_IMPL void
aclspv_stop_lnker(h_aclspv_lnker_t s_lnk) {
	unless(s_lnk) return;
	LLVMDisposeModule((s_lnk)->m_module);
}

ACLSPV_ABI_IMPL
	e_aclspv_add_obj_to_lnker
aclspv_add_obj_to_lnker(
		h_aclspv_lnker_t	h_lnk,
		const h_aclspv_obj_t* ae2f_restrict const	rd_hy_tars,
		const size_t					c_tars_len
		)
{
	size_t i;
	unless(h_lnk && h_lnk->m_module) {
		assert(ZSTR("ACLSPV_ADD_OBJ_TO_LNKER_HANDLE_INVAL"));
		return ACLSPV_ADD_OBJ_TO_LNKER_HANDLE_INVAL;
	}

	unless(rd_hy_tars && c_tars_len) {
		assert(ZSTR("ACLSPV_ADD_OBJ_TO_LNKER"));
		return ACLSPV_ADD_OBJ_TO_LNKER_ARG_NIL;
	}

	for(i = c_tars_len; i--;) {
#if 1
		if(LLVMLinkModules2(
					(h_lnk)->m_module
					, LLVMCloneModule(
						aclspv_get_module_from_obj(
							rd_hy_tars[i])
						)

				   ))
		{
			assert(ZSTR("ACLSPV_ADD_OBJ_TO_LNKER_FAILED"));
			break;
		}
#endif
	}

	return i == ae2f_static_cast(size_t, -1L) ? 
		ACLSPV_ADD_OBJ_TO_LNKER_OK : ACLSPV_ADD_OBJ_TO_LNKER_FAILED;
}
