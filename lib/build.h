#ifndef	__lib_build_h
#define __lib_build_h

#include <aclspv/build.h>
#include <pass/ctx.h>

#include "./build/id.h"

#define	_free(a, b)	free(a)

typedef struct a_aclspv_build_ctx {

	/**
	 * @var		m_ret
	 * @brief	word count for m_ret
	 * */
	aclspv_wrdcount_t	m_retcount;

	/**
	 * @var		m_id
	 * @brief	id
	 * */
	aclspv_id_t		m_id;
	aclspv_id_t		m_id_defaults[ID_DEFAULT_END];

	struct {
		x_aclspv_vec 
			m_v0;
	}	m_tmp;

	struct {
		aclspv_wrdcount_t
			m_capability,
			m_ext,
			m_memmodel,
			m_entp,
			m_execmode,
			m_name,
			m_decorate,
			m_types,
			m_vars,
			m_entpdef;
	}			m_count;

	struct {
		aclspv_wrdcount_t
			m_num_fn,
			m_num_entp;

		/**
		 * @var		m_fn
		 * @brief	normal functions
		 * @see		lib_build_fn_t
		 * */
		x_aclspv_vec	m_fn;

		/**
		 * @var		m_entp
		 * @brief	entry functions
		 * @see		lib_build_entp_t
		 * */
		x_aclspv_vec	m_entp;
	}			m_fnlist;

	/**
	 * @var		m_section
	 * @brief	each section for m_ret
	 * */
	struct {
		x_aclspv_vec
			m_entp,
			m_execmode,
			m_name,
			m_decorate,
			m_types,
			m_vars,
			m_entpdef;
	}			m_section;

	struct x_aclspv_pass_ctx	m_pass_ctx;
	/**
	 * @var		m_ret
	 * @brief	fully emitted spir-v
	 * */
	x_aclspv_vec			m_ret;
	x_aclspv_vec            m_scale_vars;
	x_aclspv_vec            m_vecid_vars;

	/** cache for elements related to unsigned integer constants [lib_build_constant] */
	x_aclspv_vec		m_constant_cache;
} x_aclspv_build_ctx;

#define	_malloc_with_zero(a)	calloc(a, 1)

/** OpCapability */
#define	lib_build_ctx_section_capability(a)	((a).m_pass_ctx.m_v0)

/** OpExtension */
#define	lib_build_ctx_section_ext(a)		((a).m_pass_ctx.m_v1)

/** OpMemoryModel */
#define	lib_build_ctx_section_memmodel(a)	((a).m_pass_ctx.m_v2)

#endif
