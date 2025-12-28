#ifndef lib_build_llvmtospv_h
#define lib_build_llvmtospv_h

#include <ae2f/Keys.h>
#include <aclspv/spvty.h>

#include <llvm-c/Core.h>
#include <llvm-c/Types.h>

#include <build.h>
#include <assert.h>
#include <spirv/unified1/spirv.h>
#include "./iddef.h"
#include "build/constant.h"
#include <limits.h>

/**
 * @brief	getting structure's size
 * @
 * */
ae2f_inline static size_t lib_build_get_llvmarr_size(
		const LLVMTypeRef		c_ty
		);

/**
 * @brief	getting structure's size
 * @
 * */
ae2f_inline static size_t lib_build_get_llvmstruct_size(
		const LLVMTypeRef		c_ty,
		unsigned			c_struct_idx,
		size_t* ae2f_restrict		rwr_elmsz_opt
		);

/**
 * @brief	default type's sizes.
 * @details	size will be counted as word.
 * */
ae2f_inline static size_t lib_build_get_llvmty_size(
		const LLVMTypeRef		c_ty
		)
{
	switch((unsigned)LLVMGetTypeKind(c_ty)) {
		case LLVMVoidTypeKind:
			return 0;
		case LLVMIntegerTypeKind:
			return 4;
		case LLVMFloatTypeKind:
			return 4;
		case LLVMDoubleTypeKind:
			return 8;
		case LLVMPointerTypeKind:
			return 8;
		case LLVMArrayTypeKind:
			return lib_build_get_llvmarr_size(c_ty);
		case LLVMStructTypeKind:
			return lib_build_get_llvmstruct_size(c_ty, UINT_MAX, ae2f_NIL);
		default: break;
	}

	/** unknown */
	assert(0);
}

ae2f_inline static size_t lib_build_get_llvmstruct_size(
		const LLVMTypeRef		c_ty,
		unsigned			c_struct_idx,
		size_t* ae2f_restrict		rwr_elmsz_opt
		)
{
	unsigned		NUM_ELM, I_ELM = 0;

	size_t			PAD = 0, ELMW;
	LLVMTypeRef		TY;

	assert(c_ty); assert(LLVMGetTypeKind(c_ty) == LLVMStructTypeKind);
	if(LLVMIsOpaqueStruct(c_ty))	return 0;

	NUM_ELM = LLVMCountStructElementTypes(c_ty);
	ELMW = 0; 

	while(I_ELM < c_struct_idx && I_ELM < NUM_ELM) {
		TY = LLVMStructGetTypeAtIndex(c_ty, I_ELM);
		PAD += ELMW;

		switch((unsigned)LLVMGetTypeKind(TY)) {
			case LLVMStructTypeKind:
				ELMW = lib_build_get_llvmstruct_size(TY, UINT_MAX, ae2f_NIL);
				break;
			case LLVMArrayTypeKind:
				ELMW = lib_build_get_llvmarr_size(TY);
				break;
			default:
				ELMW = lib_build_get_llvmty_size(c_ty);
		}

		++I_ELM;
	}

	if(rwr_elmsz_opt) *rwr_elmsz_opt = ELMW;
	return PAD;
}



ae2f_inline static size_t lib_build_get_llvmarr_size(
		const LLVMTypeRef		c_ty
		)
{
	LLVMTypeRef	TY_ELM;
	assert(c_ty); assert(LLVMGetTypeKind(c_ty) == LLVMArrayTypeKind);
	TY_ELM = LLVMGetElementType(c_ty); assert(TY_ELM);

	switch((unsigned)LLVMGetTypeKind(TY_ELM)) {
		case LLVMStructTypeKind:
			return 
				LLVMGetArrayLength2(c_ty) *
				lib_build_get_llvmstruct_size(
						TY_ELM
						, UINT_MAX
						, ae2f_NIL
						);

		case LLVMArrayTypeKind:
			return 
				(size_t)LLVMGetArrayLength2(c_ty) *
				lib_build_get_llvmarr_size(TY_ELM);
		default:
			return 
				(size_t)LLVMGetArrayLength2(c_ty) *
				lib_build_get_llvmty_size(c_ty);
	}
}

ae2f_inline static aclspv_id_t	lib_build_ty_llvmtospv(
		const LLVMTypeRef c_ty,
		const h_aclspv_build_ctx_t h_ctx
		)
{
	assert(c_ty);
	assert(h_ctx);

	switch((unsigned)LLVMGetTypeKind(c_ty)) {
		case LLVMVoidTypeKind:
			return lib_build_get_default_id(ID_DEFAULT_VOID, h_ctx);
		case LLVMIntegerTypeKind:
			return lib_build_get_default_id(ID_DEFAULT_U32, h_ctx);
		case LLVMFloatTypeKind:
			return lib_build_get_default_id(ID_DEFAULT_F32, h_ctx);
		case LLVMArrayTypeKind:
			{
				const size_t		TY_BYTE_COUNT = lib_build_get_llvmarr_size(c_ty);

				switch((uintmax_t)TY_BYTE_COUNT & 3lu) {
					case 0lu:
						return lib_build_mk_constant_arr32_id(
								sz_to_count(TY_BYTE_COUNT)
								, h_ctx
								);
					case 1lu: case 3lu:
						return lib_build_mk_constant_arr8_id(
								(aclspv_wrd_t)(TY_BYTE_COUNT)
								, h_ctx
								);
					case 2lu:
						return lib_build_mk_constant_arr16_id(
								(aclspv_wrd_t)(TY_BYTE_COUNT >> 1)
								, h_ctx
								);
					default: assert(0);
				}
			}
		case LLVMStructTypeKind: 
			{
				const size_t	SIZE 
					= lib_build_get_llvmstruct_size(c_ty, UINT_MAX, ae2f_NIL);
				return lib_build_mk_constant_arr32_id(sz_to_count(SIZE), h_ctx);
				;
			}
		default: break;
	}

	/** not handled */
	assert(0);
}

#if 0
static aclspv_wrd_t lib_build_fnctl_llvmtospv(LLVMValueRef fn) {
	aclspv_wrd_t CTL = SpvFunctionControlMaskNone;

	/* alwaysinline attribute */
	const unsigned KND_INLINE_ALWAYS = LLVMGetEnumAttributeKindForName("alwaysinline", sizeof("alwaysinline") - 1);
	const unsigned KND_NOINLINE = LLVMGetEnumAttributeKindForName("noinline", sizeof("noinline") - 1);
	const unsigned KND_RDONLY = LLVMGetEnumAttributeKindForName("readonly", sizeof("readonly") - 1);
	const unsigned KND_PURE = LLVMGetEnumAttributeKindForName("pure", sizeof("pure") - 1);


	if (LLVMGetEnumAttributeAtIndex(fn, (unsigned)LLVMAttributeFunctionIndex, KND_INLINE_ALWAYS)) {
		CTL |= SpvFunctionControlInlineMask;
	}

	/* noinline attribute */
	if (LLVMGetEnumAttributeAtIndex(fn, (unsigned)LLVMAttributeFunctionIndex, KND_NOINLINE)) {
		CTL |= SpvFunctionControlDontInlineMask;
	}

	/* readonly → CONST */
	if (LLVMGetEnumAttributeAtIndex(fn, (unsigned)LLVMAttributeFunctionIndex, KND_RDONLY)) {
		CTL |= SpvFunctionControlConstMask;
	}

	/* pure attribute */
	if (LLVMGetEnumAttributeAtIndex(fn, (unsigned)LLVMAttributeFunctionIndex, KND_PURE)) {
		CTL |= SpvFunctionControlPureMask;
	}

	return CTL;
}
#endif

#endif
