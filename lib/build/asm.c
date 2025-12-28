#include <build.h>
#include "./wrdemit.h"

#include <spirv/1.0/spirv.h>

ACLSPV_ABI_IMPL ae2f_noexcept e_fn_aclspv_pass	aclspv_build_asm(
		const LLVMModuleRef		M,
		const h_aclspv_build_ctx_t	CTX
		)
{
	IGNORE(M);

	CTX->m_retcount
		= CTX->m_count.m_capability
		+ CTX->m_count.m_ext
		+ CTX->m_count.m_memmodel
		+ CTX->m_count.m_entp
		+ CTX->m_count.m_execmode
		+ CTX->m_count.m_name
		+ CTX->m_count.m_decorate
		+ CTX->m_count.m_types
		+ CTX->m_count.m_vars
		+ CTX->m_count.m_entpdef
		+ 5;

	_aclspv_grow_vec(malloc, _free, CTX->m_ret
			, (size_t)count_to_sz(CTX->m_retcount)
			);

	unless(CTX->m_ret.m_p)		return FN_ACLSPV_PASS_ALLOC_FAILED;

	get_wrd_of_vec(&CTX->m_ret)[0] = SpvMagicNumber;
	get_wrd_of_vec(&CTX->m_ret)[1] = SpvVersion;
	get_wrd_of_vec(&CTX->m_ret)[2] = 0;
	get_wrd_of_vec(&CTX->m_ret)[3] = CTX->m_retcount;
	get_wrd_of_vec(&CTX->m_ret)[4] = 0;

#define	retbuf ((aclspv_wrd_t* ae2f_restrict)(CTX->m_ret.m_p))

#define	cpypad	retbuf + 5
#define	cpysz	count_to_sz(CTX->m_count.m_capability)

	assert(CTX->m_count.m_capability);
	assert((0[(aclspv_wrd_t* ae2f_restrict)lib_build_ctx_section_capability(*CTX).m_p] & SpvOpCapability) == SpvOpCapability);
	assert(0[(aclspv_wrd_t* ae2f_restrict)lib_build_ctx_section_capability(*CTX).m_p] == opcode_to_wrd(SpvOpCapability, 1));

	memcpy(cpypad, lib_build_ctx_section_capability(*CTX).m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)
#define	cpysz	count_to_sz(CTX->m_count.m_ext)
	memcpy(cpypad, lib_build_ctx_section_ext(*CTX).m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)	\
	+ (CTX->m_count.m_ext)
#define	cpysz	count_to_sz(CTX->m_count.m_memmodel)
	memcpy(cpypad, lib_build_ctx_section_memmodel(*CTX).m_p, (size_t)cpysz);


#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)	\
	+ (CTX->m_count.m_ext)		\
	+ (CTX->m_count.m_memmodel)
#define	cpysz	count_to_sz(CTX->m_count.m_entp)
	memcpy(cpypad, CTX->m_section.m_entp.m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)	\
	+ (CTX->m_count.m_ext)		\
	+ (CTX->m_count.m_memmodel)		\
	+ (CTX->m_count.m_entp)
#define	cpysz	count_to_sz(CTX->m_count.m_execmode)
	memcpy(cpypad, (CTX)->m_section.m_execmode.m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)	\
	+ (CTX->m_count.m_ext)		\
	+ (CTX->m_count.m_memmodel)		\
	+ (CTX->m_count.m_entp)		\
	+ (CTX->m_count.m_execmode)
#define	cpysz	count_to_sz(CTX->m_count.m_name)
	memcpy(cpypad, (CTX)->m_section.m_name.m_p, (size_t)cpysz);


#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)	\
	+ (CTX->m_count.m_ext)		\
	+ (CTX->m_count.m_memmodel)		\
	+ (CTX->m_count.m_entp)		\
	+ (CTX->m_count.m_execmode)		\
	+ (CTX->m_count.m_name)
#define	cpysz	count_to_sz(CTX->m_count.m_decorate)
	memcpy(cpypad, (CTX)->m_section.m_decorate.m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)	\
	+ (CTX->m_count.m_ext)		\
	+ (CTX->m_count.m_memmodel)		\
	+ (CTX->m_count.m_entp)		\
	+ (CTX->m_count.m_execmode)		\
	+ (CTX->m_count.m_name)		\
	+ (CTX->m_count.m_decorate)
#define	cpysz	count_to_sz(CTX->m_count.m_types)
	memcpy(cpypad, (CTX)->m_section.m_types.m_p, (size_t)cpysz);



#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)	\
	+ (CTX->m_count.m_ext)		\
	+ (CTX->m_count.m_memmodel)		\
	+ (CTX->m_count.m_entp)		\
	+ (CTX->m_count.m_execmode)		\
	+ (CTX->m_count.m_name)		\
	+ (CTX->m_count.m_decorate)		\
	+ (CTX->m_count.m_types)
#define	cpysz	count_to_sz(CTX->m_count.m_vars)
	memcpy(cpypad, (CTX)->m_section.m_vars.m_p, (size_t)cpysz);

#undef	cpypad
#undef	cpysz
#define	cpypad	retbuf + 5 \
	+ (CTX->m_count.m_capability)	\
	+ (CTX->m_count.m_ext)		\
	+ (CTX->m_count.m_memmodel)		\
	+ (CTX->m_count.m_entp)		\
	+ (CTX->m_count.m_execmode)		\
	+ (CTX->m_count.m_name)		\
	+ (CTX->m_count.m_decorate)		\
	+ (CTX->m_count.m_types)		\
	+ (CTX->m_count.m_vars)
#define	cpysz	count_to_sz(CTX->m_count.m_entpdef)
	memcpy(cpypad, (CTX)->m_section.m_entpdef.m_p, (size_t)cpysz);

#undef	retbuf

	assert(CTX->m_ret.m_p);

	return FN_ACLSPV_PASS_OK;
}
