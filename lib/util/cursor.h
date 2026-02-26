/** 
 * @file cursor.h 
 * @brief cursor master list 
 * */

#ifndef util_cursor_h
#define util_cursor_h

#include <clang-c/Index.h>

#include <aclspv.h>
#include <aclspv/spvty.h>

#include <ae2f/Keys.h>

#include "./bind.h"
#include "./literal.h"
#include "../vec.auto.h"



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
			aclspv_id_t	m_type_id;
			aclspv_id_t	m_ptr_type_id;

			/**
			 * @var	m_init_val
			 *
			 * @brief
			 * the initial value id 
			 * */
			aclspv_id_t	m_init_val;

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
			B_util_literal_t	m_mask_literal	: 2;


			/**
			 * @var	m_is_integer
			 * @brief
			 * is this integer?
			 * */
			aclspv_wrd_t	m_is_integer		: 1;

			/** have met branch? */
			aclspv_wrd_t	m_met_branch		: 1;

			/** is function actually allocated? */
			aclspv_wrd_t	m_is_allocated		: 1;

			/**
			 * @var	m_fits_32bit
			 * @brief
			 * does this fits 32-bit wide?
			 * */
			aclspv_wrd_t	m_fits_32bit		: 1;
			aclspv_wrd_t	m_fits_64bit		: 1;
			aclspv_wrd_t	m_flag_unused		: 23;

			util_literal	m_literal;


		} 
		/** when cursor kind is vardecl */
		m_var_simple;

		struct {
			/** @brief label id for goto */
			aclspv_id_t	m_id;
		} 
		/** when cursor kind is goto label */
		m_goto_lbl;

		struct {
			util_bind	m_info;
		} 
		/** when cursor kind is param decl */
		m_prm_decl;
	} m_data;
} util_cursor;

/**
 * find cursor
 * */
ae2f_extern  aclspv_wrdcount_t aclutil_find_cursor(
		const aclspv_wrdcount_t			c_num_cursor, 
		const util_cursor* ae2f_restrict const	rd_cursors,
		const CXCursor				c_cursor_to_find
		);


/** after allocation, the allocated node will be empty except for `c_cursor_to_add` since that's the key. */
ae2f_extern  aclspv_wrdcount_t aclutil_mk_cursor_base(
		const aclspv_wrdcount_t			c_num_cursor,
		x_aclspv_vec* const ae2f_restrict	h_cursors,
		const CXCursor				c_cursor_to_add
		);

ae2f_extern  enum ACLSPV_COMPILE_ aclutil_tell_cursor_lbl(
		const aclspv_wrdcount_t c_num_cursor, 
		util_cursor* ae2f_restrict const wr_cursor, 
		const h_util_ctx_t h_ctx
		);

#endif
