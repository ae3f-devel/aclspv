#include <aclspv.h>
#include <aclspv/abi.h>

#include <clang-c/CXErrorCode.h>
#include <clang-c/Index.h>

#include "./impl/asm.h"
#include "./impl/conf.h"

#include "./util/ctx.h"

#include <assert.h>
#include <string.h>

#include "./iter/root.h"


/**
 * @fn	aclspv_compile
 * @brief	compile files and generate spir-v for vulkan
 * @returns	<INIT:aclspv_free_obj>
 * */
ACLSPV_ABI_DECL e_aclspv_compile_t 
aclspv_compile(
		struct CXUnsavedFile* ae2f_restrict const 	rdwr_unsaved,
		const unsigned					c_unsaved_count,
		const char* ae2f_restrict const * ae2f_restrict const	rd_argv_opt,
		const int						c_argc,
		aclspv_wrdcount_t* ae2f_restrict			rwr_output_count_opt,
		aclspv_wrd_t* ae2f_restrict* ae2f_restrict		rwr_output,
		enum CXErrorCode* ae2f_restrict				rwr_cxerr_opt
	      ) 
{
	const CXIndex	CXIDX =	clang_createIndex(0, 0);
	e_aclspv_compile_t STATE_VAL = ACLSPV_COMPILE_OK;
	CXTranslationUnit	CXTU = ae2f_NIL;
	CXCursor		CXROOTCUR;

	enum CXErrorCode	CXERR = CXError_Success;
	x_aclspv_ctx	CTX;


	assert(rdwr_unsaved);
	assert(rwr_output);

	memset(&CTX, 0, sizeof(CTX));

	unless(CXIDX) {
		STATE_VAL = ACLSPV_COMPILE_MET_INVAL;
		goto LBL_CLEANUP;
	}

	CXERR = clang_parseTranslationUnit2(
			CXIDX, ae2f_NIL
			, (const char**)rd_argv_opt
			, c_argc
			, rdwr_unsaved, c_unsaved_count
			, CXTranslationUnit_None, &CXTU
			);

	if(CXERR) {
		STATE_VAL = ACLSPV_COMPILE_ERR_CLANG;
		goto LBL_CLEANUP;
	}

	unless(CXTU) {
		STATE_VAL = ACLSPV_COMPILE_MET_INVAL;
		goto LBL_CLEANUP;
	}

	if((STATE_VAL = impl_conf(&CTX)))
		goto LBL_CLEANUP;

	CXROOTCUR = clang_getTranslationUnitCursor(CXTU);

	clang_visitChildren(CXROOTCUR, iter_root, &CTX);

	if((STATE_VAL = impl_asm(&CTX)))
		goto LBL_CLEANUP;

LBL_CLEANUP:

	clang_disposeTranslationUnit(CXTU);
	_aclspv_stop_vec(_aclspv_free, CTX.m_constant_cache);
	_aclspv_stop_vec(_aclspv_free, CTX.m_scale_vars);
	_aclspv_stop_vec(_aclspv_free, CTX.m_vecid_vars);

	_aclspv_stop_vec(_aclspv_free, CTX.m_fnlist.m_entp);
	_aclspv_stop_vec(_aclspv_free, CTX.m_fnlist.m_fn);

	_aclspv_stop_vec(_aclspv_free, CTX.m_tmp.m_v0);

	_aclspv_stop_vec(_aclspv_free, CTX.m_section.m_capability);
	_aclspv_stop_vec(_aclspv_free, CTX.m_section.m_decorate);
	_aclspv_stop_vec(_aclspv_free, CTX.m_section.m_entp);
	_aclspv_stop_vec(_aclspv_free, CTX.m_section.m_entpdef);
	_aclspv_stop_vec(_aclspv_free, CTX.m_section.m_execmode);
	_aclspv_stop_vec(_aclspv_free, CTX.m_section.m_ext);
	_aclspv_stop_vec(_aclspv_free, CTX.m_section.m_memmodel);
	_aclspv_stop_vec(_aclspv_free, CTX.m_section.m_name);
	_aclspv_stop_vec(_aclspv_free, CTX.m_section.m_types);
	_aclspv_stop_vec(_aclspv_free, CTX.m_section.m_vars);

	if(CXIDX) clang_disposeIndex(CXIDX);
	if(rwr_cxerr_opt) *rwr_cxerr_opt = CXERR;
	if(rwr_output) *rwr_output = CTX.m_ret.m_p;
	else free(CTX.m_ret.m_p);

	if(rwr_output_count_opt) *rwr_output_count_opt = CTX.m_retcount;
	
	return STATE_VAL;
}


