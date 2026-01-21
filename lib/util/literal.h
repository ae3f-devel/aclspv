#ifndef util_literal_h
#define util_literal_h

#include <ae2f/c90/StdInt.h>

enum UTIL_LITERAL_ {

	UTIL_LITERAL_RT,

	UTIL_LITERAL_CONSTANT	= 0x1,
	UTIL_LITERAL_LITERAL	= 0x3
};

typedef union {
	int		m_api_int;
	intmax_t	m_api_intmax;
	uintmax_t	m_api_uintmax;

	u64_least	m_u64;
	double		m_dbl;
	float		m_flt;
} util_literal;

#endif
