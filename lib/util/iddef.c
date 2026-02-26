/** @file iddef.c */

#include "./iddef.h"
#include "./emitx.h"
#include "./constant.h"
#include "aclspv/spvty.h"
#include "util/id.h"

#include <assert.h>
#include <spirv/unified1/spirv.h>

 ae2f_ccconst aclspv_bool_t aclid_is_unsigned(const e_id_default c_id_default) {
	switch(c_id_default) {
		case ID_DEFAULT_U8:
		case ID_DEFAULT_U16:
		case ID_DEFAULT_U32:
		case ID_DEFAULT_U64:
			return 1;

		case ID_DEFAULT_I32:
		case ID_DEFAULT_I8:
		case ID_DEFAULT_I16:
		case ID_DEFAULT_I64:
		case ID_DEFAULT_F16:
		case ID_DEFAULT_F32:
		case ID_DEFAULT_F64:
		case ID_DEFAULT_END:
		case ID_DEFAULT_VOID:
		default:
			return 0;
	}
	return 0;
}

 ae2f_ccconst aclspv_bool_t aclid_is_signed(const e_id_default c_id_default) {
	switch(c_id_default) {
		case ID_DEFAULT_I32:
		case ID_DEFAULT_I8:
		case ID_DEFAULT_I16:
		case ID_DEFAULT_I64:
			return 1;

		case ID_DEFAULT_F16:
		case ID_DEFAULT_F32:
		case ID_DEFAULT_F64:
		case ID_DEFAULT_U8:
		case ID_DEFAULT_U16:
		case ID_DEFAULT_U32:
		case ID_DEFAULT_U64:
		case ID_DEFAULT_END:
		case ID_DEFAULT_VOID:
		default:
			return 0;
	}
}

 ae2f_ccconst aclspv_bool_t aclid_is_int(const e_id_default c_id_default) {
	return aclid_is_unsigned(c_id_default) || aclid_is_signed(c_id_default);
}

 ae2f_ccconst aclspv_bool_t aclid_is_float(const e_id_default c_id_default) {
	switch(c_id_default) {
		case ID_DEFAULT_F16:
		case ID_DEFAULT_F32:
		case ID_DEFAULT_F64:
			return 1;

		case ID_DEFAULT_I8:
		case ID_DEFAULT_I16:
		case ID_DEFAULT_I32:
		case ID_DEFAULT_I64:
		case ID_DEFAULT_U8:
		case ID_DEFAULT_U16:
		case ID_DEFAULT_U32:
		case ID_DEFAULT_U64:
		case ID_DEFAULT_END:
		case ID_DEFAULT_VOID:
		default:
			return 0;
	}
}

 ae2f_ccconst aclspv_bool_t aclid_is_default(const e_id_default c_id_default) {
	switch(c_id_default) {
		case ID_DEFAULT_F16:
		case ID_DEFAULT_F32:
		case ID_DEFAULT_F64:
		case ID_DEFAULT_I8:
		case ID_DEFAULT_I16:
		case ID_DEFAULT_I32:
		case ID_DEFAULT_I64:
		case ID_DEFAULT_END:
		case ID_DEFAULT_VOID:
		case ID_DEFAULT_U8:
		case ID_DEFAULT_U16:
		case ID_DEFAULT_U32:
		case ID_DEFAULT_U64:
			return 1;
		default:
			return 0;
	}
}

 ae2f_ccconst aclspv_bool_t 
aclid_is_number(const e_id_default c_id_default) {
	return aclid_is_int(c_id_default) || aclid_is_float(c_id_default);
}

 ae2f_ccconst e_id_default aclid_getty_float(unsigned bit_width) {
	switch(bit_width) {
		case 16:
			return ID_DEFAULT_F16;
		case 32:
			return ID_DEFAULT_F32;
		case 64:
			return ID_DEFAULT_F64;
		default:
			return ID_DEFAULT_END;
	}
}

 ae2f_ccconst e_id_default aclid_getty_signed(unsigned bit_width) {
	switch(bit_width) {
		case 8:
			return ID_DEFAULT_I8;
		case 16:
			return ID_DEFAULT_I16;
		case 32:
			return ID_DEFAULT_I32;
		case 64:
			return ID_DEFAULT_I64;
		default:
			return ID_DEFAULT_END;
	}
}

 ae2f_ccconst e_id_default aclid_getty_unsigned(unsigned bit_width) {
	switch(bit_width) {
		case 8:
			return ID_DEFAULT_U8;
		case 16:
			return ID_DEFAULT_U16;
		case 32:
			return ID_DEFAULT_U32;
		case 64:
			return ID_DEFAULT_U64;
		default:
			return ID_DEFAULT_END;
	}
}

 ae2f_ccconst unsigned aclid_get_bit_width(const e_id_default c_id_default) {
	switch(c_id_default) {
		case ID_DEFAULT_I8:
		case ID_DEFAULT_U8:
			return 8;

		case ID_DEFAULT_I16:
		case ID_DEFAULT_U16:
		case ID_DEFAULT_F16:
			return 16;

		case ID_DEFAULT_I32:
		case ID_DEFAULT_U32:
		case ID_DEFAULT_F32:
			return 32;

		case ID_DEFAULT_F64:
		case ID_DEFAULT_I64:
		case ID_DEFAULT_U64:
			return 64;

		case ID_DEFAULT_END:
		case ID_DEFAULT_VOID:
		default:
			return 0;
	}
}


 ae2f_ccconst aclspv_bool_t aclid_is_arithmetic(const aclspv_id_t c_id) {
	return aclid_is_number(c_id);
}

/**
 * @fn		util_default_decide_cast
 * @returns	0 when invalid
 * */
 ae2f_ccconst e_id_default aclid_decide_cast(const e_id_default c_id0, const e_id_default c_id1) {
	/** one is 0? */
	unless(c_id0 && c_id1)
		return c_id0 ? c_id0 : c_id1;

	unless(aclid_is_arithmetic(c_id0) && aclid_is_arithmetic(c_id1))
		return 0;

	unless(aclid_is_number(c_id0) && aclid_is_number(c_id1))
		return 0;

	if(aclid_is_float(c_id0) != aclid_is_float(c_id1))
		return aclid_is_float(c_id0) ? c_id0 : c_id1;

	return aclid_get_bit_width(c_id0) > aclid_get_bit_width(c_id1)
		? c_id0 : c_id1;
}

 aclspv_id_t aclid_mk_default_id(
		const e_id_default	c_id_default,
		h_util_ctx_t	h_ctx
		)
{
	assert(h_ctx);
	ae2f_unexpected_but_if(c_id_default == ID_DEFAULT_END)
		return 0;

	if(h_ctx->m_id_defaults[c_id_default])
		return c_id_default;

#define CTX		h_ctx
#define	ret_count	h_ctx->m_count.m_types
	switch(c_id_default) {
		default:
		case ID_DEFAULT_END:
			ae2f_unreachable();
			return 0;


		case ID_DEFAULT_VOID:
			/** OpTypeVoid */
			ae2f_expected_but_else((ret_count = emit_opcode(
							&CTX->m_section.m_types
							, ret_count
							, SpvOpTypeVoid, 1)))
				return 0;
			ae2f_expected_but_else((ret_count = util_emit_wrd(
							&CTX->m_section.m_types
							, ret_count
							, ID_DEFAULT_VOID
							))) 
				return 0;
			break;

		case ID_DEFAULT_I64:
			ae2f_expected_but_else(
					ret_count = util_emitx_4(
						&CTX->m_section.m_types
						, ret_count
						, SpvOpTypeInt
						, ID_DEFAULT_I64
						, 64, 1))
				return 0;

			ae2f_expected_but_else(
					CTX->m_count.m_capability = util_emitx_2(
						&CTX->m_section.m_capability
						, CTX->m_count.m_capability
						, SpvOpCapability
						, SpvCapabilityInt64
						)) return 0;
			break;

		case ID_DEFAULT_U64:
			ae2f_expected_but_else(
					ret_count = util_emitx_4(
						&CTX->m_section.m_types
						, ret_count
						, SpvOpTypeInt
						, ID_DEFAULT_U64
						, 64, 0))
				return 0;

			ae2f_expected_but_else(
					CTX->m_count.m_capability = util_emitx_2(
						&CTX->m_section.m_capability
						, CTX->m_count.m_capability
						, SpvOpCapability
						, SpvCapabilityInt64
						)) return 0;
			break;
		case ID_DEFAULT_U32:
			ae2f_expected_but_else(
					ret_count = util_emitx_4(
						&CTX->m_section.m_types
						, ret_count
						, SpvOpTypeInt
						, ID_DEFAULT_U32
						, 32, 0))
				return 0;
			break;

		case ID_DEFAULT_I32:
			ae2f_expected_but_else(
					ret_count = util_emitx_4(
						&CTX->m_section.m_types
						, ret_count
						, SpvOpTypeInt
						, ID_DEFAULT_I32
						, 32, 1))
				return 0;
			break;

		case ID_DEFAULT_U16:
			ae2f_expected_but_else(
					ret_count = util_emitx_4(
						&CTX->m_section.m_types
						, ret_count
						, SpvOpTypeInt
						, ID_DEFAULT_U16
						, 16, 0))
				return 0;

			ae2f_expected_but_else(
					CTX->m_count.m_capability = util_emitx_2(
						&CTX->m_section.m_capability
						, CTX->m_count.m_capability
						, SpvOpCapability
						, SpvCapabilityInt16
						)) return 0;

			break;

		case ID_DEFAULT_I16:
			ae2f_expected_but_else(
					ret_count = util_emitx_4(
						&CTX->m_section.m_types
						, ret_count
						, SpvOpTypeInt
						, ID_DEFAULT_I16
						, 16, 1))
				return 0;

			ae2f_expected_but_else(
					CTX->m_count.m_capability = util_emitx_2(
						&CTX->m_section.m_capability
						, CTX->m_count.m_capability
						, SpvOpCapability
						, SpvCapabilityInt16
						)) return 0;

			break;

		case ID_DEFAULT_U8:
			ae2f_expected_but_else(
					ret_count = util_emitx_4(
						&CTX->m_section.m_types
						, ret_count
						, SpvOpTypeInt
						, ID_DEFAULT_U8
						, 8, 0))
				return 0;

			ae2f_expected_but_else(
					CTX->m_count.m_capability = util_emitx_2(
						&CTX->m_section.m_capability
						, CTX->m_count.m_capability
						, SpvOpCapability
						, SpvCapabilityInt8
						)) return 0;
			break;

		case ID_DEFAULT_I8:
			ae2f_expected_but_else(
					ret_count = util_emitx_4(
						&CTX->m_section.m_types
						, ret_count
						, SpvOpTypeInt
						, ID_DEFAULT_I8
						, 8, 1))
				return 0;

			ae2f_expected_but_else(
					CTX->m_count.m_capability = util_emitx_2(
						&CTX->m_section.m_capability
						, CTX->m_count.m_capability
						, SpvOpCapability
						, SpvCapabilityInt8
						)) return 0;
			break;

		case ID_DEFAULT_F16:
			ae2f_expected_but_else(ret_count = util_emitx_3(
						&CTX->m_section.m_types
						, ret_count
						, SpvOpTypeFloat
						, ID_DEFAULT_F16
						, 16
						)) return 0;

			ae2f_expected_but_else(
					CTX->m_count.m_capability = util_emitx_2(
						&CTX->m_section.m_capability
						, CTX->m_count.m_capability
						, SpvOpCapability
						, SpvCapabilityFloat16
						)) return 0;
			break;

		case ID_DEFAULT_F32:
			ae2f_expected_but_else(ret_count = util_emitx_3(
						&CTX->m_section.m_types
						, ret_count
						, SpvOpTypeFloat
						, ID_DEFAULT_F32
						, 32
						)) return 0;
			break;

		case ID_DEFAULT_F64:
			ae2f_expected_but_else(ret_count = util_emitx_3(
						&CTX->m_section.m_types
						, ret_count
						, SpvOpTypeFloat
						, ID_DEFAULT_F64
						, 64
						)) return 0;

			ae2f_expected_but_else(
					CTX->m_count.m_capability = util_emitx_2(
						&CTX->m_section.m_capability
						, CTX->m_count.m_capability
						, SpvOpCapability
						, SpvCapabilityFloat64
						)) return 0;
			break;
	}

	return h_ctx->m_id_defaults[c_id_default] = c_id_default;
}
