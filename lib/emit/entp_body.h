#include <stdio.h>

#include <ae2f/Keys.h>
#include <ae2f/c90/StdInt.h>
#include <ae2f/c90/StdBool.h>

#include <clang-c/Index.h>
#include <clang-c/CXString.h>

#include <util/ctx.h>
#include <util/wrdemit.h>
#include <util/iddef.h>
#include <util/constant.h>

#include <spirv/unified1/spirv.h>

static enum CXChildVisitResult emit_entp_body(CXCursor h_cur, CXCursor h_parent, CXClientData rdwr_data) {
#define	CTX	((h_util_ctx_t)rdwr_data)
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
		default:	break;
		case CXCursor_ReturnStmt:
				ae2f_expected_but_else(CTX->m_count.m_fndef = emit_opcode(
							&CTX->m_section.m_fndef
							, CTX->m_count.m_fndef
							, SpvOpReturn, 0
							)) goto LBL_FAIL;
				return CXChildVisit_Continue;
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
#undef	CTX
}
