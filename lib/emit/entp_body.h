#ifndef	lib_emit_entp_body_h
#define	lib_emit_entp_body_h

#include <complex.h>
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

#include <attr/specid.h>

#include <spirv/unified1/spirv.h>

static enum CXChildVisitResult emit_entp_body(CXCursor h_cur, CXCursor h_parent, CXClientData rdwr_data) {
#define	CTX		((h_util_ctx_t)rdwr_data)
#define	jmpfail(c_why)	{ (CTX)->m_state = (c_why); goto LBL_FAIL; }
	const enum CXCursorKind KIND	= (h_cur).kind;
	const CXString KINDSPELL	= clang_getCursorKindSpelling(KIND);
	const CXString SPELL		= clang_getCursorSpelling(h_cur);

	(void)h_parent;
	(void)rdwr_data;

	ae2f_expected_but_else(KIND)	{
		CTX->m_state = ACLSPV_COMPILE_MET_INVAL;
		goto LBL_FAIL;
	}

	switch((uintmax_t)KIND) {
		case CXCursor_ReturnStmt:
			ae2f_expected_but_else(CTX->m_count.m_fndef = emit_opcode(
						&CTX->m_section.m_fndef
						, CTX->m_count.m_fndef
						, SpvOpReturn, 0
						)) goto LBL_FAIL;
			CTX->m_has_function_ret = 1;
			goto LBL_DONE;

		case CXCursor_LabelStmt:
			util_tell_cursor_lbl(CTX->m_num_cursor, CTX->m_cursors.m_p);

			{
				const aclspv_wrd_t IDX = util_mk_cursor_base(
						CTX->m_num_cursor
						, &CTX->m_cursors
						, h_cur);


				ae2f_unexpected_but_if(CTX->m_num_cursor + 1 == IDX)
					goto LBL_FAIL;

				if(CTX->m_num_cursor == IDX) {
					((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[IDX]
						.m_data.m_goto_lbl.m_id = CTX->m_id++;
					++CTX->m_num_cursor;
				}

				ae2f_expected_but_else(CTX->m_count.m_fndef = util_emitx_2(
							&CTX->m_section.m_fndef
							, CTX->m_count.m_fndef
							, SpvOpLabel
							, ((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[IDX]
							.m_data.m_goto_lbl.m_id
							)) goto LBL_FAIL;
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

				ae2f_expected_but_else(CTX->m_count.m_fndef = util_emitx_2(
							&CTX->m_section.m_fndef
							, CTX->m_count.m_fndef
							, SpvOpBranch
							, ((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[IDX]
							.m_data.m_goto_lbl.m_id
							)) goto LBL_FAIL;
			}
			goto LBL_DONE;

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
						TYPE_ID = util_get_default_id(ID_DEFAULT_U32_PTR_FUNC, CTX);

						((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[IDX]
							.m_data.m_var_simple.m_fits_32bit = 1;

						ae2f_unexpected_but_if(0)
						{
							ae2f_unreachable();
							case CXType_LongLong:
							case CXType_ULongLong:
							case CXType_Long:
							case CXType_ULong:
							TYPE_ID = util_get_default_id(ID_DEFAULT_U64_PTR_FUNC, CTX);
						}

						((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[IDX]
							.m_data.m_var_simple.m_is_integer = 1;
						((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[IDX]
							.m_data.m_var_simple.m_fits_64bit = 1;

						break;

						ae2f_unexpected_but_if(0) {
							ae2f_unreachable();
							case CXType_BFloat16:
							case CXType_Float16:
							TYPE_ID = util_get_default_id(ID_DEFAULT_F16_PTR_FUNC, CTX);
						}

						ae2f_unexpected_but_if(0) {
							ae2f_unreachable();
							case CXType_Float:
							TYPE_ID = util_get_default_id(ID_DEFAULT_F32_PTR_FUNC, CTX);
						}

						((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[IDX]
							.m_data.m_var_simple.m_fits_32bit = 1;

						ae2f_unexpected_but_if(0) {
							ae2f_unreachable();
							case CXType_Double:
							TYPE_ID = util_get_default_id(ID_DEFAULT_F64_PTR_FUNC, CTX);
						}
						((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[IDX]
							.m_data.m_var_simple.m_is_undefined = 1;
						((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[IDX]
							.m_data.m_var_simple.m_fits_64bit = 1;

						break;

						/** complex types. we make it later. */
					default:
						{ 
							CXString __NAME = clang_getTypeSpelling(TYPE);
							printf("WHO THE HELL ARE YOU(%u) \n", TYPE.kind);
							puts(__NAME.data);
							clang_disposeString(__NAME);
						}
						break;
				}

				unless(CURSOR.m_data.m_var_simple.m_fits_64bit) {
					intmax_t	SIZEOF = clang_Type_getSizeOf(TYPE);
					aclspv_wrd_t	WRDCOUNT = SIZEOF > 0 && SIZEOF < UINT32_MAX
						? (((aclspv_wrd_t)SIZEOF + 3) >> 2) : 0;

					ae2f_expected_but_else(WRDCOUNT) jmpfail(ACLSPV_COMPILE_MET_INVAL);

					ae2f_expected_but_else(util_mk_constant_arr32_id(WRDCOUNT, CTX))
						goto LBL_FAIL;

					ae2f_expected_but_else(CTX->m_count.m_fndef = util_emitx_variable(
								&CTX->m_section.m_fndef
								, CTX->m_count.m_fndef
								, util_mk_constant_arr32_id(WRDCOUNT, CTX)
								, CTX->m_id, SpvStorageClassFunction))
						goto LBL_FAIL;

					CURSOR.m_data.m_var_simple.m_id = CTX->m_id++;

					/** TODO: variable initialiser */
					goto LBL_DONE;
				}

				ae2f_expected_but_else(TYPE_ID)
					goto LBL_FAIL;

				ae2f_expected_but_else(CTX->m_count.m_fndef = util_emitx_variable(
							&CTX->m_section.m_fndef
							, CTX->m_count.m_fndef
							, TYPE_ID
							, CTX->m_id
							, SpvStorageClassFunction))
					goto LBL_FAIL;

				CURSOR.m_data.m_var_simple.m_id = CTX->m_id++;
#undef	CURSOR

			}
			goto LBL_DONE;

			/** TODO: implement them */
		case CXCursor_DoStmt:
		case CXCursor_IfStmt:

		case CXCursor_ForStmt:
		case CXCursor_WhileStmt:
		case CXCursor_BreakStmt:
		case CXCursor_ContinueStmt:

		case CXCursor_SwitchStmt:
		case CXCursor_CaseStmt:
		case CXCursor_AsmStmt: case CXCursor_MSAsmStmt:

		case CXCursor_CompoundStmt:
		case CXCursor_CallExpr:
		case CXCursor_CStyleCastExpr:
		case CXCursor_StmtExpr:
		case CXCursor_BinaryOperator:
		case CXCursor_CompoundAssignOperator:
		default:
			break;
	}


	puts(SPELL.data);
	puts(KINDSPELL.data);
	puts("");

	CTX->m_state = ACLSPV_COMPILE_OK;
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
	CTX->m_state = ACLSPV_COMPILE_OK;
	clang_disposeString(SPELL);
	clang_disposeString(KINDSPELL);
	return CXChildVisit_Continue;

	ae2f_unreachable();
LBL_RECURSE:
	CTX->m_state = ACLSPV_COMPILE_OK;
	clang_disposeString(SPELL);
	clang_disposeString(KINDSPELL);
	return CXChildVisit_Recurse;

#undef	CTX
#undef	jmpfail
}

#endif
