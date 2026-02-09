/** @file entp_body.h */

#ifndef	lib_emit_entp_body_h
#define	lib_emit_entp_body_h

#include <stdio.h>
#include <aclspv.h>

#include <ae2f/Keys.h>
#include <ae2f/c90/StdInt.h>
#include <ae2f/c90/StdBool.h>

#include <clang-c/Index.h>
#include <clang-c/CXString.h>

#include <util/ctx.h>
#include <util/emitx.h>
#include <util/iddef.h>
#include <util/constant.h>
#include <util/cursor.h>
#include <util/scale.h>
#include <util/u32_to_hex8.auto.h>

#include <attr/specid.h>

#include <spirv/unified1/spirv.h>

#include "./expr.h"
#include "aclspv/abi.h"
#include "aclspv/spvty.h"
#include "util/literal.h"


#if	!defined(NDEBUG) || !NDEBUG
#define	___mkname_on_dbg(c_ID)			{					\
	const aclspv_wrd_t POS	= CTX->m_count.m_name;				\
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free				\
			, CTX->m_tmp.m_v0, strlen(SPELL.data) + 10		\
			);							\
	ae2f_expected_but_else(CTX->m_tmp.m_v0.m_p)				\
	goto LBL_FAIL;								\
	((char* ae2f_restrict)CTX->m_tmp.m_v0.m_p)[8] = ':';			\
	((char* ae2f_restrict)CTX->m_tmp.m_v0.m_p)[9] = '\0';			\
	_util_u32_to_hex8((c_ID)			\
			, ((char* ae2f_restrict)CTX->m_tmp.m_v0.m_p));	\
	strcat(CTX->m_tmp.m_v0.m_p, SPELL.data);			\
	ae2f_expected_but_else((CTX->m_count.m_name = emit_opcode(	\
					&CTX->m_section.m_name		\
					, CTX->m_count.m_name	\
					, SpvOpName, 0))			\
					) goto LBL_FAIL;					\
	ae2f_expected_but_else((CTX->m_count.m_name = util_emit_wrd(		\
					&CTX->m_section.m_name		\
					, CTX->m_count.m_name	\
					, c_ID	\
					)))						\
	goto LBL_FAIL;								\
	ae2f_expected_but_else((CTX->m_count.m_name = util_emit_str(			\
					&CTX->m_section.m_name			\
					, CTX->m_count.m_name		\
					, CTX->m_tmp.m_v0.m_p 			\
					)))						\
	goto LBL_FAIL;								\
	set_oprnd_count_for_opcode(get_wrd_of_vec(					\
				&CTX->m_section.m_name)[POS]				\
				, CTX->m_count.m_name - POS - 1);				\
}
#else
#define	___mkname_on_dbg(a)
#endif

/**
 * @details
 * 		CTX->m_tmp.m_v0: name maker
 * */
static enum CXChildVisitResult emit_entp_body(CXCursor h_cur, CXCursor h_parent, CXClientData rdwr_data) {
#define	CTX		((h_util_ctx_t)rdwr_data)
#define	jmpfail(c_why)	{ (CTX)->m_err = (c_why); goto LBL_FAIL; }
	const enum CXCursorKind KIND	= (h_cur).kind;
	const CXString KINDSPELL	= clang_getCursorKindSpelling(KIND);
	const CXString SPELL		= clang_getCursorSpelling(h_cur);

	(void)h_parent;
	(void)rdwr_data;

	ae2f_expected_but_else(KIND)	{
		CTX->m_err = ACLSPV_COMPILE_MET_INVAL;
		goto LBL_FAIL;
	}

	switch((uintmax_t)KIND) {
		case CXCursor_VarDecl:
			{
				aclspv_wrdcount_t IDX = util_mk_cursor_base(
						CTX->m_num_cursor
						, &CTX->m_cursors
						, h_cur);

				aclspv_id_t	TYPE_ID = 0;

				const CXType	TYPE = clang_getCanonicalType(clang_getCursorType(h_cur));

				ae2f_unexpected_but_if(IDX == CTX->m_num_cursor + 1)
					goto LBL_FAIL;

#define	CURSOR	((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[IDX]

				if(IDX == CTX->m_num_cursor)
					++CTX->m_num_cursor;

				switch((uintmax_t)TYPE.kind) {
					case CXType_Int:
					case CXType_UInt:
					case CXType_Short:
					case CXType_UShort:
					case CXType_SChar:
					case CXType_UChar:
						TYPE_ID = util_mk_default_id(ID_DEFAULT_I32_PTR_FUNC, CTX);
						CURSOR.m_data.m_var_simple.m_type_id = ID_DEFAULT_I32;

						CURSOR.m_data.m_var_simple.m_fits_32bit = 1;

						ae2f_unexpected_but_if(0)
						{
							ae2f_unreachable();
							case CXType_LongLong:
							case CXType_ULongLong:
							case CXType_Long:
							case CXType_ULong:
							TYPE_ID = util_mk_default_id(ID_DEFAULT_I64_PTR_FUNC, CTX);
							CURSOR.m_data.m_var_simple.m_type_id = ID_DEFAULT_I64;
						}

						CURSOR.m_data.m_var_simple.m_is_integer = 1;
						CURSOR.m_data.m_var_simple.m_fits_64bit = 1;

						break;

						ae2f_unexpected_but_if(0) {
							ae2f_unreachable();
							case CXType_BFloat16:
							case CXType_Float16:
							TYPE_ID = util_mk_default_id(ID_DEFAULT_F16_PTR_FUNC, CTX);
							CURSOR.m_data.m_var_simple.m_type_id = ID_DEFAULT_F16;
						}

						ae2f_unexpected_but_if(0) {
							ae2f_unreachable();
							case CXType_Float:
							TYPE_ID = util_mk_default_id(ID_DEFAULT_F32_PTR_FUNC, CTX);
							CURSOR.m_data.m_var_simple.m_type_id = ID_DEFAULT_F32;
						}

						CURSOR.m_data.m_var_simple.m_fits_32bit = 1;

						ae2f_unexpected_but_if(0) {
							ae2f_unreachable();
							case CXType_Double:
							TYPE_ID = util_mk_default_id(ID_DEFAULT_F64_PTR_FUNC, CTX);
							CURSOR.m_data.m_var_simple.m_type_id = ID_DEFAULT_F64;
						}
						CURSOR.m_data.m_var_simple.m_fits_64bit = 1;
						CURSOR.m_data.m_var_simple.m_is_predictable = 0;

						break;

						/** complex types. we make it later. */

					case CXType_Record:
						{
							const intmax_t	_TY_SIZEOF = clang_Type_getSizeOf(TYPE);
							const uintmax_t	 TY_SIZEOF = _TY_SIZEOF < 0 ? 0 : (uintmax_t)_TY_SIZEOF;
							const aclspv_wrd_t	 TY_SIZE_WRD = TY_SIZEOF < UINT32_MAX 
								? (aclspv_wrd_t)TY_SIZEOF : 0;

							ae2f_expected_but_else(TY_SIZE_WRD)
								jmpfail(ACLSPV_COMPILE_MET_INVAL);

							ae2f_expected_but_else(TYPE_ID = util_mk_constant_ptr_func(
										(TY_SIZE_WRD + 3) >> 2
										, CTX))
								goto LBL_FAIL;
							CURSOR.m_data.m_var_simple.m_type_id = util_mk_constant_structpriv_id(
									(TY_SIZE_WRD + 3) >> 2, CTX
									);
						}
						break;

					default:
						{
							CXString __NAME = clang_getTypeSpelling(TYPE);
							err_prefix(a);
							err_call(
									fprintf, (
										stderr
										, "Met the unknown type(%u): %s\n"
										, TYPE.kind
										, (const char*)__NAME.data
										);
								)
								clang_disposeString(__NAME);
						}
						break;
				}

				ae2f_expected_but_else(TYPE_ID)
					goto LBL_FAIL;

				CURSOR.m_data.m_var_simple.m_ptr_type_id = TYPE_ID;
				CURSOR.m_data.m_var_simple.m_id = CTX->m_id++;
				CURSOR.m_data.m_var_simple.m_is_predictable = 0;
				CURSOR.m_data.m_var_simple.m_mask_literal = 0;

				/**
				 * TODO:
				 * So far I have defined the variable but not with the initialiser.
				 * Make initialising part if possible.
				 * */
				CURSOR.m_data.m_var_simple.m_is_predictable = 0;
				if(util_default_is_arithmetic(CURSOR.m_data.m_var_simple.m_type_id))
				{
					CXCursor	VAR_INIT = clang_Cursor_getVarDeclInitializer(h_cur);

#if 1
					aclspv_id_t	SPECID = 0xFFFFFFFF;

					clang_visitChildren(h_cur, attr_specid, &SPECID);

					if(SPECID != 0xFFFFFFFF) {
						/** specid! */
						util_constant* ae2f_restrict const NODE
							= util_mk_constant_node(SPECID, CTX);

#if 1 
						dbg_prefix(i);
						dbg_call(fprintf, (stderr, "Spec ID detected: %u\n", SPECID));
#endif

						if(NODE && NODE->m_const_spec_id) {
							dbg_prefix(a);
							dbg_call(fprintf, (stderr, "Spec ID definition spotted.: %u\n"
										, NODE->m_const_spec_id));

							switch(emit_expr_arithmetic_cast_non_literal(
										NODE->m_const_spec_id
										, NODE->m_const_spec_type_id
										, UTIL_LITERAL_CONSTANT
										, &CURSOR.m_data.m_var_simple.m_init_val
										, CURSOR.m_data.m_var_simple.m_type_id
										, CTX
										))
							{
								case EMIT_EXPR_SUCCESS_LITERAL:
								default:
									assert(0 && "unreachable & default");
									ae2f_unreachable();
									break;
								case EMIT_EXPR_NOT_THE_CASE:
									assert(0 && "not the case");
									ae2f_unreachable();
									break;

								case EMIT_EXPR_FAILURE:
									goto LBL_FAIL;
									ae2f_unexpected_but_if(0) {
										ae2f_unreachable();
										case EMIT_EXPR_SUCCESS:
										CURSOR.m_data.m_var_simple.m_mask_literal
											= UTIL_LITERAL_RT;
									}

									ae2f_unexpected_but_if(0) {
										ae2f_unreachable();
										case EMIT_EXPR_SUCCESS_CONSTANT:
										CURSOR.m_data.m_var_simple.m_mask_literal 
											= UTIL_LITERAL_CONSTANT;
									}

									CURSOR.m_data.m_var_simple.m_is_predictable = 1;
							}
						}


						goto LBL_DONE;
					}
#endif

					unless(clang_Cursor_isNull(VAR_INIT)) {
						dbg_puts(("Variable has constructor."));

						switch(emit_get_expr(
									&CURSOR.m_data.m_var_simple.m_init_val
									, CURSOR.m_data.m_var_simple.m_type_id
									, VAR_INIT
									, &CURSOR.m_data.m_var_simple.m_literal
									, CTX))
						{
							case EMIT_EXPR_NOT_THE_CASE:
								CTX->m_err = ACLSPV_COMPILE_NO_IMPL;
								break;

							default:
							case EMIT_EXPR_FAILURE:
								CTX->m_err = ACLSPV_COMPILE_MET_INVAL;
								break;

							case EMIT_EXPR_SUCCESS_LITERAL:
								CURSOR.m_data.m_var_simple.m_mask_literal
									|= UTIL_LITERAL_MASK_LITERAL;

								dbg_puts(("Constructor is literal."));
								ae2f_fallthrough;
							case EMIT_EXPR_SUCCESS_CONSTANT:

								dbg_puts(("Constructor may be constant."));
								CURSOR.m_data.m_var_simple.m_mask_literal 
									|= UTIL_LITERAL_CONSTANT;
								ae2f_fallthrough;
							case EMIT_EXPR_SUCCESS:
								CURSOR.m_data.m_var_simple.m_is_predictable = 1;
								break;
						}
					}
				}

#undef	CURSOR

			}
			goto LBL_DONE;

		case CXCursor_ReturnStmt:
			ae2f_expected_but_else(CTX->m_count.m_fnimpl = emit_opcode(
						&CTX->m_section.m_fnimpl
						, CTX->m_count.m_fnimpl
						, SpvOpReturn, 0
						)) goto LBL_FAIL;
			CTX->m_has_function_ret = 1;
			goto LBL_DONE;

		case CXCursor_LabelStmt:
			ae2f_unexpected_but_if(CTX->m_err = util_tell_cursor_lbl(
						CTX->m_num_cursor
						, CTX->m_cursors.m_p, CTX))
				goto LBL_FAIL;
			{

#define	CURSOR	((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[IDX]
				const aclspv_wrd_t IDX = util_mk_cursor_base(
						CTX->m_num_cursor
						, &CTX->m_cursors
						, h_cur);


				ae2f_unexpected_but_if(CTX->m_num_cursor + 1 == IDX)
					goto LBL_FAIL;

				if(CTX->m_num_cursor == IDX) {
					CURSOR.m_data.m_goto_lbl.m_id = CTX->m_id++;
					++CTX->m_num_cursor;
				}

				ae2f_expected_but_else(CTX->m_count.m_fnimpl = util_emitx_2(
							&CTX->m_section.m_fnimpl
							, CTX->m_count.m_fnimpl
							, SpvOpBranch
							, ((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[IDX]
							.m_data.m_goto_lbl.m_id
							)) goto LBL_FAIL;

				ae2f_expected_but_else(CTX->m_count.m_fnimpl = util_emitx_2(
							&CTX->m_section.m_fnimpl
							, CTX->m_count.m_fnimpl
							, SpvOpLabel
							, CURSOR.m_data.m_goto_lbl.m_id
							)) goto LBL_FAIL;

				___mkname_on_dbg(CURSOR.m_data.m_goto_lbl.m_id);
#undef	CURSOR
			} goto LBL_RECURSE;
		case CXCursor_GotoStmt:
			{
				CXCursor LBL_STMT = clang_getCursorReferenced(h_cur);
				aclspv_wrdcount_t IDX;
				assert(LBL_STMT.kind == CXCursor_LabelStmt);
				ae2f_expected_but_else(LBL_STMT.kind == CXCursor_LabelStmt)
					jmpfail(ACLSPV_COMPILE_MET_INVAL);

				IDX = util_mk_cursor_base(
						CTX->m_num_cursor
						, &CTX->m_cursors
						, LBL_STMT
						);

				ae2f_unexpected_but_if(CTX->m_num_cursor + 1 == IDX) goto LBL_FAIL;
				if(CTX->m_num_cursor == IDX) {
					((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[IDX].m_cursor
						= LBL_STMT;
					((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[IDX].m_data.m_goto_lbl.m_id
						= CTX->m_id++;

					++CTX->m_num_cursor;
				}

				ae2f_expected_but_else(CTX->m_count.m_fnimpl = util_emitx_2(
							&CTX->m_section.m_fnimpl
							, CTX->m_count.m_fnimpl
							, SpvOpBranch
							, ((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[IDX]
							.m_data.m_goto_lbl.m_id
							)) goto LBL_FAIL;

				/** just for branch */
				ae2f_expected_but_else(CTX->m_count.m_fnimpl = util_emitx_2(
							&CTX->m_section.m_fnimpl
							, CTX->m_count.m_fnimpl
							, SpvOpLabel
							, CTX->m_id++
							)) goto LBL_FAIL;
			}
			goto LBL_DONE;

			/** Literals in function scope does literally nothing in my opinion */
			goto LBL_DONE;


			/** 
			 * if this is surely just {},
			 * 	then it is just another block ready to be merged. 
			 * */
		case CXCursor_CompoundStmt:
			goto LBL_RECURSE;

		case CXCursor_IntegerLiteral:
		case CXCursor_BinaryOperator:
		case CXCursor_CompoundAssignOperator:
#if 1
			{
				const aclspv_wrdcount_t EXPR_IDX = util_mk_cursor_base(
						CTX->m_num_cursor
						, &CTX->m_cursors
						, h_cur
						);

				aclspv_id_t	CONDVALID = CTX->m_id++;

				ae2f_expected_but_else(EXPR_IDX != CTX->m_num_cursor + 1)
					goto LBL_FAIL;

				if(EXPR_IDX == CTX->m_num_cursor)
					++CTX->m_num_cursor;

				unless(emit_get_expr(
							&CONDVALID
							, 0
							, h_cur
							, 0
							, CTX))
					goto LBL_FAIL;
			}
			goto LBL_DONE;
#endif
		case CXCursor_FloatingLiteral:
		case CXCursor_UnaryOperator:
		case CXCursor_CallExpr:
		case CXCursor_ArraySubscriptExpr:
		case CXCursor_ConditionalOperator:
		case CXCursor_CStyleCastExpr:
		case CXCursor_DeclRefExpr:
		case CXCursor_UnaryExpr:
		case CXCursor_DoStmt:
		case CXCursor_IfStmt:

		case CXCursor_ForStmt:
		case CXCursor_WhileStmt:
		case CXCursor_BreakStmt:
		case CXCursor_ContinueStmt:

		case CXCursor_SwitchStmt:
		case CXCursor_CaseStmt:
		case CXCursor_AsmStmt: case CXCursor_MSAsmStmt:


		case CXCursor_StmtExpr:
		default:
			break;
	}


	warn_prefix(i);
	warn_call(fprintf, (stderr, "Met unprocessed spell(%s) in kind(%s) [NOIMPL]\n"
				, (const char*)SPELL.data
				, (const char*)KINDSPELL.data));

	CTX->m_err = ACLSPV_COMPILE_OK;
	clang_disposeString(SPELL);
	clang_disposeString(KINDSPELL);
	return CXChildVisit_Recurse;

	ae2f_unreachable();
LBL_FAIL:
	clang_disposeString(SPELL);
	clang_disposeString(KINDSPELL);
	return CXChildVisit_Break;

	ae2f_unreachable();
LBL_DONE:
	CTX->m_err = ACLSPV_COMPILE_OK;
	clang_disposeString(SPELL);
	clang_disposeString(KINDSPELL);
	return CXChildVisit_Continue;

	ae2f_unreachable();
LBL_RECURSE:
	CTX->m_err = ACLSPV_COMPILE_OK;
	clang_disposeString(SPELL);
	clang_disposeString(KINDSPELL);
	return CXChildVisit_Recurse;

#undef	CTX
#undef	jmpfail
#undef	___mkname_on_dbg
}

#endif
