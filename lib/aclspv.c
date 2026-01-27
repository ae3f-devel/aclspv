/** @file alcpsv.c */

#include <assert.h>
#include <string.h>

#include <aclspv.h>
#include <aclspv/abi.h>
#include <aclspv/spvty.h>

#include <clang-c/CXErrorCode.h>
#include <clang-c/Index.h>

#include "./impl/asm.h"
#include "./impl/conf.h"

#include "./util/ctx.h"
#include "./util/entp.h"
#include "./util/fn.h"
#include "./util/iddef.h"

#include "./emit/count_fn.h"
#include "./emit/decl_glob_obj.h"
#include "./emit/entp_body.h"


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
	CXTranslationUnit	CXTU = ae2f_NIL;
	CXCursor		CXROOTCUR;

	enum CXErrorCode	CXERR = CXError_Success;
	x_aclspv_ctx	CTX;

	unsigned	CXTU_IDX_ERR;
#define	STATE_VAL	CTX.m_err

	assert(rdwr_unsaved);
	assert(rwr_output);

	memset(&CTX, 0, sizeof(CTX));
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX.m_section.m_capability, (1 << 30));
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX.m_section.m_entp, (1 << 30));
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX.m_section.m_ext, (1 << 30));
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX.m_section.m_vars, (1 << 30));
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX.m_section.m_decorate, (1 << 30));
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX.m_section.m_execmode, (1 << 30));
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX.m_section.m_memmodel, (1 << 30));
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX.m_section.m_fndef, (1 << 30));
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX.m_section.m_types, (1 << 30));
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX.m_scale_vars, (1 << 30));
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX.m_tmp.m_v0, (1 << 30));
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX.m_tmp.m_v1, (1 << 30));

	init_scale(&CTX.m_scale_vars, 0);

	unless(ae2f_expected(CXIDX)) {
		CTX.m_err = ACLSPV_COMPILE_MET_INVAL;
		goto LBL_CLEANUP;
	}

	CXERR = clang_parseTranslationUnit2(
			CXIDX, ae2f_NIL
			, (const char**)rd_argv_opt
			, c_argc
			, rdwr_unsaved, c_unsaved_count
			, CXTranslationUnit_None, &CXTU
			);

	CXTU_IDX_ERR = clang_getNumDiagnostics(CXTU);
	while(ae2f_expected(CXTU_IDX_ERR--)) {
		const CXDiagnostic DIAG = clang_getDiagnostic(CXTU, CXTU_IDX_ERR);
		enum CXDiagnosticSeverity SEVERITY = clang_getDiagnosticSeverity(DIAG);

		CXString TXT = clang_formatDiagnostic(DIAG, clang_defaultDiagnosticDisplayOptions());

		puts(TXT.data);

		switch(SEVERITY) {
			case CXDiagnostic_Note:
				puts("NOTE");
				break;
			case CXDiagnostic_Error:
				puts("_ERR");
				break;
			case CXDiagnostic_Warning:
				puts("WARN");
				break;
			case CXDiagnostic_Ignored:
				puts("IGNR");
				break;
			case CXDiagnostic_Fatal:
				puts("FTAL");
				break;
			default:
				assert(0);
				ae2f_unreachable();
		}

		clang_disposeString(TXT);
	}

	if(ae2f_expected_not(CXERR)) {
		CTX.m_err = ACLSPV_COMPILE_ERR_CLANG;
		goto LBL_CLEANUP;
	}

	unless(ae2f_expected(CXTU)) {
		CTX.m_err = ACLSPV_COMPILE_MET_INVAL;
		goto LBL_CLEANUP;
	}

	if(ae2f_expected_not(STATE_VAL = impl_conf(&CTX)))
		goto LBL_CLEANUP;

	CXROOTCUR = clang_getTranslationUnitCursor(CXTU);

	clang_visitChildren(CXROOTCUR, emit_count_fn, &CTX);
	if(ae2f_expected_not(STATE_VAL = CTX.m_err))
		goto LBL_CLEANUP;

	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX.m_fnlist.m_entp, (size_t)(sizeof(util_entp_t) * CTX.m_fnlist.m_num_entp));
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX.m_fnlist.m_fn, (size_t)(sizeof(lib_build_fn_t) * CTX.m_fnlist.m_num_fn));

	CTX.m_tmp.m_w3 = CTX.m_id; /** anchor */
	CTX.m_id += CTX.m_fnlist.m_num_entp + CTX.m_fnlist.m_num_fn;

	clang_visitChildren(CXROOTCUR, emit_iter_entry_point, &CTX);

	CTX.m_tmp.m_w0 = 0;
	clang_visitChildren(CXROOTCUR, emit_decl_glob_obj, &CTX);

	{
		aclspv_wrdcount_t	IDX = CTX.m_fnlist.m_num_entp;
		const aclspv_wrd_t	ANCHOR = CTX.m_tmp.m_w3;

		util_mk_default_id(ID_DEFAULT_VOID, &CTX);
		util_mk_default_id(ID_DEFAULT_FN_VOID, &CTX);


		while((IDX--)) {
			((util_entp_t* ae2f_restrict)CTX.m_fnlist.m_entp.m_p)[IDX].m_id = ANCHOR + IDX;
		}

		IDX = CTX.m_fnlist.m_num_entp;
		while((IDX--)) {
			/**
			 * IDX must be w0 at the end
			 * w0: current entry point index
			 * v0: current variable table		(scale)
			 * */
#define	FNINFO	((util_entp_t* ae2f_restrict)CTX.m_fnlist.m_entp.m_p)[CTX.m_tmp.m_w0]
			x_scale* ae2f_restrict const	FNSCALE	= get_scale_from_vec(&CTX.m_scale_vars
					,  (size_t)(CTX.m_tmp.m_w0 = IDX)
					);

			CTX.m_count.m_fnimpl = 0;

			ae2f_expected_but_else(CTX.m_scale_vars.m_p && FNSCALE) {
				CTX.m_err = ACLSPV_COMPILE_MET_INVAL;
				goto LBL_CLEANUP;
			}


			CTX.m_err = ACLSPV_COMPILE_ALLOC_FAILED;
			CTX.m_tmp.m_w0 = IDX;


			ae2f_expected_but_else(CTX.m_count.m_fndef = util_emitx_5(
						&CTX.m_section.m_fndef
						, CTX.m_count.m_fndef
						, SpvOpFunction
						, ID_DEFAULT_VOID
						, FNINFO.m_id
						, 0
						, ID_DEFAULT_FN_VOID
						)) goto LBL_CLEANUP;

			ae2f_expected_but_else(CTX.m_count.m_fndef = util_emitx_2(
						&CTX.m_section.m_fndef
						, CTX.m_count.m_fndef
						, SpvOpLabel
						, CTX.m_id++))
				goto LBL_CLEANUP;

			IDX = (aclspv_wrd_t)FNSCALE->m_sz / (size_t)sizeof(util_bind);
			CTX.m_num_cursor	= IDX;

			while(IDX--) {
				const util_bind* ae2f_restrict const BUFFER = get_buf_from_scale(&CTX.m_scale_vars, FNSCALE[0]);
				const aclspv_wrd_t CURSOR_IDX = util_mk_cursor_base(
						CTX.m_num_cursor
						, &CTX.m_cursors
						, BUFFER[IDX].m_unified.m_cursor
						);

				assert(CTX.m_scale_vars.m_p);
				assert(BUFFER);

				ae2f_unexpected_but_if(CURSOR_IDX == CTX.m_num_cursor + 1)
					goto LBL_CLEANUP;

				if(CURSOR_IDX == CTX.m_num_cursor)
					++CTX.m_num_cursor;

				CURSOR_IDX[(util_cursor* ae2f_restrict)CTX.m_cursors.m_p]
					.m_data.m_prm_decl.m_info = BUFFER[IDX];
			}
			IDX = CTX.m_tmp.m_w0;


#undef	FNINFO

			CTX.m_has_function_ret	= 0;
			clang_visitChildren(((util_entp_t* ae2f_restrict)CTX.m_fnlist.m_entp.m_p)[IDX].m_fn
					, emit_entp_body
					, &CTX);
			ae2f_unexpected_but_if(STATE_VAL) goto LBL_CLEANUP;

			if(CTX.m_count.m_fnimpl) {
				_aclspv_grow_vec_with_copy(_aclspv_malloc, _aclspv_free, _aclspv_memcpy, L_new
						, CTX.m_section.m_fndef
						, (size_t)count_to_sz(CTX.m_count.m_fndef + CTX.m_count.m_fnimpl)
						);

				ae2f_expected_but_else(CTX.m_section.m_fndef.m_p) {
					CTX.m_err = ACLSPV_COMPILE_ALLOC_FAILED;
					goto LBL_CLEANUP;
				}

				memcpy(
						get_wrd_of_vec(&CTX.m_section.m_fndef) + CTX.m_count.m_fndef
						, CTX.m_section.m_fnimpl.m_p
						, (size_t)count_to_sz(CTX.m_count.m_fnimpl)
						);

				CTX.m_count.m_fndef += CTX.m_count.m_fnimpl;
			}

			unless(CTX.m_has_function_ret) {
				ae2f_expected_but_else(CTX.m_count.m_fndef = emit_opcode(
							&CTX.m_section.m_fndef
							, CTX.m_count.m_fndef
							, SpvOpReturn, 0
							)) {
					CTX.m_err = ACLSPV_COMPILE_ALLOC_FAILED;
					goto LBL_CLEANUP;
				}
			}

			ae2f_expected_but_else(CTX.m_count.m_fndef = emit_opcode(
						&CTX.m_section.m_fndef
						, CTX.m_count.m_fndef
						, SpvOpFunctionEnd, 0
						)) {
				CTX.m_err = ACLSPV_COMPILE_ALLOC_FAILED;
				goto LBL_CLEANUP;
			}

			assert(IDX == CTX.m_tmp.m_w0);
			{ ae2f_assume(IDX == CTX.m_tmp.m_w0); }
		}
	}


	if(ae2f_expected_not(STATE_VAL = CTX.m_err))
		goto LBL_CLEANUP;


	if(ae2f_expected_not(STATE_VAL = impl_asm(&CTX)))
		goto LBL_CLEANUP;

LBL_CLEANUP:
	clang_disposeTranslationUnit(CXTU);
	_aclspv_stop_vec(_aclspv_free, CTX.m_constant_cache);
	_aclspv_stop_vec(_aclspv_free, CTX.m_type_uniques);
	_aclspv_stop_vec(_aclspv_free, CTX.m_cursors);
	_aclspv_stop_vec(_aclspv_free, CTX.m_scale_vars);

	_aclspv_stop_vec(_aclspv_free, CTX.m_fnlist.m_entp);
	_aclspv_stop_vec(_aclspv_free, CTX.m_fnlist.m_fn);

	_aclspv_stop_vec(_aclspv_free, CTX.m_tmp.m_v0);
	_aclspv_stop_vec(_aclspv_free, CTX.m_tmp.m_v1);
	_aclspv_stop_vec(_aclspv_free, CTX.m_tmp.m_v2);

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
	_aclspv_stop_vec(_aclspv_free, CTX.m_section.m_fnimpl);


	if(ae2f_expected(CXIDX)) clang_disposeIndex(CXIDX);
	if(ae2f_expected(rwr_cxerr_opt)) *rwr_cxerr_opt = CXERR;
	if((rwr_output)) *rwr_output = CTX.m_ret.m_p;
	else free(CTX.m_ret.m_p);

	if(ae2f_expected(rwr_output_count_opt)) *rwr_output_count_opt = CTX.m_num_ret;

	return CTX.m_err;
}
