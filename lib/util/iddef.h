/** @file id.h @brief utility functions for id.h */

#ifndef	util_iddef_h
#define util_iddef_h

#include <assert.h>

#include <ae2f/LangVer.h>

#include <spirv/unified1/spirv.h>

#include "./id.h"
#include "./ctx.h"
#include "./emitx.h"
#include "./constant.h"

ae2f_WhenCXX(ae2f_constexpr) ae2f_WhenC(ae2f_inline ae2f_ccconst static) unsigned	
util_default_is_unsigned(const e_id_default c_id_default) {
	(void)c_id_default;
	return 0;
}

ae2f_WhenCXX(ae2f_constexpr) ae2f_WhenC(ae2f_inline ae2f_ccconst static) unsigned	
util_default_is_signed(const e_id_default c_id_default) {
	switch(c_id_default) {
		case ID_DEFAULT_I32:
		case ID_DEFAULT_I8:
		case ID_DEFAULT_I16:
		case ID_DEFAULT_I64:
			return 1;

		case ID_DEFAULT_F16:
		case ID_DEFAULT_F32:
		case ID_DEFAULT_F64:
		case ID_DEFAULT_END:
		case ID_DEFAULT_VOID:
		case ID_DEFAULT_FN_VOID:
		case ID_DEFAULT_I8_PTR_FUNC:
		case ID_DEFAULT_F16_PTR_FUNC:
		case ID_DEFAULT_F32_PTR_FUNC:
		case ID_DEFAULT_F64_PTR_FUNC:
		case ID_DEFAULT_I32_PTR_FUNC:
		case ID_DEFAULT_I16_PTR_FUNC:
		case ID_DEFAULT_I64_PTR_FUNC:
		case ID_DEFAULT_I32V4_PTR_INP:
		case ID_DEFAULT_I32V4_PTR_OUT:
		default:
			return 0;
	}
}

ae2f_WhenCXX(ae2f_constexpr) ae2f_WhenC(ae2f_inline ae2f_ccconst static) unsigned	
util_default_is_int(const e_id_default c_id_default) {
	return util_default_is_unsigned(c_id_default) || util_default_is_signed(c_id_default);
}

ae2f_WhenCXX(ae2f_constexpr) ae2f_WhenC(ae2f_inline ae2f_ccconst static) unsigned	
util_default_is_float(const e_id_default c_id_default) {
	switch(c_id_default) {
		case ID_DEFAULT_F16:
		case ID_DEFAULT_F32:
		case ID_DEFAULT_F64:
			return 1;

		case ID_DEFAULT_I8:
		case ID_DEFAULT_I16:
		case ID_DEFAULT_I32:
		case ID_DEFAULT_I64:
		case ID_DEFAULT_END:
		case ID_DEFAULT_VOID:
		case ID_DEFAULT_FN_VOID:
		case ID_DEFAULT_I8_PTR_FUNC:
		case ID_DEFAULT_F16_PTR_FUNC:
		case ID_DEFAULT_F32_PTR_FUNC:
		case ID_DEFAULT_F64_PTR_FUNC:
		case ID_DEFAULT_I16_PTR_FUNC:
		case ID_DEFAULT_I32_PTR_FUNC:
		case ID_DEFAULT_I64_PTR_FUNC:
		case ID_DEFAULT_I32V4_PTR_INP:
		case ID_DEFAULT_I32V4_PTR_OUT:
		default:
			return 0;
	}
}

ae2f_WhenCXX(ae2f_constexpr) ae2f_WhenC(ae2f_inline ae2f_ccconst static) unsigned	
util_is_default(const e_id_default c_id_default) {
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
		case ID_DEFAULT_FN_VOID:
		case ID_DEFAULT_I8_PTR_FUNC:
		case ID_DEFAULT_F16_PTR_FUNC:
		case ID_DEFAULT_F32_PTR_FUNC:
		case ID_DEFAULT_F64_PTR_FUNC:
		case ID_DEFAULT_I16_PTR_FUNC:
		case ID_DEFAULT_I32_PTR_FUNC:
		case ID_DEFAULT_I64_PTR_FUNC:
		case ID_DEFAULT_I32V4_PTR_INP:
		case ID_DEFAULT_I32V4_PTR_OUT:
			return 1;
		default:
			return 0;
	}
}

ae2f_WhenCXX(ae2f_constexpr) ae2f_WhenC(ae2f_inline ae2f_ccconst static) unsigned	
util_default_is_number(const e_id_default c_id_default) {
	return util_default_is_int(c_id_default) || util_default_is_float(c_id_default);
}

ae2f_WhenCXX(ae2f_constexpr) ae2f_WhenC(ae2f_inline ae2f_ccconst static) e_id_default
util_default_float(unsigned bit_width) {
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


ae2f_WhenCXX(ae2f_constexpr) ae2f_WhenC(ae2f_inline ae2f_ccconst static) e_id_default
util_default_unsigned(unsigned bit_width) {
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

ae2f_WhenCXX(ae2f_constexpr) ae2f_WhenC(ae2f_inline ae2f_ccconst static) unsigned	
util_default_bit_width(const e_id_default c_id_default) {
	switch(c_id_default) {
		case ID_DEFAULT_I8:
			return 8;

		case ID_DEFAULT_I16:
		case ID_DEFAULT_F16:
			return 16;

		case ID_DEFAULT_I32:
		case ID_DEFAULT_F32:
			return 32;

		case ID_DEFAULT_F64:
		case ID_DEFAULT_I64:
			return 64;

		case ID_DEFAULT_END:
		case ID_DEFAULT_VOID:
		case ID_DEFAULT_FN_VOID:
		case ID_DEFAULT_I8_PTR_FUNC:
		case ID_DEFAULT_F16_PTR_FUNC:
		case ID_DEFAULT_F32_PTR_FUNC:
		case ID_DEFAULT_F64_PTR_FUNC:
		case ID_DEFAULT_I16_PTR_FUNC:
		case ID_DEFAULT_I32_PTR_FUNC:
		case ID_DEFAULT_I64_PTR_FUNC:
		case ID_DEFAULT_I32V4_PTR_INP:
		case ID_DEFAULT_I32V4_PTR_OUT:
		default:
			return 0;
	}
}


ae2f_WhenCXX(ae2f_constexpr) ae2f_WhenC(ae2f_inline ae2f_ccconst static) unsigned	
util_default_is_arithmetic(const aclspv_id_t c_id) {
	return util_default_is_number(c_id);
}

/**
 * @fn		util_default_decide_cast
 * @returns	0 when invalid
 * */
ae2f_WhenCXX(ae2f_constexpr) ae2f_WhenC(ae2f_inline ae2f_ccconst static) e_id_default
util_default_decide_cast(const e_id_default c_id0, const e_id_default c_id1) {
	/** one is 0? */
	unless(c_id0 && c_id1)
		return c_id0 ? c_id0 : c_id1;

	unless(util_default_is_arithmetic(c_id0) && util_default_is_arithmetic(c_id1))
		return 0;

	unless(util_default_is_number(c_id0) && util_default_is_number(c_id1))
		return 0;

	if(util_default_is_float(c_id0) != util_default_is_float(c_id1))
		return util_default_is_float(c_id0) ? c_id0 : c_id1;

	return util_default_bit_width(c_id0) > util_default_bit_width(c_id1)
		? c_id0 : c_id1;
}

ae2f_inline static aclspv_id_t util_mk_default_id(
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

			{
				aclspv_wrd_t	STORAGE_CLASS;
				aclspv_id_t	I32V4;

				ae2f_unexpected_but_if(0) {
					case ID_DEFAULT_I32V4_PTR_INP:
						STORAGE_CLASS = SpvStorageClassInput;
				}

				ae2f_unexpected_but_if(0) {
					case ID_DEFAULT_I32V4_PTR_OUT:
						STORAGE_CLASS = SpvStorageClassOutput;
				}

				ae2f_expected_but_else(I32V4 = util_mk_constant_vec32_id(4, CTX)) {
					return 0;
				}

				ae2f_expected_but_else(ret_count = util_emitx_type_pointer(
							&CTX->m_section.m_types
							, ret_count
							, c_id_default
							, STORAGE_CLASS
							, I32V4)) 
					return 0;
			} break;


		case ID_DEFAULT_VOID:
			/** OpTypeVoid */
			ae2f_expected_but_else((ret_count = emit_opcode(&CTX->m_section.m_types, ret_count, SpvOpTypeVoid, 1)))
				return 0;
			ae2f_expected_but_else((ret_count = util_emit_wrd(&CTX->m_section.m_types, ret_count, ID_DEFAULT_VOID))) 
				return 0;
			break;

		case ID_DEFAULT_I64_PTR_FUNC:
			ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_I64, CTX))
				return 0;
			ae2f_expected_but_else(ret_count = util_emitx_type_pointer(
						&CTX->m_section.m_types
						, ret_count
						, ID_DEFAULT_I64_PTR_FUNC
						, SpvStorageClassFunction
						, ID_DEFAULT_I64)) return 0;
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

		case ID_DEFAULT_I32_PTR_FUNC:
			ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_I32, CTX))
				return 0;
			ae2f_expected_but_else(ret_count = util_emitx_type_pointer(
						&CTX->m_section.m_types
						, ret_count
						, ID_DEFAULT_I32_PTR_FUNC
						, SpvStorageClassFunction
						, ID_DEFAULT_I32)) return 0;
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


		case ID_DEFAULT_I16_PTR_FUNC:
			ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_I16, CTX))
				return 0;
			ae2f_expected_but_else(ret_count = util_emitx_type_pointer(
						&CTX->m_section.m_types
						, ret_count
						, ID_DEFAULT_I16_PTR_FUNC
						, SpvStorageClassFunction
						, ID_DEFAULT_I16)) return 0;
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

		case ID_DEFAULT_I8_PTR_FUNC:
			ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_I8, CTX))
				return 0;
			ae2f_expected_but_else(ret_count = util_emitx_type_pointer(
						&CTX->m_section.m_types
						, ret_count
						, ID_DEFAULT_I8_PTR_FUNC
						, SpvStorageClassFunction
						, ID_DEFAULT_I8)) return 0;
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

		case ID_DEFAULT_F16_PTR_FUNC:
			ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_F16, CTX))
				return 0;
			ae2f_expected_but_else(ret_count = util_emitx_type_pointer(
						&CTX->m_section.m_types
						, ret_count
						, ID_DEFAULT_F16_PTR_FUNC
						, SpvStorageClassFunction
						, ID_DEFAULT_F16)) return 0;
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

		case ID_DEFAULT_F32_PTR_FUNC:
			ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_F32, CTX))
				return 0;
			ae2f_expected_but_else(ret_count = util_emitx_type_pointer(
						&CTX->m_section.m_types
						, ret_count
						, ID_DEFAULT_F32_PTR_FUNC
						, SpvStorageClassFunction
						, ID_DEFAULT_F32)) return 0;
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

		case ID_DEFAULT_F64_PTR_FUNC:
			ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_F64, CTX))
				return 0;
			ae2f_expected_but_else(ret_count = util_emitx_type_pointer(
						&CTX->m_section.m_types
						, ret_count
						, ID_DEFAULT_F64_PTR_FUNC
						, SpvStorageClassFunction
						, ID_DEFAULT_F64)) return 0;
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

		case ID_DEFAULT_FN_VOID:
			/** OpTypeFunction %void () */
			ae2f_expected_but_else(ret_count = util_emitx_3(
						&CTX->m_section.m_types
						, ret_count
						, SpvOpTypeFunction
						, ID_DEFAULT_FN_VOID
						, ID_DEFAULT_VOID)) return 0;
#define EMIT_POS	CTX->m_count.m_name
			{
				const aclspv_wrd_t	POS = EMIT_POS;
				unless((EMIT_POS = emit_opcode(&CTX->m_section.m_name, EMIT_POS, SpvOpName, 0)))
					return 0;
				unless((EMIT_POS = util_emit_wrd(&CTX->m_section.m_name, EMIT_POS, ID_DEFAULT_FN_VOID)))
					return 0;
				unless((EMIT_POS = util_emit_str(&CTX->m_section.m_name, EMIT_POS, "::fn_void")))
					return 0;
				set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_section.m_name)[POS], EMIT_POS - POS - 1);
			}
#undef	EMIT_POS
			break;
	}
#undef	ret_count
#undef	CTX

	return h_ctx->m_id_defaults[c_id_default] = c_id_default;
}

#endif
