/** @file lnker.h @brief links compiled objects */

#ifndef aclspv_lnker_h
#define aclspv_lnker_h

#include "./abi.h"
#include <ae2f/Keys.h>

#include "./obj.h"

/** 
 * @class	x_aclspv_linker
 * @brief	a class to link the compiled objects
 * */
typedef struct {
	int u; 
} x_aclspv_lnker;

typedef x_aclspv_lnker* ae2f_restrict h_aclspv_lnker_t;

/**
 * @enum	e_aclspv_init_lnker
 * @brief	result of `aclspv_init_lnker`
 * */
typedef enum {
	ACLSPV_INIT_LNKER_OK
} e_aclspv_init_lnker;

/**
 * @method	aclspv_init_lnker
 * @memberof	x_aclspv_lnker
 * @brief	initialise the linker.
 * @param	i_lnk	<INIT:aclspv_stop_lnker>
 * */
ae2f_extern ACLSPV_ABI_DECL
e_aclspv_init_lnker
aclspv_init_lnker(h_aclspv_lnker_t i_lnk);

/**
 * @method	aclspv_stop_lnker
 * @memberof	x_aclspv_lnker
 * @brief	stops the linker
 * @param	s_lnk	<STOP>
 * */
ae2f_extern ACLSPV_ABI_DECL void
aclspv_stop_lnker(h_aclspv_lnker_t s_lnk);

/** 
 * @enum	e_aclspv_add_obj_to_lnker
 * @brief	result for `aclspv_add_obj_to_lnker`
 * */
typedef enum {
	ACLSPV_ADD_OBJ_TO_LNKER_OK
} e_aclspv_add_obj_to_lnker;

/**
 * @method	aclspv_add_obj_to_lnker
 * @memberof	x_aclspv_lnker
 * @brief	add `hy_tar` on `h_lnk`
 * @param	h_lnk	<HANDLE>
 * @param	hy_tar	<HANDLE> <REFDBY:h_lnk>
 * @return	see `e_aclspv_init_lnker`
 * */
ae2f_extern ACLSPV_ABI_DECL void
aclspv_add_obj_to_lnker(
		h_aclspv_lnker_t	h_lnk,
		const h_aclspv_obj_t* ae2f_restrict const	rd_hy_tars,
		const size_t					c_tars_len
		);

#endif
