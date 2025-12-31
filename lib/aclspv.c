#include <assert.h>
#include <string.h>

#include <aclspv.h>
#include <aclspv/abi.h>

#include <clang-c/CXErrorCode.h>
#include <clang-c/Index.h>

#include "./impl/asm.h"
#include "./impl/conf.h"

#include "./util/ctx.h"
#include "./util/entp.h"
#include "./util/fn.h"

#include "./emit/count_fn.h"
#include "./emit/decl_glob_obj.h"
#include "./emit/body.h"
#include "aclspv/spvty.h"
#include "util/id.h"
#include "util/iddef.h"


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
	init_scale(&CTX.m_scale_vars, 0);

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

	clang_visitChildren(CXROOTCUR, emit_count_fn, &CTX);
	if((STATE_VAL = CTX.m_state))
		goto LBL_CLEANUP;

	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX.m_fnlist.m_entp, (size_t)(sizeof(util_entp_t) * CTX.m_fnlist.m_num_entp));
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX.m_fnlist.m_fn, (size_t)(sizeof(lib_build_fn_t) * CTX.m_fnlist.m_num_fn));

	CTX.m_tmp.m_w3 = CTX.m_id; /** anchor */
	CTX.m_id += CTX.m_fnlist.m_num_entp + CTX.m_fnlist.m_num_fn;

	clang_visitChildren(CXROOTCUR, emit_decl_glob_obj, &CTX);

	{
		aclspv_wrdcount_t	IDX = CTX.m_fnlist.m_num_entp;
		const aclspv_wrd_t	ANCHOR = CTX.m_tmp.m_w3;

		util_get_default_id(ID_DEFAULT_VOID, &CTX);
		util_get_default_id(ID_DEFAULT_FN_VOID, &CTX);


		while(IDX--) {
			((util_entp_t* ae2f_restrict)CTX.m_fnlist.m_entp.m_p)[IDX].m_id = ANCHOR + IDX;
		}

		IDX = CTX.m_fnlist.m_num_entp;

		while(IDX--) {
			CTX.m_tmp.m_w0 = (ID_DEFAULT_FN_VOID);
			CTX.m_tmp.m_w1 = ((util_entp_t* ae2f_restrict)CTX.m_fnlist.m_entp.m_p)[IDX].m_id;
			CTX.m_tmp.m_w2 = (ID_DEFAULT_VOID);
			clang_visitChildren(((util_entp_t* ae2f_restrict)CTX.m_fnlist.m_entp.m_p)[IDX].m_fn, emit_body, &CTX);
		}
	}


	if((STATE_VAL = CTX.m_state))
		goto LBL_CLEANUP;


	if((STATE_VAL = impl_asm(&CTX)))
		goto LBL_CLEANUP;

LBL_CLEANUP:
	clang_disposeTranslationUnit(CXTU);
	_aclspv_stop_vec(_aclspv_free, CTX.m_constant_cache);
	_aclspv_stop_vec(_aclspv_free, CTX.m_vecid_vars);
	_aclspv_stop_vec(_aclspv_free, CTX.m_scale_vars);

	_aclspv_stop_vec(_aclspv_free, CTX.m_fnlist.m_entp);
	_aclspv_stop_vec(_aclspv_free, CTX.m_fnlist.m_fn);

	_aclspv_stop_vec(_aclspv_free, CTX.m_tmp.m_v0);

	_aclspv_stop_vec(_aclspv_free, CTX.m_section.m_capability);
	_aclspv_stop_vec(_aclspv_free, CTX.m_section.m_decorate);
	_aclspv_stop_vec(_aclspv_free, CTX.m_section.m_entp);
	_aclspv_stop_vec(_aclspv_free, CTX.m_section.m_fndef);
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


