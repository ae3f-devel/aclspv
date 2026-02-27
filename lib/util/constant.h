/** @file constant.h */

#ifndef	lib_build_constant_h
#define lib_build_constant_h


#include <ae2f/Keys.h>
#include <ae2f/c90/StdInt.h>

#include <aclspv/spvty.h>
#include <aclspv/abi.h>

#include "./ctx.h"
#include "./id.h"

#include <spirv/unified1/spirv.h>

typedef struct {
	aclspv_wrd_t		m_key_0;
	aclspv_wrd_t		m_key_1;
	aclspv_id_t		m_const_val_id;
	aclspv_id_t		m_const_val64_id;
	aclspv_id_t		m_const_val_f32_id;
	aclspv_id_t		m_const_val_f64_id;
	aclspv_id_t		m_const_spec_id;
	e_id_default		m_const_spec_type;
} util_constant;

typedef util_constant* ae2f_restrict p_util_constant_t;


ACLSPV_ABI_DECL aclspv_id_t aclutil_mk_constant_val_id(
		const aclspv_wrd_t c_val,
		h_util_ctx_t h_ctx
		);

ACLSPV_ABI_DECL aclspv_id_t aclutil_mk_constant_val64_id(
		const u64_least c_val, 
		h_util_ctx_t h_ctx
		);


ACLSPV_ABI_DECL aclspv_id_t aclutil_mk_constant_val_f32_id(
		const float c_val, 
		h_util_ctx_t h_ctx
		);


ACLSPV_ABI_DECL aclspv_id_t aclutil_mk_constant_val_f64_id(
		const double c_val, h_util_ctx_t h_ctx);

ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_spec_id(
		const aclspv_wrd_t c_key, const aclspv_wrd_t c_val, 
		h_util_ctx_t h_ctx
		);

ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_spec64_id(
		const aclspv_wrd_t c_key, 
		const u64_least c_val,
		h_util_ctx_t h_ctx
		);

ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_spec_id_f64(
		const aclspv_wrd_t c_key, 
		const double c_val, h_util_ctx_t h_ctx);


ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_spec_id_f32(
		const aclspv_wrd_t c_key, 
		const float c_val, h_util_ctx_t h_ctx);


ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_vec32_id(
		const aclspv_wrd_t c_val, 
		h_util_ctx_t h_ctx);


ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_arr8_id(const u64_least c_arrcount, h_util_ctx_t h_ctx);

ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_arr16_id(const u64_least c_arrcount, h_util_ctx_t h_ctx);

ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_arr32_id(const u64_least c_arrcount, h_util_ctx_t h_ctx);

ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_arr128_id(const u64_least c_arrcount, h_util_ctx_t h_ctx);

ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_struct_id(const aclspv_wrd_t c_arrcount, h_util_ctx_t h_ctx);

ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_struct128_id(const aclspv_wrd_t c_veccount, h_util_ctx_t h_ctx);

ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_structprivspec_id(const aclspv_wrd_t c_key, const u64_least c_wrdcount, h_util_ctx_t h_ctx);


ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_structpriv_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx);


ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_ptr_psh_id(const aclspv_wrd_t c_wrdcount, const h_util_ctx_t h_ctx);

ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_ptr_storage_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx);

ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_ptr_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx);


ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_ptr_func(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx);


ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_ptr_uniform_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx);

ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_ptr_uniformconstant_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx);

ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_ptr_workspec_id(const aclspv_wrd_t c_key, const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx);


ACLSPV_ABI_DECL aclspv_id_t	aclutil_mk_constant_ptr_work_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx);


ae2f_inline static aclspv_id_t	aclutil_mk_constant_ptr_by_enum(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx, enum SpvStorageClass_ c_class) {
	switch((aclspv_wrd_t)c_class) {
		case SpvStorageClassWorkgroup:
			return aclutil_mk_constant_ptr_work_id(c_wrdcount, h_ctx);
		case SpvStorageClassPrivate:
			return aclutil_mk_constant_ptr_id(c_wrdcount, h_ctx);
		case SpvStorageClassUniform:
			return aclutil_mk_constant_ptr_uniform_id(c_wrdcount, h_ctx);
		case SpvStorageClassStorageBuffer:
			return aclutil_mk_constant_ptr_storage_id(c_wrdcount, h_ctx);
		case SpvStorageClassPushConstant:
			return aclutil_mk_constant_ptr_psh_id(c_wrdcount, h_ctx);

		default:
			ae2f_unreachable();
			return 0;
	}

}

ACLSPV_ABI_DECL util_constant* aclutil_mk_constant_node(
		const u64_least	c_key,
		h_util_ctx_t	h_ctx
		);
#endif
