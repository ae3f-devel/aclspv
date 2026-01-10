#ifndef util_cursor_h
#define util_cursor_h

#include <clang-c/Index.h>
#include <aclspv/spvty.h>
#include <ae2f/Keys.h>
#include <assert.h>
#include "../vec.auto.h"

#include "./bind.h"

typedef struct {
	/**
	 * @var		m_cursor
	 * @brief	`CXCursorKind` could be retrieved here
	 * */
	CXCursor	m_cursor;

	/**
	 * @var		m_data
	 * @brief	additional data
	 * */
	union {
		struct {
			/**
			 * @var	m_id
			 *
			 * @brief
			 * the id of variable 
			 * */
			aclspv_id_t	m_id;

			/**
			 * @var	m_init_val_0
			 * @var	m_init_val_1
			 *
			 * @brief
			 * the initial value id 
			 * */
			aclspv_id_t	m_init_val_0;
			aclspv_id_t	m_init_val_1;

			/**
			 * @var m_data_0
			 * @var m_data_1
			 * @brief
			 * evaluated data.
			 * meaningless when `m_is_32bit` is false.
			 * */
			aclspv_wrd_t	m_data_0;
			aclspv_wrd_t	m_data_1;

			/** 
			 * @var m_is_constant
			 *
			 * @brief
			 * is initial value constant?
			 * is this variable not seem to be modified?
			 * 	> or is modified value predictable?
			 * is this variable not seem to be referenced?
			 * */
			aclspv_wrd_t	m_is_predictable	: 1;

			/**
			 * @var	m_is_undefined
			 * @brief
			 * is its first value not specified yet?
			 * */
			aclspv_wrd_t	m_is_undefined		: 1;

			/**
			 * @var	m_is_used
			 * @brief
			 * has this variable's value been used lately?
			 * */
			aclspv_wrd_t	m_is_used		: 1;

			/**
			 * @var	m_is_integer
			 * @brief
			 * is this integer?
			 * */
			aclspv_wrd_t	m_is_integer		: 1;

			/**
			 * @var	m_fits_32bit
			 * @brief
			 * does this fits 32-bit wide?
			 * */
			aclspv_wrd_t	m_fits_32bit		: 1;
			aclspv_wrd_t	m_fits_64bit		: 1;
		} m_var_simple;

		struct {
			/** @brief label id for goto */
			aclspv_id_t	m_id;
		} m_goto_lbl;

		struct {
			util_bind	m_info;
		} m_prm_decl;
	} m_data;
} util_cursor;

/**
 * find cursor
 * */
ae2f_inline static aclspv_wrdcount_t util_find_cursor(
		const aclspv_wrdcount_t			c_num_cursor, 
		const util_cursor* ae2f_restrict const	rd_cursors,
		const CXCursor				c_cursor_to_find
		)
{
	aclspv_wrdcount_t	IDX = c_num_cursor;
	unless(rd_cursors)	return c_num_cursor;

	while(IDX--) {
		if(clang_equalCursors(rd_cursors[IDX].m_cursor, c_cursor_to_find))
			break;
	}

	if(IDX < c_num_cursor)
		return IDX;

	return c_num_cursor;
}

ae2f_inline static aclspv_wrdcount_t util_mk_cursor_base(
		const aclspv_wrdcount_t			c_num_cursor,
		x_aclspv_vec* const ae2f_restrict	h_cursors,
		const CXCursor				c_cursor_to_add
		)
{
	aclspv_wrdcount_t	IDX;

	IDX = util_find_cursor(c_num_cursor, h_cursors->m_p, c_cursor_to_add);
	{ ae2f_assume((size_t)(c_num_cursor * sizeof(util_cursor)) >= h_cursors->m_sz); }
	unless(IDX == c_num_cursor) return IDX;

	_aclspv_grow_vec_with_copy(
			_aclspv_malloc
			, _aclspv_free
			, _aclspv_memcpy
			, L_new
			, h_cursors[0]
			, ((size_t)((c_num_cursor + 1) * sizeof(util_cursor)))
			);

	ae2f_expected_but_else(h_cursors->m_p)
		return c_num_cursor + 1;

	((util_cursor* ae2f_restrict)h_cursors->m_p)[IDX].m_cursor = c_cursor_to_add;
	return c_num_cursor;
}

ae2f_inline static void util_tell_cursor_lbl(const aclspv_wrdcount_t c_num_cursor, util_cursor* ae2f_restrict const wr_cursor) {
	aclspv_wrdcount_t	IDX = c_num_cursor;
	assert(wr_cursor || !c_num_cursor);
	{ ae2f_assume(wr_cursor || !c_num_cursor); }
	ae2f_expected_if(wr_cursor) while(IDX--) {
		if(wr_cursor[IDX].m_cursor.kind == CXCursor_VarDecl) {
			wr_cursor[IDX].m_data.m_var_simple.m_is_undefined	= 0;
			wr_cursor[IDX].m_data.m_var_simple.m_is_predictable	= 0;
		}
	}
}


#endif
