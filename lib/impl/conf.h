
#include <util/wrdemit.h>
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

	unless(wrd_caps_count = emit_opcode(&wrd_caps, 0, SpvOpCapability, 1))
		return ACLSPV_COMPILE_ALLOC_FAILED;

	assert((0[(aclspv_wrd_t* ae2f_restrict)h_ctx->m_section.m_capability.m_p] & SpvOpCapability) == SpvOpCapability);

	unless(wrd_caps_count = emit_wrd(&wrd_caps, wrd_caps_count, SpvCapabilityShader))
		return ACLSPV_COMPILE_ALLOC_FAILED;
	assert(wrd_caps.m_p);

	assert(0[(aclspv_wrd_t* ae2f_restrict)wrd_caps.m_p]);

	/*** Extension Default **/
	unless(wrd_ext_count = emit_opcode(&wrd_ext, 0, SpvOpExtension, 0))
		return ACLSPV_COMPILE_ALLOC_FAILED;
	unless(wrd_ext_count = emit_str(&wrd_ext, wrd_ext_count, "SPV_KHR_storage_buffer_storage_class"))
		return ACLSPV_COMPILE_ALLOC_FAILED;

	set_oprnd_count_for_opcode(get_wrd_of_vec(&wrd_ext)[0], (aclspv_wrd_t)(wrd_ext_count - 1));

	/*** Memory Model **/
	unless((h_ctx->m_count.m_memmodel = emit_opcode(
					&h_ctx->m_section.m_memmodel 
					, 0, SpvOpMemoryModel, 2)))
		return ACLSPV_COMPILE_ALLOC_FAILED;

	unless((h_ctx->m_count.m_memmodel = emit_wrd(
					&h_ctx->m_section.m_memmodel
					, h_ctx->m_count.m_memmodel, SpvAddressingModelLogical 
					)))
		return ACLSPV_COMPILE_ALLOC_FAILED;

	unless((h_ctx->m_count.m_memmodel = emit_wrd(
					&h_ctx->m_section.m_memmodel
					, h_ctx->m_count.m_memmodel, SpvMemoryModelGLSL450 
					)))
		return ACLSPV_COMPILE_ALLOC_FAILED;

	return ACLSPV_COMPILE_OK;
}
