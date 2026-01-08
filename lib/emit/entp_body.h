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

#include <spirv/unified1/spirv.h>

static enum CXChildVisitResult emit_entp_body(CXCursor h_cur, CXCursor h_parent, CXClientData rdwr_data) {
#define	CTX		((h_util_ctx_t)rdwr_data)
#define	jmpfail(c_why)	(CTX)->m_state = (c_why); goto LBL_FAIL;
	const enum CXCursorKind KIND	= clang_getCursorKind(h_cur);
	const CXString KINDSPELL	= clang_getCursorKindSpelling(KIND);
	const CXString SPELL		= clang_getCursorSpelling(h_cur);

	(void)h_parent;
	(void)rdwr_data;

	ae2f_expected_but_else(KIND)	{
		CTX->m_state = ACLSPV_COMPILE_MET_INVAL;
		goto LBL_FAIL;
	}

	switch((uintmax_t)KIND) {
		/** No implementation */
		case CXCursor_GotoStmt:
			jmpfail(ACLSPV_COMPILE_NO_IMPL);

		case CXCursor_ReturnStmt:
			ae2f_expected_but_else(CTX->m_count.m_fndef = emit_opcode(
						&CTX->m_section.m_fndef
						, CTX->m_count.m_fndef
						, SpvOpReturn, 0
						)) goto LBL_FAIL;
			goto LBL_DONE;


		case CXCursor_LabelStmt:
			goto LBL_IGNORE;


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
		default:
			break;
	}

	switch((uintmax_t)KIND) {
		case CXCursor_CallExpr:
		case CXCursor_CStyleCastExpr:
		case CXCursor_StmtExpr:
		default:
			break;
	}

	switch((uintmax_t)KIND) {
		case CXCursor_VarDecl:
		default:
			break;
	}

	switch((uintmax_t)KIND) {
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
	return CXChildVisit_Continue;

	ae2f_unreachable();
LBL_FAIL:
	clang_disposeString(SPELL);
	clang_disposeString(KINDSPELL);
	return CXChildVisit_Break;

	ae2f_unreachable();
LBL_IGNORE:
LBL_DONE:
	clang_disposeString(SPELL);
	clang_disposeString(KINDSPELL);
	return CXChildVisit_Continue;

#undef	CTX
#undef	jmpfail
}
