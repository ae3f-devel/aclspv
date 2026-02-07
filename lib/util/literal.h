#ifndef util_literal_h
#define util_literal_h

#include <ae2f/c90/StdInt.h>

enum UTIL_LITERAL_MASK_ {

	UTIL_LITERAL_MASK_RT,

	UTIL_LITERAL_MASK_CONSTANT	= 0x1,
	UTIL_LITERAL_MASK_LITERAL	= 0x3
};

enum UTIL_LITERAL_ {
	UTIL_LITERAL_RT,
	UTIL_LITERAL_CONSTANT,
	UTIL_LITERAL_LITERAL
};

typedef uint_least32_t	B_util_literal_t;
typedef union {
	int		m_api_int;
	intmax_t	m_api_intmax;
	uintmax_t	m_api_uintmax;

	u64_least	m_u64;
	double		m_dbl;
	float		m_flt;
} util_literal;

#include <ae2f/Keys.h>
#include <spirv/unified1/spirv.h>

#include "./iddef.h"
#include "./log.h"

struct util_literal_eval_ {
	int		m_found;
	util_literal	m_ret;
};

ae2f_inline static ae2f_ccconst struct util_literal_eval_	util_literal_eval2(
		const util_literal	c_oprndA,
		const util_literal	c_oprndB,
		const enum SpvOp_	c_opcode,
		const e_id_default	c_type
		)
{
	struct util_literal_eval_ RET;

#if 0
	RET.m_found = 0;
	return RET;
#endif



	switch((uintmax_t)c_type) {
#define	EXPR_TWO(L_oper, L_mem, c_case)					\
		case	c_case:						\
									\
		RET.m_ret.L_mem = c_oprndA.L_mem L_oper c_oprndB.L_mem;	\
		RET.m_found = 1;					\

		case ID_DEFAULT_I8:
		case ID_DEFAULT_I16:
		case ID_DEFAULT_I32:
		case ID_DEFAULT_I64:
			/** FIXME: in arithmetic sign matters */
			switch((uintmax_t)c_opcode) {
				EXPR_TWO(+, m_api_intmax, SpvOpIAdd) {}
				return RET;

				EXPR_TWO(-, m_api_intmax, SpvOpISub) {}
				return RET;

				EXPR_TWO(*, m_api_intmax, SpvOpIMul) {}
				return RET;
				EXPR_TWO(/, m_api_intmax, SpvOpSDiv) {}
				return RET;
				EXPR_TWO(%, m_api_intmax, SpvOpSRem) {}
				return RET;

				default:
				RET.m_found = 0;
				return RET;
			}

		case ID_DEFAULT_F32:
			switch((uintmax_t)c_opcode) {
				case SpvOpFAdd:
					RET.m_ret.m_flt
						= c_oprndA.m_flt + c_oprndB.m_flt;
					RET.m_found = 1;
					return RET;
				default:
					RET.m_found = 0;
					return RET;
			}


		case ID_DEFAULT_F64:
			switch((uintmax_t)c_opcode) {
				case SpvOpFAdd:
					RET.m_ret.m_dbl
						= c_oprndA.m_dbl + c_oprndB.m_dbl;
					RET.m_found = 1;
					return RET;
				default:
					RET.m_found = 0;
					return RET;
			}
			break;

		default:
			break;
	}

	RET.m_found = 0;

#undef	EXPR_TWO
	return RET;
}

#endif
