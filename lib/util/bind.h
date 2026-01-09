#ifndef util_bind_h
#define util_bind_h

#include <aclspv/spvty.h>
#include <clang-c/Index.h>

typedef	struct {
	aclspv_wrd_t		m_storage_class;
	aclspv_id_t		m_var_elm_type_id;
	aclspv_id_t		m_var_type_id;
	aclspv_id_t		m_var_id;
	aclspv_wrdcount_t	m_arg_idx;
	aclspv_wrdcount_t	m_entp_idx;
	CXCursor		m_cursor;
}	util_bind_unified;

typedef union {
	util_bind_unified		m_unified;

	struct {
		util_bind_unified	m_unified;

		/** extension */
		aclspv_wrd_t		m_set;
		aclspv_wrdcount_t	m_binding;
	}	m_bindable;

	struct {
		util_bind_unified	m_unified;

		/** extension */
		aclspv_wrdcount_t	m_arr_count_id;
	}	m_work;

	struct {
		util_bind_unified	m_unified;

		aclspv_wrdcount_t	m_location;
	}	m_io;
} util_bind;

#endif
