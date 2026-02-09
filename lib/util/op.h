#ifndef util_op_h
#define util_op_h

enum UTIL_OPBINCFG_ {
	UTIL_OPBINCFG_NUMOPRND,

	/** @see ./literal.h */
	UTIL_OPBINCFG_MASKLITERAL,
	UTIL_OPBINCFG_SPECCONSTANTOP,
	UTIL_OPBINCFG_RESTYID,
	UTIL_OPBINCFG_RESID,
	UTIL_OPBINCFG_OPCODE,
	UTIL_OPBINCFG_END
};

enum UTIL_OPBIN2_ {
	UTIL_OPBIN2_NONE,
	UTIL_OPBIN2_ADD,
	UTIL_OPBIN2_SUB,
	UTIL_OPBIN2_MUL,
	UTIL_OPBIN2_REM,
	UTIL_OPBIN2_DIV
};

#include <ae2f/Keys.h>

#include "./iddef.h"
#include "./log.h"
#include "./literal.h"
#include <spirv/unified1/spirv.h>

typedef struct {
	aclspv_wrd_t	m_type_original;
	aclspv_wrd_t	m_mask_literal;
	util_literal	m_literal;
} util_opbincfg_extra;

static ae2f_ccconst enum SpvOp_ util_opbin2_to_spvop(const e_id_default c_id, const enum UTIL_OPBIN2_ c_op2) {
	switch(c_op2) {
		case UTIL_OPBIN2_NONE:
		default:
			return 0;

		case UTIL_OPBIN2_ADD:
			if(util_default_is_int(c_id))
				return SpvOpIAdd;
			else if(util_default_is_float(c_id))
				return SpvOpFAdd;
			else return 0;
		case UTIL_OPBIN2_DIV:
			if(util_default_is_signed(c_id))
				return SpvOpSDiv;
			else if (util_default_is_unsigned(c_id))
				return SpvOpUDiv;
			else if(util_default_is_float(c_id))
				return SpvOpFDiv;
			else return 0;
		case UTIL_OPBIN2_MUL:
			if(util_default_is_signed(c_id))
				return SpvOpIMul;
			else if (util_default_is_unsigned(c_id))
				return SpvOpIMul;
			else if(util_default_is_float(c_id))
				return SpvOpFMul;
			else return 0;
		case UTIL_OPBIN2_REM:
			if(util_default_is_signed(c_id))
				return SpvOpSRem;
			else if (util_default_is_unsigned(c_id))
				return SpvOpUMod;
			else if(util_default_is_float(c_id))
				return SpvOpFMod;
			else return 0;
		case UTIL_OPBIN2_SUB:
			if(util_default_is_signed(c_id))
				return SpvOpISub;
			else if (util_default_is_unsigned(c_id))
				return SpvOpISub;
			else if(util_default_is_float(c_id))
				return SpvOpFSub;
			else return 0;
	}
}

#endif
