/**
 * @file	spvty.h
 * @brief	type definitions for spir-v output.
 * */

#ifndef	aclspv_spvty_h
#define aclspv_spvty_h

#include <ae2f/c90/StdInt.h>

/**
 * @typedef	aclspv_wrd_t
 * @brief	integer as word
 * */
#ifndef aclspv_wrd_t
#define aclspv_wrd_t	aclspv_wrd_t
typedef uint32_t aclspv_wrd_t;
#endif

extern char ACLSPV_CHECK[sizeof(aclspv_wrd_t) >= 4 ? 1 : -1];

/**
 * @typedef	aclspv_opcode_t
 * @brief	integer as operation code
 * */
#ifndef aclspv_opcode_t
#define aclspv_opcode_t	aclspv_opcode_t
typedef uint_fast16_t aclspv_opcode_t;
#endif

extern char ACLSPV_CHECK[sizeof(aclspv_opcode_t) >= 2 ? 1 : -1];

/**
 * @def		ACLSPV_MASK_OPCODE
 * @brief	mask for opcode
 * */
#define	ACLSPV_MASK_OPCODE	((aclspv_wrd_t)0x0000ffff)
/**
 * @def		ACLSPV_MASK_NOPRNDS
 * @brief	mask for number of operands
 * */
#define ACLSPV_MASK_NOPRNDS	((aclspv_wrd_t)0xffff0000)

/**
 * @def		ACLSPV_MASK_WRD
 * @brief	mask for word
 * */
#define ACLSPV_MASK_WRD		((aclspv_wrd_t)0xffffffff)


/**
 * @typedef	aclspv_num_opprm
 * @brief	number of parameter of opcode (aka `aclspv_opcode_t`)
 * */
typedef aclspv_wrd_t	aclspv_num_opprm_t;

/**
 * @typedef	aclspv_wrdcount_t
 * @brief	the integer type represents the number of word.
 * */
typedef	aclspv_wrd_t	aclspv_wrdcount_t;

/**
 * @typedef	aclpsv_id_t
 * @brief	represents id in spir-v
 * */
typedef	aclspv_wrd_t	aclspv_id_t;

typedef aclspv_wrd_t	aclspv_bool_t;

#endif
