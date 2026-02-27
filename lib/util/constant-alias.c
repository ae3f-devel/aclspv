/**
 * @file	constant-alias.c
 * @brief	type related to constant 
 * 	- calling cxtp so downward compatibility
 * 	- later I will clean them up
 * */

#include "./constant.h"
#include "./cxtp.h"
#include "./iddef.h"


ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_vec32_id(const aclspv_wrd_t c_val, h_util_ctx_t h_ctx)
{
	return aclid_mk_default_id(ID_DEFAULT_I32, h_ctx) 
		? aclutil_mk_cxtp_vec(ID_DEFAULT_I32, c_val, h_ctx)
		: 0;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_arr8_id(const u64_least c_arrcount, h_util_ctx_t h_ctx)
{
	const aclspv_id_t	VAL = c_arrcount <= 0xFFFFFFFF 
		? aclutil_mk_constant_val_id((aclspv_wrd_t)c_arrcount, h_ctx)
		: aclutil_mk_constant_val64_id(c_arrcount, h_ctx);

	return aclid_mk_default_id(ID_DEFAULT_I8, h_ctx) && VAL
		? aclutil_mk_cxtp_arr2(ID_DEFAULT_I8, VAL, 4, h_ctx)
		: 0;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_arr16_id(const u64_least c_arrcount, h_util_ctx_t h_ctx)
{
	const aclspv_id_t	VAL = c_arrcount <= 0xFFFFFFFF 
		? aclutil_mk_constant_val_id((aclspv_wrd_t)c_arrcount, h_ctx)
		: aclutil_mk_constant_val64_id(c_arrcount, h_ctx);

	return aclid_mk_default_id(ID_DEFAULT_I16, h_ctx) && VAL
		? aclutil_mk_cxtp_arr2(ID_DEFAULT_I16, VAL, 4, h_ctx)
		: 0;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_arr32_id(const u64_least c_arrcount, h_util_ctx_t h_ctx)
{
	const aclspv_id_t	VAL = c_arrcount <= 0xFFFFFFFF 
		? aclutil_mk_constant_val_id((aclspv_wrd_t)c_arrcount, h_ctx)
		: aclutil_mk_constant_val64_id(c_arrcount, h_ctx);

	return aclid_mk_default_id(ID_DEFAULT_I32, h_ctx) && VAL
		? aclutil_mk_cxtp_arr2(ID_DEFAULT_I32, VAL, 4, h_ctx)
		: 0;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_arr128_id(const u64_least c_arrcount, h_util_ctx_t h_ctx)
{
	const aclspv_id_t ID_U32V4	= aclutil_mk_constant_vec32_id(4, h_ctx);
	const aclspv_id_t	VAL = c_arrcount <= 0xFFFFFFFF 
		? aclutil_mk_constant_val_id((aclspv_wrd_t)c_arrcount, h_ctx)
		: aclutil_mk_constant_val64_id(c_arrcount, h_ctx);

	return VAL && ID_U32V4
		? aclutil_mk_cxtp_arr2(ID_U32V4, VAL, 16, h_ctx)
		: 0;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_struct_id(const aclspv_wrd_t c_arrcount, h_util_ctx_t h_ctx) {
	const aclspv_id_t ID_ARR32 = aclutil_mk_constant_arr32_id(c_arrcount, h_ctx);

	return ID_ARR32 ? aclutil_mk_cxtp_struct_single(
			ID_ARR32
			, ACLSPV_CXTP_STRUCT_DECORATION_FIELD0_BLOCK
			, 0, h_ctx) : 0;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_struct128_id(const aclspv_wrd_t c_arrcount, h_util_ctx_t h_ctx) {
	const aclspv_id_t ID_ARR128 = aclutil_mk_constant_arr128_id(c_arrcount, h_ctx);
	return ID_ARR128 ? aclutil_mk_cxtp_struct_single(
			ID_ARR128
			, ACLSPV_CXTP_STRUCT_DECORATION_FIELD0_BLOCK
			, 0, h_ctx) : 0;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_structpriv_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	const aclspv_id_t ID_ARR32 = aclutil_mk_constant_arr32_id(c_wrdcount, h_ctx);

	return ID_ARR32 ? aclutil_mk_cxtp_struct_single(
			ID_ARR32
			, 0
			, 0xFFFFFFFF, h_ctx) : 0;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_structprivspec_id(
		const aclspv_wrd_t c_key, 
		const u64_least c_wrdcount, 
		h_util_ctx_t h_ctx
		) 
{
	aclspv_id_t	NUM_ID = aclutil_mk_constant_spec_id(c_key, (aclspv_wrd_t)c_wrdcount, h_ctx);  
	aclspv_id_t	ARR32_SPEC;

	ae2f_expected_but_else(aclid_mk_default_id(ID_DEFAULT_I32, h_ctx))
		return 0;

	unless(NUM_ID)
		ae2f_expected_but_else(NUM_ID = aclutil_mk_constant_spec64_id(c_key, c_wrdcount, h_ctx))
		return 0;

	ae2f_expected_but_else(ARR32_SPEC = aclutil_mk_cxtp_arr2(
			ID_DEFAULT_I32, NUM_ID
			, 4, h_ctx))
		return 0;

	return aclutil_mk_cxtp_struct_single(
			ARR32_SPEC
			, 0
			, 0xFFFFFFFF
			, h_ctx);
}
