#ifndef lib_build_ctx_h
#define lib_build_ctx_h

#include "../vec.auto.h"
#include "./id.h"

#include <aclspv.h>
#include <aclspv/spvty.h>

#define	_free(a, b)	free(a)

typedef struct a_aclspv_ctx {
	e_aclspv_compile_t	m_state;

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
		aclspv_wrd_t	m_w0;
		aclspv_wrd_t	m_w1;
		aclspv_wrd_t	m_w2;
		aclspv_wrd_t	m_w3;
		aclspv_wrd_t	m_w4;

		x_aclspv_vec	m_v0;
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
			m_fndef;
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
			m_capability,
			m_ext,
			m_memmodel,
			m_entp,
			m_execmode,
			m_name,
			m_decorate,
			m_types,
			m_vars,
			m_fndef;
	}			m_section;

	/**
	 * @var		m_ret
	 * @brief	fully emitted spir-v
	 * */
	x_aclspv_vec		m_ret;
	x_aclspv_vec            m_scale_vars;
#if 1
	x_aclspv_vec            m_vecid_vars;
#endif

	/** cache for elements related to unsigned integer constants [lib_build_constant] */
	x_aclspv_vec		m_constant_cache;
} x_aclspv_ctx;

typedef x_aclspv_ctx* ae2f_restrict h_aclspv_ctx_t;
typedef h_aclspv_ctx_t h_util_ctx_t;

#define	_malloc_with_zero(a)	calloc(a, 1)

#endif
