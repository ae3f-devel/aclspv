
#include <util/emitx.h>
#include <util/id.h>

#include <aclspv.h>
#include <aclspv/abi.h>

#include <spirv/unified1/spirv.h>

#define	wrd_caps	h_ctx->m_section.m_capability 
#define wrd_caps_count	h_ctx->m_count.m_capability

#define	wrd_ext		h_ctx->m_section.m_ext
#define	wrd_ext_count	h_ctx->m_count.m_ext

ae2f_inline static e_aclspv_compile_t impl_conf(
		const h_util_ctx_t		h_ctx
		)
{
	aclspv_wrd_t	POS = 0;

	assert(h_ctx);
	h_ctx->m_id = ID_DEFAULT_END;
	
	h_ctx->m_count.m_capability	= 0;
	h_ctx->m_count.m_ext		= 0;
	h_ctx->m_count.m_memmodel	= 0;
	h_ctx->m_count.m_entp		= 0;
	h_ctx->m_count.m_execmode	= 0;
	h_ctx->m_count.m_name		= 0;
	h_ctx->m_count.m_decorate	= 0;
	h_ctx->m_count.m_types		= 0;
	h_ctx->m_count.m_vars		= 0;
	h_ctx->m_count.m_fndef		= 0;

	/*** Shader Capability **/

	ae2f_expected_but_else(wrd_caps_count = util_emitx_2(
				&wrd_caps
				, wrd_caps_count
				, SpvOpCapability
				, SpvCapabilityShader))
		return ACLSPV_COMPILE_ALLOC_FAILED;

	unless(h_ctx->m_is_logical) {
		ae2f_expected_but_else(wrd_caps_count = util_emitx_2(
					&wrd_caps
					, wrd_caps_count
					, SpvOpCapability
					, SpvCapabilityAddresses))
			return ACLSPV_COMPILE_ALLOC_FAILED;
	}

	unless(h_ctx->m_is_for_gl) {
		ae2f_expected_but_else(wrd_caps_count = util_emitx_2(
					&wrd_caps
					, wrd_caps_count
					, SpvOpCapability
					, SpvCapabilityVulkanMemoryModel))
			return ACLSPV_COMPILE_ALLOC_FAILED;
	}

	/*** Extension Default **/
	POS = wrd_ext_count;
	unless(wrd_ext_count = emit_opcode(&wrd_ext, wrd_ext_count, SpvOpExtension, 0))
		return ACLSPV_COMPILE_ALLOC_FAILED;
	unless(wrd_ext_count = util_emit_str(&wrd_ext, wrd_ext_count, "SPV_KHR_storage_buffer_storage_class"))
		return ACLSPV_COMPILE_ALLOC_FAILED;
	set_oprnd_count_for_opcode(get_wrd_of_vec(&wrd_ext)[POS], (aclspv_wrd_t)(wrd_ext_count - POS - 1));

	/*** Memory Model **/
	ae2f_expected_but_else(h_ctx->m_count.m_memmodel = util_emitx_3(
				&h_ctx->m_section.m_memmodel
				, 0, SpvOpMemoryModel
				, h_ctx->m_is_logical 
				? SpvAddressingModelLogical
				: h_ctx->m_is_buffer_64 ? SpvAddressingModelPhysical64 : SpvAddressingModelPhysical32 
				, h_ctx->m_is_for_gl ? SpvMemoryModelGLSL450 : SpvMemoryModelVulkan))
		return ACLSPV_COMPILE_ALLOC_FAILED;

	return ACLSPV_COMPILE_OK;
}
