
#include <spirv/1.0/spirv.h>
#include <aclspv.h>

#include <util/iddef.h>
#include <util/wrdemit.h>

ae2f_inline static ae2f_noexcept e_aclspv_compile_t impl_asm(
		const h_aclspv_build_ctx_t	h_ctx
		)
{
	h_ctx->m_retcount
		= h_ctx->m_count.m_capability
		+ h_ctx->m_count.m_ext
		+ h_ctx->m_count.m_memmodel
		+ h_ctx->m_count.m_entp
		+ h_ctx->m_count.m_execmode
		+ h_ctx->m_count.m_name
		+ h_ctx->m_count.m_decorate
		+ h_ctx->m_count.m_types
		+ h_ctx->m_count.m_vars
		+ h_ctx->m_count.m_entpdef
		+ 5;

	_aclspv_grow_vec(malloc, _free, h_ctx->m_ret
			, (size_t)count_to_sz(h_ctx->m_retcount)
			);

	unless(h_ctx->m_ret.m_p)		return ACLSPV_COMPILE_ALLOC_FAILED;

	get_wrd_of_vec(&h_ctx->m_ret)[0] = SpvMagicNumber;
	get_wrd_of_vec(&h_ctx->m_ret)[1] = SpvVersion;
	get_wrd_of_vec(&h_ctx->m_ret)[2] = 0;
	get_wrd_of_vec(&h_ctx->m_ret)[3] = h_ctx->m_retcount;
	get_wrd_of_vec(&h_ctx->m_ret)[4] = 0;

#define	retbuf ((aclspv_wrd_t* ae2f_restrict)(h_ctx->m_ret.m_p))

#define	cpypad	retbuf + 5
#define	cpysz	count_to_sz(h_ctx->m_count.m_capability)

	assert(h_ctx->m_count.m_capability);
	assert((0[(aclspv_wrd_t* ae2f_restrict)h_ctx->m_section.m_capability.m_p] & SpvOpCapability) == SpvOpCapability);
	assert(0[(aclspv_wrd_t* ae2f_restrict)h_ctx->m_section.m_capability.m_p] == opcode_to_wrd(SpvOpCapability, 1));

	memcpy(cpypad, h_ctx->m_section.m_capability.m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (h_ctx->m_count.m_capability)
#define	cpysz	count_to_sz(h_ctx->m_count.m_ext)
	memcpy(cpypad, h_ctx->m_section.m_ext.m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (h_ctx->m_count.m_capability)	\
	+ (h_ctx->m_count.m_ext)
#define	cpysz	count_to_sz(h_ctx->m_count.m_memmodel)
	memcpy(cpypad, h_ctx->m_section.m_memmodel.m_p, (size_t)cpysz);


#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (h_ctx->m_count.m_capability)	\
	+ (h_ctx->m_count.m_ext)		\
	+ (h_ctx->m_count.m_memmodel)
#define	cpysz	count_to_sz(h_ctx->m_count.m_entp)
	memcpy(cpypad, h_ctx->m_section.m_entp.m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (h_ctx->m_count.m_capability)	\
	+ (h_ctx->m_count.m_ext)		\
	+ (h_ctx->m_count.m_memmodel)		\
	+ (h_ctx->m_count.m_entp)
#define	cpysz	count_to_sz(h_ctx->m_count.m_execmode)
	memcpy(cpypad, (h_ctx)->m_section.m_execmode.m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (h_ctx->m_count.m_capability)	\
	+ (h_ctx->m_count.m_ext)		\
	+ (h_ctx->m_count.m_memmodel)		\
	+ (h_ctx->m_count.m_entp)		\
	+ (h_ctx->m_count.m_execmode)
#define	cpysz	count_to_sz(h_ctx->m_count.m_name)
	memcpy(cpypad, (h_ctx)->m_section.m_name.m_p, (size_t)cpysz);


#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (h_ctx->m_count.m_capability)	\
	+ (h_ctx->m_count.m_ext)		\
	+ (h_ctx->m_count.m_memmodel)		\
	+ (h_ctx->m_count.m_entp)		\
	+ (h_ctx->m_count.m_execmode)		\
	+ (h_ctx->m_count.m_name)
#define	cpysz	count_to_sz(h_ctx->m_count.m_decorate)
	memcpy(cpypad, (h_ctx)->m_section.m_decorate.m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (h_ctx->m_count.m_capability)	\
	+ (h_ctx->m_count.m_ext)		\
	+ (h_ctx->m_count.m_memmodel)		\
	+ (h_ctx->m_count.m_entp)		\
	+ (h_ctx->m_count.m_execmode)		\
	+ (h_ctx->m_count.m_name)		\
	+ (h_ctx->m_count.m_decorate)
#define	cpysz	count_to_sz(h_ctx->m_count.m_types)
	memcpy(cpypad, (h_ctx)->m_section.m_types.m_p, (size_t)cpysz);



#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (h_ctx->m_count.m_capability)	\
	+ (h_ctx->m_count.m_ext)		\
	+ (h_ctx->m_count.m_memmodel)		\
	+ (h_ctx->m_count.m_entp)		\
	+ (h_ctx->m_count.m_execmode)		\
	+ (h_ctx->m_count.m_name)		\
	+ (h_ctx->m_count.m_decorate)		\
	+ (h_ctx->m_count.m_types)
#define	cpysz	count_to_sz(h_ctx->m_count.m_vars)
	memcpy(cpypad, (h_ctx)->m_section.m_vars.m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (h_ctx->m_count.m_capability)	\
	+ (h_ctx->m_count.m_ext)		\
	+ (h_ctx->m_count.m_memmodel)		\
	+ (h_ctx->m_count.m_entp)		\
	+ (h_ctx->m_count.m_execmode)		\
	+ (h_ctx->m_count.m_name)		\
	+ (h_ctx->m_count.m_decorate)		\
	+ (h_ctx->m_count.m_types)		\
	+ (h_ctx->m_count.m_vars)
#define	cpysz	count_to_sz(h_ctx->m_count.m_entpdef)
	memcpy(cpypad, (h_ctx)->m_section.m_entpdef.m_p, (size_t)cpysz);

#undef	retbuf

	assert(h_ctx->m_ret.m_p);

	return ACLSPV_COMPILE_OK;
}
