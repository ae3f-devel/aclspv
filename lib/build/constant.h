#ifndef	lib_build_constant_h
#define lib_build_constant_h

#include "./iddef.h"

#include <build.h>
#include <string.h>
#include <ae2f/c90/StdInt.h>

typedef struct {
	aclspv_wrdcount_t	m_key;
	aclspv_id_t		m_const_val_id;

	/** array type id */
	aclspv_id_t		m_arr8_id;
	aclspv_id_t		m_arr16_id;
	aclspv_id_t		m_arr32_id;

	/** u32_vec4_id */
	aclspv_id_t		m_arr128_id;

	/** struct type id (for storage) */
	aclspv_id_t		m_struct_id;
	aclspv_id_t		m_struct128_id;

	/** struct type id (for private, workgroup) */
	aclspv_id_t		m_structpriv_id;
	aclspv_id_t		m_structpriv128_id;

	/** push constant pointer id */
	aclspv_id_t		m_ptr_psh;

	/** storage buffer pointer id */
	aclspv_id_t		m_ptr_storage;

	/** uniform pointer id */
	aclspv_id_t		m_ptr_uniform;

	/** workgroup pointer id */
	aclspv_id_t		m_ptr_work;

	/** normal pointer id */
	aclspv_id_t		m_ptr;
} lib_build_constant;

typedef lib_build_constant* ae2f_restrict p_lib_build_constant_t;

ae2f_inline ae2f_ccpure static lib_build_constant* lib_build_get_constant_node(
		const aclspv_wrdcount_t c_key, 
		h_aclspv_build_ctx_t h_ctx
		)
{
	const aclspv_wrdcount_t	COUNT = (aclspv_wrdcount_t)(h_ctx->m_constant_cache.m_sz / (size_t)sizeof(lib_build_constant));
	aclspv_wrdcount_t	LEFT	= 0;
	aclspv_wrdcount_t	RIGHT	= COUNT - 1;

	if(COUNT) {
		while(LEFT < RIGHT) {
			const aclspv_wrdcount_t	MIDDLE	= LEFT + ((RIGHT - LEFT) >> 1);


			if(((p_lib_build_constant_t)h_ctx->m_constant_cache.m_p)[MIDDLE].m_key == c_key)
				return &((p_lib_build_constant_t)h_ctx->m_constant_cache.m_p)[MIDDLE];

			if(((p_lib_build_constant_t)h_ctx->m_constant_cache.m_p)[MIDDLE].m_key < c_key) {
				LEFT = MIDDLE + 1;
			} else {
				RIGHT = MIDDLE - 1;
			}
		}


		if(((p_lib_build_constant_t)h_ctx->m_constant_cache.m_p)[LEFT].m_key == c_key)
			return &((p_lib_build_constant_t)h_ctx->m_constant_cache.m_p)[LEFT];
	}

	return ae2f_NIL;
}

ae2f_inline static lib_build_constant* lib_build_mk_constant_node(
		const aclspv_wrdcount_t	c_key,
		h_aclspv_build_ctx_t h_ctx
		)
{
	const aclspv_wrdcount_t	COUNT = (aclspv_wrdcount_t)(h_ctx->m_constant_cache.m_sz / (size_t)sizeof(lib_build_constant));
	aclspv_wrdcount_t	LEFT	= 0;
	aclspv_wrdcount_t	RIGHT	= COUNT - 1;

	if(COUNT) {
		while(LEFT < RIGHT) {
			const aclspv_wrdcount_t	MIDDLE	= LEFT + ((RIGHT - LEFT) >> 1);

			if(((p_lib_build_constant_t)h_ctx->m_constant_cache.m_p)[MIDDLE].m_key == c_key)
				return &((p_lib_build_constant_t)h_ctx->m_constant_cache.m_p)[MIDDLE];

			if(((p_lib_build_constant_t)h_ctx->m_constant_cache.m_p)[MIDDLE].m_key < c_key) {
				LEFT = MIDDLE + 1;
			} else {
				RIGHT = MIDDLE > 0 ? MIDDLE - 1 : 0;
			}
		}

		if(((p_lib_build_constant_t)h_ctx->m_constant_cache.m_p)[LEFT].m_key == c_key)
			return &((p_lib_build_constant_t)h_ctx->m_constant_cache.m_p)[LEFT];
	}

	_aclspv_grow_vec_with_copy(
			_aclspv_malloc, _aclspv_free
			, _aclspv_memcpy
			, L_new, h_ctx->m_constant_cache
			, (size_t)(h_ctx->m_constant_cache.m_sz + (size_t)sizeof(lib_build_constant))
			);
	unless(h_ctx->m_constant_cache.m_p) return ae2f_NIL;

	memmove(
			&((p_lib_build_constant_t)(h_ctx->m_constant_cache.m_p))[LEFT + 1]
			, &((p_lib_build_constant_t)(h_ctx->m_constant_cache.m_p))[LEFT]
			, (size_t)((COUNT - LEFT) * (sizeof(lib_build_constant)))
	       );

	/** sanity check for future possibility of having more elements */
	memset(&((p_lib_build_constant_t)(h_ctx->m_constant_cache.m_p))[LEFT], 0, sizeof(lib_build_constant));
	((p_lib_build_constant_t)(h_ctx->m_constant_cache.m_p))[LEFT].m_key = c_key;

	return &((p_lib_build_constant_t)(h_ctx->m_constant_cache.m_p))[LEFT];
}

#include <spirv/unified1/spirv.h>
#include "./wrdemit.h"

ae2f_inline static aclspv_id_t	lib_build_mk_constant_val_id(const aclspv_wrd_t c_val, h_aclspv_build_ctx_t h_ctx)
{
	lib_build_constant* ae2f_restrict const C = lib_build_mk_constant_node(c_val, h_ctx);
	unless(C) return 0;
	unless(C->m_key == c_val) return 0;
	if(C->m_const_val_id) return C->m_const_val_id;

	unless(lib_build_get_default_id(ID_DEFAULT_U32, h_ctx))
		return 0;

	/** OpConstant */
	unless((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpConstant, 3))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, ID_DEFAULT_U32))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, c_val))) return 0;  

	C->m_const_val_id = h_ctx->m_id++;
	return C->m_const_val_id;
}

ae2f_inline static aclspv_id_t	lib_build_mk_constant_arr8_id(const aclspv_wrd_t c_arrcount, h_aclspv_build_ctx_t h_ctx)
{
	lib_build_constant* ae2f_restrict const C = lib_build_mk_constant_node(c_arrcount, h_ctx);
	unless(C) return 0;
	unless(C->m_key == c_arrcount) return 0;
	unless(C->m_const_val_id)
		C->m_const_val_id = lib_build_mk_constant_val_id(c_arrcount, h_ctx);

	if(C->m_arr8_id) return C->m_arr8_id;
	unless(lib_build_get_default_id(ID_DEFAULT_U8, h_ctx))
		return 0;
	if(C->m_key == 1) return ID_DEFAULT_U8;


	/** OpArray */
	unless((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypeArray, 3))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, ID_DEFAULT_U8))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, C->m_const_val_id))) return 0;

	unless((h_ctx->m_count.m_decorate = emit_opcode(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, SpvOpDecorate, 3)))
		return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, h_ctx->m_id))) 
		return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, SpvDecorationArrayStride))) 
		return 0;
	/** TODO: research this */
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, 4)))
		return 0;


	C->m_arr8_id = h_ctx->m_id++;

	return C->m_arr8_id;
}

ae2f_inline static aclspv_id_t	lib_build_mk_constant_arr16_id(const aclspv_wrd_t c_arrcount, h_aclspv_build_ctx_t h_ctx)
{
	lib_build_constant* ae2f_restrict const C = lib_build_mk_constant_node(c_arrcount, h_ctx);
	unless(C) return 0;
	unless(C->m_key == c_arrcount) return 0;
	unless(C->m_const_val_id)
		C->m_const_val_id = lib_build_mk_constant_val_id(c_arrcount, h_ctx);

	if(C->m_arr16_id) return C->m_arr16_id;
	unless(lib_build_get_default_id(ID_DEFAULT_U16, h_ctx))
		return 0;
	if(C->m_key == 1) return ID_DEFAULT_U16;

	/** OpArray */
	unless((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypeArray, 3))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, ID_DEFAULT_U16))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, C->m_const_val_id))) return 0;

	unless((h_ctx->m_count.m_decorate = emit_opcode(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, SpvOpDecorate, 3)))
		return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, h_ctx->m_id))) 
		return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, SpvDecorationArrayStride))) 
		return 0;
	/** TODO: research this */
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, 4)))
		return 0;


	C->m_arr16_id = h_ctx->m_id++;

	return C->m_arr16_id;
}

ae2f_inline static aclspv_id_t	lib_build_mk_constant_arr32_id(const aclspv_wrd_t c_arrcount, h_aclspv_build_ctx_t h_ctx)
{
	lib_build_constant* ae2f_restrict const C = lib_build_mk_constant_node(c_arrcount, h_ctx);
	unless(C) return 0;
	unless(C->m_key == c_arrcount) return 0;
	unless(C->m_const_val_id)
		C->m_const_val_id = lib_build_mk_constant_val_id(c_arrcount, h_ctx);

	if(C->m_arr32_id) return C->m_arr32_id;
	unless(lib_build_get_default_id(ID_DEFAULT_U32, h_ctx))
		return 0;
	if(C->m_key == 1) return ID_DEFAULT_U32;

	/** OpArray */
	unless((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypeArray, 3))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, ID_DEFAULT_U32))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, C->m_const_val_id))) return 0;

	unless((h_ctx->m_count.m_decorate = emit_opcode(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, SpvOpDecorate, 3))) 
		return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, h_ctx->m_id))) 
		return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, SpvDecorationArrayStride))) 
		return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, 4)))
		return 0;


	C->m_arr32_id = h_ctx->m_id++;

	return C->m_arr32_id;
}

ae2f_inline static aclspv_id_t	lib_build_mk_constant_arr128_id(const aclspv_wrd_t c_arrcount, h_aclspv_build_ctx_t h_ctx)
{
	lib_build_constant* ae2f_restrict const C = lib_build_mk_constant_node(c_arrcount, h_ctx);
	unless(C) return 0;
	unless(C->m_key == c_arrcount) return 0;
	unless(C->m_const_val_id)
		C->m_const_val_id = lib_build_mk_constant_val_id(c_arrcount, h_ctx);

	if(C->m_arr128_id) return C->m_arr128_id;
	unless(lib_build_get_default_id(ID_DEFAULT_U32_VEC4, h_ctx))
		return 0;
	if(C->m_key == 1) return ID_DEFAULT_U32_VEC4;

	/** OpArray */
	unless((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypeArray, 3))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, ID_DEFAULT_U32_VEC4))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, C->m_const_val_id))) return 0;

	unless((h_ctx->m_count.m_decorate = emit_opcode(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, SpvOpDecorate, 3))) 
		return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, h_ctx->m_id))) 
		return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate
					, SpvDecorationArrayStride))) 
		return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, 16)))
		return 0;


	C->m_arr128_id= h_ctx->m_id++;

	return C->m_arr128_id;
}

ae2f_inline static aclspv_id_t	lib_build_mk_constant_struct_id(const aclspv_wrd_t c_wrdcount, h_aclspv_build_ctx_t h_ctx) {
	lib_build_constant* ae2f_restrict const C = lib_build_mk_constant_node(c_wrdcount, h_ctx);
	unless(C) return 0;
	unless(C->m_key == c_wrdcount) return 0;
	unless(C->m_arr32_id && C->m_key > 1)
		C->m_arr32_id = lib_build_mk_constant_arr32_id(c_wrdcount, h_ctx);

	if(C->m_struct_id) return C->m_struct_id;

	unless((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, SpvOpTypeStruct, 2))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, h_ctx->m_id))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, C->m_arr32_id))) return 0;



	unless((h_ctx->m_count.m_decorate = emit_opcode(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, SpvOpDecorate, 2))) 
		return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, h_ctx->m_id))) 
		return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, SpvDecorationBlock))) 
		return 0;

	/* Member Offset */
	unless((h_ctx->m_count.m_decorate = emit_opcode(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, SpvOpMemberDecorate, 4))) return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, h_ctx->m_id))) return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, 0))) return 0;  /** member index 0 */
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, SpvDecorationOffset))) return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, 0))) return 0;

	C->m_struct_id = h_ctx->m_id++;
	return C->m_struct_id;
}

ae2f_inline static aclspv_id_t	lib_build_mk_constant_struct128_id(const aclspv_wrd_t c_veccount, h_aclspv_build_ctx_t h_ctx) {
	lib_build_constant* ae2f_restrict const C = lib_build_mk_constant_node(c_veccount, h_ctx);
	unless(C) return 0;
	unless(C->m_key == c_veccount) return 0;
	unless(C->m_arr128_id && C->m_key > 1)
		C->m_arr128_id = lib_build_mk_constant_arr128_id(c_veccount, h_ctx);

	if(C->m_struct128_id) return C->m_struct128_id;

	unless((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, SpvOpTypeStruct, 2))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, h_ctx->m_id))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, C->m_arr128_id))) return 0;



	unless((h_ctx->m_count.m_decorate = emit_opcode(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, SpvOpDecorate, 2))) 
		return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, h_ctx->m_id))) 
		return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, SpvDecorationBlock))) 
		return 0;

	/* Member Offset */
	unless((h_ctx->m_count.m_decorate = emit_opcode(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, SpvOpMemberDecorate, 4))) return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, h_ctx->m_id))) return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, 0))) return 0;  /** member index 0 */
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, SpvDecorationOffset))) return 0;
	unless((h_ctx->m_count.m_decorate = emit_wrd(&h_ctx->m_section.m_decorate
					, h_ctx->m_count.m_decorate, 0))) return 0;

	C->m_struct_id = h_ctx->m_id++;
	return C->m_struct_id;
}

ae2f_inline static aclspv_id_t	lib_build_mk_constant_structpriv_id(const aclspv_wrd_t c_wrdcount, h_aclspv_build_ctx_t h_ctx) {
	lib_build_constant* ae2f_restrict const C = lib_build_mk_constant_node(c_wrdcount, h_ctx);
	unless(C) return 0;
	unless(C->m_key == c_wrdcount) return 0;
	unless(C->m_arr32_id && C->m_key > 1)
		C->m_arr32_id = lib_build_mk_constant_arr32_id(c_wrdcount, h_ctx);

	if(C->m_structpriv_id) return C->m_structpriv_id;

	unless((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, SpvOpTypeStruct, 2))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, h_ctx->m_id))) return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types, C->m_arr32_id))) return 0;

	C->m_structpriv_id = h_ctx->m_id++;
	return C->m_structpriv_id;
}


ae2f_inline static aclspv_id_t	lib_build_mk_constant_ptr_psh_id(const aclspv_wrd_t c_wrdcount, h_aclspv_build_ctx_t h_ctx) {
	lib_build_constant* ae2f_restrict const C = lib_build_mk_constant_node(c_wrdcount, h_ctx);
	unless(C) return 0;
	unless(C->m_key == c_wrdcount) return 0;
	unless(C->m_struct_id)
		C->m_struct_id = lib_build_mk_constant_struct_id(c_wrdcount, h_ctx);

	if(C->m_ptr_psh) return C->m_ptr_psh;

	unless((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypePointer, 3))) 
		return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) 
		return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvStorageClassPushConstant))) 
		return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, C->m_struct_id)))
		return 0;

	C->m_ptr_psh = h_ctx->m_id++;

	return C->m_ptr_psh;
}

ae2f_inline static aclspv_id_t	lib_build_mk_constant_ptr_storage_id(const aclspv_wrd_t c_wrdcount, h_aclspv_build_ctx_t h_ctx) {
	lib_build_constant* ae2f_restrict const C = lib_build_mk_constant_node(c_wrdcount, h_ctx);
	unless(C) return 0;
	unless(C->m_key == c_wrdcount) return 0;
	unless(C->m_struct_id)
		C->m_struct_id = lib_build_mk_constant_struct_id(c_wrdcount, h_ctx);

	if(C->m_ptr_storage) return C->m_ptr_storage;

	unless((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypePointer, 3))) 
		return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) 
		return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvStorageClassStorageBuffer))) 
		return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, C->m_struct_id)))
		return 0;

	C->m_ptr_storage = h_ctx->m_id++;

	return C->m_ptr_storage;
}

ae2f_inline static aclspv_id_t	lib_build_mk_constant_ptr_id(const aclspv_wrd_t c_wrdcount, h_aclspv_build_ctx_t h_ctx) {
	lib_build_constant* ae2f_restrict const C = lib_build_mk_constant_node(c_wrdcount, h_ctx);

	unless(C) return 0;
	unless(C->m_key == c_wrdcount) return 0;
	unless(C->m_structpriv_id)
		C->m_structpriv_id = lib_build_mk_constant_structpriv_id(c_wrdcount, h_ctx);

	if(C->m_ptr) return C->m_ptr;


	unless((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypePointer, 3))) 
		return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) 
		return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvStorageClassPrivate))) 
		return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, C->m_structpriv_id)))
		return 0;

	C->m_ptr = h_ctx->m_id++;

	return C->m_ptr;
}

ae2f_inline static aclspv_id_t	lib_build_mk_constant_ptr_uniform_id(const aclspv_wrd_t c_wrdcount, h_aclspv_build_ctx_t h_ctx) {
	lib_build_constant* ae2f_restrict const C = lib_build_mk_constant_node(c_wrdcount, h_ctx);
	unless(C) return 0;
	unless(C->m_key == c_wrdcount) return 0;
	unless(C->m_struct128_id)
		C->m_struct128_id = lib_build_mk_constant_struct128_id(sz_to_count(c_wrdcount), h_ctx);

	if(C->m_ptr_uniform) return C->m_ptr_uniform;

	unless((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypePointer, 3))) 
		return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) 
		return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvStorageClassUniform))) 
		return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, C->m_struct128_id)))
		return 0;

	C->m_ptr_uniform = h_ctx->m_id++;

	return C->m_ptr_uniform;
}

ae2f_inline static aclspv_id_t	lib_build_mk_constant_ptr_work_id(const aclspv_wrd_t c_wrdcount, h_aclspv_build_ctx_t h_ctx) {
	lib_build_constant* ae2f_restrict C = lib_build_mk_constant_node(c_wrdcount, h_ctx);
	unless(C) return 0;
	unless(C->m_key == c_wrdcount) return 0;
	unless(C->m_structpriv_id)
		(void)lib_build_mk_constant_structpriv_id(c_wrdcount, h_ctx);

	if(C->m_ptr_work) return C->m_ptr_work;


	unless((h_ctx->m_count.m_types = emit_opcode(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvOpTypePointer, 3))) 
		return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, h_ctx->m_id))) 
		return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, SpvStorageClassWorkgroup))) 
		return 0;
	unless((h_ctx->m_count.m_types = emit_wrd(&h_ctx->m_section.m_types, h_ctx->m_count.m_types, C->m_structpriv_id)))
		return 0;

	C->m_ptr_work = h_ctx->m_id++;

	return C->m_ptr_work;
}

ae2f_inline static aclspv_id_t	lib_build_mk_constant_ptr_by_enum(const aclspv_wrd_t c_wrdcount, h_aclspv_build_ctx_t h_ctx, enum SpvStorageClass_ c_class) {
	switch((aclspv_wrd_t)c_class) {
		case SpvStorageClassWorkgroup:
			return lib_build_mk_constant_ptr_work_id(c_wrdcount, h_ctx);
		case SpvStorageClassPrivate:
			return lib_build_mk_constant_ptr_id(c_wrdcount, h_ctx);
		case SpvStorageClassUniform:
			return lib_build_mk_constant_ptr_uniform_id(c_wrdcount, h_ctx);
		case SpvStorageClassStorageBuffer:
			return lib_build_mk_constant_ptr_storage_id(c_wrdcount, h_ctx);
		case SpvStorageClassPushConstant:
			return lib_build_mk_constant_ptr_psh_id(c_wrdcount, h_ctx);

		default:
			return 0;
	}

}

#endif
