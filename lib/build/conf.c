#include <build.h>

#include "./wrdemit.h"
#include "./id.h"

#include <spirv/unified1/spirv.h>

#define	wrd_caps	lib_build_ctx_section_capability(*CTX)
#define wrd_caps_count	CTX->m_count.m_capability

#define	wrd_ext		lib_build_ctx_section_ext(*CTX)
#define	wrd_ext_count	CTX->m_count.m_ext

ACLSPV_ABI_IMPL ae2f_noexcept e_fn_aclspv_pass aclspv_build_conf(
		const LLVMModuleRef		M,
		const h_aclspv_build_ctx_t	CTX
		)
{
	IGNORE(M);
	CTX->m_id = ID_DEFAULT_END;
	
	CTX->m_count.m_capability	= 0;
	CTX->m_count.m_ext		= 0;
	CTX->m_count.m_memmodel		= 0;
	CTX->m_count.m_entp		= 0;
	CTX->m_count.m_execmode		= 0;
	CTX->m_count.m_name		= 0;
	CTX->m_count.m_decorate		= 0;
	CTX->m_count.m_types		= 0;
	CTX->m_count.m_vars		= 0;
	CTX->m_count.m_entpdef		= 0;

	/*** Shader Capability **/

	unless(wrd_caps_count = emit_opcode(&wrd_caps, 0, SpvOpCapability, 1))
		return FN_ACLSPV_PASS_ALLOC_FAILED;

	assert((0[(aclspv_wrd_t* ae2f_restrict)lib_build_ctx_section_capability(*CTX).m_p] & SpvOpCapability) == SpvOpCapability);

	unless(wrd_caps_count = emit_wrd(&wrd_caps, wrd_caps_count, SpvCapabilityShader))
		return FN_ACLSPV_PASS_ALLOC_FAILED;
	assert(wrd_caps.m_p);

	assert(0[(aclspv_wrd_t* ae2f_restrict)wrd_caps.m_p]);

	/*** Extension Default **/
	unless(wrd_ext_count = emit_opcode(&wrd_ext, 0, SpvOpExtension, 0))
		return FN_ACLSPV_PASS_ALLOC_FAILED;
	unless(wrd_ext_count = emit_str(&wrd_ext, wrd_ext_count, "SPV_KHR_storage_buffer_storage_class"))
		return FN_ACLSPV_PASS_ALLOC_FAILED;

	set_oprnd_count_for_opcode(get_wrd_of_vec(&wrd_ext)[0], (aclspv_wrd_t)(wrd_ext_count - 1));

	/*** Memory Model **/
	unless((CTX->m_count.m_memmodel = emit_opcode(
					&lib_build_ctx_section_memmodel(*CTX)
					, 0, SpvOpMemoryModel, 2)))
		return FN_ACLSPV_PASS_ALLOC_FAILED;

	unless((CTX->m_count.m_memmodel = emit_wrd(
					&lib_build_ctx_section_memmodel(*CTX)
					, CTX->m_count.m_memmodel, SpvAddressingModelLogical 
					)))
		return FN_ACLSPV_PASS_ALLOC_FAILED;

	unless((CTX->m_count.m_memmodel = emit_wrd(
					&lib_build_ctx_section_memmodel(*CTX)
					, CTX->m_count.m_memmodel, SpvMemoryModelGLSL450 
					)))
		return FN_ACLSPV_PASS_ALLOC_FAILED;

	return FN_ACLSPV_PASS_OK;
}
