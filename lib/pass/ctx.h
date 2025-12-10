#ifndef	aclspv_pass_ctx_h
#define	aclspv_pass_ctx_h

#include <aclspv/pass.h>
#include "./ctx.auto.h"
#include <ae2f/c90/Limits.h>
#include <ae2f/c90/StdBool.h>
#include <ae2f/c90/StdInt.h>

/** 
 * @def		IMPL_PASS_RET 
 * @brief	utility to setup pass definition
 * */
#define IMPL_PASS_RET	ACLSPV_ABI_IMPL ae2f_noexcept e_fn_aclspv_pass

/**
 * @def		IGNORE
 * @brief	ignore `x`
 * */
#define IGNORE(x)	(void)(x)

typedef struct {
	void* ae2f_restrict 	
		m_p;
	size_t	m_sz;
} x_aclspv_vec;

struct x_aclspv_pass_ctx {
	x_aclspv_vec	m_v0, m_v1, m_v2;
};

#endif
