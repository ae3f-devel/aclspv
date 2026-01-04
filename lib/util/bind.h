#ifndef util_bind_h
#define util_bind_h

#include <aclspv/spvty.h>
#include <llvm-c/Core.h>

typedef union {
	struct {
		aclspv_wrd_t		m_storage_class;
		aclspv_id_t		m_var_elm_type_id;
		aclspv_id_t		m_var_type_id;
		aclspv_id_t		m_var_id;
		aclspv_wrdcount_t	m_arg_idx;
		aclspv_wrdcount_t	m_entp_idx;
	}	m_unified;

	struct {
		aclspv_wrd_t		m_storage_class;

		aclspv_id_t		m_var_elm_type_id;
		aclspv_id_t		m_var_type_id;
		aclspv_id_t		m_var_id;

		aclspv_wrdcount_t	m_arg_idx;

		aclspv_wrdcount_t	m_entp_idx;

		/** extension */
		aclspv_wrd_t		m_set;
		aclspv_wrdcount_t	m_binding;
	}	m_bindable;

	struct {
		aclspv_wrd_t		m_storage_class;
		aclspv_id_t		m_var_elm_type_id;
		aclspv_id_t		m_var_type_id;
		aclspv_id_t		m_var_id;
		aclspv_id_t		m_arg_idx;
		aclspv_wrdcount_t	m_entp_idx;

		/** extension */
		aclspv_wrdcount_t	m_arr_count_id;
	}	m_work;
} util_bind;

#endif
