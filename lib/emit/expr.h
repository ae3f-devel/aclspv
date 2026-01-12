/** @file expr.h */

#ifndef	lib_emit_expr_h
#define	lib_emit_expr_h

#include "aclspv/abi.h"
#include "util/id.h"
#include <complex.h>
#include <assert.h>

#include <aclspv.h>

#include <ae2f/Keys.h>
#include <ae2f/c90/StdInt.h>
#include <ae2f/c90/StdBool.h>
#include <ae2f/c90/Limits.h>

#include <clang-c/Index.h>
#include <clang-c/CXString.h>

#include <util/ctx.h>
#include <util/emitx.h>
#include <util/iddef.h>
#include <util/constant.h>
#include <util/cursor.h>
#include <util/fn.h>
#include <util/scale.h>

#include <spirv/unified1/spirv.h>


static enum CXChildVisitResult emit_expr(
		CXCursor h_cur,
		CXCursor h_parent,
		CXClientData h_ctx
		);

typedef enum EMIT_EXPR_ {

	EMIT_EXPR_FAILURE,

	/** succeed, value was not constant */
	EMIT_EXPR_SUCCESS,

	/** succeed, value was constant. */
	EMIT_EXPR_SUCCESS_CONSTANT,

	/** succeed, value was literal. */
	EMIT_EXPR_SUCCESS_LITERAL,

	EMIT_EXPR_NOT_THE_CASE
} e_emit_expr_bin_1_t;

typedef union {
	int		m_api_int;
	intmax_t	m_api_intmax;
	uintmax_t	m_api_uintmax;

	u64_least	m_u64;
	double		m_dbl;
} emit_expr_literal;

ae2f_inline static enum EMIT_EXPR_ emit_expr_arithmetic_cast(
		const aclspv_id_t	c_old_id,
		const aclspv_id_t	c_old_type,
		const aclspv_wrd_t	c_is_old_constant,
		const aclspv_wrd_t	c_is_old_literal,
		emit_expr_literal	c_old_literal,

		aclspv_id_t		c_new_id,
		aclspv_id_t		c_new_type,
		emit_expr_literal* ae2f_restrict wr_new_literal_opt,
		const h_util_ctx_t	h_ctx
		)
{
	assert(c_old_type);	assert(c_new_type);
	assert(c_old_id);	assert(c_new_id);
	assert(h_ctx);

	if(c_is_old_literal) {
		unless(util_default_is_number(c_old_type) && util_default_is_number(c_new_type))
			return EMIT_EXPR_NOT_THE_CASE;

		if(util_default_is_float(c_old_type) && util_default_is_int(c_new_type))
			c_old_literal.m_api_intmax = (imax)c_old_literal.m_dbl;

		if(util_default_is_int(c_old_type) && util_default_is_float(c_new_type))
			c_old_literal.m_dbl = (double)c_old_literal.m_api_intmax;

		if(wr_new_literal_opt) *wr_new_literal_opt = c_old_literal;

		if(util_default_bit_width(c_new_type) == 64) {
			ae2f_expected_but_else(h_ctx->m_count.m_types
					= util_emitx_5(
						&h_ctx->m_section.m_types
						, h_ctx->m_count.m_types
						, SpvOpConstant
						, c_new_type
						, c_new_id
						, (aclspv_wrd_t)(c_old_literal.m_u64		& 0xFFFFFFFF)
						, (aclspv_wrd_t)(c_old_literal.m_u64 >> 32)	& 0xFFFFFFFF))
				return EMIT_EXPR_FAILURE;

			return EMIT_EXPR_SUCCESS_LITERAL;
		}

		ae2f_expected_but_else(h_ctx->m_count.m_types
				= util_emitx_4(
					&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types
					, SpvOpConstant
					, c_new_type
					, c_new_id
					, (aclspv_wrd_t)(c_old_literal.m_u64
						& ((1ul << util_default_bit_width(c_new_type)) - 1ul))
					))
			return EMIT_EXPR_FAILURE;

		return EMIT_EXPR_SUCCESS_LITERAL;
	}

	if(util_default_is_int(c_old_type) && util_default_is_int(c_new_type)) {
		if(c_is_old_constant) {
			ae2f_expected_but_else(h_ctx->m_count.m_types
					= util_emitx_5(
						&h_ctx->m_section.m_types
						, h_ctx->m_count.m_types
						, SpvOpSpecConstantOp
						, c_new_type
						, c_new_id
						, SpvOpSConvert
						, c_old_id))
				return EMIT_EXPR_FAILURE;

			return EMIT_EXPR_SUCCESS_CONSTANT;
		} else {
			ae2f_expected_but_else(h_ctx->m_count.m_fnimpl
					= util_emitx_4(
						&h_ctx->m_section.m_fnimpl
						, h_ctx->m_count.m_fnimpl
						, SpvOpSConvert
						, c_new_type
						, c_new_id
						, c_old_id
						)
					) return EMIT_EXPR_FAILURE;
			return EMIT_EXPR_SUCCESS;
		}
	}

	if (util_default_is_float(c_old_type) && util_default_is_float(c_new_type)) {
		if(c_is_old_constant) {
			ae2f_expected_but_else(h_ctx->m_count.m_types
					= util_emitx_5(
						&h_ctx->m_section.m_types
						, h_ctx->m_count.m_types
						, SpvOpSpecConstantOp
						, c_new_type
						, c_new_id
						, SpvOpFConvert
						, c_old_id))
				return EMIT_EXPR_FAILURE;

			return EMIT_EXPR_SUCCESS_CONSTANT;
		} else {
			ae2f_expected_but_else(h_ctx->m_count.m_fnimpl
					= util_emitx_4(
						&h_ctx->m_section.m_fnimpl
						, h_ctx->m_count.m_fnimpl
						, SpvOpFConvert
						, c_new_type
						, c_new_id
						, c_old_id
						)
					) return EMIT_EXPR_FAILURE;
			return EMIT_EXPR_SUCCESS;
		}
	}

	/** TWO ARE DIFFERENT */
	if(util_default_is_float(c_new_type)) {
		ae2f_expected_but_else(h_ctx->m_count.m_fnimpl
				= util_emitx_4(
					&h_ctx->m_section.m_fnimpl
					, h_ctx->m_count.m_fnimpl
					, SpvOpConvertSToF 
					, c_new_type
					, c_new_id
					, c_old_id
					)
				) return EMIT_EXPR_FAILURE;

		return EMIT_EXPR_SUCCESS;
	}

	if(util_default_is_int(c_new_type)) {
		ae2f_expected_but_else(h_ctx->m_count.m_fnimpl
				= util_emitx_4(
					&h_ctx->m_section.m_fnimpl
					, h_ctx->m_count.m_fnimpl
					, SpvOpConvertFToS
					, c_new_type
					, c_new_id
					, c_old_id
					)
				) return EMIT_EXPR_FAILURE;

		return EMIT_EXPR_SUCCESS;
	}

	assert(0 && "NOT_THE_CASE");
	return EMIT_EXPR_NOT_THE_CASE;
}

/** 
 * binary expression with 1 operators.
 *
 * @param	c_id_scalar_for_cast
 * 		if casting is needed, specify the type via here
 * 		0 for no casting
 *
 * */
ae2f_inline static enum EMIT_EXPR_ emit_expr_bin_1(
		const CXCursor		c_cur,
		const h_util_ctx_t	h_ctx,
		const aclspv_id_t	c_newid,
		e_id_default* ae2f_restrict const	rdwr_type_req
		) {
	emit_expr_literal EVRES;
#define	TYPE_NEW_REQ	((*rdwr_type_req))

	ae2f_expected_but_else(h_ctx)
		return EMIT_EXPR_FAILURE;

	switch((uintmax_t)c_cur.kind) {
		/** unknown */
		default:
			return EMIT_EXPR_NOT_THE_CASE;

			/** literals */

		case CXCursor_IntegerLiteral:
		case CXCursor_FloatingLiteral:
			{
				CXEvalResult EVAL = clang_Cursor_Evaluate(c_cur);

				/** no cleanup needed. init already fucked. */
				ae2f_expected_but_else (EVAL) {
					assert(0 && "EVAL is not running");
					return EMIT_EXPR_FAILURE;
				}

				switch((uintmax_t)clang_EvalResult_getKind(EVAL)) {
					case CXEval_Int:
						EVRES.m_api_uintmax = clang_EvalResult_getAsUnsigned(EVAL);
						clang_EvalResult_dispose(EVAL);

						unless(TYPE_NEW_REQ) {
							ae2f_expected_but_else(util_mk_default_id(
										TYPE_NEW_REQ = 
										EVRES.m_api_uintmax <= 0xFFFFFFFF
										? ID_DEFAULT_U32 : ID_DEFAULT_U64
										, h_ctx))
								return EMIT_EXPR_FAILURE;	
						}

						return emit_expr_arithmetic_cast(
								1
								, EVRES.m_api_uintmax <= 0xFFFFFFFF 
								? ID_DEFAULT_U32 : ID_DEFAULT_U64
								, 0, 1, EVRES
								, c_newid, TYPE_NEW_REQ
								, ae2f_NIL
								, h_ctx);
						break;

					case CXEval_Float:
						EVRES.m_dbl = clang_EvalResult_getAsDouble(EVAL);
						clang_EvalResult_dispose(EVAL);

						unless(TYPE_NEW_REQ) {
							ae2f_expected_but_else(util_mk_default_id(
										TYPE_NEW_REQ = ID_DEFAULT_F32
										, h_ctx))
								return EMIT_EXPR_FAILURE;	
						}

						return emit_expr_arithmetic_cast(
								1, ID_DEFAULT_F64
								, 0, 1, EVRES
								, c_newid, TYPE_NEW_REQ
								, ae2f_NIL
								, h_ctx);
						break;
					default:
						assert(0 && "unknown evaluation");
						return EMIT_EXPR_FAILURE;
				}
			} assert(0); ae2f_unreachable();

		case CXCursor_DeclRefExpr:
			{
				const CXCursor DECL = clang_getCursorReferenced(c_cur);
				const aclspv_wrd_t	DECL_IDX = util_find_cursor(
						h_ctx->m_num_cursor
						, h_ctx->m_cursors.m_p
						, DECL
						);

				ae2f_expected_but_else(DECL_IDX < h_ctx->m_num_cursor)
					return EMIT_EXPR_FAILURE;

#define	DECL_INFO	DECL_IDX[((util_cursor* ae2f_restrict)h_ctx->m_cursors.m_p)]

				/** NaN is undefined behaviour here */
				unless(util_default_is_number(DECL_INFO.m_data.m_var_simple.m_type_id))
					return EMIT_EXPR_FAILURE;

				unless(TYPE_NEW_REQ) {
					TYPE_NEW_REQ = DECL_INFO.m_data.m_var_simple.m_type_id;
				}

				unless(util_default_is_number(TYPE_NEW_REQ))
					return EMIT_EXPR_FAILURE;

				if(TYPE_NEW_REQ == DECL_INFO.m_data.m_var_simple.m_type_id) {
					ae2f_expected_but_else(h_ctx->m_count.m_fnimpl = util_emitx_4(
								&h_ctx->m_section.m_fnimpl
								, h_ctx->m_count.m_fnimpl
								, SpvOpLoad
								, DECL_INFO.m_data.m_var_simple.m_type_id
								, c_newid
								, DECL_INFO.m_data.m_var_simple.m_id
								)) return EMIT_EXPR_FAILURE;

					return EMIT_EXPR_SUCCESS;
				}

				ae2f_expected_but_else(h_ctx->m_count.m_fnimpl = util_emitx_4(
							&h_ctx->m_section.m_fnimpl
							, h_ctx->m_count.m_fnimpl
							, SpvOpLoad
							, DECL_INFO.m_data.m_var_simple.m_type_id
							, h_ctx->m_id++
							, DECL_INFO.m_data.m_var_simple.m_id
							)) return EMIT_EXPR_FAILURE;

				return emit_expr_arithmetic_cast(
						h_ctx->m_id - 1
						, DECL_INFO.m_data.m_var_simple.m_type_id
						, 0, 0, EVRES
						, c_newid, TYPE_NEW_REQ
						, ae2f_NIL, h_ctx);

#undef	DECL_INFO
			}
	}

	assert(0 && "no way you came here");
	ae2f_unreachable();
}


#if 1
ae2f_inline static enum EMIT_EXPR_ emit_expr_bin_2(
		const CXCursor				c_cur,
		const h_util_ctx_t			h_ctx, 
		const aclspv_id_t			c_newid,
		const aclspv_id_t			c_result_type_id,
		x_aclspv_vec* ae2f_restrict const	h_cmdscale,
		aclspv_id_t				wr_operands[2]
		)
{

	x_scale* ae2f_restrict	NEW_SCALE;
	aclspv_wrd_t		OPCODE;
	ae2f_expected_but_else(h_ctx) {
		assert(0 && "emit_expr_bin_2::h_ctx is null");
		return EMIT_EXPR_FAILURE;
	}

	switch((umax)c_cur.kind) {
		default:
			return EMIT_EXPR_NOT_THE_CASE;

		case CXCursor_BinaryOperator:
		case CXCursor_CompoundAssignOperator:
			break;
	}

	switch((uintmax_t)clang_getCursorBinaryOperatorKind(c_cur)) {
		default:
			return EMIT_EXPR_NOT_THE_CASE;

		case CXBinaryOperator_Add:
			switch(c_result_type_id) {
				case 0:
				case ID_DEFAULT_U8:
				case ID_DEFAULT_U16:
				case ID_DEFAULT_U32:
				case ID_DEFAULT_U64:
				case ID_DEFAULT_I32:
					OPCODE = SpvOpIAdd;
					break;

				case ID_DEFAULT_F16:
				case ID_DEFAULT_F32:
				case ID_DEFAULT_F64:
					OPCODE = SpvOpFAdd;
					break;

				default:
					/** not supported */
					assert(0 && "no support::add");
					return EMIT_EXPR_FAILURE;
			} break;

		case CXBinaryOperator_Sub:
			switch(c_result_type_id) {
				case ID_DEFAULT_U8:
				case ID_DEFAULT_U16:
				case ID_DEFAULT_U32:
				case ID_DEFAULT_U64:
				case ID_DEFAULT_I32:
					OPCODE = SpvOpISub;
					break;

				case ID_DEFAULT_F16:
				case ID_DEFAULT_F32:
				case ID_DEFAULT_F64:
					OPCODE = SpvOpFSub;
					break;

				default:
					/** not supported */
					assert(0 && "no support::sub");
					return EMIT_EXPR_FAILURE;
			} break;

		case CXBinaryOperator_Mul:
			switch(c_result_type_id) {
				case ID_DEFAULT_U8:
				case ID_DEFAULT_U16:
				case ID_DEFAULT_U32:
				case ID_DEFAULT_U64:
				case ID_DEFAULT_I32:
					OPCODE = SpvOpIMul;
					break;

				case ID_DEFAULT_F16:
				case ID_DEFAULT_F32:
				case ID_DEFAULT_F64:
					OPCODE = SpvOpFMul;
					break;

				default:
					/** not supported */
					assert(0 && "no support::mul");
					return EMIT_EXPR_FAILURE;
			} break;

		case CXBinaryOperator_Div:
			switch(c_result_type_id) {
				case ID_DEFAULT_U8:
				case ID_DEFAULT_U16:
				case ID_DEFAULT_U32:
				case ID_DEFAULT_U64:
					OPCODE = SpvOpUDiv;
					break;

				case ID_DEFAULT_I32:
					OPCODE = SpvOpSDiv;
					break;

				case ID_DEFAULT_F16:
				case ID_DEFAULT_F32:
				case ID_DEFAULT_F64:
					OPCODE = SpvOpFDiv;
					break;

				default:
					/** not supported */
					assert(0 && "no support::div");
					return EMIT_EXPR_FAILURE;
			} break;

		case CXBinaryOperator_Rem:
			switch(c_result_type_id) {
				case ID_DEFAULT_U8:
				case ID_DEFAULT_U16:
				case ID_DEFAULT_U32:
				case ID_DEFAULT_U64:
					OPCODE = SpvOpUMod;
					break;

				case ID_DEFAULT_I32:
					OPCODE = SpvOpSRem;
					break;

				default:
					/** not supported */
					assert(0 && "no support::rem");
					return EMIT_EXPR_FAILURE;
			} break;
	}


	{
		aclspv_wrd_t* ae2f_restrict VEC;
		NEW_SCALE = mk_scale_from_vec(h_cmdscale, count_to_sz(8));
		ae2f_expected_but_else(NEW_SCALE)
			return EMIT_EXPR_FAILURE;

		VEC = get_buf_from_scale2(aclspv_wrd_t, h_cmdscale, *NEW_SCALE);

		ae2f_expected_but_else(VEC)
			return EMIT_EXPR_FAILURE;

		if(c_newid == h_ctx->m_id)
			++h_ctx->m_id;

		VEC[0] = 2;	/** number of operands */
		VEC[1] = 0;	/** is not constant? */
		VEC[2] = opcode_to_wrd(SpvOpSpecConstantOp, 5);
		VEC[3] = c_result_type_id;
		VEC[4] = c_newid;
		VEC[5] = OPCODE;
		VEC[6] = h_ctx->m_id++;
		VEC[7] = h_ctx->m_id++;

		if(wr_operands) {
			wr_operands[0] = VEC[6];
			wr_operands[1] = VEC[7];
		}
	}

	return EMIT_EXPR_SUCCESS_CONSTANT;
}

ae2f_inline static aclspv_id_t emit_expr_type(const CXType type, const h_util_ctx_t CTX)
{
	aclspv_id_t	TYPE_ID = 0;

	switch((uintmax_t)type.kind) {
		case CXType_Int:
		case CXType_UInt:
			TYPE_ID = util_mk_default_id(ID_DEFAULT_U32, CTX);
			break;

		case CXType_Short:
		case CXType_UShort:
			TYPE_ID = util_mk_default_id(ID_DEFAULT_U16, CTX);
			break;

		case CXType_SChar:
		case CXType_UChar:
			TYPE_ID = util_mk_default_id(ID_DEFAULT_U8, CTX);
			break;

		case CXType_Float:
			TYPE_ID = util_mk_default_id(ID_DEFAULT_F32, CTX);
			break;

		case CXType_Double:
			TYPE_ID = util_mk_default_id(ID_DEFAULT_F64, CTX);
			break;

		case CXType_Long:
		case CXType_ULong:
		case CXType_LongLong:
		case CXType_ULongLong:
			TYPE_ID = util_mk_default_id(ID_DEFAULT_U64, CTX);
			break;

		default:
			TYPE_ID = 0;
			break;
	}

	return TYPE_ID;
}
#endif

static enum CXChildVisitResult emit_expr(
		CXCursor h_cur,
		CXCursor h_parent,
		CXClientData rdwr_data
		)
{
#define	CTX		((h_util_ctx_t)(((uptr* ae2f_restrict)rdwr_data)[0]))
#define STCK_COUNT	((uptr* ae2f_restrict)(rdwr_data))[1]
#define ID_REQ		((aclspv_wrd_t)(((uptr* ae2f_restrict)(rdwr_data))[2]))
#define	IS_NOT_CONSTANT	((uptr* ae2f_restrict)(rdwr_data))[3]
#define	TYPE_REQUIRED	(((uptr* ae2f_restrict)(rdwr_data))[4])
#define	COUNTER_OPRND	(((uptr* ae2f_restrict)(rdwr_data))[5])

	/** for stack tmp.m_v0 will be used. */
#define	STCK		CTX->m_tmp.m_v0
#define	CMDSTCK_SCALE	CTX->m_tmp.m_v1
	h_scale_t LST_SCALE				= get_last_scale_from_vec(&CMDSTCK_SCALE);
	aclspv_wrd_t* ae2f_restrict LST_SCALE_BUF	= get_buf_from_scale2(aclspv_wrd_t, &CMDSTCK_SCALE, *LST_SCALE);
	aclspv_id_t ID_REQ_CURRENT			= ID_REQ;

	if(STCK_COUNT) {
		ae2f_expected_but_else(STCK.m_p) {
			CTX->m_err = ACLSPV_COMPILE_ALLOC_FAILED;
			assert(0 && "stack is acting funny");
			return CXChildVisit_Break;
		}

		ID_REQ_CURRENT = get_wrd_of_vec(&STCK)[STCK_COUNT - 1];
	}

	ae2f_expected_but_else(LST_SCALE_BUF) {
		CTX->m_err = ACLSPV_COMPILE_ALLOC_FAILED;
		return CXChildVisit_Break;
	}

	{
		const CXString SPELL		= clang_getCursorSpelling(h_cur);
		const CXString KINDSPELL	= clang_getCursorKindSpelling(h_cur.kind);
		printf("EMITEXPR(%u) STACK(%lu) LST_SCALE_BUF(%lu %u): ", ID_REQ_CURRENT, STCK_COUNT, LST_SCALE->m_id, LST_SCALE_BUF[0]);
		puts(SPELL.data);
		puts(KINDSPELL.data);
		clang_disposeString(SPELL);
		clang_disposeString(KINDSPELL);

#if 0
		return CXChildVisit_Recurse;
#endif
	}

	(void)h_parent;

	switch(emit_expr_bin_1(h_cur, CTX, ID_REQ_CURRENT, (e_id_default* ae2f_restrict)&TYPE_REQUIRED)) {
		case EMIT_EXPR_FAILURE:
			assert(0 && "bin_1 is fishy");
			return CXChildVisit_Break;

		case EMIT_EXPR_SUCCESS:
			IS_NOT_CONSTANT = 1;
			LST_SCALE_BUF[1] = 1;
			ae2f_fallthrough;

		case EMIT_EXPR_SUCCESS_LITERAL:
		case EMIT_EXPR_SUCCESS_CONSTANT:
			(void)(STCK_COUNT && --STCK_COUNT);
			++COUNTER_OPRND;
			while(LST_SCALE->m_id &&  COUNTER_OPRND == LST_SCALE_BUF[0]) {
				puts("SPOTTED!!");
				get_buf_from_scale2(aclspv_wrd_t, &CMDSTCK_SCALE, *get_prv_from_scale(&CMDSTCK_SCALE, *LST_SCALE))[1] 
					|= LST_SCALE_BUF[1];

				if(LST_SCALE_BUF[1]) {
#define	XORSWAP(a, b)	(a) ^= (b); (b) ^= (a); (a) ^= (b);
					/** 
					 * here, operation is not constant.
					 *
					 * current
					 * [2] OpSpecConstant opcode (ignorance required)
					 * [3] result type
					 * [4] result id
					 * [5] literal opcode (opspecconstant evaluation)
					 * ...
					 *
					 *
					 * we want
					 * [3] opcode with operand count
					 * [4] result type
					 * [5] result id
					 * */

					/** 
					 * swap [3] and [5]
					 *
					 * after this will
					 * [3] literal opcode (without operand)
					 * [4] result id
					 * [5] result type
					 * * */
					XORSWAP(LST_SCALE_BUF[3], LST_SCALE_BUF[5]);

					/** we have four operands (including opcode) */
					LST_SCALE_BUF[3] |= mk_noprnds(4);

					/**
					 * swap [4] and [5].
					 *
					 * after this will
					 * [3] opcode
					 * [4] result type
					 * [5] result id
					 *
					 * that matches what we want.
					 * */
					XORSWAP(LST_SCALE_BUF[4], LST_SCALE_BUF[5]);
#undef	XORSWAP
				}

#define		TMPL_SECTION	(*(IS_NOT_CONSTANT ? &CTX->m_section.m_fnimpl	: &CTX->m_section.m_types))
#define		TMPL_COUNT	(*(IS_NOT_CONSTANT ? &CTX->m_count.m_fnimpl	: &CTX->m_count.m_types))

				_aclspv_grow_vec_with_copy(
						_aclspv_malloc
						, _aclspv_free
						, memcpy
						, L_new
						, TMPL_SECTION
						, count_to_sz(TMPL_COUNT + sz_to_count(LST_SCALE->m_sz) - 2 - LST_SCALE_BUF[1])
						);

				ae2f_expected_but_else(TMPL_SECTION.m_p) {
					CTX->m_err = ACLSPV_COMPILE_ALLOC_FAILED;
					return CXChildVisit_Break;
				}

				memcpy(
						get_wrd_of_vec(&TMPL_SECTION) + TMPL_COUNT
						, LST_SCALE_BUF + 2 + LST_SCALE_BUF[1]
						, LST_SCALE->m_sz - count_to_sz(2 + (aclspv_wrd_t)LST_SCALE_BUF[1])
				      );

				TMPL_COUNT += sz_to_count(LST_SCALE->m_sz) - 2 - LST_SCALE_BUF[1];
#undef		TMPL_SECTION
#undef		TMPL_COUNT

				del_scale_from_vec_last(&CMDSTCK_SCALE);
				LST_SCALE = get_last_scale_from_vec(&CMDSTCK_SCALE);
				LST_SCALE_BUF = get_buf_from_scale2(aclspv_wrd_t, &CMDSTCK_SCALE, *LST_SCALE);

				COUNTER_OPRND	= 1;
				STCK_COUNT	-= LST_SCALE_BUF[0] - 1;
			} return CXChildVisit_Recurse;
		case EMIT_EXPR_NOT_THE_CASE:
			break;
		default:
			ae2f_unreachable();
	}

	{
		aclspv_wrd_t	OPRNDS[2];
		switch(emit_expr_bin_2(
					h_cur
					, CTX
					, ID_REQ_CURRENT
					, (aclspv_wrd_t)TYPE_REQUIRED
					, &CMDSTCK_SCALE, OPRNDS))
		{
			case EMIT_EXPR_FAILURE:
				assert(0 && "FAILURE_expr_bin2");
				return CXChildVisit_Break;

			case EMIT_EXPR_SUCCESS:
			case EMIT_EXPR_SUCCESS_CONSTANT:
			case EMIT_EXPR_SUCCESS_LITERAL:
				printf("OPERANDS %u %u\n", OPRNDS[0], OPRNDS[1]);

				ae2f_expected_but_else(STCK_COUNT = util_emit_wrd(
							&STCK, (aclspv_wrd_t)STCK_COUNT + 1
							, OPRNDS[0])) {
					CTX->m_err = ACLSPV_COMPILE_ALLOC_FAILED;
					return CXChildVisit_Break;
				}

				get_wrd_of_vec(&STCK)[STCK_COUNT - 2] = OPRNDS[1];


				ae2f_fallthrough;
			case EMIT_EXPR_NOT_THE_CASE:
				break;
			default:
				ae2f_unreachable();
		}
	}

	CTX->m_err = ACLSPV_COMPILE_OK;
	return CXChildVisit_Recurse;

#undef	CTX
#undef	STCK_COUNT
#undef	STCK
#undef	CMDSTCK_SCALE
#undef	ID_REQ
}

ae2f_inline static enum EMIT_EXPR_ emit_get_expr(const aclspv_id_t c_id_req, const aclspv_id_t c_type_req, const CXCursor c_cur, h_util_ctx_t h_ctx) {
	uptr BUF[6];
	aclspv_wrd_t* ae2f_restrict		SCALE_BOOT;

	BUF[0]		= (uptr)h_ctx;					/* context. must be pointer to context. */
	BUF[1]		= 0;						/* stack count. must be 0. */
	BUF[2]		= c_id_req;					/* result value id requested */
	BUF[3]		= 0;						/* is-not-constant. must be 0. */
	BUF[4]		= c_type_req ? c_type_req : ID_DEFAULT_U32;	/* type required */
	BUF[5]		= 0;						/* count for operand */

	puts("HERE IS YOUR CALL");

	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, h_ctx->m_tmp.m_v0, 100);
	ae2f_expected_but_else(h_ctx->m_tmp.m_v0.m_p)
		return EMIT_EXPR_FAILURE;

	ae2f_expected_but_else(init_scale(&h_ctx->m_tmp.m_v1, 2))
		return EMIT_EXPR_FAILURE;

	SCALE_BOOT = get_buf_from_scale2(aclspv_wrd_t, &h_ctx->m_tmp.m_v1, *get_last_scale_from_vec(&h_ctx->m_tmp.m_v1));
	assert(SCALE_BOOT);
	SCALE_BOOT[0] = 0;
	SCALE_BOOT[1] = 0;

	if(emit_expr(c_cur, c_cur, BUF) == CXChildVisit_Break)
		return EMIT_EXPR_FAILURE;

	ae2f_expected_but_else(h_ctx->m_tmp.m_v0.m_p)
		return EMIT_EXPR_FAILURE;

	ae2f_expected_but_else(h_ctx->m_tmp.m_v1.m_p)
		return EMIT_EXPR_FAILURE;

	clang_visitChildren(c_cur, emit_expr, BUF);

	return BUF[3] ? EMIT_EXPR_SUCCESS : EMIT_EXPR_SUCCESS_CONSTANT;
}

#endif
