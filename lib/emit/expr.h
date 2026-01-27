/** @file expr.h */

#ifndef	lib_emit_expr_h
#define	lib_emit_expr_h

#include "aclspv/spvty.h"
#include "util/id.h"
#include "util/op.h"
#include <assert.h>

#include <aclspv.h>
#include <stdio.h>

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
#include <util/literal.h>
#include <util/log.h>

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

typedef util_literal emit_expr_literal;

ae2f_inline static enum EMIT_EXPR_ emit_expr_arithmetic_cast_non_literal(
		const aclspv_id_t	c_old_id,
		const aclspv_id_t	c_old_type,
		const B_util_literal_t	c_flag_literal,
		aclspv_id_t		c_new_id,
		aclspv_id_t		c_new_type,
		const h_util_ctx_t	h_ctx
		)
{
	assert(c_old_type);	assert(c_new_type);
	assert(c_old_id);	assert(c_new_id);
	assert(h_ctx);

	dbg_puts(("Arithmetic cast: non-literal"));
	if(util_default_is_int(c_old_type) && util_default_is_int(c_new_type)) {
		if(c_flag_literal & UTIL_LITERAL_CONSTANT) {
			if(c_old_type == c_new_type) {
				aclspv_wrd_t	NEGATE_1 = h_ctx->m_id++;

				ae2f_expected_but_else(h_ctx->m_count.m_types
						= util_emitx_5(
							&h_ctx->m_section.m_types
							, h_ctx->m_count.m_types
							, SpvOpSpecConstantOp
							, c_new_type
							, NEGATE_1
							, SpvOpSNegate
							, c_old_id))
					return EMIT_EXPR_FAILURE;

				ae2f_expected_but_else(h_ctx->m_count.m_types
						= util_emitx_5(
							&h_ctx->m_section.m_types
							, h_ctx->m_count.m_types
							, SpvOpSpecConstantOp
							, c_new_type
							, c_new_id
							, SpvOpSNegate
							, NEGATE_1))
					return EMIT_EXPR_FAILURE;

				return EMIT_EXPR_SUCCESS_CONSTANT;
			}

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
			if(c_new_type == c_old_type) {
				aclspv_wrd_t	NEGATE_1 = h_ctx->m_id++;
				ae2f_expected_but_else(h_ctx->m_count.m_fnimpl
						= util_emitx_4(
							&h_ctx->m_section.m_fnimpl
							, h_ctx->m_count.m_fnimpl
							, SpvOpSNegate
							, c_new_type
							, NEGATE_1
							, c_old_id
							)
						) return EMIT_EXPR_FAILURE;

				ae2f_expected_but_else(h_ctx->m_count.m_fnimpl
						= util_emitx_4(
							&h_ctx->m_section.m_fnimpl
							, h_ctx->m_count.m_fnimpl
							, SpvOpSNegate
							, c_new_type
							, c_new_id
							, NEGATE_1
							)
						) return EMIT_EXPR_FAILURE;

				return EMIT_EXPR_SUCCESS;
			}

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
		if(c_flag_literal & UTIL_LITERAL_CONSTANT) {
			if(c_old_type == c_new_type) {
				aclspv_wrd_t	NEGATE_1 = h_ctx->m_id++;

				ae2f_expected_but_else(h_ctx->m_count.m_types
						= util_emitx_5(
							&h_ctx->m_section.m_types
							, h_ctx->m_count.m_types
							, SpvOpSpecConstantOp
							, c_new_type
							, NEGATE_1
							, SpvOpFNegate
							, c_old_id))
					return EMIT_EXPR_FAILURE;

				ae2f_expected_but_else(h_ctx->m_count.m_types
						= util_emitx_5(
							&h_ctx->m_section.m_types
							, h_ctx->m_count.m_types
							, SpvOpSpecConstantOp
							, c_new_type
							, c_new_id
							, SpvOpFNegate
							, NEGATE_1))
					return EMIT_EXPR_FAILURE;

				return EMIT_EXPR_SUCCESS_CONSTANT;
			}

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
			if(c_new_type == c_old_type) {
				aclspv_wrd_t	NEGATE_1 = h_ctx->m_id++;
				ae2f_expected_but_else(h_ctx->m_count.m_fnimpl
						= util_emitx_4(
							&h_ctx->m_section.m_fnimpl
							, h_ctx->m_count.m_fnimpl
							, SpvOpFNegate
							, c_new_type
							, NEGATE_1
							, c_old_id
							)
						) return EMIT_EXPR_FAILURE;

				ae2f_expected_but_else(h_ctx->m_count.m_fnimpl
						= util_emitx_4(
							&h_ctx->m_section.m_fnimpl
							, h_ctx->m_count.m_fnimpl
							, SpvOpFNegate
							, c_new_type
							, c_new_id
							, NEGATE_1
							)
						) return EMIT_EXPR_FAILURE;
				return EMIT_EXPR_SUCCESS;
			}

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

ae2f_inline static enum EMIT_EXPR_ emit_expr_arithmetic_cast(
		const aclspv_id_t	c_old_id,
		const aclspv_id_t	c_old_type,
		const B_util_literal_t	c_flag_literal,
		const emit_expr_literal	c_old_literal,

		aclspv_id_t		c_new_id,
		aclspv_id_t		c_new_type,
		emit_expr_literal* ae2f_restrict wr_new_literal_opt,
		const h_util_ctx_t	h_ctx
		)
{
	emit_expr_literal	RESULT = c_old_literal;
	assert(c_old_type);	assert(c_new_type);
	assert(c_new_id);
	assert(h_ctx);

	unless(c_flag_literal & UTIL_LITERAL_LITERAL) 
		return emit_expr_arithmetic_cast_non_literal(
				c_old_id, c_old_type
				, c_flag_literal, c_new_id
				, c_new_type, h_ctx
				);

	dbg_prefix(a);
	dbg_call(ae2f_NONE, (fprintf(
					stderr
					, "Arithmetic cast: literal old[%u] new [%u]\n"
					, c_old_type, c_new_type
				    )));

	unless(util_default_is_number(c_old_type) && util_default_is_number(c_new_type))
		return EMIT_EXPR_NOT_THE_CASE;

	if(util_default_is_float(c_old_type) && util_default_is_int(c_new_type)) {
		dbg_puts(("Cast (float -> int)"));
		switch(util_default_bit_width(c_old_type)) {
			case 64:
				dbg_prefix();
				dbg_call(fprintf, (stderr, "Cast from literal(double): %f\n", c_old_literal.m_dbl));
				RESULT.m_api_intmax = (imax)c_old_literal.m_dbl;
				break;
			case 32:
				dbg_prefix();
				dbg_call(fprintf, (stderr, "Cast from literal(float): %f\n", c_old_literal.m_flt));
				RESULT.m_api_intmax = (imax)c_old_literal.m_flt;
				break;
			default:
				assert(0 && "unknown float bit-width");
				return EMIT_EXPR_FAILURE;
		}

		dbg_prefix();
		dbg_call(fprintf, (stderr, "Result literal(intmax): %ld\n", RESULT.m_api_intmax));
	}

	if(util_default_is_int(c_old_type) && util_default_is_float(c_new_type)) {
		dbg_prefix(a);
		dbg_call(ae2f_NONE, (fprintf(stderr, "Cast (int -> float): %ld\n", c_old_literal.m_api_intmax)));
		switch(util_default_bit_width(c_new_type)) {
			case 64:
				RESULT.m_dbl = (double)c_old_literal.m_api_intmax;
				dbg_prefix();
				dbg_call(fprintf, (stderr, "Literal result(double): %f\n", RESULT.m_dbl));
				break;

			case 32:
				RESULT.m_flt = (float)c_old_literal.m_api_intmax;
				dbg_prefix();
				dbg_call(fprintf, (stderr, "Literal result(float): %f\n", RESULT.m_flt));
				break;
			default:
				assert(0 && "unknown float bit-width");
				return EMIT_EXPR_FAILURE;
		}
	}

	if(util_default_is_float(c_old_type) && util_default_is_float(c_new_type) && c_old_type != c_new_type) {
		switch(util_default_bit_width(c_new_type)) {
			case 64:
				RESULT.m_dbl = c_old_literal.m_flt;
				break;
			case 32:
				RESULT.m_flt = (float)c_old_literal.m_dbl;
				break;
			default:
				assert(0 && "unknown float bit-width");
				return EMIT_EXPR_FAILURE;
		}
	}

	if(wr_new_literal_opt) *wr_new_literal_opt = RESULT;

	if(util_default_bit_width(c_new_type) == 64) {
		dbg_call(fprintf, (
					stderr
					, "Width is 64 %lu\n"
					, RESULT.m_u64
				  ));
		dbg_call(fprintf, (
					stderr
					, "Width is 64 %f\n"
					, RESULT.m_dbl
				  ));

		ae2f_expected_but_else(h_ctx->m_count.m_types
				= util_emitx_5(
					&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types
					, SpvOpConstant
					, c_new_type
					, c_new_id
					, (aclspv_wrd_t)(RESULT.m_api_intmax		& 0xFFFFFFFF)
					, (aclspv_wrd_t)(RESULT.m_api_intmax >> 32)	& 0xFFFFFFFF))
			return EMIT_EXPR_FAILURE;
		return EMIT_EXPR_SUCCESS_LITERAL;
	}


	dbg_prefix(a);
	dbg_call(fprintf, (
				stderr
				, "Width is 32 %u\n"
				, (aclspv_wrd_t)RESULT.m_api_intmax
			  ));
	dbg_call(fprintf, (
				stderr
				, "Width is 32 %f\n"
				, RESULT.m_flt
			  ));

	ae2f_expected_but_else(h_ctx->m_count.m_types = util_emitx_4(
				&h_ctx->m_section.m_types
				, h_ctx->m_count.m_types
				, SpvOpConstant
				, c_new_type
				, c_new_id
				, (aclspv_wrd_t)(RESULT.m_api_intmax)
				))
		return EMIT_EXPR_FAILURE;

	return EMIT_EXPR_SUCCESS_LITERAL;
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
		const CXCursor			c_cur,
		const h_util_ctx_t		h_ctx,
		const aclspv_id_t		c_newid,
		e_id_default* ae2f_restrict	wr_type_cast,
		e_id_default* ae2f_restrict	wr_type_fetched,
		util_literal* ae2f_restrict const	rwr_literal_opt
		) {
	emit_expr_literal EVRES;

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
						dbg_puts(("Literal: uintmax"));
						EVRES.m_api_uintmax = clang_EvalResult_getAsUnsigned(EVAL);
						clang_EvalResult_dispose(EVAL);



						dbg_prefix(i);
						dbg_call(fprintf, (stderr
									, "Value fetched: %lu\n"
									, EVRES.m_api_uintmax
								  )
							);

						*wr_type_fetched = *wr_type_cast  =  util_default_decide_cast(
								*wr_type_cast
								, 

								(EVRES.m_api_uintmax <= 0xFFFFFFFF 
									? ID_DEFAULT_I32 : ID_DEFAULT_I64)
								);

						assert(*wr_type_cast);
						assert(*wr_type_fetched);

						dbg_prefix(i);
						dbg_call(fprintf, (stderr
									, "Value of `wr_type_cast`: %u\n"
									, *wr_type_cast
								  )
							);

						ae2f_expected_but_else(util_mk_default_id(
									*wr_type_cast
									, h_ctx)) {
							err_prefix(a);
							err_call(fprintf, (stderr
										, "Default id fetch has failed. %u\n"
										, *wr_type_cast
									  )
								);
							return EMIT_EXPR_FAILURE;
						}

						return emit_expr_arithmetic_cast(
								1
								, (EVRES.m_api_uintmax <= 0xFFFFFFFF 
									? ID_DEFAULT_I32 : ID_DEFAULT_I64)
								, UTIL_LITERAL_MASK_LITERAL
								, EVRES
								, c_newid, *wr_type_cast
								, rwr_literal_opt
								, h_ctx);

					case CXEval_Float:
						dbg_puts(("Literal: double (float casted)"));
						EVRES.m_dbl = clang_EvalResult_getAsDouble(EVAL);
						dbg_prefix(a);
						dbg_call(fprintf, (stderr, "Literal fetched: %f\n", EVRES.m_dbl));

						clang_EvalResult_dispose(EVAL);

						*wr_type_fetched = *wr_type_cast = util_default_decide_cast(
								*wr_type_cast
								, ID_DEFAULT_F32
								);

						assert(*wr_type_cast);
						assert(*wr_type_fetched);

						dbg_prefix(a);
						dbg_call(fprintf, (stderr, "wr_type_cast: %u\n", *wr_type_cast));

						ae2f_expected_but_else(util_mk_default_id(
									*wr_type_cast
									, h_ctx))
							return EMIT_EXPR_FAILURE;

						return emit_expr_arithmetic_cast(
								1, ID_DEFAULT_F64
								, UTIL_LITERAL_MASK_LITERAL
								, EVRES
								, c_newid
								, *wr_type_cast
								, rwr_literal_opt 
								, h_ctx);
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
				unless(util_default_is_number(DECL_INFO.m_data.m_var_simple.m_type_id)) {
					err_puts(("Arithmetic is no number [NOIMPL]"));
					return EMIT_EXPR_FAILURE;
				}

				*wr_type_fetched = DECL_INFO.m_data.m_var_simple.m_type_id;
				*wr_type_cast = util_default_decide_cast(
						*wr_type_cast
						, *wr_type_fetched 
						);

				if(DECL_INFO.m_data.m_var_simple.m_mask_literal & UTIL_LITERAL_LITERAL) {
					dbg_puts(("Variable is literal"));
					return emit_expr_arithmetic_cast(
							1, DECL_INFO.m_data.m_var_simple.m_type_id
							, UTIL_LITERAL_MASK_LITERAL
							, DECL_INFO.m_data.m_var_simple.m_literal
							, c_newid
							, *wr_type_fetched = *wr_type_cast
							, rwr_literal_opt
							, h_ctx);
				}

				if(DECL_INFO.m_data.m_var_simple.m_is_predictable) {
					dbg_puts(("Variable is predictable"));

					return emit_expr_arithmetic_cast_non_literal(
							DECL_INFO.m_data.m_var_simple.m_init_val
							, DECL_INFO.m_data.m_var_simple.m_type_id
							, DECL_INFO.m_data.m_var_simple.m_mask_literal
							, c_newid
							, *wr_type_fetched = *wr_type_cast
							, h_ctx);
				}

				dbg_puts(("Load from memory"));
				ae2f_expected_but_else(h_ctx->m_count.m_fnimpl = util_emitx_4(
							&h_ctx->m_section.m_fnimpl
							, h_ctx->m_count.m_fnimpl
							, SpvOpLoad
							, DECL_INFO.m_data.m_var_simple.m_type_id
							, c_newid
							, DECL_INFO.m_data.m_var_simple.m_id
							)) return EMIT_EXPR_FAILURE;

				return EMIT_EXPR_SUCCESS;
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
	enum UTIL_OPBIN2_	OPCODE;

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
			OPCODE = UTIL_OPBIN2_ADD;
			break;

		case CXBinaryOperator_Sub:
			OPCODE = UTIL_OPBIN2_SUB;
			break;

		case CXBinaryOperator_Mul:
			OPCODE = UTIL_OPBIN2_MUL;
			break;

		case CXBinaryOperator_Div:
			OPCODE = UTIL_OPBIN2_DIV;
			break;

		case CXBinaryOperator_Rem:
			OPCODE = UTIL_OPBIN2_REM;
			break;
	}

	{
		aclspv_wrd_t* ae2f_restrict VEC;

		dbg_puts(("==[ Scale Check"));

		NEW_SCALE = get_last_scale_from_vec(h_cmdscale);
		VEC = get_buf_from_scale2(aclspv_wrd_t, h_cmdscale, *NEW_SCALE);
		dbg_prefix(a);
		dbg_call(fprintf, (stderr, "\tNew scale 0: %u[%p]\n", VEC[1], (void*)(VEC + 1)));

		NEW_SCALE = mk_scale_from_vec(h_cmdscale, count_to_sz(8) + (sizeof(util_literal) << 1));
		ae2f_expected_but_else(NEW_SCALE)
			return EMIT_EXPR_FAILURE;

		VEC = get_buf_from_scale2(aclspv_wrd_t, h_cmdscale, *NEW_SCALE);
		VEC[1] = UTIL_LITERAL_MASK_LITERAL;

		dbg_prefix(a);
		dbg_call(fprintf, (stderr, "\tNew scale 1: %u[%p]\n", VEC[1], (void*)(VEC + 1)));

		dbg_prefix(a);
		dbg_call(fprintf, (stderr, "\tOld scale 1: %u[%p]\n"
					, get_buf_from_scale2(aclspv_wrd_t, h_cmdscale, *get_prv_from_scale(h_cmdscale, *NEW_SCALE))[1]
					, (void*)(get_buf_from_scale2(aclspv_wrd_t, h_cmdscale, *get_prv_from_scale(h_cmdscale, *NEW_SCALE)) + 1)
				  ));

		dbg_puts(("]== Scale Check"));

		ae2f_expected_but_else(VEC)
			return EMIT_EXPR_FAILURE;

		if(c_newid == h_ctx->m_id)
			++h_ctx->m_id;

		VEC[0] = 2;			/** number of operands */
		VEC[1] = UTIL_LITERAL_MASK_LITERAL;	/** is not constant? */
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

	return EMIT_EXPR_SUCCESS_LITERAL;
}

ae2f_inline static aclspv_id_t emit_expr_type(const CXType type, const h_util_ctx_t CTX)
{
	aclspv_id_t	TYPE_ID = 0;

	switch((uintmax_t)type.kind) {
		case CXType_Int:
		case CXType_UInt:
			TYPE_ID = util_mk_default_id(ID_DEFAULT_I32, CTX);
			break;

		case CXType_Short:
		case CXType_UShort:
			TYPE_ID = util_mk_default_id(ID_DEFAULT_I16, CTX);
			break;

		case CXType_SChar:
		case CXType_UChar:
			TYPE_ID = util_mk_default_id(ID_DEFAULT_I8, CTX);
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
			TYPE_ID = util_mk_default_id(ID_DEFAULT_I64, CTX);
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
#define	CTX			((h_util_ctx_t)(((uptr* ae2f_restrict)rdwr_data)[0]))
#define EXPRSTCK_COUNT		(((uptr* ae2f_restrict)(rdwr_data))[1])
#define ID_REQ			((aclspv_wrd_t)(((uptr* ae2f_restrict)(rdwr_data))[2]))
#define	IS_NOT_CONSTANT		((uptr* ae2f_restrict)(rdwr_data))[3]
#define	TYPE_CAST_CURRENT	(*(aclspv_wrd_t*)&(((uptr* ae2f_restrict)(rdwr_data))[4]))
#define	NUM_CMDCOUNTERSTCK	(((uptr* ae2f_restrict)(rdwr_data))[5])

	/** for stack tmp.m_v0 will be used. */
#define	EXPRSTCK	CTX->m_tmp.m_v0
#define	CMDSTCK_SCALE	CTX->m_tmp.m_v1
#define	CMDCOUNTERSTCK	CTX->m_tmp.m_v2
#define	LST_CMD_HEADER		4
#define	LST_CMD_COUNT		(LST_CMD_HEADER /** header */ + (LST_SCALE_BUF[0]))
#define	LST_CMDBUF_COUNT	(LST_CMD_HEADER /** header */ + (LST_SCALE_BUF[0] << 1))

	typedef struct {
		u32_least	m_id_req;
	} exprstck;

	typedef struct {
		u32_least	m_count_oprnd;
	} cmdcounterstckelm;

	char aclspv_STATICASSERT_ELMSIZE[sizeof(exprstck) == sizeof(cmdcounterstckelm) && sizeof(cmdcounterstckelm) == sizeof(u32_least)];

#define	BUF_EXPRSTCK(a)	((exprstck* ae2f_restrict)(EXPRSTCK).m_p)
#define	GETLASTELEXPRSTCK(a)	BUF_EXPRSTCK(c_stck)[EXPRSTCK_COUNT - 1]
#define	BUF_CMDCOUNTERSTCK(a)	((cmdcounterstckelm* ae2f_restrict)(CMDCOUNTERSTCK).m_p)
#define	GETLASTEL_CMDCOUNTERSTCK(a)	BUF_CMDCOUNTERSTCK()[NUM_CMDCOUNTERSTCK - 1]

	h_scale_t LST_SCALE				= get_last_scale_from_vec(&CMDSTCK_SCALE);
	aclspv_wrd_t* ae2f_restrict LST_SCALE_BUF	= get_buf_from_scale2(aclspv_wrd_t, &CMDSTCK_SCALE, *LST_SCALE);
	aclspv_id_t ID_REQ_CURRENT			= ID_REQ;

	util_literal* ae2f_restrict LST_LITERAL 
		= ((util_literal* ae2f_restrict)(void* ae2f_restrict)(&LST_SCALE_BUF[(LST_CMDBUF_COUNT + 2)]));

	util_literal* ae2f_restrict RET_LITERAL
		= (void*)(((uptr* ae2f_restrict)(rdwr_data)) + 6);
	aclspv_id_t* ae2f_restrict LST_TYPE_ORIGINAL
		= ((aclspv_id_t* ae2f_restrict)(void* ae2f_restrict)(LST_LITERAL)) - LST_SCALE_BUF[0];


	(void)aclspv_STATICASSERT_ELMSIZE;

	if(EXPRSTCK_COUNT) {
		ae2f_expected_but_else(EXPRSTCK.m_p) {
			CTX->m_err = ACLSPV_COMPILE_ALLOC_FAILED;
			assert(0 && "stack is acting funny");
			return CXChildVisit_Break;
		}

		ID_REQ_CURRENT = GETLASTELEXPRSTCK(EXPRSTCK).m_id_req;
	}

	ae2f_expected_but_else(LST_SCALE_BUF) {
		CTX->m_err = ACLSPV_COMPILE_ALLOC_FAILED;
		return CXChildVisit_Break;
	}

	(void)h_parent;

	dbg_prefix(i);
	dbg_call(fprintf, (stderr, "Checking evaluator 0: %u[%p]\n"
				, get_buf_from_scale2(aclspv_wrd_t, &CMDSTCK_SCALE, *get_prv_from_scale(&CMDSTCK_SCALE, *LST_SCALE))[1] 
				, (void*)(get_buf_from_scale2(aclspv_wrd_t, &CMDSTCK_SCALE, *get_prv_from_scale(&CMDSTCK_SCALE, *LST_SCALE)) + 1)
			  ));

#if 1
	dbg_prefix(i);
	dbg_call(ae2f_NONE, (fprintf(stderr, "Operand Counter: %u[%p]\n"
					, GETLASTEL_CMDCOUNTERSTCK().m_count_oprnd
					, (void*)&GETLASTEL_CMDCOUNTERSTCK().m_count_oprnd
				    )));
#endif

	dbg_prefix(i);
	dbg_call(ae2f_NONE, (fprintf(
					stderr
					, "Type Original before ExprBin1: %u\n"
					, LST_TYPE_ORIGINAL[GETLASTEL_CMDCOUNTERSTCK(a).m_count_oprnd]
				    )));
	switch(emit_expr_bin_1(
				h_cur
				, CTX
				, ID_REQ_CURRENT
				, &TYPE_CAST_CURRENT
				, LST_TYPE_ORIGINAL + GETLASTEL_CMDCOUNTERSTCK(a).m_count_oprnd
				, RET_LITERAL
			      )) {
		case EMIT_EXPR_FAILURE:
			assert(0 && "bin_1 is fishy");
			CTX->m_err = ACLSPV_COMPILE_MET_INVAL;
			return CXChildVisit_Break;

		case EMIT_EXPR_SUCCESS:
			dbg_prefix(a);
			dbg_call(ae2f_NONE, (fprintf(stderr, "RTBinEx1 Success: TypeCastedCurrent: %u\n", TYPE_CAST_CURRENT)));
			IS_NOT_CONSTANT = UTIL_LITERAL_RT;
			LST_SCALE_BUF[1] = UTIL_LITERAL_MASK_RT;

			ae2f_unexpected_but_if(0) {
				ae2f_unreachable();
				case EMIT_EXPR_SUCCESS_LITERAL:
				dbg_prefix(a);
				dbg_call(fprintf, (stderr, "Binary expression was literal [%p]%u\n"
							, (void*)(LST_SCALE_BUF + 1)
							, LST_SCALE_BUF[1])
					);

				dbg_prefix(a);
				dbg_call(ae2f_NONE, (fprintf(stderr, "\tTypeCastedCurrent: %u\n", TYPE_CAST_CURRENT)));
			}

			ae2f_unexpected_but_if(0) {
				ae2f_unreachable();
				case EMIT_EXPR_SUCCESS_CONSTANT:
				dbg_prefix(a);
				dbg_call(fprintf, (stderr, "Binary expression was constant [%p]%u\n"
							, (void*)(LST_SCALE_BUF + 1)
							, LST_SCALE_BUF[1])
					);

				IS_NOT_CONSTANT &= UTIL_LITERAL_CONSTANT;
				LST_SCALE_BUF[1] &= UTIL_LITERAL_MASK_CONSTANT;
			}


			LST_LITERAL[GETLASTEL_CMDCOUNTERSTCK(i).m_count_oprnd] = *RET_LITERAL;

			(void)(EXPRSTCK_COUNT && --EXPRSTCK_COUNT);
			++GETLASTEL_CMDCOUNTERSTCK(a).m_count_oprnd;

			while(LST_SCALE->m_id && GETLASTEL_CMDCOUNTERSTCK().m_count_oprnd == LST_SCALE_BUF[0]) {
				aclspv_id_t	
					OLD_ID0 =	LST_SCALE_BUF[UTIL_OPBINCFG_END]
					, OLD_ID1 =	LST_SCALE_BUF[UTIL_OPBINCFG_END + 1];

				util_literal	NEW_LITERAL_0 = LST_LITERAL[0];
				util_literal 	NEW_LITERAL_1 = LST_LITERAL[1];

				fixme_puts("Arithmetic cast should be tested on switch");
				dbg_prefix(a);
				dbg_call(fprintf, (stderr, "SCALEBUF[5](opcode) (%p)%u\n"
							, (void*)(LST_SCALE_BUF + 5)
							, LST_SCALE_BUF[5]));

				LST_SCALE_BUF[UTIL_OPBINCFG_END] = CTX->m_id++;
				LST_SCALE_BUF[UTIL_OPBINCFG_END + 1] = CTX->m_id++;

				emit_expr_arithmetic_cast(
						OLD_ID0
						, LST_TYPE_ORIGINAL[0]
						, (B_util_literal_t)LST_SCALE_BUF[1] 
						, NEW_LITERAL_0
						, LST_SCALE_BUF[UTIL_OPBINCFG_END]
						, TYPE_CAST_CURRENT
						, &LST_LITERAL[0]
						, CTX
						);

				LST_TYPE_ORIGINAL[0] = TYPE_CAST_CURRENT;

				emit_expr_arithmetic_cast(
						OLD_ID1
						, LST_TYPE_ORIGINAL[1]
						, (B_util_literal_t)LST_SCALE_BUF[1] 
						, NEW_LITERAL_1
						, LST_SCALE_BUF[UTIL_OPBINCFG_END + 1]
						, TYPE_CAST_CURRENT
						, &LST_LITERAL[1]
						, CTX
						);

				LST_TYPE_ORIGINAL[1] = TYPE_CAST_CURRENT;

				get_buf_from_scale2(aclspv_wrd_t
						, &CMDSTCK_SCALE
						, *get_prv_from_scale(&CMDSTCK_SCALE, *LST_SCALE
							))[1] &= LST_SCALE_BUF[1];

				dbg_prefix(a);
				dbg_call(fprintf, (stderr, "LST_SCALE_BUF[5] (%p)%u\n"
							, (void*)(LST_SCALE_BUF + 5)
							, LST_SCALE_BUF[5]));
				ae2f_expected_but_else(LST_SCALE_BUF[5] = util_opbin2_to_spvop(
							TYPE_CAST_CURRENT
							, LST_SCALE_BUF[5]
							))
				{
					err_puts(("SpvOp went undefined"));
				}

				switch((unsigned)LST_SCALE_BUF[1]) {
					case UTIL_LITERAL_MASK_RT:
						dbg_puts(("The result is rt(runtime)."));
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
						break;

					case UTIL_LITERAL_MASK_LITERAL: case UTIL_LITERAL_LITERAL:
						{

							struct util_literal_eval_ EVAL_LIT;

							dbg_puts("==[ `util_literal_eval2` will be invoked.");

							dbg_prefix(a);
							dbg_call(fprintf, (stderr, "\tLST_SCALE_BUF[5] (%p)%u\n"
										, (void*)(LST_SCALE_BUF + 5)
										, LST_SCALE_BUF[5]));
							dbg_prefix(a);
							dbg_call(fprintf, (
										stderr
										, "\tTYPE_CAST_CURRENT %u\n"
										, TYPE_CAST_CURRENT)
								);


							dbg_prefix(a);
							dbg_call(ae2f_NONE, (fprintf(stderr, "\tTwo literal as umax: %lu %lu\n"
											, LST_LITERAL[0].m_api_uintmax
											, LST_LITERAL[1].m_api_uintmax
										    )));

							dbg_prefix(a);
							dbg_call(ae2f_NONE, (fprintf(stderr, "\tTwo literal as double: %f %f\n"
											, LST_LITERAL[0].m_dbl
											, LST_LITERAL[1].m_dbl
										    )));

							dbg_prefix(a);
							dbg_call(ae2f_NONE, (fprintf(stderr, "\tTwo literal as float: %f %f\n"
											, LST_LITERAL[0].m_flt
											, LST_LITERAL[1].m_flt
										    )));

							EVAL_LIT = util_literal_eval2(
									LST_LITERAL[0]
									, LST_LITERAL[1]
									, LST_SCALE_BUF[5]
									,(e_id_default)TYPE_CAST_CURRENT
									);

							dbg_prefix(a);
							dbg_call(ae2f_NONE, (fprintf(stderr, "\tResult as umax: %lu\n"
											, EVAL_LIT.m_ret.m_api_uintmax
										    )));

							dbg_prefix(a);
							dbg_call(ae2f_NONE, (fprintf(stderr, "\tResult as double: %f\n"
											, EVAL_LIT.m_ret.m_dbl
										    )));

							dbg_prefix(a);
							dbg_call(ae2f_NONE, (fprintf(stderr, "\tResult as float: %f\n"
											, EVAL_LIT.m_ret.m_flt
										    )));

							dbg_prefix(a);
							dbg_call(fprintf, (stderr, "\tLST_SCALE_BUF[5] (%p)%u\n"
										, (void*)(LST_SCALE_BUF + 5)
										, LST_SCALE_BUF[5]));
							dbg_prefix(a);
							dbg_call(fprintf, (
										stderr
										, "\tTYPE_CAST_CURRENT %u\n"
										, TYPE_CAST_CURRENT)
								);
							dbg_prefix(a);
							dbg_call(fprintf, (
										stderr
										, "\tEVAL_LIT.m_found %d\n"
										, EVAL_LIT.m_found)
								);

							dbg_puts("]== `util_literal_eval2` will be invoked.");

							if(EVAL_LIT.m_found) 
							{
								const h_scale_t PRVSCALE = get_prv_from_scale(
										&CMDSTCK_SCALE
										, (*LST_SCALE));

								aclspv_wrd_t* const ae2f_restrict PRVBUF = get_buf_from_scale2(
										aclspv_wrd_t
										, &CMDSTCK_SCALE
										, *PRVSCALE
										);

								const aclspv_wrd_t PRVIDX =
									(NUM_CMDCOUNTERSTCK - 1 ? NUM_CMDCOUNTERSTCK - 2 : 0)
									[BUF_CMDCOUNTERSTCK(a)].m_count_oprnd
									;

								const aclspv_wrd_t PRVNUMOPRND = PRVBUF[0];

								util_literal* ae2f_restrict const PRVBUFLITERAL = 
									(void* ae2f_restrict)
									&PRVBUF[(PRVNUMOPRND << 1) + UTIL_OPBINCFG_END]
									;

								dbg_puts(("The result is literal."));
								fixme_puts("Add verification for emit_expr_arithmetic_cast");

								emit_expr_arithmetic_cast(
										0
										, (aclspv_id_t)TYPE_CAST_CURRENT
										, UTIL_LITERAL_MASK_LITERAL
										, EVAL_LIT.m_ret
										, LST_SCALE_BUF[4]
										, (aclspv_id_t)TYPE_CAST_CURRENT
										, PRVBUFLITERAL + PRVIDX
										, CTX
										);

								*RET_LITERAL = PRVBUFLITERAL[PRVIDX];

								goto LBL_WIPE_ONE_SCALE;
							}

							LST_SCALE_BUF[1] = UTIL_LITERAL_MASK_CONSTANT;
						}

						ae2f_fallthrough;
					case UTIL_LITERAL_MASK_CONSTANT: 
						dbg_puts(("The result is constant(fallback)."));
						break;
					default:
						err_puts(("unknown literal"));
						ae2f_unreachable();
				}

#define		TMPL_SECTION	(*((IS_NOT_CONSTANT) == UTIL_LITERAL_RT ? &CTX->m_section.m_fnimpl	: &CTX->m_section.m_types))
#define		TMPL_COUNT	(*((IS_NOT_CONSTANT) == UTIL_LITERAL_RT ? &CTX->m_count.m_fnimpl	: &CTX->m_count.m_types))

				dbg_prefix(a);
				dbg_call(fprintf, (stderr, "IS_NOT_CONSTANT: %lu\n", IS_NOT_CONSTANT));

				_aclspv_grow_vec_with_copy(
						_aclspv_malloc
						, _aclspv_free
						, memcpy
						, L_new
						, TMPL_SECTION
						, (size_t)count_to_sz(TMPL_COUNT + (LST_CMD_COUNT)
							- (LST_SCALE_BUF[1] == UTIL_LITERAL_RT))
						);

				ae2f_expected_but_else(TMPL_SECTION.m_p) {
					CTX->m_err = ACLSPV_COMPILE_ALLOC_FAILED;
					return CXChildVisit_Break;
				}

				if((UTIL_LITERAL_MASK_RT == LST_SCALE_BUF[1]) == 1) {
					dbg_puts(("(UTIL_LITERAL_RT == LST_SCALE_BUF[1]) == 1"));
				}

				dbg_puts("TMPCHECK: Is before memcpy okay?");

				memcpy(
						get_wrd_of_vec(&TMPL_SECTION) + TMPL_COUNT
						, LST_SCALE_BUF + 2 + (LST_SCALE_BUF[1] == UTIL_LITERAL_RT)
						, (size_t)count_to_sz(LST_CMD_COUNT - (LST_SCALE_BUF[1] == UTIL_LITERAL_RT))
				      );

				TMPL_COUNT += LST_CMD_COUNT - (LST_SCALE_BUF[1] == UTIL_LITERAL_RT);
#undef		TMPL_SECTION
#undef		TMPL_COUNT


LBL_WIPE_ONE_SCALE:

				del_scale_from_vec_last(&CMDSTCK_SCALE);
				LST_SCALE = get_last_scale_from_vec(&CMDSTCK_SCALE);
				LST_SCALE_BUF = get_buf_from_scale2(aclspv_wrd_t, &CMDSTCK_SCALE, *LST_SCALE);

				(void)(NUM_CMDCOUNTERSTCK && --NUM_CMDCOUNTERSTCK);

				EXPRSTCK_COUNT	-= LST_SCALE_BUF[0] - 1;

				LST_LITERAL 
					= ((util_literal* ae2f_restrict)
							(void* ae2f_restrict)(&LST_SCALE_BUF[(LST_CMDBUF_COUNT + 2)]));

				LST_TYPE_ORIGINAL
					= ((aclspv_id_t* ae2f_restrict)(void* ae2f_restrict)(LST_LITERAL)) - LST_SCALE_BUF[0];

				LST_TYPE_ORIGINAL[GETLASTEL_CMDCOUNTERSTCK(a).m_count_oprnd] = TYPE_CAST_CURRENT;
				(void)++GETLASTEL_CMDCOUNTERSTCK(a).m_count_oprnd;
			}
			return CXChildVisit_Recurse;
		case EMIT_EXPR_NOT_THE_CASE:
			break;
		default:
			assert(0);
			ae2f_unreachable();
	}

	dbg_prefix(i);
	dbg_call(fprintf, (stderr, "Checking evaluator 1: %u[%p]\n"
				, get_buf_from_scale2(aclspv_wrd_t, &CMDSTCK_SCALE, *get_prv_from_scale(&CMDSTCK_SCALE, *LST_SCALE))[1] 
				, (void*)(get_buf_from_scale2(aclspv_wrd_t, &CMDSTCK_SCALE, *get_prv_from_scale(&CMDSTCK_SCALE, *LST_SCALE)) + 1)
			  ));


	{
		aclspv_wrd_t	OPRNDS[2];
		switch(emit_expr_bin_2(
					h_cur
					, CTX
					, ID_REQ_CURRENT
					, (aclspv_wrd_t)TYPE_CAST_CURRENT
					, &CMDSTCK_SCALE, OPRNDS))
		{
			case EMIT_EXPR_FAILURE:
				assert(0 && "FAILURE_expr_bin2");
				return CXChildVisit_Break;

			case EMIT_EXPR_SUCCESS:
			case EMIT_EXPR_SUCCESS_CONSTANT:
			case EMIT_EXPR_SUCCESS_LITERAL:
				dbg_prefix();
				dbg_call(fprintf, (stderr
							, "Operands %u %u\n"
							, OPRNDS[0], OPRNDS[1])
					);

				_aclspv_grow_vec_with_copy(
						_aclspv_malloc,  _aclspv_free, memcpy
						, dest, EXPRSTCK, (size_t)(sizeof(exprstck) * (EXPRSTCK_COUNT + 2))
						);

				ae2f_expected_but_else(EXPRSTCK.m_p) {
					CTX->m_err = ACLSPV_COMPILE_ALLOC_FAILED;
					return CXChildVisit_Break;
				}

				EXPRSTCK_COUNT += 2;
				BUF_EXPRSTCK(EXPRSTCK)[EXPRSTCK_COUNT - 1].m_id_req = OPRNDS[0];
				BUF_EXPRSTCK(EXPRSTCK)[EXPRSTCK_COUNT - 2].m_id_req = OPRNDS[1];

				_aclspv_grow_vec_with_copy(
						_aclspv_malloc,  _aclspv_free, memcpy
						, dest, CMDCOUNTERSTCK
						, (size_t)(sizeof(cmdcounterstckelm) * (NUM_CMDCOUNTERSTCK + 5))
						);

				ae2f_expected_but_else(CMDCOUNTERSTCK.m_p) {
					CTX->m_err = ACLSPV_COMPILE_ALLOC_FAILED;
					return CXChildVisit_Break;
				}

				++NUM_CMDCOUNTERSTCK;
				GETLASTEL_CMDCOUNTERSTCK(a).m_count_oprnd = 0;

				ae2f_fallthrough;
			case EMIT_EXPR_NOT_THE_CASE:
				break;
			default:
				ae2f_unreachable();
		}
	}

	dbg_prefix(i);
	dbg_call(fprintf, (stderr, "Checking evaluator 2: %u[%p]\n"
				, get_buf_from_scale2(aclspv_wrd_t, &CMDSTCK_SCALE, *get_prv_from_scale(&CMDSTCK_SCALE, *LST_SCALE))[1] 
				, (void*)(get_buf_from_scale2(aclspv_wrd_t, &CMDSTCK_SCALE, *get_prv_from_scale(&CMDSTCK_SCALE, *LST_SCALE)) + 1)
			  ));

	CTX->m_err = ACLSPV_COMPILE_OK;

	/** assert(TYPE_CAST_CURRENT); */
	dbg_prefix(a);
	dbg_call(ae2f_NONE, (fprintf(stderr, "Type Casted Current: %u\n", TYPE_CAST_CURRENT)));

	return CXChildVisit_Recurse;

#undef	CTX
#undef	EXPRSTCK_COUNT
#undef	EXPRSTCK
#undef	CMDSTCK_SCALE
#undef	ID_REQ
#undef	LST_CMD_COUNT
#undef	LST_CMDBUF_COUNT
#undef	BUF_EXPRSTCK
#undef	GETLASTELEXPRSTCK
#undef	CMDCOUNTERSTCK
#undef	NUM_CMDCOUNTERSTCK
#undef	GETLASTEL_CMDCOUNTERSTCK
#undef	BUF_CMDCOUNTERSTCK
}

ae2f_inline static enum EMIT_EXPR_ emit_get_expr(
		const aclspv_id_t c_id_req
		, const aclspv_id_t c_type_req
		, const CXCursor c_cur
		, util_literal* ae2f_restrict	wr_literal_opt
		, h_util_ctx_t h_ctx
		) {
	uptr BUF[6 + (((sizeof(util_literal) + sizeof(uptr) - 1) / sizeof(uptr)))];
	aclspv_wrd_t* ae2f_restrict SCALE_BOOT;
	const aclspv_id_t ID_REDIRECT	= h_ctx->m_id++;

	BUF[0]		= (uptr)h_ctx;					/* context. must be pointer to context. */
	BUF[1]		= 0;						/* stack count. must be 0. */
	BUF[2]		= ID_REDIRECT;					/* result value id requested */
	BUF[3]		= UTIL_LITERAL_MASK_LITERAL;			/* see enum UTIL_LITERAL_MASK_ */
	BUF[4]		= ID_DEFAULT_I32;				/* type casted */
	BUF[5]		= 0;						/* count for operand */

	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, h_ctx->m_tmp.m_v0, 1 << 10);
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, h_ctx->m_tmp.m_v1, 1 << 10);
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, h_ctx->m_tmp.m_v2, 1 << 10);

	ae2f_expected_but_else(h_ctx->m_tmp.m_v0.m_p)
		return EMIT_EXPR_FAILURE;

	ae2f_expected_but_else(h_ctx->m_tmp.m_v1.m_p)
		return EMIT_EXPR_FAILURE;

	ae2f_expected_but_else(init_scale(&h_ctx->m_tmp.m_v1, count_to_sz(2)))
		return EMIT_EXPR_FAILURE;

	SCALE_BOOT = get_buf_from_scale2(aclspv_wrd_t, &h_ctx->m_tmp.m_v1, *get_last_scale_from_vec(&h_ctx->m_tmp.m_v1));
	assert(SCALE_BOOT);
	assert(!get_last_scale_from_vec(&h_ctx->m_tmp.m_v1)->m_id);

	SCALE_BOOT[0] = 0;
	SCALE_BOOT[1] = UTIL_LITERAL_MASK_LITERAL;

	((umax* ae2f_restrict)h_ctx->m_tmp.m_v2.m_p)[0] = 0;

	dbg_prefix(a);
	dbg_call(fprintf, (stderr
				, "Initialising first constant sector %u[%p]\n"
				, SCALE_BOOT[1], (void*)(SCALE_BOOT + 1))
		);

	if(emit_expr(c_cur, c_cur, BUF) == CXChildVisit_Break)
		return EMIT_EXPR_FAILURE;

	ae2f_expected_but_else(h_ctx->m_tmp.m_v0.m_p)
		return EMIT_EXPR_FAILURE;

	ae2f_expected_but_else(h_ctx->m_tmp.m_v1.m_p)
		return EMIT_EXPR_FAILURE;


	dbg_prefix(a);
	dbg_call(fprintf, (stderr
				, "Constant flag after first emit_expr %u[%p]\n"
				, SCALE_BOOT[1], (void*)(SCALE_BOOT + 1))
		);

	clang_visitChildren(c_cur, emit_expr, BUF);

	dbg_prefix(a);
	dbg_call(fprintf, (stderr, "BUF[3]: %u\n", (unsigned)BUF[3]));

	dbg_prefix(a);
	dbg_call(fprintf, (stderr, "Type returned (BUF[4]): %u\n", (unsigned)BUF[4]));

	dbg_prefix(a);
	dbg_call(fprintf, (stderr, "ID Required(aka BUF[2]): %u\n", (unsigned)BUF[2]));

	return	emit_expr_arithmetic_cast(
			ID_REDIRECT
			, (aclspv_wrd_t)BUF[4]
			, (aclspv_wrd_t)BUF[3]
			, *(util_literal*ae2f_restrict)(void* ae2f_restrict)(BUF + 6)
			, c_id_req
			, c_type_req
			, wr_literal_opt
			, h_ctx);
}

#endif
