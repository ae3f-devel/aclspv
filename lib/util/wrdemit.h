#ifndef build_wrdemit_h
#define build_wrdemit_h

#include <stddef.h>
#include <aclspv/spvty.h>

#include <ae2f/c90/StdInt.h>
#include <ae2f/Keys.h>

#include <assert.h>
#include <string.h>

#include "./ctx.h"

/** size_t for spir-v format */
typedef	aclspv_wrdcount_t	spvsz_t;

/** 
 * @def 	sz_to_count 
 * @brief	byte size to word count
 * */
#define sz_to_count(c_sz)	\
	(((spvsz_t)(c_sz) + (spvsz_t)(sizeof(aclspv_wrd_t)) - 1) / (spvsz_t)(sizeof(aclspv_wrd_t)))


/**
 * @def		count_to_sz
 * @brief	word count to byte size
 * */
#define count_to_sz(c_count)	((spvsz_t)(((spvsz_t)(c_count)) * (spvsz_t)(sizeof(aclspv_wrd_t))))

/**
 * @def		get_wrd_of_vec
 * @brief	get word buffer from vector
 * */
#define get_wrd_of_vec(vec)	((aclspv_wrd_t* ae2f_restrict)((vec)->m_p))

/**
 *
 * @fn		emit_wrd
 * @brief	try emitting `c_wrd` on `h_vec`
 * @return	new c_wrdcount available. 0 when failed allocating.
 * */
static ae2f_inline spvsz_t util_emit_wrd(
		x_aclspv_vec* ae2f_restrict const h_wrds, 
		const spvsz_t c_wrdcount, 
		const aclspv_wrd_t c_wrd
		)
{
	if(count_to_sz(c_wrdcount + 1) > (spvsz_t)h_wrds->m_sz)
		_aclspv_grow_vec_with_copy(
				_aclspv_malloc, _aclspv_free, _aclspv_memcpy 
				, L_new, *h_wrds
				, ((size_t)count_to_sz((c_wrdcount + 1) << 1))
				);

	ae2f_expected_but_else(h_wrds->m_p) return 0;
	if(sz_to_count(h_wrds->m_sz) > UINT32_MAX) return 0;
	if(count_to_sz(c_wrdcount) >= ((spvsz_t)h_wrds->m_sz))
		return 0;

	get_wrd_of_vec(h_wrds)[c_wrdcount] = c_wrd;
	return c_wrdcount + 1;
}

#define	mk_noprnds(c_num_opprm)	\
	((((aclspv_wrd_t)((c_num_opprm) + 1)) << 16) & (aclspv_wrd_t)ACLSPV_MASK_NOPRNDS)

#define	opcode_to_wrd(c_opcode, c_num_opprm)	\
	((aclspv_wrd_t)(((aclspv_wrd_t)(c_opcode) & ACLSPV_MASK_OPCODE) | mk_noprnds(c_num_opprm)))
/**
 * @def		emit_opcode
 * @brief	try emit opcode with num_opprm
 * */
#define	emit_opcode(h_wrds, c_wrdcount, c_opcode, c_num_opprm_opt)	\
	(util_emit_wrd(h_wrds, c_wrdcount, opcode_to_wrd(c_opcode, c_num_opprm_opt)))

#define set_oprnd_count_for_opcode(cr_wrd, c_num_opprm)	\
	(cr_wrd) = opcode_to_wrd((cr_wrd & ACLSPV_MASK_OPCODE), ((aclspv_num_opprm_t)(c_num_opprm)))

/**
 * @fn		emit_str
 * @brief	emit string with word size padded.
 * @returns	0 when failed.
 * */
ae2f_inline static spvsz_t util_emit_str(
		x_aclspv_vec* ae2f_restrict const h_wrds, 
		const spvsz_t c_wrdcount, 
		const char* ae2f_restrict const rd_str
		)
{
	/** string len: null included. */
	const size_t	_len		= strlen(rd_str) + 1;
	const spvsz_t	len		= (_len > (size_t)UINT32_MAX) ? 0 : (spvsz_t)_len;
	const spvsz_t	pad_wrds	= (!!((len) & 3)) + ((len) >> 2);

	ae2f_expected_but_else(len)		return 0;

	_aclspv_grow_vec_with_copy(_aclspv_malloc, _aclspv_free, _aclspv_memcpy, L_new
			, *h_wrds, (size_t)count_to_sz(pad_wrds + c_wrdcount));

	ae2f_expected_but_else(h_wrds->m_p)	return 0;
	assert(count_to_sz(pad_wrds + c_wrdcount) <= (spvsz_t)h_wrds->m_sz);

	memset(&get_wrd_of_vec(h_wrds)[c_wrdcount], 0, (size_t)count_to_sz(pad_wrds));
	_aclspv_memcpy(&get_wrd_of_vec(h_wrds)[c_wrdcount], rd_str, len);

	return c_wrdcount + pad_wrds;
}

#endif
