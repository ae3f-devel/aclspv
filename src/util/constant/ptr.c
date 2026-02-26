/** @file ptr.auto.h @brief pointer emission utility */

#ifndef constant_ptr_auto_h
#define constant_ptr_auto_h

#include <ae2f/Macro.h>

#if !__ae2f_MACRO_GENERATED

#include <util/constant.h>
#define	L_m_ptr		m_ptr_storage
#define L_m_ptr_elm	m_struct_id


typedef aclspv_id_t mk_ptr_elm_t(const aclspv_wrd_t, const h_util_ctx_t);
void	util_mk_constant_ptr_tmpl(
		aclspv_id_t, const aclspv_wrd_t, const h_util_ctx_t
		, mk_ptr_elm_t, enum SpvStorageClass_
		);

#endif

/**
 * @def		_util_mk_constant_ptr_tmpl
 * @brief	function utility to make a pointer type
 * @returns	generated id
 * @returns	0 when allocation failure.
 * 
 * @tparam	L_new	new variable name inside the scope.
 * 			good for preventing shadowing.
 *
 * @tparam	L_m_ptr	member name in `util_constant` to store the id of pointer type.
 *
 * @tparam	L_m_ptr_elm
 * 			member name in `util_constant` to store the id of pointee type.
 *
 * @param	aclspv_id_t ret	
 * <RET>
 * 			return value
 *
 * @param	const aclspv_wrd_t c_key
 * <CONST>
 * 			desired key for constant.
 *
 * @param	const h_util_ctx_t h_ctx
 * <HANDLE>
 * 			general context for compiler
 *
 * @param	mk_ptr_elm_t fn_mk_ptr_elm_ty
 * <FN>
 * 			a function to make pointee type
 * 			
 * */
ae2f_MAC((L_new, L_m_ptr, L_m_ptr_elm, )) util_mk_constant_ptr_tmpl(
		aclspv_id_t		ret,
		const aclspv_wrd_t	c_key, 
		const h_util_ctx_t	h_ctx,
		mk_ptr_elm_t		fn_mk_ptr_elm_ty,
		const enum SpvStorageClass_	c_storage_class
		)
{
	(ret) = 0;
	do {
		util_constant* ae2f_restrict const L_new = aclutil_mk_constant_node(c_key, h_ctx);
		ae2f_expected_but_else(L_new) break;

		if(L_new->L_m_ptr) {
			(ret) = L_new->L_m_ptr;
			break;
		}

		unless(L_new->L_m_ptr_elm)
			ae2f_expected_but_else(L_new->L_m_ptr_elm = fn_mk_ptr_elm_ty(c_key, h_ctx))
			break;

		ae2f_expected_but_else((h_ctx)->m_count.m_types = util_emitx_type_pointer(
					&(h_ctx)->m_section.m_types
					, (h_ctx)->m_count.m_types
					, (h_ctx)->m_id
					, c_storage_class 
					, L_new->L_m_ptr_elm
					)) break;

		(ret) = L_new->L_m_ptr = h_ctx->m_id++;
	} while(ae2f_unexpected(0));
}

#endif
