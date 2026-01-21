/** @file cursor.h @brief cursor master list */

#ifndef util_cursor_h
#define util_cursor_h

#include <clang-c/Index.h>
#include <aclspv/spvty.h>
#include <ae2f/Keys.h>
#include <assert.h>
#include <spirv/unified1/spirv.h>
#include "../vec.auto.h"

#include "./bind.h"
#include "./ctx.h"
#include "./emitx.h"
#include "./u32_to_hex8.auto.h"
#include "./literal.h"

#include "aclspv.h"
#include "aclspv/abi.h"

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
			aclspv_wrd_t	m_is_constant		: 1;
			aclspv_wrd_t	m_is_literal		: 1;


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

/** after allocation, the allocated node will be empty except for `c_cursor_to_add` since that's the key. */
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

ae2f_inline static e_aclspv_compile_t util_tell_cursor_lbl(
		const aclspv_wrdcount_t c_num_cursor, 
		util_cursor* ae2f_restrict const wr_cursor, 
		const h_util_ctx_t h_ctx
		) {
	aclspv_wrdcount_t	IDX = c_num_cursor;
	assert(wr_cursor || !c_num_cursor);
	{ ae2f_assume(wr_cursor || !c_num_cursor); }
	ae2f_expected_if(wr_cursor) while(IDX--) {
#define	CURSORDATA	wr_cursor[IDX].m_data.m_var_simple
		if(wr_cursor[IDX].m_cursor.kind == CXCursor_VarDecl) {
			CURSORDATA.m_met_branch	= 1;
			unless(CURSORDATA.m_is_allocated) {
				CURSORDATA.m_is_allocated = 1;

				ae2f_expected_but_else(h_ctx->m_count.m_fndef = util_emitx_variable(
							&h_ctx->m_section.m_fndef
							, h_ctx->m_count.m_fndef
							, CURSORDATA.m_ptr_type_id
							, CURSORDATA.m_id
							, SpvStorageClassFunction))
					return ACLSPV_COMPILE_ALLOC_FAILED;

				if(CURSORDATA.m_init_val && !ae2f_expected(
							h_ctx->m_count.m_fnimpl = util_emitx_3(
								&h_ctx->m_section.m_fnimpl
								, h_ctx->m_count.m_fnimpl
								, SpvOpStore
								, CURSORDATA.m_id
								, CURSORDATA.m_init_val
								)
							)
				  ) return ACLSPV_COMPILE_ALLOC_FAILED;

#if	!defined(NDEBUG) || !NDEBUG
				{
					const aclspv_wrd_t POS	= h_ctx->m_count.m_name;
					CXString SPELL		= clang_getCursorSpelling(wr_cursor[IDX].m_cursor);

					_aclspv_grow_vec(_aclspv_malloc, _aclspv_free	
							, h_ctx->m_tmp.m_v0, strlen(SPELL.data) + 10
							);							
					ae2f_expected_but_else(h_ctx->m_tmp.m_v0.m_p)		
						goto LBL_FAIL;								
					((char* ae2f_restrict)h_ctx->m_tmp.m_v0.m_p)[8] = ':';			
					((char* ae2f_restrict)h_ctx->m_tmp.m_v0.m_p)[9] = '\0';			
					_util_u32_to_hex8((CURSORDATA.m_id)
							, ((char* ae2f_restrict)h_ctx->m_tmp.m_v0.m_p));	
					strcat(h_ctx->m_tmp.m_v0.m_p, SPELL.data);			
					ae2f_expected_but_else((h_ctx->m_count.m_name = emit_opcode(	
									&h_ctx->m_section.m_name		
									, h_ctx->m_count.m_name	
									, SpvOpName, 0))			
							) goto LBL_FAIL;					
					ae2f_expected_but_else((h_ctx->m_count.m_name = util_emit_wrd(		
									&h_ctx->m_section.m_name		
									, h_ctx->m_count.m_name	
									, CURSORDATA.m_id
									)))						
						goto LBL_FAIL;								
					ae2f_expected_but_else((h_ctx->m_count.m_name = util_emit_str(			
									&h_ctx->m_section.m_name			
									, h_ctx->m_count.m_name		
									, h_ctx->m_tmp.m_v0.m_p 			
									)))						
						goto LBL_FAIL;								
					set_oprnd_count_for_opcode(get_wrd_of_vec(					
								&h_ctx->m_section.m_name)[POS]				
							, h_ctx->m_count.m_name - POS - 1);

					clang_disposeString(SPELL);
					continue;
					ae2f_unreachable();
LBL_FAIL:
					clang_disposeString(SPELL);
					return ACLSPV_COMPILE_ALLOC_FAILED;
				}
#else
#endif
			}
		}
#undef	CURSORDATA
	}

	return ACLSPV_COMPILE_OK;
}


#endif
