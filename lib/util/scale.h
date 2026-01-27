/**
 * @file	scale.h
 * @brief	Kind of single linked list where only last one could be resized.
 * */

#ifndef pass_scale_h
#define pass_scale_h

#include <ae2f/c90/StdInt.h>
#include "../vec.auto.h"
#include <assert.h>

#include "./log.h"
#include "aclspv/spvty.h"


typedef struct x_scale x_scale;
typedef x_scale* ae2f_restrict h_scale_t;

struct x_scale {
	size_t			m_id;
	size_t			m_nxt;
	size_t			m_prv;
	size_t			m_buf;
	size_t			m_sz;
};

#define	SCALE_HEADER_SIZE	\
	((size_t)(sizeof(x_scale) + sizeof(size_t)))

#define	get_scale_header_from_vec(h_alloc)	((size_t* ae2f_restrict)((h_alloc)->m_p))
#define get_first_scale_from_vec(h_alloc)	((h_scale_t)(get_scale_header_from_vec(h_alloc) + 1))

#define	get_last_scale_from_vec(h_alloc)	\
	((h_scale_t)( (uintptr_t)(*get_scale_header_from_vec(h_alloc)) + (uintptr_t)((h_alloc)->m_p) ))

#define	get_buf_from_scale(h_alloc, c_scale)	\
	(ae2f_expected((h_alloc)->m_p) \
	 ? (void* ae2f_restrict)(((uintptr_t)(h_alloc)->m_p) + ((uintptr_t)(c_scale).m_buf)) \
	 : ((void* ae2f_restrict)ae2f_NIL))

#define	get_buf_from_scale2(buf_T, h_alloc, c_scale)	\
	((buf_T* ae2f_restrict)get_buf_from_scale(h_alloc, c_scale))

#define	get_nxt_from_scale(h_alloc, c_scale)	\
	((x_scale* ae2f_restrict)(((uintptr_t)(h_alloc)->m_p) + ((uintptr_t)(c_scale).m_nxt)))

#define	get_prv_from_scale(h_alloc, c_scale)	\
	((x_scale* ae2f_restrict)(((uintptr_t)(h_alloc)->m_p) + ((uintptr_t)(c_scale).m_prv)))

/** 
 * @fn	 	init_scale
 * @brief	intialise the scale
 * @return	pointer to the first scale of `h_alloc`. `ae2f_NIL` when failed.
 * */
ae2f_inline ae2f_retnew static x_scale* init_scale(
		x_aclspv_vec* ae2f_restrict const	h_alloc,
		const size_t				c_init_sz
		)
{
	_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, *h_alloc, c_init_sz + SCALE_HEADER_SIZE);

	unless(h_alloc->m_p)
		return ae2f_NIL;

	*get_scale_header_from_vec(h_alloc) = sizeof(size_t);

	get_first_scale_from_vec(h_alloc)->m_buf = SCALE_HEADER_SIZE;
	get_first_scale_from_vec(h_alloc)->m_sz = c_init_sz;
	get_first_scale_from_vec(h_alloc)->m_nxt = 0;
	get_first_scale_from_vec(h_alloc)->m_prv = sizeof(size_t);
	get_first_scale_from_vec(h_alloc)->m_id = 0;

	assert(get_first_scale_from_vec(h_alloc) == get_last_scale_from_vec(h_alloc));
	return get_first_scale_from_vec(h_alloc);
}

/**
 * @fn		init_scale
 * @brief	get the pointer of `c_idx`th scale from `h_alloc`
 * @return	`ae2f_NIL` on failure.
 * */
ae2f_inline ae2f_ccconst static x_scale* get_scale_from_vec(
		x_aclspv_vec* ae2f_restrict const	h_alloc,
		const size_t				c_idx
		)
{
	size_t i = c_idx;
	x_scale* ae2f_restrict ret = get_first_scale_from_vec(h_alloc);
	ae2f_expected_but_else(ret) {
		assert("non-alloc" && 0);
		return ae2f_NIL;
	}

	ae2f_unexpected_but_if(h_alloc->m_sz < SCALE_HEADER_SIZE) {
		assert("misconfigured" && 0);
		return ae2f_NIL;
	}

	while(i--) {
		ae2f_expected_but_else(ret->m_nxt) {
			assert("exceeded" && 0);
			ae2f_unreachable();
			return ae2f_NIL;
		}

		ret = get_nxt_from_scale(h_alloc, *ret);
	}

	assert(ret->m_id == c_idx);
	return ret;
}

/**
 * @fn		grow_last_scale
 * @brief	grow the last scale of `h_alloc` in `c_newsize`
 * @return	0 when succeed.
 * */
ae2f_inline	static int_fast8_t grow_last_scale(
		x_aclspv_vec* ae2f_restrict const	h_alloc,
		const size_t				c_newsize
		)
{

	assert(h_alloc);
	unless(h_alloc->m_p) {
		assert(0);
		return 1;
	}

	if(get_last_scale_from_vec(h_alloc)->m_nxt)	return 1;

	_aclspv_grow_vec_with_copy(
			_aclspv_malloc, _aclspv_free, _aclspv_memcpy
			, L_new
			, *h_alloc
			, c_newsize + get_last_scale_from_vec(h_alloc)->m_buf
			);

	unless(h_alloc->m_p) return 1;

	get_last_scale_from_vec(h_alloc)->m_sz = c_newsize;

	return !(h_alloc && h_alloc->m_p);
}

ae2f_retnew ae2f_inline static x_scale* del_scale_from_vec_last(x_aclspv_vec* ae2f_restrict const h_alloc) {
	h_scale_t	lst = get_last_scale_from_vec(h_alloc);
	h_scale_t	lst_prv;

	*get_scale_header_from_vec(h_alloc)
		= lst->m_prv;

	assert(lst);
	lst_prv	= get_prv_from_scale(h_alloc, *lst);
	assert(lst_prv);
	lst_prv->m_nxt = 0;
	return		lst_prv;
}

/**
 * @fn		mk_scale_from_vec
 * @brief	make new scale from `h_alloc` and returns its pointer
 * @details	pointer of new scale or `ae2f_NIL` when failed.
 * */
ae2f_retnew ae2f_inline static x_scale* mk_scale_from_vec(
		x_aclspv_vec* ae2f_restrict const	h_alloc,
		const size_t				c_init_size_opt
		)
{
	h_scale_t	
		lst = get_last_scale_from_vec(h_alloc)
		, nxt;
	const size_t	newsz = lst->m_buf + lst->m_sz + c_init_size_opt + (size_t)sizeof(x_scale);

	assert(lst); 
	assert(h_alloc);
	assert(h_alloc->m_p);
	assert(*get_scale_header_from_vec(h_alloc));
	assert(lst->m_buf);

	_aclspv_grow_vec_with_copy(
			_aclspv_malloc, _aclspv_free, _aclspv_memcpy, L_new
			, *h_alloc
			, newsz
			);

	assert(h_alloc->m_p);
	ae2f_expected_but_else(h_alloc->m_p) return ae2f_NIL;

	lst = get_last_scale_from_vec(h_alloc);
	lst->m_nxt = lst->m_buf + lst->m_sz;

	*get_scale_header_from_vec(h_alloc)
		= lst->m_nxt;

	nxt = get_nxt_from_scale(h_alloc, *lst);

	assert(nxt != lst);

	nxt->m_buf = lst->m_nxt + (size_t)sizeof(x_scale);
	nxt->m_sz = c_init_size_opt;
	nxt->m_nxt = 0;
	nxt->m_id = lst->m_id + 1;
	nxt->m_prv = (size_t)((const char*)lst - (const char*)h_alloc->m_p);

	assert(get_first_scale_from_vec(h_alloc) < get_last_scale_from_vec(h_alloc));
	assert(lst->m_buf);
	assert(lst->m_nxt);
	assert(nxt->m_buf);

	return nxt;
}



#endif
