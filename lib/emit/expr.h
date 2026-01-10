#ifndef	lib_emit_expr_h
#define	lib_emit_expr_h

#include <stdio.h>
#include <complex.h>
#include <assert.h>

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
#include <util/fn.h>

#include <spirv/unified1/spirv.h>

static aclspv_id_t emit_expr_type(CXType type, h_util_ctx_t CTX);

static enum CXChildVisitResult emit_expr(
		CXCursor h_cur,
		CXCursor h_parent,
		CXClientData h_ctx
		);

static aclspv_id_t emit_expr_binary(
		CXCursor h_cur,
		CXCursor h_parent,
		h_util_ctx_t CTX
		)
{
	aclspv_id_t	RESULT_ID = 0;
	aclspv_id_t	LHS_ID = 0;
	aclspv_id_t	RHS_ID = 0;
	CXType		LHS_TYPE, RHS_TYPE, RESULT_TYPE;
	CXCursor	LHS_CURSOR, RHS_CURSOR;
	CXString	OP_SPELLING;
	enum CXBinaryOperatorKind OP_KIND;

	OP_KIND = clang_getCursorBinaryOperatorKind(h_cur);
	LHS_CURSOR = clang_Cursor_getArgument(h_cur, 0);
	RHS_CURSOR = clang_Cursor_getArgument(h_cur, 1);

	LHS_TYPE = clang_getCanonicalType(clang_getCursorType(LHS_CURSOR));
	RHS_TYPE = clang_getCanonicalType(clang_getCursorType(RHS_CURSOR));
	RESULT_TYPE = clang_getCanonicalType(clang_getCursorType(h_cur));

	(void)h_parent;

	ae2f_expected_if(LHS_CURSOR.kind) {
		aclspv_wrdcount_t LHS_IDX = util_mk_cursor_base(CTX->m_num_cursor, &CTX->m_cursors, LHS_CURSOR);
		if(LHS_IDX < CTX->m_num_cursor) {
			clang_visitChildren(LHS_CURSOR, emit_expr, CTX);
			LHS_ID = ((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[LHS_IDX].m_data.m_var_simple.m_id;
		}
	}

	ae2f_expected_if(RHS_CURSOR.kind) {
		aclspv_wrdcount_t RHS_IDX = util_mk_cursor_base(CTX->m_num_cursor, &CTX->m_cursors, RHS_CURSOR);
		if(RHS_IDX < CTX->m_num_cursor) {
			clang_visitChildren(RHS_CURSOR, emit_expr, CTX);
			RHS_ID = ((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[RHS_IDX].m_data.m_var_simple.m_id;
		}
	}


	ae2f_expected_if(LHS_ID && RHS_ID) {
	} else {
		CTX->m_err = ACLSPV_COMPILE_MET_INVAL;
		return 0;
	}


	switch((uintmax_t)OP_KIND) {
		case CXBinaryOperator_Add: {
			enum SpvOp_ OPCODE;
			ae2f_expected_if(LHS_TYPE.kind == CXType_Float || LHS_TYPE.kind == CXType_Double
					|| RHS_TYPE.kind == CXType_Float || RHS_TYPE.kind == CXType_Double)
				OPCODE = SpvOpFAdd;
			else
				OPCODE = SpvOpIAdd;

			ae2f_expected_but_else(CTX->m_count.m_fndef = util_emitx_5(
						&CTX->m_section.m_fndef
						, CTX->m_count.m_fndef
						, OPCODE
						, emit_expr_type(RESULT_TYPE, CTX)
						, CTX->m_id
						, LHS_ID
						, RHS_ID
						)) return 0;
			RESULT_ID = CTX->m_id++;
			break;
		}

		case CXBinaryOperator_Sub: {
			enum SpvOp_ OPCODE;
			ae2f_expected_if(LHS_TYPE.kind == CXType_Float || LHS_TYPE.kind == CXType_Double
					|| RHS_TYPE.kind == CXType_Float || RHS_TYPE.kind == CXType_Double)
				OPCODE = SpvOpFSub;
			else
				OPCODE = SpvOpISub;

			ae2f_expected_but_else(CTX->m_count.m_fndef = util_emitx_5(
						&CTX->m_section.m_fndef
						, CTX->m_count.m_fndef
						, OPCODE
						, emit_expr_type(RESULT_TYPE, CTX)
						, CTX->m_id
						, LHS_ID
						, RHS_ID
						)) return 0;
			RESULT_ID = CTX->m_id++;
			break;
		}

		case CXBinaryOperator_Mul: {
			enum SpvOp_ OPCODE;
			ae2f_expected_if(LHS_TYPE.kind == CXType_Float || LHS_TYPE.kind == CXType_Double
					|| RHS_TYPE.kind == CXType_Float || RHS_TYPE.kind == CXType_Double)
				OPCODE = SpvOpFMul;
			else
				OPCODE = SpvOpIMul;

			ae2f_expected_but_else(CTX->m_count.m_fndef = util_emitx_5(
						&CTX->m_section.m_fndef
						, CTX->m_count.m_fndef
						, OPCODE
						, emit_expr_type(RESULT_TYPE, CTX)
						, CTX->m_id
						, LHS_ID
						, RHS_ID
						)) return 0;
			RESULT_ID = CTX->m_id++;
			break;
		}

		case CXBinaryOperator_Div: {
			enum SpvOp_ OPCODE;
			ae2f_expected_if(LHS_TYPE.kind == CXType_Float || LHS_TYPE.kind == CXType_Double
					|| RHS_TYPE.kind == CXType_Float || RHS_TYPE.kind == CXType_Double)
				OPCODE = SpvOpFDiv;
			else if (LHS_TYPE.kind == CXType_UInt || RHS_TYPE.kind == CXType_UInt)
				OPCODE = SpvOpUDiv;
			else
				OPCODE = SpvOpSDiv;

			ae2f_expected_but_else(CTX->m_count.m_fndef = util_emitx_5(
						&CTX->m_section.m_fndef
						, CTX->m_count.m_fndef
						, OPCODE
						, emit_expr_type(RESULT_TYPE, CTX)
						, CTX->m_id
						, LHS_ID
						, RHS_ID
						)) return 0;
			RESULT_ID = CTX->m_id++;
			break;
		}

		case CXBinaryOperator_Rem: {
			enum SpvOp_ OPCODE;
			ae2f_expected_if(LHS_TYPE.kind == CXType_Float || LHS_TYPE.kind == CXType_Double
					|| RHS_TYPE.kind == CXType_Float || RHS_TYPE.kind == CXType_Double)
				OPCODE = SpvOpFRem;
			else
				OPCODE = SpvOpSRem;

			ae2f_expected_but_else(CTX->m_count.m_fndef = util_emitx_5(
						&CTX->m_section.m_fndef
						, CTX->m_count.m_fndef
						, OPCODE
						, emit_expr_type(RESULT_TYPE, CTX)
						, CTX->m_id
						, LHS_ID
						, RHS_ID
						)) return 0;
			RESULT_ID = CTX->m_id++;
			break;
		}

		default: {
			OP_SPELLING = clang_getCursorSpelling(h_cur);
			printf("Unsupported binary operator: %s\n", (const char*)OP_SPELLING.data);
			clang_disposeString(OP_SPELLING);
			CTX->m_err = ACLSPV_COMPILE_MET_INVAL;
			return 0;
		}
	}

	CTX->m_err = ACLSPV_COMPILE_OK;
	return RESULT_ID;
}

static aclspv_id_t emit_expr_unary(
		CXCursor h_cur,
		CXCursor h_parent,
		h_util_ctx_t CTX
		)
{
	(void)h_cur;
	(void)h_parent;
	(void)CTX;

	CTX->m_err = ACLSPV_COMPILE_MET_INVAL;
	return 0;
}

static aclspv_id_t emit_expr_call(
		CXCursor h_cur,
		CXCursor h_parent,
		h_util_ctx_t CTX
		)
{
	CXCursor		CALLEE;
	CXString		CALLEE_NAME;
	int			NUM_ARGS;

	(void)h_parent;

	CALLEE = clang_getCursorReferenced(h_cur);
	CALLEE_NAME = clang_getCursorSpelling(CALLEE);
	NUM_ARGS = clang_Cursor_getNumArguments(h_cur);

	(void)NUM_ARGS;

	clang_disposeString(CALLEE_NAME);
	CTX->m_err = ACLSPV_COMPILE_MET_INVAL;
	return 0;
}

static aclspv_id_t emit_expr_array_subscript(
		CXCursor h_cur,
		CXCursor h_parent,
		h_util_ctx_t CTX
		)
{
	CXType			ELEMENT_TYPE;
	CXType			POINTER_TYPE;
	aclspv_id_t		RESULT_ID = 0;
	aclspv_id_t		BASE_ID = 0;
	aclspv_id_t		INDEX_ID = 0;
	CXCursor		BASE_CURSOR, INDEX_CURSOR;
	aclspv_wrdcount_t	BASE_IDX;
	aclspv_wrdcount_t	INDEX_IDX;

	(void)h_parent;

	BASE_CURSOR = clang_Cursor_getArgument(h_cur, 0);
	INDEX_CURSOR = clang_Cursor_getArgument(h_cur, 1);

	ae2f_expected_if(BASE_CURSOR.kind && INDEX_CURSOR.kind) {
		BASE_IDX = util_mk_cursor_base(CTX->m_num_cursor, &CTX->m_cursors, BASE_CURSOR);
		if(BASE_IDX < CTX->m_num_cursor) {
			clang_visitChildren(BASE_CURSOR, emit_expr, CTX);
			BASE_ID = ((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[BASE_IDX].m_data.m_var_simple.m_id;
		}

		INDEX_IDX = util_mk_cursor_base(CTX->m_num_cursor, &CTX->m_cursors, INDEX_CURSOR);
		if(INDEX_IDX < CTX->m_num_cursor) {
			clang_visitChildren(INDEX_CURSOR, emit_expr, CTX);
			INDEX_ID = ((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[INDEX_IDX].m_data.m_var_simple.m_id;
		}
	}

	ae2f_expected_if(BASE_ID && INDEX_ID) {
	} else {
		CTX->m_err = ACLSPV_COMPILE_MET_INVAL;
		return 0;
	}

	ELEMENT_TYPE = clang_getCanonicalType(clang_getCursorType(h_cur));
	POINTER_TYPE = clang_getPointeeType(ELEMENT_TYPE);

	ae2f_expected_but_else(CTX->m_count.m_fndef = util_emitx_5(
				&CTX->m_section.m_fndef
				, CTX->m_count.m_fndef
				, SpvOpAccessChain
				, emit_expr_type(POINTER_TYPE, CTX)
				, CTX->m_id
				, BASE_ID
				, INDEX_ID
				)) return 0;

	RESULT_ID = CTX->m_id++;

	CTX->m_err = ACLSPV_COMPILE_OK;
	return RESULT_ID;
}

static aclspv_id_t emit_expr_conditional(
		CXCursor h_cur,
		CXCursor h_parent,
		h_util_ctx_t CTX
		)
{
	(void)h_cur;
	(void)h_parent;
	(void)CTX;

	CTX->m_err = ACLSPV_COMPILE_MET_INVAL;
	return 0;
}

static aclspv_id_t emit_expr_cast(
		CXCursor h_cur,
		CXCursor h_parent,
		h_util_ctx_t CTX
		)
{
	(void)h_cur;
	(void)h_parent;
	(void)CTX;

	CTX->m_err = ACLSPV_COMPILE_MET_INVAL;
	return 0;
}

static aclspv_id_t emit_expr_decl_ref(
		CXCursor h_cur,
		CXCursor h_parent,
		h_util_ctx_t CTX
		)
{
	CXCursor		REFERENCED;
	aclspv_wrdcount_t	CURSOR_IDX;
	aclspv_id_t		VAR_ID = 0;

	(void)h_parent;

	REFERENCED = clang_getCursorReferenced(h_cur);

	if(REFERENCED.kind == CXCursor_ParmDecl || REFERENCED.kind == CXCursor_VarDecl) {
		CURSOR_IDX = util_find_cursor(CTX->m_num_cursor, CTX->m_cursors.m_p, REFERENCED);

		if(CURSOR_IDX < CTX->m_num_cursor) {
			VAR_ID = ((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[CURSOR_IDX].m_data.m_var_simple.m_id;
		} else {
			VAR_ID = 0;
		}
	}

	CTX->m_err = ACLSPV_COMPILE_OK;
	return VAR_ID;
}

static aclspv_id_t emit_expr_literal(
		CXCursor h_cur,
		CXCursor h_parent,
		h_util_ctx_t CTX
		)
{
	CXType			LITERAL_TYPE;
	intmax_t		LITERAL_VALUE;
	aclspv_id_t		CONST_ID = 0;

	(void)h_parent;

	LITERAL_TYPE = clang_getCursorType(h_cur);

	switch((uintmax_t)LITERAL_TYPE.kind) {
		case CXType_Int:
		case CXType_UInt:
		case CXType_Short:
		case CXType_UShort:
		case CXType_SChar:
		case CXType_UChar:
		case CXType_Long:
		case CXType_ULong:
		case CXType_LongLong:
		case CXType_ULongLong:
			LITERAL_VALUE = clang_Cursor_getNumArguments(h_cur);
			CONST_ID = util_mk_constant_val_id((aclspv_wrd_t)LITERAL_VALUE, CTX);
			break;

		case CXType_Float:
		case CXType_Double:
			CONST_ID = 0;
			break;

		default:
			CONST_ID = 0;
			CTX->m_err = ACLSPV_COMPILE_MET_INVAL;
			break;
	}

	CTX->m_err = ACLSPV_COMPILE_OK;
	return CONST_ID;
}

static aclspv_id_t emit_expr_type(CXType type, h_util_ctx_t CTX)
{
	aclspv_id_t	TYPE_ID = 0;

	switch((uintmax_t)type.kind) {
		case CXType_Int:
		case CXType_UInt:
			TYPE_ID = util_get_default_id(ID_DEFAULT_U32, CTX);
			break;

		case CXType_Short:
		case CXType_UShort:
			TYPE_ID = util_get_default_id(ID_DEFAULT_U16, CTX);
			break;

		case CXType_SChar:
		case CXType_UChar:
			TYPE_ID = util_get_default_id(ID_DEFAULT_U8, CTX);
			break;

		case CXType_Float:
			TYPE_ID = util_get_default_id(ID_DEFAULT_F32, CTX);
			break;

		case CXType_Double:
			TYPE_ID = util_get_default_id(ID_DEFAULT_F64, CTX);
			break;

		case CXType_Long:
		case CXType_ULong:
		case CXType_LongLong:
		case CXType_ULongLong:
			TYPE_ID = util_get_default_id(ID_DEFAULT_U64, CTX);
			break;

		default:
			TYPE_ID = 0;
			break;
	}

	(void)TYPE_ID;
	CTX->m_err = ACLSPV_COMPILE_OK;
	return TYPE_ID;
}

static enum CXChildVisitResult emit_expr(
		CXCursor h_cur,
		CXCursor h_parent,
		CXClientData h_ctx
		)
{
#define	CTX	((h_util_ctx_t)h_ctx)
	aclspv_id_t	EXPR_ID = 0;
	enum CXCursorKind	KIND = h_cur.kind;

	(void)h_parent;

	switch((uintmax_t)KIND) {
		case CXCursor_BinaryOperator:
		case CXCursor_CompoundAssignOperator:
			EXPR_ID = emit_expr_binary(h_cur, h_parent, CTX);
			break;

		case CXCursor_UnaryOperator:
			EXPR_ID = emit_expr_unary(h_cur, h_parent, CTX);
			break;

		case CXCursor_CallExpr:
			EXPR_ID = emit_expr_call(h_cur, h_parent, CTX);
			break;

		case CXCursor_ArraySubscriptExpr:
			EXPR_ID = emit_expr_array_subscript(h_cur, h_parent, CTX);
			break;

		case CXCursor_ConditionalOperator:
			EXPR_ID = emit_expr_conditional(h_cur, h_parent, CTX);
			break;

		case CXCursor_CStyleCastExpr:
			EXPR_ID = emit_expr_cast(h_cur, h_parent, CTX);
			break;

		case CXCursor_DeclRefExpr:
			EXPR_ID = emit_expr_decl_ref(h_cur, h_parent, CTX);
			break;

		case CXCursor_IntegerLiteral:
		case CXCursor_FloatingLiteral:
			EXPR_ID = emit_expr_literal(h_cur, h_parent, CTX);
			break;

		default:
			return CXChildVisit_Recurse;
		}

		unless(EXPR_ID) {
			aclspv_wrdcount_t CURSOR_IDX = util_find_cursor(
					CTX->m_num_cursor
					, CTX->m_cursors.m_p
					, h_cur
					);

			if(CURSOR_IDX < CTX->m_num_cursor) {
				((util_cursor* ae2f_restrict)CTX->m_cursors.m_p)[CURSOR_IDX].m_data.m_var_simple.m_id = EXPR_ID;
			}
			return CXChildVisit_Break;
		}

		CTX->m_err = ACLSPV_COMPILE_OK;
		return CXChildVisit_Continue;

#undef	CTX
}

#endif
