#ifndef	util_iddef_h
#define util_iddef_h

#include <assert.h>

#include <ae2f/LangVer.h>

#include <spirv/unified1/spirv.h>

#include "./id.h"
#include "./ctx.h"
#include "./emitx.h"

ae2f_WhenCXX(ae2f_constexpr) ae2f_WhenC(ae2f_inline ae2f_ccconst static) unsigned	
util_is_default_id_int(const e_id_default c_id_default) {
	if(c_id_default == ID_DEFAULT_U8)	return 1;
	if(c_id_default == ID_DEFAULT_U16)	return 1;
	if(c_id_default == ID_DEFAULT_U32)	return 1;

	return 0;
}

ae2f_inline static aclspv_id_t util_get_default_id(
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
			ae2f_expected_but_else((ret_count = emit_opcode(&CTX->m_section.m_types, ret_count, SpvOpTypeVoid, 1)))
				return 0;
			ae2f_expected_but_else((ret_count = util_emit_wrd(&CTX->m_section.m_types, ret_count, ID_DEFAULT_VOID))) 
				return 0;
			break;

		case ID_DEFAULT_U32:
			ae2f_expected_but_else((ret_count = emit_opcode(
							&CTX->m_section.m_types
							, ret_count, SpvOpTypeInt, 3)))
				return 0;
			ae2f_expected_but_else((ret_count = util_emit_wrd(&CTX->m_section.m_types, ret_count, ID_DEFAULT_U32))) 
				return 0;
			ae2f_expected_but_else((ret_count = util_emit_wrd(&CTX->m_section.m_types, ret_count, 32))) 
				return 0;
			/** unsigned */
			ae2f_expected_but_else((ret_count = util_emit_wrd(&CTX->m_section.m_types, ret_count, 0))) 
				return 0;
			break;

		case ID_DEFAULT_I32:
			ae2f_expected_but_else((ret_count = emit_opcode(
							&CTX->m_section.m_types
							, ret_count, SpvOpTypeInt, 3)))
				return 0;
			ae2f_expected_but_else((ret_count = util_emit_wrd(&CTX->m_section.m_types, ret_count, ID_DEFAULT_I32))) 
				return 0;
			ae2f_expected_but_else((ret_count = util_emit_wrd(&CTX->m_section.m_types, ret_count, 32))) 
				return 0;
			/** signed */
			ae2f_expected_but_else((ret_count = util_emit_wrd(&CTX->m_section.m_types, ret_count, 1))) 
				return 0;
			break;

		case ID_DEFAULT_U16:
			ae2f_expected_but_else((ret_count = emit_opcode(&CTX->m_section.m_types, ret_count, SpvOpTypeInt, 3))) 
				return 0;
			ae2f_expected_but_else((ret_count = util_emit_wrd(&CTX->m_section.m_types, ret_count, ID_DEFAULT_U16))) 
				return 0;
			ae2f_expected_but_else((ret_count = util_emit_wrd(&CTX->m_section.m_types, ret_count, 16))) 
				return 0;
			/** unsigned */
			ae2f_expected_but_else((ret_count = util_emit_wrd(&CTX->m_section.m_types, ret_count, 0))) 
				return 0;
			break;

		case ID_DEFAULT_U8:
			ae2f_expected_but_else((ret_count = emit_opcode(&CTX->m_section.m_types, ret_count, SpvOpTypeInt, 3))) 
				return 0;
			ae2f_expected_but_else((ret_count = util_emit_wrd(&CTX->m_section.m_types, ret_count, ID_DEFAULT_U8))) 
				return 0;
			ae2f_expected_but_else((ret_count = util_emit_wrd(&CTX->m_section.m_types, ret_count, 8)))
				return 0;
			/** unsigned */
			ae2f_expected_but_else((ret_count = util_emit_wrd(&CTX->m_section.m_types, ret_count, 0))) 
				return 0;
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
