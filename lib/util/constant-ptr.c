/** 
 * @file	constant-ptr.c
 * @brief	constant.h implementation (pointer edition)
 * */

#include "./constant.h"
#include "./cxtp.h"


ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_ptr_psh_id(const aclspv_wrd_t c_wrdcount, const h_util_ctx_t h_ctx) {

	const aclspv_id_t ELM = aclutil_mk_constant_struct_id(c_wrdcount, h_ctx);
	return ELM 
		? aclutil_mk_cxtp_ptr(ELM, SpvStorageClassPushConstant, h_ctx) 
		: 0;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_ptr_storage_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	const aclspv_id_t ELM = aclutil_mk_constant_struct_id(c_wrdcount, h_ctx);
	return ELM
		? aclutil_mk_cxtp_ptr(ELM, SpvStorageClassStorageBuffer, h_ctx) 
		: 0;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_ptr_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	const aclspv_id_t ELM = aclutil_mk_constant_structpriv_id(c_wrdcount, h_ctx);
	return ELM
		? aclutil_mk_cxtp_ptr(ELM, SpvStorageClassPrivate, h_ctx) 
		: 0;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_ptr_func(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	const aclspv_id_t ELM = aclutil_mk_constant_structpriv_id(c_wrdcount, h_ctx);
	return ELM
		? aclutil_mk_cxtp_ptr(ELM, SpvStorageClassFunction, h_ctx) 
		: 0;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_ptr_work_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	const aclspv_id_t ELM = aclutil_mk_constant_structpriv_id(c_wrdcount, h_ctx);
	return ELM
		? aclutil_mk_cxtp_ptr(ELM, SpvStorageClassWorkgroup, h_ctx) 
		: 0;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_ptr_uniform_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	const aclspv_id_t ELM = aclutil_mk_constant_struct128_id((c_wrdcount + 3) >> 2, h_ctx);
	return ELM
		? aclutil_mk_cxtp_ptr(ELM, SpvStorageClassUniform, h_ctx) 
		: 0;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_ptr_uniformconstant_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {

	const aclspv_id_t ELM = aclutil_mk_constant_struct128_id((c_wrdcount + 3) >> 2, h_ctx);
	return ELM
		? aclutil_mk_cxtp_ptr(ELM, SpvStorageClassUniformConstant, h_ctx) 
		: 0;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_ptr_workspec_id(const aclspv_wrd_t c_key, const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	const aclspv_id_t ELM = aclutil_mk_constant_structprivspec_id(c_key, c_wrdcount, h_ctx);
	return ELM
		? aclutil_mk_cxtp_ptr(ELM, SpvStorageClassWorkgroup, h_ctx) 
		: 0;
}
