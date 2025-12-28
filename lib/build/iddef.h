#ifndef	lib_build_iddef_h
#define lib_build_iddef_h

#include <build.h>
#include <assert.h>

#include "./id.h"
#include "./wrdemit.h"

#include <spirv/unified1/spirv.h>

ae2f_inline static aclspv_id_t lib_build_get_default_id(
		const e_id_default	c_id_default,
		h_aclspv_build_ctx_t	h_ctx
		) 
{
	assert(h_ctx);

	if(c_id_default == ID_DEFAULT_END) return 0;

	if(h_ctx->m_id_defaults[c_id_default])
		return c_id_default;

	h_ctx->m_id_defaults[c_id_default]
		= c_id_default;

#define CTX		h_ctx
#define	ret_count	h_ctx->m_count.m_types
	switch(c_id_default) {
		default:
		case ID_DEFAULT_END:
			return 0;

		case ID_DEFAULT_VOID:
			/** OpTypeVoid */
			unless((ret_count = emit_opcode(&CTX->m_section.m_types, ret_count, SpvOpTypeVoid, 1)))
				return 0;
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, ID_DEFAULT_VOID))) 
				return 0;
			break;

		case ID_DEFAULT_U32_VEC4:
			/** OpTypeFunction %void () */
			unless((ret_count = emit_opcode(&CTX->m_section.m_types, ret_count, SpvOpTypeVector, 3)))
				return 0;
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, ID_DEFAULT_U32_VEC4)))
				return 0;
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, ID_DEFAULT_U32)))
				return 0;
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, 4)))
				return 0;
			unless(lib_build_get_default_id(ID_DEFAULT_U32, h_ctx))
				return 0;
			break;

		case ID_DEFAULT_U32:
			unless((ret_count = emit_opcode(
							&CTX->m_section.m_types
							, ret_count, SpvOpTypeInt, 3)))
				return 0;
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, ID_DEFAULT_U32))) 
				return 0;
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, 32))) 
				return 0;
			/** unsigned */
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, 0))) 
				return 0;
			break;

		case ID_DEFAULT_U16:
			unless((ret_count = emit_opcode(&CTX->m_section.m_types, ret_count, SpvOpTypeInt, 3))) 
				return 0;
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, ID_DEFAULT_U16))) 
				return 0;
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, 16))) 
				return 0;
			/** unsigned */
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, 0))) 
				return 0;
			break;

		case ID_DEFAULT_U8:
			unless((ret_count = emit_opcode(&CTX->m_section.m_types, ret_count, SpvOpTypeInt, 3))) 
				return 0;
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, ID_DEFAULT_U8))) 
				return 0;
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, 8)))
				return 0;
			/** unsigned */
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, 0))) 
				return 0;
			break;

		case ID_DEFAULT_F32:
			unless((ret_count = emit_opcode(&CTX->m_section.m_types, ret_count, SpvOpTypeFloat, 2))) 
				return 0;
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, ID_DEFAULT_F32))) 
				return 0;
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, 32)))
				return 0;
			break;

		case ID_DEFAULT_FN_VOID:
			/** OpTypeFunction %void () */
			unless((ret_count = emit_opcode(&CTX->m_section.m_types, ret_count, SpvOpTypeFunction, 2)))
				return 0;
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, ID_DEFAULT_FN_VOID)))
				return 0;
			unless((ret_count = emit_wrd(&CTX->m_section.m_types, ret_count, ID_DEFAULT_VOID)))
				return 0;
			break;
	}
#undef	ret_count
#undef	CTX

	return c_id_default;
}

#endif
