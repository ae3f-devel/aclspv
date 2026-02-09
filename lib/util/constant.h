/** @file constant.h */

#ifndef	lib_build_constant_h
#define lib_build_constant_h

#include <string.h>

#include <ae2f/Keys.h>
#include <ae2f/c90/StdInt.h>

#include <aclspv/spvty.h>

#include "./id.h"
#include "./ctx.h"
#include "./emitx.h"

#include "./constant/ptr.auto.h"

/** TODO: make this modular. this is risky */
ae2f_inline static aclspv_id_t util_mk_default_id(
		const e_id_default	c_id_default,
		h_util_ctx_t	h_ctx
		);

typedef struct {
	aclspv_wrd_t		m_key_0;
	aclspv_wrd_t		m_key_1;
	aclspv_id_t		m_const_val_id;
	aclspv_id_t		m_const_val64_id;
	aclspv_id_t		m_const_val_f32_id;
	aclspv_id_t		m_const_val_f64_id;
	aclspv_id_t		m_const_spec_id;
	aclspv_id_t		m_const_spec64_id;
	aclspv_id_t		m_const_spec_f32_id;
	aclspv_id_t		m_const_spec_f64_id;
	aclspv_id_t		m_const_spec_type_id;

	/** array type id */
	aclspv_id_t		m_arr8_id;
	aclspv_id_t		m_arr16_id;
	aclspv_id_t		m_arr32_id;
	aclspv_id_t		m_arrspec32_id;

	aclspv_id_t		m_vec32_id;

	/** u32_vec4_id */
	aclspv_id_t		m_arr128_id;

	/** struct type id (for storage) */
	aclspv_id_t		m_struct_id;
	aclspv_id_t		m_struct128_id;

	/** struct type id (for private, workgroup) */
	aclspv_id_t		m_structpriv_id;
	aclspv_id_t		m_structprivspec_id;
	aclspv_id_t		m_structpriv128_id;

	/** push constant pointer id */
	aclspv_id_t		m_ptr_psh;

	/** storage buffer pointer id */
	aclspv_id_t		m_ptr_storage;
	aclspv_id_t		m_ptr_func;

	/** uniform pointer id */
	aclspv_id_t		m_ptr_uniform;
	aclspv_id_t		m_ptr_uniformconst;

	/** workgroup pointer id */
	aclspv_id_t		m_ptr_work;
	aclspv_id_t		m_ptr_workspec;

	/** normal pointer id */
	aclspv_id_t		m_ptr;
} util_constant;

typedef util_constant* ae2f_restrict p_util_constant_t;

ae2f_inline ae2f_ccpure static util_constant* util_get_constant_node(
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

ae2f_inline static util_constant* util_mk_constant_node(
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

ae2f_inline static aclspv_id_t	util_mk_constant_val_id(const aclspv_wrd_t c_val, h_util_ctx_t h_ctx)
{
	util_constant* ae2f_restrict const C = util_mk_constant_node((u64_least)c_val, h_ctx);
	ae2f_expected_but_else(C) return 0;

	if(C->m_const_val_id) return C->m_const_val_id;


	ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_I32, h_ctx))
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

ae2f_inline static aclspv_id_t	util_mk_constant_val64_id(const u64_least c_val, h_util_ctx_t h_ctx)
{
	util_constant* ae2f_restrict const C = util_mk_constant_node(c_val, h_ctx);
	ae2f_expected_but_else(C) return 0;

	if(C->m_const_val64_id) return C->m_const_val64_id;


	ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_I64, h_ctx))
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

ae2f_inline static aclspv_id_t	util_mk_constant_val_f32_id(const float c_val, h_util_ctx_t h_ctx)
{
	union {
		float		m_flt;
		u32_least	m_u32;
	} FLT_U32;

	util_constant* ae2f_restrict C;

	FLT_U32.m_flt = c_val;
	C = util_mk_constant_node(FLT_U32.m_u32, h_ctx);

	ae2f_expected_but_else(C) return 0;

	if(C->m_const_val_f32_id) return C->m_const_val_f32_id;

	ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_F32, h_ctx))
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

ae2f_inline static aclspv_id_t	util_mk_constant_val_f64_id(const double c_val, h_util_ctx_t h_ctx)
{
	union {
		double		m_flt;
		u64_least	m_u64;
	} FLT_U64;

	util_constant* ae2f_restrict C;

	FLT_U64.m_flt = c_val;
	C = util_mk_constant_node(FLT_U64.m_u64, h_ctx);

	ae2f_expected_but_else(C) return 0;

	if(C->m_const_val_f64_id) return C->m_const_val_f64_id;

	ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_F64, h_ctx))
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

ae2f_inline static aclspv_id_t	util_mk_constant_spec_id(const aclspv_wrd_t c_key, const aclspv_wrd_t c_val, h_util_ctx_t h_ctx)
{
	util_constant* ae2f_restrict const C = util_mk_constant_node(c_key, h_ctx);
	ae2f_expected_but_else(ae2f_expected(C)) return 0;

	if(C->m_const_spec_id) return C->m_const_spec_id;

	ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_I32, h_ctx))
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

	C->m_const_spec_id = h_ctx->m_id++;
	return C->m_const_spec_id;
}

ae2f_inline static aclspv_id_t	util_mk_constant_spec_id_f32(const aclspv_wrd_t c_key, const float c_val, h_util_ctx_t h_ctx)
{
	union {
		float		m_f;
		aclspv_wrd_t	m_u;
	} FTOI;
	util_constant* ae2f_restrict const C = util_mk_constant_node(c_key, h_ctx);
	ae2f_expected_but_else(ae2f_expected(C)) return 0;
	FTOI.m_f = c_val;

	if(C->m_const_spec_f32_id) return C->m_const_spec_f32_id;

	ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_F32, h_ctx))
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

	C->m_const_spec_f32_id = h_ctx->m_id++;
	return C->m_const_spec_f32_id;
}

ae2f_inline static aclspv_id_t	util_mk_constant_spec64_id(const aclspv_wrd_t c_key, const u64_least c_val, h_util_ctx_t h_ctx)
{
	util_constant* ae2f_restrict const C = util_mk_constant_node(c_key, h_ctx);
	ae2f_expected_but_else(ae2f_expected(C)) return 0;

	if(C->m_const_spec64_id) return C->m_const_spec64_id;

	ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_I64, h_ctx))
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

	C->m_const_spec64_id = h_ctx->m_id++;
	return C->m_const_spec64_id;
}

ae2f_inline static aclspv_id_t	util_mk_constant_spec_id_f64(const aclspv_wrd_t c_key, const double c_val, h_util_ctx_t h_ctx)
{
	union {
		double		m_f;
		u64_least	m_u;
	} FTOI;

	util_constant* ae2f_restrict const C = util_mk_constant_node(c_key, h_ctx);
	FTOI.m_f = c_val;

	ae2f_expected_but_else(ae2f_expected(C)) return 0;

	if(C->m_const_spec_f64_id) return C->m_const_spec_f64_id;

	ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_F64, h_ctx))
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

	C->m_const_spec_f64_id = h_ctx->m_id++;
	return C->m_const_spec_f64_id;
}

ae2f_inline static aclspv_id_t	util_mk_constant_vec32_id(const aclspv_wrd_t c_val, h_util_ctx_t h_ctx)
{
	util_constant* ae2f_restrict const C = util_mk_constant_node(c_val, h_ctx);
	ae2f_expected_but_else(C) return 0;
	ae2f_expected_but_else(C->m_key_0 == c_val && !C->m_key_1) return 0;


	if(C->m_vec32_id) return C->m_vec32_id;


	ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_I32, h_ctx))
		return 0;

	if(c_val == 1) return ID_DEFAULT_I32;

	/** OpTypeVector */
	ae2f_expected_but_else((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypeVector, 3))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, ID_DEFAULT_I32))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, c_val))) return 0;  

	C->m_vec32_id = h_ctx->m_id++;

	return C->m_vec32_id;
}

ae2f_inline static aclspv_id_t	util_mk_constant_arr8_id(const aclspv_wrd_t c_arrcount, h_util_ctx_t h_ctx)
{
	util_constant* ae2f_restrict const C = util_mk_constant_node(c_arrcount, h_ctx);
	ae2f_expected_but_else(C) return 0;
	ae2f_expected_but_else(C->m_key_0 == c_arrcount && !C->m_key_1) return 0;
	ae2f_expected_but_else(C->m_const_val_id)
		C->m_const_val_id = util_mk_constant_val_id(c_arrcount, h_ctx);

	if(C->m_arr8_id) return C->m_arr8_id;
	ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_I8, h_ctx))
		return 0;
	if(c_arrcount == 1) return ID_DEFAULT_I8;

	/** OpArray */
	ae2f_expected_but_else((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypeArray, 3))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, ID_DEFAULT_I8))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, C->m_const_val_id))) return 0;

	ae2f_expected_but_else((h_ctx->m_count.m_decorate = emit_opcode(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, SpvOpDecorate, 3)))
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_decorate = util_emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, h_ctx->m_id))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_decorate = util_emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, SpvDecorationArrayStride))) 
		return 0;
	/** TODO: research this */
	ae2f_expected_but_else((h_ctx->m_count.m_decorate = util_emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, 4)))
		return 0;


	C->m_arr8_id = h_ctx->m_id++;

	return C->m_arr8_id;
}

ae2f_inline static aclspv_id_t	util_mk_constant_arr16_id(const aclspv_wrd_t c_arrcount, h_util_ctx_t h_ctx)
{
	util_constant* ae2f_restrict const C = util_mk_constant_node(c_arrcount, h_ctx);
	ae2f_expected_but_else(C) return 0;
	ae2f_expected_but_else(C->m_key_0 == c_arrcount && !C->m_key_1) return 0;
	ae2f_expected_but_else(C->m_const_val_id)
		C->m_const_val_id = util_mk_constant_val_id(c_arrcount, h_ctx);

	if(C->m_arr16_id) return C->m_arr16_id;
	ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_I16, h_ctx))
		return 0;
	if(c_arrcount == 1) return ID_DEFAULT_I16;

	/** OpArray */
	ae2f_expected_but_else((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypeArray, 3))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, ID_DEFAULT_I16))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, C->m_const_val_id))) return 0;

	ae2f_expected_but_else((h_ctx->m_count.m_decorate = emit_opcode(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, SpvOpDecorate, 3)))
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_decorate = util_emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, h_ctx->m_id))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_decorate = util_emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, SpvDecorationArrayStride))) 
		return 0;
	/** TODO: research this */
	ae2f_expected_but_else((h_ctx->m_count.m_decorate = util_emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, 4)))
		return 0;


	C->m_arr16_id = h_ctx->m_id++;

	return C->m_arr16_id;
}

ae2f_inline static aclspv_id_t	util_mk_constant_arr32_id(const aclspv_wrd_t c_arrcount, h_util_ctx_t h_ctx)
{
	util_constant* ae2f_restrict const C = util_mk_constant_node(c_arrcount, h_ctx);
	ae2f_expected_but_else(C) return 0;
	ae2f_expected_but_else(C->m_key_0 == c_arrcount && !C->m_key_1) return 0;
	ae2f_expected_but_else(C->m_const_val_id)
		C->m_const_val_id = util_mk_constant_val_id(c_arrcount, h_ctx);

	if(C->m_arr32_id) return C->m_arr32_id;
	ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_I32, h_ctx))
		return 0;
	if(c_arrcount == 1) return ID_DEFAULT_I32;

	/** OpArray */
	ae2f_expected_but_else((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypeArray, 3))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, ID_DEFAULT_I32))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, C->m_const_val_id))) return 0;

	ae2f_expected_but_else((h_ctx->m_count.m_decorate = emit_opcode(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, SpvOpDecorate, 3))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_decorate = util_emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, h_ctx->m_id))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_decorate = util_emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, SpvDecorationArrayStride))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_decorate = util_emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, 4)))
		return 0;


	C->m_arr32_id = h_ctx->m_id++;

	return C->m_arr32_id;
}

ae2f_inline static aclspv_id_t	util_mk_constant_arrspec32_id(const aclspv_wrd_t c_key, const aclspv_wrd_t c_arrcount, h_util_ctx_t h_ctx)
{
	util_constant* ae2f_restrict const C = util_mk_constant_node(c_key, h_ctx);
	ae2f_expected_but_else(C) return 0;
	ae2f_expected_but_else(C->m_key_0 == c_key && !C->m_key_1) return 0;
	ae2f_expected_but_else(C->m_const_spec_id)
		util_mk_constant_spec_id(c_key, c_arrcount, h_ctx);

	if(C->m_arrspec32_id) return C->m_arrspec32_id;
	ae2f_expected_but_else(util_mk_default_id(ID_DEFAULT_I32, h_ctx))
		return 0;

	/** OpArray */
	ae2f_expected_but_else((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypeArray, 3))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, ID_DEFAULT_I32))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, C->m_const_spec_id))) return 0;

	ae2f_expected_but_else((h_ctx->m_count.m_decorate = emit_opcode(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, SpvOpDecorate, 3))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_decorate = util_emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, h_ctx->m_id))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_decorate = util_emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, SpvDecorationArrayStride))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_decorate = util_emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, 4)))
		return 0;


	C->m_arrspec32_id = h_ctx->m_id++;

	return C->m_arrspec32_id;
}

ae2f_inline static aclspv_id_t	util_mk_constant_arr128_id(const aclspv_wrd_t c_arrcount, h_util_ctx_t h_ctx)
{
	util_constant* ae2f_restrict const C	= util_mk_constant_node(c_arrcount, h_ctx);
	const aclspv_id_t ID_U32V4	= util_mk_constant_vec32_id(4, h_ctx);

	ae2f_expected_but_else(C)			return 0;
	ae2f_expected_but_else(C->m_key_0 == c_arrcount && !C->m_key_1)	return 0;
	ae2f_expected_but_else(util_mk_constant_val_id(c_arrcount, h_ctx)) return 0;
	assert(ID_U32V4);


	if(C->m_arr128_id) return C->m_arr128_id;

	if(c_arrcount == 1) return ID_U32V4;



	assert(C->m_const_val_id);

	/** OpArray */
	ae2f_expected_but_else(h_ctx->m_count.m_types = (aclspv_wrd_t)util_emitx_4(
				&h_ctx->m_section.m_types
				, h_ctx->m_count.m_types
				, SpvOpTypeArray
				, h_ctx->m_id
				, ID_U32V4
				, C->m_const_val_id
				)) return 0;

	ae2f_expected_but_else(util_emitx_4(
				&h_ctx->m_section.m_decorate
				, h_ctx->m_count.m_decorate
				, SpvOpDecorate, h_ctx->m_id
				, SpvDecorationArrayStride, 16))
		return 0;

	assert(h_ctx->m_id);
	assert(C->m_arr128_id);
	C->m_arr128_id = h_ctx->m_id++;

	return C->m_arr128_id;
}

ae2f_inline static aclspv_id_t	util_mk_constant_struct_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	util_constant* ae2f_restrict const C = util_mk_constant_node(c_wrdcount, h_ctx);
	ae2f_expected_but_else(C) return 0;
	ae2f_expected_but_else(C->m_key_0 == c_wrdcount && !C->m_key_1) return 0;
	ae2f_expected_but_else(C->m_arr32_id && c_wrdcount > 1)
		C->m_arr32_id = util_mk_constant_arr32_id(c_wrdcount, h_ctx);

	if(C->m_struct_id) return C->m_struct_id;

	ae2f_expected_but_else((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, SpvOpTypeStruct, 2))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, h_ctx->m_id))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, C->m_arr32_id))) return 0;



	ae2f_expected_but_else((h_ctx->m_count.m_decorate = emit_opcode(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, SpvOpDecorate, 2))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_decorate = util_emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, h_ctx->m_id))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_decorate = util_emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, SpvDecorationBlock))) 
		return 0;

	/* Member Offset */
	ae2f_expected_but_else(h_ctx->m_count.m_decorate = util_emitx_5(
				&h_ctx->m_section.m_decorate
				, h_ctx->m_count.m_decorate
				, SpvOpMemberDecorate
				, h_ctx->m_id
				, 0
				, SpvDecorationOffset
				, 0
				)) return 0;

	C->m_struct_id = h_ctx->m_id++;
	return C->m_struct_id;
}

ae2f_inline static aclspv_id_t	util_mk_constant_struct128_id(const aclspv_wrd_t c_veccount, h_util_ctx_t h_ctx) {
#if 1
	util_constant* ae2f_restrict const C = util_mk_constant_node(c_veccount, h_ctx);

	ae2f_expected_but_else(C) {
		assert(0 && "C is null");
		return 0;
	}
	ae2f_expected_but_else(C->m_arr128_id = util_mk_constant_arr128_id(c_veccount, h_ctx)) {
		assert(0 && "arr128 is null");
		return 0;
	}
	if(C->m_struct128_id)	return C->m_struct128_id;
	assert(!C->m_struct128_id);

	ae2f_expected_but_else(h_ctx->m_count.m_types = util_emitx_3(
				&h_ctx->m_section.m_types
				, h_ctx->m_count.m_types
				, SpvOpTypeStruct
				, h_ctx->m_id
				, C->m_arr128_id)) {
		assert(0 && "alloc failed");
		return 0;
	}

	ae2f_expected_but_else((h_ctx->m_count.m_decorate = emit_opcode(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, SpvOpDecorate, 2))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_decorate = util_emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, h_ctx->m_id))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_decorate = util_emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, SpvDecorationBlock))) 
		return 0;

	/* Member Offset */
	ae2f_expected_but_else(h_ctx->m_count.m_decorate = util_emitx_5(
				&h_ctx->m_section.m_decorate
				, h_ctx->m_count.m_decorate
				, SpvOpMemberDecorate
				, h_ctx->m_id
				, 0
				, SpvDecorationOffset
				, 0
				)) return 0;


	C->m_struct128_id = h_ctx->m_id++;

	return C->m_struct128_id;
#else
	const aclspv_id_t RET = util_mk_constant_struct_id(c_veccount << 2, h_ctx);
	return RET;
#endif
}

ae2f_inline static aclspv_id_t	util_mk_constant_structprivspec_id(const aclspv_wrd_t c_key, const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	util_constant* ae2f_restrict const C = util_mk_constant_node(c_key, h_ctx);
	ae2f_expected_but_else(C) return 0;
	ae2f_expected_but_else(C->m_key_0 == c_key && !C->m_key_1) return 0;
	ae2f_expected_but_else(C->m_arrspec32_id)
		util_mk_constant_arrspec32_id(c_key, c_wrdcount, h_ctx);

	if(C->m_structprivspec_id) return C->m_structprivspec_id;

	ae2f_expected_but_else((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, SpvOpTypeStruct, 2))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, h_ctx->m_id))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, C->m_arrspec32_id))) return 0;

	C->m_structprivspec_id = h_ctx->m_id++;
	return C->m_structprivspec_id;
}

ae2f_inline static aclspv_id_t	util_mk_constant_structpriv_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	util_constant* ae2f_restrict const C = util_mk_constant_node(c_wrdcount, h_ctx);
	ae2f_expected_but_else(C) return 0;
	ae2f_expected_but_else(C->m_key_0 == c_wrdcount && !C->m_key_1) return 0;
	ae2f_expected_but_else(C->m_arr32_id)
		ae2f_expected_but_else(C->m_arr32_id = util_mk_constant_arr32_id(c_wrdcount, h_ctx))
		return 0;

	if(C->m_structpriv_id) return C->m_structpriv_id;

	ae2f_expected_but_else((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, SpvOpTypeStruct, 2))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, h_ctx->m_id))) return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, C->m_arr32_id))) return 0;

	C->m_structpriv_id = h_ctx->m_id++;
	return C->m_structpriv_id;
}


ae2f_inline static aclspv_id_t	util_mk_constant_ptr_psh_id(const aclspv_wrd_t c_wrdcount, const h_util_ctx_t h_ctx) {

	aclspv_id_t	ID;
	_util_mk_constant_ptr_tmpl(
			L_new, m_ptr_psh
			, m_struct_id
			, ID, c_wrdcount
			, h_ctx
			, util_mk_constant_struct_id
			, SpvStorageClassPushConstant);
	return ID;
}

ae2f_inline static aclspv_id_t	util_mk_constant_ptr_storage_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	aclspv_id_t	ID;
	_util_mk_constant_ptr_tmpl(
			L_new, m_ptr_storage
			, m_struct_id
			, ID, c_wrdcount
			, h_ctx
			, util_mk_constant_struct_id
			, SpvStorageClassStorageBuffer);
	return ID;
}

ae2f_inline static aclspv_id_t	util_mk_constant_ptr_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	aclspv_id_t	ID;
	_util_mk_constant_ptr_tmpl(
			L_new, m_ptr
			, m_structpriv_id
			, ID, c_wrdcount
			, h_ctx
			, util_mk_constant_structpriv_id
			, SpvStorageClassPrivate);
	return ID;
}

ae2f_inline static aclspv_id_t	util_mk_constant_ptr_func(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	aclspv_id_t	ID;
	_util_mk_constant_ptr_tmpl(
			L_new, m_ptr_func
			, m_structpriv_id
			, ID, c_wrdcount
			, h_ctx
			, util_mk_constant_structpriv_id
			, SpvStorageClassFunction);
	return ID;
}

ae2f_inline static aclspv_id_t	util_mk_constant_ptr_uniform_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	util_constant* ae2f_restrict const C = util_mk_constant_node(c_wrdcount, h_ctx);
	ae2f_expected_but_else(C) return 0;
	ae2f_expected_but_else(C->m_struct128_id)
		C->m_struct128_id = util_mk_constant_struct128_id(sz_to_count(c_wrdcount), h_ctx);

	if(C->m_ptr_uniform) return C->m_ptr_uniform;

	ae2f_expected_but_else((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypePointer, 3))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvStorageClassUniform))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, C->m_struct128_id)))
		return 0;

	C->m_ptr_uniform = h_ctx->m_id++;

	return C->m_ptr_uniform;
}

ae2f_inline static aclspv_id_t	util_mk_constant_ptr_uniformconstant_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	util_constant* ae2f_restrict const C = util_mk_constant_node(c_wrdcount, h_ctx);
	ae2f_expected_but_else(C) return 0;
	ae2f_expected_but_else(C->m_struct128_id)
		C->m_struct128_id = util_mk_constant_struct128_id(sz_to_count(c_wrdcount), h_ctx);

	if(C->m_ptr_uniformconst) return C->m_ptr_uniformconst;

	ae2f_expected_but_else((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypePointer, 3))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvStorageClassUniformConstant))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, C->m_struct128_id)))
		return 0;

	C->m_ptr_uniformconst = h_ctx->m_id++;

	return C->m_ptr_uniformconst;
}

ae2f_inline static aclspv_id_t	util_mk_constant_ptr_workspec_id(const aclspv_wrd_t c_key, const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	util_constant* ae2f_restrict C = util_mk_constant_node(c_key, h_ctx);
	ae2f_expected_but_else(C) return 0;
	ae2f_expected_but_else(C->m_structprivspec_id)
		(void)util_mk_constant_structprivspec_id(c_key, c_wrdcount, h_ctx);

	if(C->m_ptr_workspec) return C->m_ptr_workspec;


	ae2f_expected_but_else((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypePointer, 3))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvStorageClassWorkgroup))) 
		return 0;
	ae2f_expected_but_else((h_ctx->m_count.m_types = util_emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, C->m_structpriv_id)))
		return 0;

	C->m_ptr_workspec = h_ctx->m_id++;

	return C->m_ptr_workspec;
}

ae2f_inline static aclspv_id_t	util_mk_constant_ptr_work_id(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx) {
	aclspv_id_t	ID;
	_util_mk_constant_ptr_tmpl(
			L_new, m_ptr_work
			, m_structpriv_id
			, ID, c_wrdcount
			, h_ctx
			, util_mk_constant_structpriv_id
			, SpvStorageClassWorkgroup);
	return ID;
}

ae2f_inline static aclspv_id_t	util_mk_constant_ptr_by_enum(const aclspv_wrd_t c_wrdcount, h_util_ctx_t h_ctx, enum SpvStorageClass_ c_class) {
	switch((aclspv_wrd_t)c_class) {
		case SpvStorageClassWorkgroup:
			return util_mk_constant_ptr_work_id(c_wrdcount, h_ctx);
		case SpvStorageClassPrivate:
			return util_mk_constant_ptr_id(c_wrdcount, h_ctx);
		case SpvStorageClassUniform:
			return util_mk_constant_ptr_uniform_id(c_wrdcount, h_ctx);
		case SpvStorageClassStorageBuffer:
			return util_mk_constant_ptr_storage_id(c_wrdcount, h_ctx);
		case SpvStorageClassPushConstant:
			return util_mk_constant_ptr_psh_id(c_wrdcount, h_ctx);

		default:
			ae2f_unreachable();
			return 0;
	}

}

#endif
