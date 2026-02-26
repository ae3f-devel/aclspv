/** @file id.h @brief utility functions for id.h */

#ifndef	util_iddef_h
#define util_iddef_h


#include "./id.h"
#include "./ctx.h"


 ae2f_ccconst aclspv_bool_t aclid_is_unsigned(const e_id_default c_id_default);
 ae2f_ccconst aclspv_bool_t aclid_is_signed(const e_id_default c_id_default);

 ae2f_ccconst aclspv_bool_t aclid_is_int(const e_id_default c_id_default);

 ae2f_ccconst aclspv_bool_t aclid_is_float(const e_id_default c_id_default);

 ae2f_ccconst aclspv_bool_t aclid_is_default(const e_id_default c_id_default);

 ae2f_ccconst aclspv_bool_t aclid_is_number(const e_id_default c_id_default);

 ae2f_ccconst e_id_default aclid_getty_float(unsigned bit_width);

 ae2f_ccconst e_id_default aclid_getty_unsigned(unsigned bit_width);
 ae2f_ccconst e_id_default aclid_getty_signed(unsigned bit_width);

 ae2f_ccconst unsigned aclid_get_bit_width(const e_id_default c_id_default);

 ae2f_ccconst aclspv_bool_t aclid_is_arithmetic(const aclspv_id_t c_id);

 aclspv_id_t aclid_mk_default_id(
		const e_id_default	c_id_default,
		h_util_ctx_t	h_ctx
		);


 ae2f_ccconst e_id_default aclid_decide_cast(const e_id_default c_id0, const e_id_default c_id1);

#endif
