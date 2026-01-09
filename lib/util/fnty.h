#ifndef util_fnty_h
#define util_fnty_h

#include <clang-c/Index.h>
#include <aclspv/spvty.h>

/**
 * @class	util_fnty
 * @brief	function pointer type identifier
 * */
typedef struct {
	aclspv_id_t	m_id;
	CXType		m_type;
} util_fnty;

#endif 
