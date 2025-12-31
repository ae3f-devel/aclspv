#include "util/is_kernel.h"
#include <ae2f/Keys.h>
#include <ae2f/c90/StdInt.h>
#include <ae2f/c90/StdBool.h>
#include <string.h>
#include <stdlib.h>

#include <clang-c/Index.h>
#include <clang-c/CXString.h>

#include <util/ctx.h>
#include <util/wrdemit.h>
#include <util/iddef.h>
#include <util/constant.h>

#include <spirv/unified1/spirv.h>

typedef struct {
	aclspv_id_t	m_fn_id;
	aclspv_id_t	m_entry_label_id;
	aclspv_id_t	m_return_label_id;
	aclspv_wrdcount_t	m_basic_block_count;
} emit_body_fn_ctx_t;

typedef enum {
	BODY_CTX_OK = 0,
	BODY_CTX_ALLOC_FAILED,
	BODY_CTX_UNSUPPORTED
} e_body_ctx_state;

typedef struct {
	e_body_ctx_state	m_state;
	emit_body_fn_ctx_t*	ae2f_restrict	m_fn_ctx;
	aclspv_id_t		m_result_id;
} emit_body_ctx_t;

static aclspv_id_t emit_type_from_clang(CXType clang_type, h_util_ctx_t h_ctx);
static aclspv_id_t emit_expr(CXCursor h_cur, h_util_ctx_t h_ctx);
static enum CXChildVisitResult emit_stmt(CXCursor h_cur, CXCursor h_parent, CXClientData rdwr_data);

static aclspv_id_t emit_expr_literal(CXCursor h_cur, h_util_ctx_t h_ctx) {
	CXString spell;
	const char* text;
	aclspv_id_t result_id;
	CXType type;
	int val;

	spell = clang_getCursorSpelling(h_cur);
	text = clang_getCString(spell);
	result_id = 0;
	type = clang_getCursorType(h_cur);

	if (type.kind == CXType_Int) {
		val = atoi(text);
		result_id = util_mk_constant_val_id((aclspv_wrd_t)val, h_ctx);
	}

	clang_disposeString(spell);
	return result_id;
}

static aclspv_id_t emit_expr_decl_ref(CXCursor h_cur, h_util_ctx_t h_ctx) {
	(void)h_cur;
	(void)h_ctx;
	return 0;
}

static aclspv_id_t emit_expr_unary(CXCursor h_cur, h_util_ctx_t h_ctx) {
	(void)h_cur;
	(void)h_ctx;
	return 0;
}

static aclspv_id_t emit_expr_binary(CXCursor h_cur, h_util_ctx_t h_ctx) {
	CXString op_str;
	const char* op;
	enum CXCursorKind kind;

	kind = clang_getCursorKind(h_cur);

	if (kind == CXCursor_BinaryOperator) {
		op_str = clang_getCursorSpelling(h_cur);
		op = clang_getCString(op_str);
		(void)op;
		clang_disposeString(op_str);
	}

	(void)h_ctx;
	return 0;
}

static aclspv_id_t emit_expr(CXCursor h_cur, h_util_ctx_t h_ctx) {
	enum CXCursorKind kind;

	kind = clang_getCursorKind(h_cur);

	switch((uintmax_t)kind) {
		case CXCursor_IntegerLiteral:
		case CXCursor_FloatingLiteral:
			return emit_expr_literal(h_cur, h_ctx);

		case CXCursor_DeclRefExpr:
			return emit_expr_decl_ref(h_cur, h_ctx);

		case CXCursor_UnaryOperator:
			return emit_expr_unary(h_cur, h_ctx);

		case CXCursor_BinaryOperator:
			return emit_expr_binary(h_cur, h_ctx);

		default:
			break;
	}

	return 0;
}

static enum CXChildVisitResult emit_stmt(CXCursor h_cur, CXCursor h_parent, CXClientData rdwr_data) {
	enum CXCursorKind kind;
	h_util_ctx_t h_ctx;

	(void)h_parent;
	h_ctx = (h_util_ctx_t)rdwr_data;
	kind = clang_getCursorKind(h_cur);

	switch((uintmax_t)kind) {
		case CXCursor_ReturnStmt: {
			aclspv_id_t value_id;
			value_id = 0;
			if (clang_Cursor_getNumArguments(h_cur) > 0) {
				value_id = emit_expr(h_cur, h_ctx);
			}
			if (value_id) {
				unless((h_ctx->m_count.m_fndef = emit_opcode(&h_ctx->m_section.m_fndef, h_ctx->m_count.m_fndef, SpvOpReturnValue, 1)))
					return CXChildVisit_Break;
				unless((h_ctx->m_count.m_fndef = emit_wrd(&h_ctx->m_section.m_fndef, h_ctx->m_count.m_fndef, value_id)))
					return CXChildVisit_Break;
			} else {
				unless((h_ctx->m_count.m_fndef = emit_opcode(&h_ctx->m_section.m_fndef, h_ctx->m_count.m_fndef, SpvOpReturn, 0)))
					return CXChildVisit_Break;
			}
			return CXChildVisit_Continue;
		}

		case CXCursor_VarDecl: {
			CXType type;
			aclspv_id_t type_id;
			aclspv_id_t var_id;

			type = clang_getCursorType(h_cur);
			type_id = emit_type_from_clang(type, h_ctx);

			unless(type_id) return CXChildVisit_Break;

			var_id = h_ctx->m_id++;

			unless((h_ctx->m_count.m_fndef = emit_opcode(&h_ctx->m_section.m_fndef, h_ctx->m_count.m_fndef, SpvOpVariable, 3)))
				return CXChildVisit_Break;
			unless((h_ctx->m_count.m_fndef = emit_wrd(&h_ctx->m_section.m_fndef, h_ctx->m_count.m_fndef, type_id)))
				return CXChildVisit_Break;
			unless((h_ctx->m_count.m_fndef = emit_wrd(&h_ctx->m_section.m_fndef, h_ctx->m_count.m_fndef, var_id)))
				return CXChildVisit_Break;
			unless((h_ctx->m_count.m_fndef = emit_wrd(&h_ctx->m_section.m_fndef, h_ctx->m_count.m_fndef, SpvStorageClassFunction)))
				return CXChildVisit_Break;

			return CXChildVisit_Continue;
		}

		case CXCursor_CompoundStmt:
			return CXChildVisit_Recurse;

		default:
			break;
	}

	return CXChildVisit_Continue;
}

static aclspv_id_t emit_type_from_clang(CXType clang_type, h_util_ctx_t h_ctx) {
	switch((uintmax_t)clang_type.kind) {
		case CXType_Void:
			return util_get_default_id(ID_DEFAULT_VOID, h_ctx);

		case CXType_Int:
			if (clang_Type_getSizeOf(clang_type) == 4)
				return util_get_default_id(ID_DEFAULT_U32, h_ctx);
			break;

		case CXType_Float:
			return util_get_default_id(ID_DEFAULT_F32, h_ctx);

		case CXType_Pointer:
			return util_mk_constant_ptr_storage_id(1, h_ctx);

		default:
			break;
	}

	return 0;
}

static enum CXChildVisitResult emit_body(CXCursor h_cur, CXCursor h_parent, CXClientData rdwr_data) {
#define	CTX	((h_util_ctx_t)rdwr_data)
	CXType ret_type;
	aclspv_id_t ret_type_id = CTX->m_tmp.m_w2;
	const aclspv_id_t fn_type_id = CTX->m_tmp.m_w0;
	const aclspv_id_t fn_id = CTX->m_tmp.m_w1;
	aclspv_id_t label_id;

	unless(h_cur.kind == CXCursor_CompoundStmt && h_parent.kind == CXCursor_FunctionDecl && util_is_kernel(h_parent))
		return CXChildVisit_Recurse;


	ret_type = clang_getCursorResultType(h_parent);
	ret_type_id = emit_type_from_clang(ret_type, CTX);

	if (ret_type.kind != CXType_Void && !ret_type_id)
		return CXChildVisit_Break;

	unless((CTX->m_count.m_fndef = emit_opcode(&CTX->m_section.m_fndef, CTX->m_count.m_fndef, SpvOpFunction, 4)))
		return CXChildVisit_Break;
	unless((CTX->m_count.m_fndef = emit_wrd(&CTX->m_section.m_fndef, CTX->m_count.m_fndef, ret_type_id)))
		return CXChildVisit_Break;
	unless((CTX->m_count.m_fndef = emit_wrd(&CTX->m_section.m_fndef, CTX->m_count.m_fndef, fn_id)))
		return CXChildVisit_Break;
	unless((CTX->m_count.m_fndef = emit_wrd(&CTX->m_section.m_fndef, CTX->m_count.m_fndef, 0)))
		return CXChildVisit_Break;
	unless((CTX->m_count.m_fndef = emit_wrd(&CTX->m_section.m_fndef, CTX->m_count.m_fndef, fn_type_id)))
		return CXChildVisit_Break;

	label_id = CTX->m_id++;
	unless((CTX->m_count.m_fndef = emit_opcode(&CTX->m_section.m_fndef, CTX->m_count.m_fndef, SpvOpLabel, 1)))
		return CXChildVisit_Break;
	unless((CTX->m_count.m_fndef = emit_wrd(&CTX->m_section.m_fndef, CTX->m_count.m_fndef, label_id)))
		return CXChildVisit_Break;

	clang_visitChildren(h_cur, emit_stmt, CTX);

	if (clang_getCursorResultType(h_parent).kind == CXType_Void) {
		unless((CTX->m_count.m_fndef = emit_opcode(&CTX->m_section.m_fndef, CTX->m_count.m_fndef, SpvOpReturn, 0)))
			return CXChildVisit_Break;
	}

	unless((CTX->m_count.m_fndef = emit_opcode(&CTX->m_section.m_fndef, CTX->m_count.m_fndef, SpvOpFunctionEnd, 0)))
		return CXChildVisit_Break;

#undef	CTX

	return CXChildVisit_Continue;
}
