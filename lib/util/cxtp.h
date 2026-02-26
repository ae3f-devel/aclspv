#ifndef aclutil_cxtp_h
#define	aclutil_cxtp_h

#include "util/ctx.h"
#include <clang-c/Index.h>
#include <aclspv/spvty.h>
#include <ae2f/Keys.h>
#include <spirv/unified1/spirv.h>

enum ACLUTIL_CXTYPE_ {
	ACLUTIL_CXTYPE_UNIT,
	ACLUTIL_CXTYPE_FUNC,
	ACLUTIL_CXTYPE_FUNC_NOPRM,
	ACLUTIL_CXTYPE_VECTOR,
	ACLUTIL_CXTYPE_ARRAY,
	ACLUTIL_CXTYPE_PTR
};

typedef struct {
	aclspv_id_t		m_type_id;
	enum ACLUTIL_CXTYPE_ 	m_class;

	union {
		struct {
			aclspv_id_t	m_type;
			aclspv_id_t	m_num_id;
		} m_arr;

		struct {
			aclspv_id_t	m_type;
			aclspv_wrd_t	m_num;
		} m_vec;

		struct {
			aclspv_id_t		m_type;
			enum SpvStorageClass_	m_storage;
		} m_ptr;

		struct {
			CXType			m_type;
			CXCursor		m_cursor;
		} m_fn;

		struct {
			aclspv_id_t	m_ret_id;
		} m_fn_no_proto;
	} m_info;	/** must be plain old buffer (pob) */
} aclutil_cxtp;

/** Returns the type id of type */
ae2f_extern aclspv_id_t aclutil_mk_cxtp_by_cursor(
		const CXCursor		c_cursor,
		const CXType		c_type,
		const h_aclspv_ctx_t	h_ctx
		);

ae2f_extern aclspv_id_t aclutil_mk_cxtp_arr(
		const aclspv_id_t	c_type_elm,
		const aclspv_id_t	c_num_arr,
		const h_aclspv_ctx_t	h_ctx
		);

ae2f_extern aclspv_id_t aclutil_mk_cxtp_vec(
		const aclspv_id_t	c_type_elm,
		const aclspv_wrd_t	c_num_arr,
		const h_aclspv_ctx_t	h_ctx
		);

ae2f_extern aclspv_id_t aclutil_mk_cxtp_ptr(
		const aclspv_id_t		c_type_elm,
		const enum SpvStorageClass_	c_storage_class,
		const h_aclspv_ctx_t		h_ctx
		);

ae2f_extern aclspv_id_t aclutil_mk_cxtp_fn_no_prm(
		const aclspv_id_t	c_ret_id,
		const h_aclspv_ctx_t	h_ctx
		);

ae2f_extern aclspv_id_t aclutil_mk_cxtp_fn_with_prm(
		const CXCursor		c_cursor_compound,
		const CXType		c_type_compound,
		const h_aclspv_ctx_t	h_ctx
		);

#endif
