
#include "./constant.h"
#include "./constant/ptr.auto.h"
#include "./emitx.h"
#include "./iddef.h"
#include "util/id.h"

#include <string.h>

ae2f_inline static ae2f_ccpure util_constant* util_get_constant_node(
		const u64_least	c_key, 
		h_util_ctx_t	h_ctx
		)
{
	const aclspv_wrdcount_t	COUNT = (aclspv_wrdcount_t)(h_ctx->m_constant_cache.m_sz / (size_t)sizeof(util_constant));
	aclspv_wrdcount_t	LEFT	= 0;
	aclspv_wrdcount_t	RIGHT	= COUNT;

	ae2f_expected_if(COUNT) {
		const uint_least64_t	KEY_MADE_L = 
				(u64_least)(((p_util_constant_t)h_ctx->m_constant_cache.m_p)
				[LEFT].m_key_0)
				| ((u64_least)((p_util_constant_t)h_ctx->m_constant_cache.m_p)
				[LEFT].m_key_1) << 32;

		while(LEFT < RIGHT) {
			const aclspv_wrdcount_t	MIDDLE	= LEFT + ((RIGHT - LEFT) >> 1);
			const uint_least64_t	KEY_MADE = 
				(u64_least)(((p_util_constant_t)h_ctx->m_constant_cache.m_p)
				[MIDDLE].m_key_0)
				| ((u64_least)((p_util_constant_t)h_ctx->m_constant_cache.m_p)
				[MIDDLE].m_key_1) << 32;

			if(KEY_MADE == c_key)
				return &((p_util_constant_t)h_ctx->m_constant_cache.m_p)[MIDDLE];
			if(KEY_MADE < c_key) {
				LEFT = MIDDLE + 1;
			} else {
				RIGHT = MIDDLE;
			}
		}


		if(KEY_MADE_L == c_key)
			return &((p_util_constant_t)h_ctx->m_constant_cache.m_p)[LEFT];
	}

	return ae2f_NIL;
}

ACLSPV_ABI_IMPL util_constant* aclutil_mk_constant_node(
		const u64_least	c_key,
		h_util_ctx_t	h_ctx
		)
{
	const aclspv_wrdcount_t	COUNT = (aclspv_wrdcount_t)(h_ctx->m_constant_cache.m_sz / (size_t)sizeof(util_constant));
	aclspv_wrdcount_t	LEFT	= 0;
	aclspv_wrdcount_t	RIGHT	= COUNT;
	size_t			NSIZE;

	if(COUNT) {
		const uint_least64_t	KEY_MADE_L = 
				(u64_least)(((p_util_constant_t)h_ctx->m_constant_cache.m_p)
				[LEFT].m_key_0)
				| ((u64_least)((p_util_constant_t)h_ctx->m_constant_cache.m_p)
				[LEFT].m_key_1) << 32;

		while(LEFT < RIGHT) {
			const aclspv_wrdcount_t	MIDDLE	= LEFT + ((RIGHT - LEFT) >> 1);
			const uint_least64_t	KEY_MADE = 
				(u64_least)(((p_util_constant_t)h_ctx->m_constant_cache.m_p)
				[MIDDLE].m_key_0)
				| ((u64_least)((p_util_constant_t)h_ctx->m_constant_cache.m_p)
				[MIDDLE].m_key_1) << 32;

			if(KEY_MADE == c_key)
				return &((p_util_constant_t)h_ctx->m_constant_cache.m_p)[MIDDLE];

			if(KEY_MADE < c_key) {
				LEFT = MIDDLE + 1;
			} else {
				RIGHT = MIDDLE;
			}
		}

		if(KEY_MADE_L == c_key)
			return &((p_util_constant_t)h_ctx->m_constant_cache.m_p)[LEFT];

	}


	NSIZE = (size_t)(h_ctx->m_constant_cache.m_sz + (size_t)sizeof(util_constant));

	_aclspv_grow_vec_with_copy(
			_aclspv_malloc, _aclspv_free
			, _aclspv_memcpy
			, L_new, h_ctx->m_constant_cache
			, NSIZE
			);
	ae2f_expected_but_else(h_ctx->m_constant_cache.m_p) return ae2f_NIL;

	memmove(
			&((p_util_constant_t)(h_ctx->m_constant_cache.m_p))[LEFT + 1]
			, &((p_util_constant_t)(h_ctx->m_constant_cache.m_p))[LEFT]
			, (size_t)((COUNT - LEFT) * (sizeof(util_constant)))
	       );

	/** sanity check for future possibility of having more elements */
	memset(&((p_util_constant_t)(h_ctx->m_constant_cache.m_p))[LEFT], 0, sizeof(util_constant));
	((p_util_constant_t)(h_ctx->m_constant_cache.m_p))[LEFT].m_key_0 
		= (u32_least)(c_key & 0xFFFFFFFF);
	((p_util_constant_t)(h_ctx->m_constant_cache.m_p))[LEFT].m_key_1 
		= (u32_least)(c_key >> 32) & 0xFFFFFFFF;

	return &((p_util_constant_t)(h_ctx->m_constant_cache.m_p))[LEFT];
}

#include <spirv/unified1/spirv.h>
#include "./wrdemit.h"

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_val_id(const aclspv_wrd_t c_val, h_util_ctx_t h_ctx)
{
	util_constant* ae2f_restrict const C = aclutil_mk_constant_node((u64_least)c_val, h_ctx);
	ae2f_expected_but_else(C) return 0;

	if(C->m_const_val_id) return C->m_const_val_id;


	ae2f_expected_but_else(aclid_mk_default_id(ID_DEFAULT_I32, h_ctx))
		return 0;

	ae2f_expected_but_else(h_ctx->m_count.m_types = util_emitx_4(
			&h_ctx->m_section.m_types
			, h_ctx->m_count.m_types
			, SpvOpConstant
			, ID_DEFAULT_I32
			, h_ctx->m_id
			, c_val
		    )) return 0;

	C->m_const_val_id = h_ctx->m_id++;

	return C->m_const_val_id;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_val64_id(const u64_least c_val, h_util_ctx_t h_ctx)
{
	util_constant* ae2f_restrict const C = aclutil_mk_constant_node(c_val, h_ctx);
	ae2f_expected_but_else(C) return 0;

	if(C->m_const_val64_id) return C->m_const_val64_id;


	ae2f_expected_but_else(aclid_mk_default_id(ID_DEFAULT_I64, h_ctx))
		return 0;

	ae2f_expected_but_else(h_ctx->m_count.m_types = util_emitx_5(
			&h_ctx->m_section.m_types
			, h_ctx->m_count.m_types
			, SpvOpConstant
			, ID_DEFAULT_I64
			, h_ctx->m_id
			, (u32_least)(c_val & 0xFFFFFFFF)
			, (u32_least)(c_val >> 32) & 0xFFFFFFFF
		    )) return 0;

	C->m_const_val64_id = h_ctx->m_id++;

	return C->m_const_val64_id;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_val_f32_id(const float c_val, h_util_ctx_t h_ctx)
{
	union {
		float		m_flt;
		u32_least	m_u32;
	} FLT_U32;

	util_constant* ae2f_restrict C;

	FLT_U32.m_flt = c_val;
	C = aclutil_mk_constant_node(FLT_U32.m_u32, h_ctx);

	ae2f_expected_but_else(C) return 0;

	if(C->m_const_val_f32_id) return C->m_const_val_f32_id;

	ae2f_expected_but_else(aclid_mk_default_id(ID_DEFAULT_F32, h_ctx))
		return 0;

	ae2f_expected_but_else(h_ctx->m_count.m_types = util_emitx_4(
			&h_ctx->m_section.m_types
			, h_ctx->m_count.m_types
			, SpvOpConstant
			, ID_DEFAULT_F32
			, h_ctx->m_id
			, FLT_U32.m_u32
		    )) return 0;

	C->m_const_val_f32_id = h_ctx->m_id++;
	return C->m_const_val_f32_id;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_val_f64_id(const double c_val, h_util_ctx_t h_ctx)
{
	union {
		double		m_flt;
		u64_least	m_u64;
	} FLT_U64;

	util_constant* ae2f_restrict C;

	FLT_U64.m_flt = c_val;
	C = aclutil_mk_constant_node(FLT_U64.m_u64, h_ctx);

	ae2f_expected_but_else(C) return 0;

	if(C->m_const_val_f64_id) return C->m_const_val_f64_id;

	ae2f_expected_but_else(aclid_mk_default_id(ID_DEFAULT_F64, h_ctx))
		return 0;

	ae2f_expected_but_else(h_ctx->m_count.m_types = util_emitx_5(
			&h_ctx->m_section.m_types
			, h_ctx->m_count.m_types
			, SpvOpConstant
			, ID_DEFAULT_F64
			, h_ctx->m_id
			, (u32_least)(FLT_U64.m_u64 & 0xFFFFFFFF)
			, (u32_least)((FLT_U64.m_u64 >> 32) & 0xFFFFFFFF)
		    )) return 0;

	C->m_const_val_f64_id = h_ctx->m_id++;
	return C->m_const_val_f64_id;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_spec_id(
		const aclspv_wrd_t c_key, const aclspv_wrd_t c_val, 
		h_util_ctx_t h_ctx
		)
{
	util_constant* ae2f_restrict const C = aclutil_mk_constant_node(c_key, h_ctx);
	ae2f_expected_but_else((C)) return 0;
	if(C->m_const_spec_id)
		return C->m_const_spec_type == ID_DEFAULT_I32
			? C->m_const_spec_id 
			: 0;

	ae2f_expected_but_else(aclid_mk_default_id(ID_DEFAULT_I32, h_ctx))
		return 0;

	ae2f_expected_but_else(h_ctx->m_count.m_types = util_emitx_4(
			&h_ctx->m_section.m_types
			, h_ctx->m_count.m_types
			, SpvOpSpecConstant
			, ID_DEFAULT_I32
			, h_ctx->m_id
			, c_val))
		return 0;

	ae2f_expected_but_else(h_ctx->m_count.m_decorate = util_emitx_4(
			&h_ctx->m_section.m_decorate
			, h_ctx->m_count.m_decorate
			, SpvOpDecorate
			, h_ctx->m_id
			, SpvDecorationSpecId
			, c_key)) return 0;

	C->m_const_spec_id	= h_ctx->m_id++;
	C->m_const_spec_type	= ID_DEFAULT_I32;
	return C->m_const_spec_id;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_spec_id_f32(const aclspv_wrd_t c_key, const float c_val, h_util_ctx_t h_ctx)
{
	union {
		float		m_f;
		aclspv_wrd_t	m_u;
	} FTOI;
	util_constant* ae2f_restrict const C = aclutil_mk_constant_node(c_key, h_ctx);
	ae2f_expected_but_else(ae2f_expected(C)) return 0;
	FTOI.m_f = c_val;

	if(C->m_const_spec_id) 
		return C->m_const_spec_type == ID_DEFAULT_F32 
			? C->m_const_spec_id 
			: 0;

	ae2f_expected_but_else(aclid_mk_default_id(ID_DEFAULT_F32, h_ctx))
		return 0;

	ae2f_expected_but_else(h_ctx->m_count.m_types = util_emitx_4(
			&h_ctx->m_section.m_types
			, h_ctx->m_count.m_types
			, SpvOpSpecConstant
			, ID_DEFAULT_F32
			, h_ctx->m_id
			, FTOI.m_u))
		return 0;

	ae2f_expected_but_else(h_ctx->m_count.m_decorate = util_emitx_4(
			&h_ctx->m_section.m_decorate
			, h_ctx->m_count.m_decorate
			, SpvOpDecorate
			, h_ctx->m_id
			, SpvDecorationSpecId
			, c_key)) return 0;

	C->m_const_spec_id	= h_ctx->m_id++;
	C->m_const_spec_type	= ID_DEFAULT_F32;

	return C->m_const_spec_id;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_spec64_id(
		const aclspv_wrd_t c_key, 
		const u64_least c_val, h_util_ctx_t h_ctx)
{
	util_constant* ae2f_restrict const C = aclutil_mk_constant_node(c_key, h_ctx);
	ae2f_expected_but_else(ae2f_expected(C)) return 0;

	if(C->m_const_spec_id) 
		return C->m_const_spec_type == ID_DEFAULT_I64 
			? C->m_const_spec_id
			: 0;

	ae2f_expected_but_else(aclid_mk_default_id(ID_DEFAULT_I64, h_ctx))
		return 0;

	ae2f_expected_but_else(h_ctx->m_count.m_types = util_emitx_5(
			&h_ctx->m_section.m_types
			, h_ctx->m_count.m_types
			, SpvOpSpecConstant
			, ID_DEFAULT_I64
			, h_ctx->m_id
			, (u32_least)(c_val & 0xFFFFFFFF)
			, (u32_least)(c_val >> 32) & 0xFFFFFFFF
			))
		return 0;

	ae2f_expected_but_else(h_ctx->m_count.m_decorate = util_emitx_4(
			&h_ctx->m_section.m_decorate
			, h_ctx->m_count.m_decorate
			, SpvOpDecorate
			, h_ctx->m_id
			, SpvDecorationSpecId
			, c_key)) return 0;

	C->m_const_spec_id	= h_ctx->m_id++;
	C->m_const_spec_type	= ID_DEFAULT_I64;
	return C->m_const_spec_id;
}

ACLSPV_ABI_IMPL aclspv_id_t	aclutil_mk_constant_spec_id_f64(
		const aclspv_wrd_t c_key, 
		const double c_val, h_util_ctx_t h_ctx)
{
	union {
		double		m_f;
		u64_least	m_u;
	} FTOI;

	util_constant* ae2f_restrict const C = aclutil_mk_constant_node(c_key, h_ctx);
	FTOI.m_f = c_val;

	ae2f_expected_but_else(ae2f_expected(C)) return 0;

	if(C->m_const_spec_id) 
		return C->m_const_spec_type == ID_DEFAULT_F64 
			? C->m_const_spec_id 
			: 0;

	ae2f_expected_but_else(aclid_mk_default_id(ID_DEFAULT_F64, h_ctx))
		return 0;

	ae2f_expected_but_else(h_ctx->m_count.m_types = util_emitx_5(
			&h_ctx->m_section.m_types
			, h_ctx->m_count.m_types
			, SpvOpSpecConstant
			, ID_DEFAULT_F64
			, h_ctx->m_id
			, (u32_least)(FTOI.m_u & 0xFFFFFFFF)
			, (u32_least)(FTOI.m_u >> 32) & 0xFFFFFFFF
			))
		return 0;

	ae2f_expected_but_else(h_ctx->m_count.m_decorate = util_emitx_4(
			&h_ctx->m_section.m_decorate
			, h_ctx->m_count.m_decorate
			, SpvOpDecorate
			, h_ctx->m_id
			, SpvDecorationSpecId
			, c_key)) return 0;

	C->m_const_spec_id	= h_ctx->m_id++;
	C->m_const_spec_type	= ID_DEFAULT_F64;
	return C->m_const_spec_id;
}


