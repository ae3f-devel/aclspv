/** @file ctx.h @brief the context of a compiler (something like a state machine) */

#ifndef lib_build_ctx_h
#define lib_build_ctx_h

#include "../vec.auto.h"
#include "./id.h"

#include <aclspv.h>
#include <aclspv/spvty.h>

#define	_free(a, b)	free(a)

typedef struct a_aclspv_ctx {
	aclspv_wrd_t		m_has_function_ret : 1;
	aclspv_wrd_t		m_is_buffer_64 : 1;
	aclspv_wrd_t		m_not_dbg : 1;
	aclspv_wrd_t		m_is_for_gl : 1;

	/**
	 * @brief	when on, ignores `m_is_buffer_64`.
	 * */
	aclspv_wrd_t		m_is_logical : 1;

	e_aclspv_compile_t	m_err;

	/**
	 * @var		m_ret
	 * @brief	word count for m_ret
	 * */
	aclspv_wrdcount_t	m_num_ret;

	/** 
	 * @brief
	 * number of `m_cursors`.	\n
	 * for its use see util/cursor.h
	 *
	 * @details
	 * reset on every function
	 * */
	aclspv_wrdcount_t	m_num_cursor;

	/** @see `m_type_uniques` */
	aclspv_wrdcount_t	m_num_type_uniques;

	/**
	 * @var		m_id
	 * @brief	id
	 * */
	aclspv_id_t		m_id;

	/**
	 * @var		m_ret
	 * @brief	fully emitted spir-v
	 * */
	x_aclspv_vec		m_ret;

	/** 
	 * @var m_scale_vars 
	 * @see util_bind 
	 * */
	x_aclspv_vec            m_scale_vars;

	/** cache for elements related to unsigned integer constants [lib_build_constant] */
	x_aclspv_vec		m_constant_cache;

	/** 
	 * @brief
	 * cache for cursors for parsing one function 
	 * for its use see util/cursor.h
	 *
	 * [util_cursor]
	 *
	 * @details
	 * reset on every function
	 * */
	x_aclspv_vec		m_cursors;

	/** 
	 * @brief
	 * cache for complex types which needs to be stored somewhere
	 * for its use see util/type_unique.h
	 *
	 * [util_type_unique]
	 * */
	x_aclspv_vec		m_type_uniques;

	struct {
		aclspv_wrd_t	m_w0;
		aclspv_wrd_t	m_w1;
		aclspv_wrd_t	m_w2;
		aclspv_wrd_t	m_w3;

		x_aclspv_vec	m_v0;
		x_aclspv_vec	m_v1;
		x_aclspv_vec	m_v2;
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
			m_fndef,
			m_fnimpl;
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
			m_fndef,
			m_fnimpl;
	}			m_section;

	aclspv_id_t		m_id_defaults[ID_DEFAULT_END];
} x_aclspv_ctx;

typedef x_aclspv_ctx* ae2f_restrict h_aclspv_ctx_t;
typedef h_aclspv_ctx_t h_util_ctx_t;

#define	_malloc_with_zero(a)	calloc(a, 1)

#endif
