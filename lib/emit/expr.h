/** @file expr.h */

#ifndef	lib_emit_expr_h
#define	lib_emit_expr_h

#include "aclspv/abi.h"
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
				aclspv_id_t ID_1 = util_mk_constant_val_id(1, h_ctx);
				ae2f_expected_but_else(ID_1)	return EMIT_EXPR_FAILURE;

				ae2f_expected_but_else(h_ctx->m_count.m_types
						= util_emitx_7(
							&h_ctx->m_section.m_types
							, h_ctx->m_count.m_types
							, SpvOpSpecConstantOp
							, c_new_type
							, c_new_id
							, SpvOpSelect
							, ID_1
							, c_old_id
							, ID_1))
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
				aclspv_id_t ID_0 = util_mk_constant_val_id(0, h_ctx);
				ae2f_expected_but_else(ID_0)	return EMIT_EXPR_FAILURE;

				ae2f_expected_but_else(h_ctx->m_count.m_fnimpl
						= util_emitx_5(
							&h_ctx->m_section.m_fnimpl
							, h_ctx->m_count.m_fnimpl
							, SpvOpIAdd
							, c_new_type
							, c_new_id
							, c_old_id
							, ID_0
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

				aclspv_id_t ID_0 = util_mk_constant_val_f32_id(0, h_ctx);
				aclspv_id_t ID_1 = util_mk_constant_val_id(1, h_ctx);
				ae2f_expected_but_else(ID_0)	return EMIT_EXPR_FAILURE;
				ae2f_expected_but_else(ID_1)	return EMIT_EXPR_FAILURE;

				ae2f_expected_but_else(h_ctx->m_count.m_types
						= util_emitx_7(
							&h_ctx->m_section.m_types
							, h_ctx->m_count.m_types
							, SpvOpSpecConstantOp
							, c_new_type
							, c_new_id
							, SpvOpSelect
							, ID_1
							, c_old_id
							, ID_0))
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
				aclspv_id_t ID_0 = util_mk_constant_val_f32_id(0, h_ctx);
				ae2f_expected_but_else(ID_0)	return EMIT_EXPR_FAILURE;

				ae2f_expected_but_else(h_ctx->m_count.m_fnimpl
						= util_emitx_5(
							&h_ctx->m_section.m_fnimpl
							, h_ctx->m_count.m_fnimpl
							, SpvOpFAdd
							, c_new_type
							, c_new_id
							, c_old_id
							, ID_0
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

	unless(c_flag_literal & UTIL_LITERAL_LITERAL) {
		return emit_expr_arithmetic_cast_non_literal(
				c_old_id, c_old_type
				, c_flag_literal, c_new_id
				, c_new_type, h_ctx
				);
	}

	dbg_prefix(a);
	dbg_call(ae2f_NONE, (fprintf(
					stderr
					, "Arithmetic cast: literal old[%u] new [%u]\n"
					, c_old_type, c_new_type
				    )));

	unless(util_default_is_number(c_old_type) && util_default_is_number(c_new_type)) {
		assert(util_default_is_number(c_old_type));
		assert(util_default_is_number(c_new_type));
		return EMIT_EXPR_NOT_THE_CASE;
	}

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

	else if(util_default_is_int(c_old_type) && util_default_is_float(c_new_type)) {
		dbg_prefix(a);
		dbg_call((void), (fprintf(stderr, "Cast (int -> float): %ld\n", c_old_literal.m_api_intmax)));
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

	else if(util_default_is_float(c_old_type) && util_default_is_float(c_new_type) && c_old_type != c_new_type) {
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
				, (aclspv_wrd_t)(RESULT.m_api_uintmax)
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
		aclspv_id_t* ae2f_restrict const	wr_newid,
		e_id_default* ae2f_restrict const	wr_type_cast,
		e_id_default* ae2f_restrict const	wr_type_fetched,
		util_literal* ae2f_restrict const	rwr_literal_opt
		) {
	emit_expr_literal EVRES;
	assert(wr_newid);

	ae2f_expected_but_else(h_ctx)
		return EMIT_EXPR_FAILURE;

	switch((uintmax_t)c_cur.kind) {
		/** unknown */
		default:
			dbg_prefix(a);
			dbg_call((void), (fprintf(stderr
							, "current cursor kind is not known by bin_1 %u\n"
							, c_cur.kind
							)));
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
						dbg_puts(("Literal: intmax"));
						EVRES.m_api_intmax = clang_EvalResult_getAsLongLong(EVAL);
						clang_EvalResult_dispose(EVAL);

						dbg_prefix(i);
						dbg_call(fprintf, (stderr
									, "Value fetched: %lu\n"
									, EVRES.m_api_uintmax
								  )
							);

						*wr_type_fetched = (EVRES.m_api_uintmax <= 0xFFFFFFFF 
									? ID_DEFAULT_I32 : ID_DEFAULT_I64);

						*wr_type_cast  =  util_default_decide_cast(
								*wr_type_cast
								, *wr_type_fetched
								);

						if(rwr_literal_opt) *rwr_literal_opt = EVRES;

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
										, "Cast id fetch has failed. %u\n"
										, *wr_type_cast
									  )
								);
							return EMIT_EXPR_FAILURE;
						}

						ae2f_expected_but_else(util_mk_default_id(
									*wr_type_fetched
									, h_ctx)) {
							err_prefix(a);
							err_call(fprintf, (stderr
										, "Default id fetch has failed. %u\n"
										, *wr_type_cast
									  )
								);
							return EMIT_EXPR_FAILURE;
						}

#if 1
						switch(util_default_bit_width(*wr_type_fetched)) {
							case 32:
								dbg_puts("I32");
								ae2f_expected_but_else(*wr_newid = util_mk_constant_val_id(
										(aclspv_wrd_t)EVRES.m_u64
										, h_ctx
										))
									return EMIT_EXPR_FAILURE;

								return EMIT_EXPR_SUCCESS_LITERAL;

							case 64:
								dbg_puts("I64");
								*wr_newid = h_ctx->m_id++;
								ae2f_expected_but_else(util_emitx_5(
											&h_ctx->m_section.m_types
											, h_ctx->m_count.m_types
											, SpvOpConstant
											, ID_DEFAULT_I64
											, *wr_newid
											, EVRES.m_u64 & 0xFFFFFFFF
											, (aclspv_wrd_t)((EVRES.m_u64 >> 32) & 0xFFFFFFFF)
											))
									return EMIT_EXPR_FAILURE;

								return EMIT_EXPR_SUCCESS_LITERAL;
							default:
								assert(0);
								ae2f_unreachable();
						}
#endif

					case CXEval_Float:
						dbg_puts(("Literal: double (float casted)"));
						EVRES.m_flt = (float)clang_EvalResult_getAsDouble(EVAL);
						dbg_prefix(a);
						dbg_call(fprintf, (stderr, "Literal fetched: %f\n", EVRES.m_flt));

						clang_EvalResult_dispose(EVAL);

						*wr_type_fetched = ID_DEFAULT_F32;
						*wr_type_cast = util_default_decide_cast(
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

						*rwr_literal_opt = EVRES;

						ae2f_expected_but_else(*wr_newid = util_mk_constant_val_f32_id(
									EVRES.m_flt
									, h_ctx
									))
							return EMIT_EXPR_FAILURE;

						return EMIT_EXPR_SUCCESS_LITERAL;

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
					*rwr_literal_opt = DECL_INFO.m_data.m_var_simple.m_literal;
					*wr_newid = DECL_INFO.m_data.m_var_simple.m_init_val;
					return EMIT_EXPR_SUCCESS_LITERAL;
				}

				if(DECL_INFO.m_data.m_var_simple.m_is_predictable) {
					dbg_puts(("Variable is predictable"));
					*wr_newid = DECL_INFO.m_data.m_var_simple.m_init_val;
					return DECL_INFO.m_data.m_var_simple.m_mask_literal & UTIL_LITERAL_CONSTANT
						? EMIT_EXPR_SUCCESS_CONSTANT
						: EMIT_EXPR_SUCCESS;
				}

				dbg_puts(("Load from memory"));
				*wr_newid = h_ctx->m_id++;
				ae2f_expected_but_else(h_ctx->m_count.m_fnimpl = util_emitx_4(
							&h_ctx->m_section.m_fnimpl
							, h_ctx->m_count.m_fnimpl
							, SpvOpLoad
							, DECL_INFO.m_data.m_var_simple.m_type_id
							, *wr_newid
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

	(void)wr_operands;

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


		NEW_SCALE = get_last_scale_from_vec(h_cmdscale);
		VEC = get_buf_from_scale2(aclspv_wrd_t, h_cmdscale, *NEW_SCALE);

		NEW_SCALE = mk_scale_from_vec(
				h_cmdscale
				, 
				count_to_sz(UTIL_OPBINCFG_END + 2)
				+ (sizeof(util_opbincfg_extra) << 1)
				);
		ae2f_expected_but_else(NEW_SCALE)
			return EMIT_EXPR_FAILURE;

		VEC = get_buf_from_scale2(aclspv_wrd_t, h_cmdscale, *NEW_SCALE);
		VEC[1] = UTIL_LITERAL_MASK_LITERAL;

		ae2f_expected_but_else(VEC)
			return EMIT_EXPR_FAILURE;

		VEC[0] = 2;			/** number of operands */
		VEC[1] = UTIL_LITERAL_MASK_LITERAL;	/** is not constant? */
		VEC[2] = opcode_to_wrd(SpvOpSpecConstantOp, 5);
		VEC[3] = c_result_type_id;
		VEC[4] = c_newid;
		VEC[5] = OPCODE;
		VEC[6] = 0;
		VEC[7] = 0;
	}

	return EMIT_EXPR_SUCCESS_LITERAL;
}

/** when error occurs, return value will be 1 */
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


ae2f_inline static int emit_expr_carve_one_scale(
		uintptr_t* const ae2f_restrict	rdwr_data
		) {
#define CTX			((h_util_ctx_t)((rdwr_data))[0])
#define EXPRSTCK_COUNT		(((rdwr_data))[1])
#define ID_REQ			(*((aclspv_wrd_t* ae2f_restrict)(void* ae2f_restrict)&(((rdwr_data))[2])))
#define	IS_NOT_CONSTANT		((rdwr_data))[3]
#define	TYPE_CAST_CURRENT	(*(aclspv_wrd_t*)&(((rdwr_data))[4]))
#define	NUM_CMDCOUNTERSTCK	(((rdwr_data))[5])

#define	EXPRSTCK	CTX->m_tmp.m_v0
#define	CMDSTCK_SCALE	CTX->m_tmp.m_v1
#define	CMDCOUNTERSTCK	CTX->m_tmp.m_v2
	h_scale_t LST_SCALE				= get_last_scale_from_vec(&CMDSTCK_SCALE);
	aclspv_wrd_t* ae2f_restrict LST_SCALE_BUF	= get_buf_from_scale2(aclspv_wrd_t, &CMDSTCK_SCALE, *LST_SCALE);
	util_literal* ae2f_restrict RET_LITERAL
		= (void*)(((uptr* ae2f_restrict)(rdwr_data)) + 6);

	aclspv_wrd_t	NUM_OPRND = LST_SCALE_BUF[0];
	util_opbincfg_extra* ae2f_restrict LST_EXTRA 
		= (void* ae2f_restrict)(LST_SCALE_BUF + NUM_OPRND + UTIL_OPBINCFG_END);


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

	/****************************************/
	const h_scale_t PRVSCALE = get_prv_from_scale(
			&CMDSTCK_SCALE
			, (*LST_SCALE));

	aclspv_wrd_t* const ae2f_restrict PRVBUF = get_buf_from_scale2(
			aclspv_wrd_t
			, &CMDSTCK_SCALE
			, *PRVSCALE
			);

#define PRVIDX	\
	((NUM_CMDCOUNTERSTCK - 2)	\
	 [BUF_CMDCOUNTERSTCK(a)].m_count_oprnd)

	const aclspv_wrd_t PRVNUMOPRND = PRVBUF[0];

	util_opbincfg_extra* ae2f_restrict const PRVEXTRA = 
		(void* ae2f_restrict)
		&PRVBUF[(PRVNUMOPRND) + UTIL_OPBINCFG_END]
		;

	/****************************************/


	assert(PRVIDX < PRVBUF[0]);


	LST_SCALE_BUF[UTIL_OPBINCFG_RESID] = ID_REQ = CTX->m_id++;
	PRVBUF[UTIL_OPBINCFG_END + PRVIDX] = ID_REQ;
	PRVBUF[UTIL_OPBINCFG_MASKLITERAL] &= PRVEXTRA[PRVIDX].m_mask_literal = LST_SCALE_BUF[UTIL_OPBINCFG_MASKLITERAL];
	PRVEXTRA[PRVIDX].m_type_original = LST_SCALE_BUF[UTIL_OPBINCFG_RESTYID];

	(void)aclspv_STATICASSERT_ELMSIZE;

	dbg_prefix(a);
	dbg_call((void), (fprintf(stderr, "Carve-One[%lu, [%p]%lu]: %u\n"
					,* (size_t*)CMDSTCK_SCALE.m_p
					, (void*)&LST_SCALE->m_id
					, LST_SCALE->m_id
					, PRVIDX
				 )));


	while(NUM_OPRND--) {
		if(LST_SCALE_BUF[UTIL_OPBINCFG_RESTYID] != LST_EXTRA[NUM_OPRND].m_type_original) {
			const util_literal OLD_LITERAL = LST_EXTRA[NUM_OPRND].m_literal;
			aclspv_id_t OLD_ID 
				= LST_SCALE_BUF[UTIL_OPBINCFG_END + NUM_OPRND];
			LST_SCALE_BUF[UTIL_OPBINCFG_END + NUM_OPRND] = CTX->m_id++;

			printf("ARITHMETIC CAST ON LOOP[%u]: %u %u\n"
					, NUM_OPRND
					, OLD_ID, LST_SCALE_BUF[UTIL_OPBINCFG_END + NUM_OPRND]
			      );

			switch(emit_expr_arithmetic_cast(
					OLD_ID
					, LST_EXTRA[NUM_OPRND].m_type_original
					, LST_EXTRA[NUM_OPRND].m_mask_literal
					, OLD_LITERAL
					, LST_SCALE_BUF[UTIL_OPBINCFG_END + NUM_OPRND]
					, LST_SCALE_BUF[UTIL_OPBINCFG_RESTYID]
					, &LST_EXTRA[NUM_OPRND].m_literal
					, CTX
					))
			{
				case EMIT_EXPR_FAILURE:
				default:
				case EMIT_EXPR_NOT_THE_CASE:
					return 0;


				case EMIT_EXPR_SUCCESS:
					LST_SCALE_BUF[UTIL_OPBINCFG_MASKLITERAL] = 0;
					ae2f_fallthrough;
				case EMIT_EXPR_SUCCESS_CONSTANT:
					LST_SCALE_BUF[UTIL_OPBINCFG_MASKLITERAL] &= 1;
					ae2f_fallthrough;
				case EMIT_EXPR_SUCCESS_LITERAL:
					break;
			}
		}
	}

	ae2f_expected_but_else(LST_SCALE_BUF[5] = util_opbin2_to_spvop(
				UTIL_OPBINCFG_RESTYID[LST_SCALE_BUF]
				, LST_SCALE_BUF[5]
				))
	{
		err_puts(("SpvOp went undefined"));
	} else {
		dbg_prefix(a);
		dbg_call((void), (fprintf(stderr, "SpvOp_[%u]\n", LST_SCALE_BUF[5])));
	}

	switch((unsigned)LST_SCALE_BUF[UTIL_OPBINCFG_MASKLITERAL]) {
		case UTIL_LITERAL_MASK_RT:
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

				EVAL_LIT = util_literal_eval2(
						LST_EXTRA[0].m_literal
						, LST_EXTRA[1].m_literal
						, LST_SCALE_BUF[5]
						,LST_SCALE_BUF[UTIL_OPBINCFG_RESTYID]
						);

				if(EVAL_LIT.m_found)
				{
					dbg_puts(("The result is literal."));
					fixme_puts("Add verification for emit_expr_arithmetic_cast");

#if 0
					emit_expr_arithmetic_cast(
							0
							, LST_SCALE_BUF[UTIL_OPBINCFG_RESTYID]
							, UTIL_LITERAL_MASK_LITERAL
							, EVAL_LIT.m_ret
							, LST_SCALE_BUF[UTIL_OPBINCFG_RESID]
						, LST_SCALE_BUF[UTIL_OPBINCFG_RESTYID]
							, &PRVEXTRA[PRVIDX].m_literal
							, CTX
							);

					*RET_LITERAL = PRVEXTRA[PRVIDX].m_literal;
					return 0;
#endif

					*RET_LITERAL = PRVEXTRA[PRVIDX].m_literal = EVAL_LIT.m_ret;
					PRVEXTRA[PRVIDX].m_mask_literal = 3;
					PRVEXTRA[PRVIDX].m_type_original = LST_SCALE_BUF[UTIL_OPBINCFG_RESTYID];
					return 0;
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


#define		TMPL_SECTION	(*((LST_SCALE_BUF[1]) == UTIL_LITERAL_RT ? &CTX->m_section.m_fnimpl	: &CTX->m_section.m_types))
#define		TMPL_COUNT	(*((LST_SCALE_BUF[1]) == UTIL_LITERAL_RT ? &CTX->m_count.m_fnimpl	: &CTX->m_count.m_types))

	dbg_puts("vec_with_copy");
	_aclspv_grow_vec_with_copy(
			_aclspv_malloc
			, _aclspv_free
			, memcpy
			, L_new
			, TMPL_SECTION
			, (size_t)count_to_sz(TMPL_COUNT + (LST_SCALE_BUF[0] + UTIL_OPBINCFG_RESID)
				- (LST_SCALE_BUF[1] == UTIL_LITERAL_RT))
			);

	ae2f_expected_but_else(TMPL_SECTION.m_p) {
		CTX->m_err = ACLSPV_COMPILE_ALLOC_FAILED;
		return 1;
	}

	dbg_puts("TMPCHECK: Is before memcpy okay?");

#define        LST_CMD_COUNT           (UTIL_OPBINCFG_RESID /** header */ + (LST_SCALE_BUF[0]))
	memcpy(
			get_wrd_of_vec(&TMPL_SECTION) + TMPL_COUNT
			, LST_SCALE_BUF + UTIL_OPBINCFG_SPECCONSTANTOP + (!LST_SCALE_BUF[1])
			, (size_t)count_to_sz(LST_CMD_COUNT - (!LST_SCALE_BUF[1]))
	      );

	TMPL_COUNT += LST_CMD_COUNT - (LST_SCALE_BUF[1] == UTIL_LITERAL_RT);

#undef		LST_CMD_COUNT
#undef		TMPL_SECTION
#undef		TMPL_COUNT

#undef	EXPRSTCK_COUNT
#undef	ID_REQ
#undef	IS_NOT_CONSTANT
#undef	TYPE_CAST_CURRENT
#undef	NUM_CMDCOUNTERSTCK
#undef	CMDSTCK_SCALE
#undef	BUF_EXPRSTCK
#undef	GETLASTELEXPRSTCK
#undef	BUF_CMDCOUNTERSTCK
#undef	GETLASTEL_CMDCOUNTERSTCK

#undef	EXPRSTCK	/** CTX->m_tmp.m_v0 */
#undef	CMDSTCK_SCALE	/** CTX->m_tmp.m_v1 */
#undef	CMDCOUNTERSTCK	/** CTX->m_tmp.m_v2 */
#undef	CTX

	return 0;
}

static enum CXChildVisitResult emit_expr(
		CXCursor h_cur,
		CXCursor h_parent,
		CXClientData rdwr_data
		)
{
#define	CTX			((h_util_ctx_t)(((uptr* ae2f_restrict)rdwr_data)[0]))
#define EXPRSTCK_COUNT		(((uptr* ae2f_restrict)(rdwr_data))[1])
#define ID_REQ			(*((aclspv_wrd_t* ae2f_restrict)(void* ae2f_restrict)&(((uptr* ae2f_restrict)(rdwr_data))[2])))
#define	IS_NOT_CONSTANT		((uptr* ae2f_restrict)(rdwr_data))[3]
#define	TYPE_CAST_CURRENT	(*(aclspv_wrd_t*)&(((uptr* ae2f_restrict)(rdwr_data))[4]))
#define	NUM_CMDCOUNTERSTCK	(((uptr* ae2f_restrict)(rdwr_data))[5])

	/** for stack tmp.m_v0 will be used. */
#define	EXPRSTCK	CTX->m_tmp.m_v0
#define	CMDSTCK_SCALE	CTX->m_tmp.m_v1
#define	CMDCOUNTERSTCK	CTX->m_tmp.m_v2

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
	util_literal* ae2f_restrict RET_LITERAL
		= (void*)(((uptr* ae2f_restrict)(rdwr_data)) + 6);

	util_opbincfg_extra* ae2f_restrict	LST_EXTRA
		= (void* ae2f_restrict)(&LST_SCALE_BUF[(UTIL_OPBINCFG_END + LST_SCALE_BUF[0])]);

	(void)aclspv_STATICASSERT_ELMSIZE;

#if 1
	if(EXPRSTCK_COUNT) {
		ae2f_expected_but_else(EXPRSTCK.m_p) {
			CTX->m_err = ACLSPV_COMPILE_ALLOC_FAILED;
			assert(0 && "stack is acting funny");
			return CXChildVisit_Break;
		}

	}
#else
	(void)EXPRSTCK_COUNT;
#endif

	ae2f_expected_but_else(LST_SCALE_BUF) {
		CTX->m_err = ACLSPV_COMPILE_ALLOC_FAILED;
		return CXChildVisit_Break;
	}

	(void)h_parent;

#if 1
	dbg_prefix(i);
	dbg_call(ae2f_NONE, (fprintf(stderr, "Operand Counter: %u/%u[%lu]\n"
					, GETLASTEL_CMDCOUNTERSTCK().m_count_oprnd
					, LST_SCALE_BUF[0]
					, LST_SCALE->m_id
				    )));
#endif
	ID_REQ = CTX->m_id++;
	LST_EXTRA[GETLASTEL_CMDCOUNTERSTCK(a).m_count_oprnd].m_mask_literal = UTIL_LITERAL_MASK_LITERAL;

	/** Recurse filter */
	switch((uintmax_t)h_cur.kind) {
		default:
			break;
		case CXCursor_UnexposedExpr:
			return CXChildVisit_Recurse;
	}

	switch(emit_expr_bin_1(
				h_cur
				, CTX
				, &ID_REQ
				, &LST_SCALE_BUF[UTIL_OPBINCFG_RESTYID]
				, &LST_EXTRA[GETLASTEL_CMDCOUNTERSTCK(a).m_count_oprnd].m_type_original
				, &LST_EXTRA[GETLASTEL_CMDCOUNTERSTCK(i).m_count_oprnd].m_literal
			      )) {
		case EMIT_EXPR_FAILURE:
			assert(0 && "bin_1 is fishy");
			CTX->m_err = ACLSPV_COMPILE_MET_INVAL;
			return CXChildVisit_Break;

		case EMIT_EXPR_SUCCESS:
			IS_NOT_CONSTANT = UTIL_LITERAL_RT;
			LST_SCALE_BUF[1] = UTIL_LITERAL_MASK_RT;
			LST_EXTRA[GETLASTEL_CMDCOUNTERSTCK(a).m_count_oprnd].m_mask_literal = 0;

			ae2f_unexpected_but_if(0) {
				ae2f_unreachable();
				case EMIT_EXPR_SUCCESS_LITERAL:
				*RET_LITERAL = LST_EXTRA[GETLASTEL_CMDCOUNTERSTCK(i).m_count_oprnd].m_literal; 
			}

			ae2f_unexpected_but_if(0) {
				ae2f_unreachable();
				case EMIT_EXPR_SUCCESS_CONSTANT:

				IS_NOT_CONSTANT &= UTIL_LITERAL_MASK_CONSTANT;
				LST_SCALE_BUF[1] &= UTIL_LITERAL_MASK_CONSTANT;
				LST_EXTRA[GETLASTEL_CMDCOUNTERSTCK(a).m_count_oprnd].m_mask_literal &= UTIL_LITERAL_MASK_CONSTANT;
			}

			LST_SCALE_BUF[UTIL_OPBINCFG_END + GETLASTEL_CMDCOUNTERSTCK(i).m_count_oprnd] = ID_REQ;

			(void)(EXPRSTCK_COUNT && --EXPRSTCK_COUNT);
			(void)(LST_SCALE->m_id && ++GETLASTEL_CMDCOUNTERSTCK(a).m_count_oprnd);

			TYPE_CAST_CURRENT = util_default_decide_cast(LST_SCALE_BUF[UTIL_OPBINCFG_RESTYID], TYPE_CAST_CURRENT);

			while(LST_SCALE->m_id && GETLASTEL_CMDCOUNTERSTCK().m_count_oprnd >= LST_SCALE_BUF[0]) {
				GETLASTEL_CMDCOUNTERSTCK().m_count_oprnd = LST_SCALE_BUF[0] - 1;
				LST_SCALE		= get_last_scale_from_vec(&CMDSTCK_SCALE);
				LST_SCALE_BUF		= get_buf_from_scale2(aclspv_wrd_t, &CMDSTCK_SCALE, *LST_SCALE);

				LST_EXTRA
					= (void* ae2f_restrict)(&LST_SCALE_BUF[(UTIL_OPBINCFG_END + LST_SCALE_BUF[0])]);
				dbg_prefix(a);
				dbg_call((void), (fprintf(
								stderr
								, "Last scale ID[%lu]: [%p]%lu\n"
								, *((size_t*)CMDSTCK_SCALE.m_p)
								, (void*)&LST_SCALE->m_id
								, LST_SCALE->m_id
							 )));
				ae2f_unexpected_but_if(emit_expr_carve_one_scale(rdwr_data))
					return CXChildVisit_Break;

				del_scale_from_vec_last(&CMDSTCK_SCALE);
				(void)(NUM_CMDCOUNTERSTCK && --NUM_CMDCOUNTERSTCK);

				LST_SCALE		= get_last_scale_from_vec(&CMDSTCK_SCALE);
				LST_SCALE_BUF		= get_buf_from_scale2(aclspv_wrd_t, &CMDSTCK_SCALE, *LST_SCALE);

				LST_EXTRA
					= (void* ae2f_restrict)(&LST_SCALE_BUF[(UTIL_OPBINCFG_END + LST_SCALE_BUF[0])]);
#if 1
				(void)(LST_SCALE->m_id && ++GETLASTEL_CMDCOUNTERSTCK(a).m_count_oprnd);
#endif
			}

			return CXChildVisit_Recurse;
		case EMIT_EXPR_NOT_THE_CASE:
			dbg_puts("Operator 1: not the case");
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

	switch(emit_expr_bin_2(
				h_cur
				, CTX
				, 0
				, 0
				, &CMDSTCK_SCALE
				, 0))
	{
		case EMIT_EXPR_FAILURE:
			assert(0 && "FAILURE_expr_bin2");
			return CXChildVisit_Break;

		case EMIT_EXPR_SUCCESS:
		case EMIT_EXPR_SUCCESS_CONSTANT:
		case EMIT_EXPR_SUCCESS_LITERAL:
			_aclspv_grow_vec_with_copy(
					_aclspv_malloc,  _aclspv_free, memcpy
					, dest, EXPRSTCK, (size_t)(sizeof(exprstck) * (EXPRSTCK_COUNT + 2))
					);

			ae2f_expected_but_else(EXPRSTCK.m_p) {
				CTX->m_err = ACLSPV_COMPILE_ALLOC_FAILED;
				return CXChildVisit_Break;
			}

			EXPRSTCK_COUNT += 2;

			_aclspv_grow_vec_with_copy(
					_aclspv_malloc,  _aclspv_free, memcpy
					, dest, CMDCOUNTERSTCK
					, (size_t)(sizeof(cmdcounterstckelm) * (NUM_CMDCOUNTERSTCK + 5))
					);

			ae2f_expected_but_else(CMDCOUNTERSTCK.m_p) {
				CTX->m_err = ACLSPV_COMPILE_ALLOC_FAILED;
				return CXChildVisit_Break;
			}

#if 0
			++GETLASTEL_CMDCOUNTERSTCK(a).m_count_oprnd;
#endif
			++NUM_CMDCOUNTERSTCK;
			GETLASTEL_CMDCOUNTERSTCK(a).m_count_oprnd = 0;

			break;
		case EMIT_EXPR_NOT_THE_CASE:
			dbg_puts("Evaluator two: not the case");
			break;
		default:
			ae2f_unreachable();
	}

	dbg_prefix(i);
	dbg_call(fprintf, (stderr, "Checking evaluator 2: %u[%p]\n"
				, get_buf_from_scale2(aclspv_wrd_t, &CMDSTCK_SCALE, *get_prv_from_scale(&CMDSTCK_SCALE, *LST_SCALE))[1] 
				, (void*)(get_buf_from_scale2(aclspv_wrd_t, &CMDSTCK_SCALE, *get_prv_from_scale(&CMDSTCK_SCALE, *LST_SCALE)) + 1)
			  ));

	CTX->m_err = ACLSPV_COMPILE_OK;

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

	BUF[0]		= (uptr)h_ctx;					/* context. must be pointer to context. */
	BUF[1]		= 0;						/* stack count. must be 0. */
	BUF[2]		= 0;						/* result value id */
	BUF[3]		= UTIL_LITERAL_MASK_LITERAL;			/* see enum UTIL_LITERAL_MASK_ */
	BUF[4]		= 0;						/* type casted */
	BUF[5]		= 0;						/* count for operand */

	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, h_ctx->m_tmp.m_v0, 1 << 10);
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, h_ctx->m_tmp.m_v1, 1 << 10);
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, h_ctx->m_tmp.m_v2, 1 << 10);

	ae2f_expected_but_else(h_ctx->m_tmp.m_v0.m_p)
		return EMIT_EXPR_FAILURE;

	ae2f_expected_but_else(h_ctx->m_tmp.m_v1.m_p)
		return EMIT_EXPR_FAILURE;

	ae2f_expected_but_else(init_scale(&h_ctx->m_tmp.m_v1
				, count_to_sz(UTIL_OPBINCFG_END) + sizeof(aclspv_wrd_t)
				+ sizeof(util_opbincfg_extra)
				))
		return EMIT_EXPR_FAILURE;

	SCALE_BOOT = get_buf_from_scale2(aclspv_wrd_t, &h_ctx->m_tmp.m_v1, *get_last_scale_from_vec(&h_ctx->m_tmp.m_v1));
	assert(SCALE_BOOT);
	assert(!get_last_scale_from_vec(&h_ctx->m_tmp.m_v1)->m_id);

	SCALE_BOOT[UTIL_OPBINCFG_NUMOPRND]		= 1;
	SCALE_BOOT[UTIL_OPBINCFG_MASKLITERAL]		= UTIL_LITERAL_MASK_LITERAL;
	SCALE_BOOT[UTIL_OPBINCFG_RESTYID]		= 0;
	SCALE_BOOT[UTIL_OPBINCFG_RESID]			= 0;
	SCALE_BOOT[UTIL_OPBINCFG_OPCODE]		= 0;
#define INIT_EXTRA ((util_opbincfg_extra* ae2f_restrict)(void* ae2f_restrict)(&SCALE_BOOT[UTIL_OPBINCFG_END + 1]))

	INIT_EXTRA->m_mask_literal		= 3;
	INIT_EXTRA->m_type_original		= 0;
	INIT_EXTRA->m_literal.m_api_uintmax	= 0;

#undef	INIT_EXTRA

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
			(aclspv_wrdcount_t)BUF[2]
			, (aclspv_wrd_t)BUF[4]
			, (aclspv_wrd_t)BUF[3]
			, *(util_literal*ae2f_restrict)(void* ae2f_restrict)(BUF + 6)
			, c_id_req
			, c_type_req
			, wr_literal_opt
			, h_ctx);
}

#endif
