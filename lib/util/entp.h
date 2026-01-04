#ifndef __lib_build_entp
#define __lib_build_entp

#include <aclspv/spvty.h>
#include <clang-c/Index.h>

typedef struct {
	aclspv_id_t	m_id;

	/** parameter count (excluding push constants) */
	aclspv_wrd_t	m_nprm;

	struct {
		aclspv_id_t	m_push_struct;
		aclspv_id_t	m_push_ptr;
		aclspv_id_t	m_push_var;
	} m_push_ids;

	CXCursor	m_fn;
} util_entp_t;

#endif
