
#include "./cxtp.h"
#include "./constant.h"
#include "./emitx.h"
#include "aclspv.h"
#include "util/iddef.h"

#include <ae2f/cc.h>
#include <clang-c/Index.h>
#include <assert.h>

#include <attr/storage_class.h>
#include <attr/specid.h>
#include <complex.h>
#include <spirv/unified1/spirv.h>


static enum CXChildVisitResult	__is_vector_visit(
		CXCursor h_cur,
		CXCursor h_parent,
		CXClientData ae2f_restrict wr_storage_class
		)
{
	CXString TEXT;
	enum CXChildVisitResult RES = CXChildVisit_Break;

	unless(h_cur.kind == CXCursor_AnnotateAttr)
		return CXChildVisit_Recurse;
	TEXT = clang_getCursorSpelling(h_cur);

	unless(strstr(TEXT.data, "aclspv_vec")) {
		RES = CXChildVisit_Recurse;
		goto LBL_FINI;
	}

	*((enum ACLUTIL_CXTYPE_* ae2f_restrict)wr_storage_class) = ACLUTIL_CXTYPE_VECTOR;

LBL_FINI:
	clang_disposeString(TEXT);
	return RES;

	(void)h_parent;
}


ae2f_inline static aclspv_bool_t __node_equal(const aclutil_cxtp* const ae2f_restrict A, const aclutil_cxtp* const ae2f_restrict B)
{
	if(A->m_class != B->m_class) return 0;
	switch(A->m_class) {
		case ACLUTIL_CXTYPE_FUNC:
			return clang_equalTypes(
					A->m_info.m_fn.m_type
					, B->m_info.m_fn.m_type
					) && clang_equalCursors(
						A->m_info.m_fn.m_cursor
						, B->m_info.m_fn.m_cursor);

		case ACLUTIL_CXTYPE_PTR:
		case ACLUTIL_CXTYPE_VECTOR:
		case ACLUTIL_CXTYPE_ARRAY:
		case ACLUTIL_CXTYPE_FUNC_NOPRM:
		case ACLUTIL_CXTYPE_STRUCT_SINGLE:
		default:
			return !memcmp(&A->m_info, &B->m_info, sizeof(A->m_info));

		case ACLUTIL_CXTYPE_UNIT:
			break;
	}

	return 0;
}

ae2f_inline static aclspv_wrdcount_t __find_node(const aclutil_cxtp* const ae2f_restrict rd_tar, const h_util_ctx_t h_ctx) {
	aclspv_wrdcount_t IDX = (h_ctx)->m_num_type_uniques;
	unless(h_ctx->m_type_uniques.m_p) 
		return 0xFFFFFFFF;

	if((h_ctx)->m_num_type_uniques * sizeof(aclutil_cxtp) > h_ctx->m_type_uniques.m_sz)
		return 0xFFFFFFFF;

	while(IDX-- 
			&& !__node_equal(
				rd_tar
				, ((const aclutil_cxtp* ae2f_restrict)h_ctx->m_type_uniques.m_p) + IDX)) 
	{}

	return IDX;
}

#define	__get_type_uniques(h_ctx) ((aclutil_cxtp* ae2f_restrict)h_ctx->m_type_uniques.m_p)

ae2f_inline static struct __ret { 
	aclspv_id_t	m_id; 
	aclspv_bool_t	m_isfirst; 
} __ret(const h_aclspv_ctx_t h_ctx, const aclutil_cxtp* rd_cxtp)
{
	aclspv_wrdcount_t	IDX;
	struct	__ret		RET;

#define	_return(c_id, c_is_first)	{ RET.m_id = c_id; RET.m_isfirst = c_is_first; return RET; }

	IDX = __find_node(rd_cxtp, h_ctx);
	if(~IDX)
		_return(__get_type_uniques(h_ctx)[IDX].m_type_id, 0);

	IDX = h_ctx->m_num_type_uniques;
	if(sizeof(aclutil_cxtp) * (IDX + 1) > h_ctx->m_type_uniques.m_sz) {
		const size_t	NEWSIZE = (h_ctx->m_type_uniques.m_sz << 1) + 1;
		_aclspv_grow_vec_with_copy(_aclspv_malloc, _aclspv_free, memcpy
				, L_new, h_ctx->m_type_uniques, NEWSIZE);
	}

	ae2f_expected_but_else(h_ctx->m_type_uniques.m_p)
		_return(0, 1);

	++h_ctx->m_num_type_uniques;

	{
		const aclspv_id_t	ID = h_ctx->m_id++;
		__get_type_uniques(h_ctx)[IDX] = *rd_cxtp;
		__get_type_uniques(h_ctx)[IDX].m_type_id = ID;
		_return(ID, 1);
	}

	ae2f_unreachable();

#undef	_return
}

aclspv_id_t aclutil_mk_cxtp_arr(
		const aclspv_id_t	c_type_elm,
		const aclspv_id_t	c_num_arr,
		const h_aclspv_ctx_t	h_ctx
		)
{
	return aclutil_mk_cxtp_arr2(
			c_type_elm, c_num_arr
			, 0xFFFFFFFF
			, h_ctx
			);
}

aclspv_id_t aclutil_mk_cxtp_arr2(
		const aclspv_id_t	c_type_elm,
		const aclspv_id_t	c_num_arr,
		const aclspv_wrd_t	c_decorate_stride,
		const h_aclspv_ctx_t	h_ctx
		)
{
	aclutil_cxtp		TAR;

	memset(&TAR, 0, sizeof(TAR));

	TAR.m_class = ACLUTIL_CXTYPE_ARRAY;
	TAR.m_info.m_arr.m_num_id = c_num_arr;
	TAR.m_info.m_arr.m_type = c_type_elm;

	{
		const struct __ret ID = __ret(h_ctx, &TAR);
		ae2f_expected_but_else(ID.m_id) return 0;
		unless(ID.m_isfirst)		return ID.m_id;

		ae2f_expected_but_else(h_ctx->m_count.m_types = util_emitx_4(
				&h_ctx->m_section.m_types
				, h_ctx->m_count.m_types
				, SpvOpTypeArray
				, ID.m_id
				, c_type_elm
				, c_num_arr
				)) return 0;

		if(c_decorate_stride ^ 0xFFFFFFFF)
			ae2f_expected_but_else(h_ctx->m_count.m_decorate = util_emitx_4(
						&h_ctx->m_section.m_decorate
						, h_ctx->m_count.m_decorate
						, SpvOpDecorate
						, ID.m_id
						, SpvDecorationArrayStride
						, c_decorate_stride
						)) return 0;

		return ID.m_id;
	}
}

aclspv_id_t aclutil_mk_cxtp_struct_single(
		const aclspv_id_t	c_type_member,
		const B_aclspv_cxtp_struct_decoration_field0_t	
					c_decorate_field0,
		const aclspv_wrd_t	c_memdecorate_offset,
		const h_aclspv_ctx_t	h_ctx
		)
{
	aclutil_cxtp		TAR;

	memset(&TAR, 0, sizeof(TAR));

	TAR.m_class						= ACLUTIL_CXTYPE_STRUCT_SINGLE;
	TAR.m_info.m_struct_single.m_mem_id			= c_type_member;
	TAR.m_info.m_struct_single.m_decoration.m_field0	= c_decorate_field0;
	TAR.m_info.m_struct_single.m_mem_decoration.m_offset	= c_memdecorate_offset;

	{
		const struct __ret ID = __ret(h_ctx, &TAR);
		ae2f_expected_but_else(ID.m_id) return 0;
		unless(ID.m_isfirst)		return ID.m_id;

		ae2f_expected_but_else(h_ctx->m_count.m_types = util_emitx_3(
				&h_ctx->m_section.m_types
				, h_ctx->m_count.m_types
				, SpvOpTypeStruct
				, ID.m_id
				, c_type_member
				)) return 0;

		if(c_decorate_field0 & (ACLSPV_CXTP_STRUCT_DECORATION_FIELD0_BLOCK))
			ae2f_expected_but_else(h_ctx->m_count.m_decorate = util_emitx_3(
						&h_ctx->m_section.m_decorate
						, h_ctx->m_count.m_decorate
						, SpvOpDecorate
						, ID.m_id
						, SpvDecorationBlock
						)) return 0;

		if(c_memdecorate_offset != 0xFFFFFFFF) {
			ae2f_expected_but_else(h_ctx->m_count.m_decorate = util_emitx_5(
						&h_ctx->m_section.m_decorate
						, h_ctx->m_count.m_decorate
						, SpvOpMemberDecorate
						, ID.m_id
						, 0
						, SpvDecorationOffset
						, 0
						)) return 0;
		}

		return ID.m_id;
	}
}

aclspv_id_t aclutil_mk_cxtp_vec(
		const aclspv_id_t	c_type_elm,
		const aclspv_wrd_t	c_num_arr,
		const h_aclspv_ctx_t	h_ctx
		)
{
	aclutil_cxtp		TAR;

	memset(&TAR, 0, sizeof(TAR));

	TAR.m_class = ACLUTIL_CXTYPE_VECTOR;
	TAR.m_info.m_arr.m_num_id = c_num_arr;
	TAR.m_info.m_arr.m_type = c_type_elm;

	{
		const struct __ret ID = __ret(h_ctx, &TAR); 
		ae2f_expected_but_else(ID.m_id) return 0;
		unless(ID.m_isfirst)		return ID.m_id;

		ae2f_expected_but_else(h_ctx->m_count.m_types = util_emitx_4(
					&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types
					, SpvOpTypeVector
					, ID.m_id
					, c_type_elm
					, c_num_arr
					)) return 0;

		return ID.m_id;
	}
}

aclspv_id_t aclutil_mk_cxtp_ptr(
		const aclspv_id_t		c_type_elm,
		const enum SpvStorageClass_	c_storage_class,
		const h_aclspv_ctx_t		h_ctx
		)
{
	aclutil_cxtp		TAR;

	memset(&TAR, 0, sizeof(TAR));

	TAR.m_class			= ACLUTIL_CXTYPE_PTR;
	TAR.m_info.m_ptr.m_type		= c_type_elm;
	TAR.m_info.m_ptr.m_storage	= c_storage_class;

	{
		const struct __ret ID = __ret(h_ctx, &TAR); 
		ae2f_expected_but_else(ID.m_id) return 0;
		unless(ID.m_isfirst)		return ID.m_id;

		ae2f_expected_but_else(h_ctx->m_count.m_types = util_emitx_4(
					&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types
					, SpvOpTypePointer
					, ID.m_id
					, c_storage_class
					, c_type_elm
					)) return 0;

		return ID.m_id;
	}
}

aclspv_id_t aclutil_mk_cxtp_fn_no_prm(
		const aclspv_id_t	c_ret_id,
		const h_aclspv_ctx_t	h_ctx
		)
{
	aclutil_cxtp	TAR;
	memset(&TAR, 0, sizeof(TAR));
	TAR.m_class				= ACLUTIL_CXTYPE_FUNC_NOPRM;
	TAR.m_info.m_fn_no_proto.m_ret_id	= c_ret_id;

	{
		const struct __ret ID = __ret(h_ctx, &TAR);
		ae2f_expected_but_else(ID.m_id) return 0;
		unless(ID.m_isfirst)		return ID.m_id;

		ae2f_expected_but_else(h_ctx->m_count.m_types = util_emitx_3(
					&h_ctx->m_section.m_types
					, h_ctx->m_count.m_types
					, SpvOpTypeFunction
					, ID.m_id
					, c_ret_id
					)) return 0;

		return ID.m_id;
	}
}

aclspv_id_t aclutil_mk_cxtp_fn_with_prm(
		const CXCursor		c_cursor_compound,
		const CXType		c_type_compound,
		const h_aclspv_ctx_t	h_ctx
		)
{
	aclutil_cxtp	TAR;
	memset(&TAR, 0, sizeof(TAR));
	TAR.m_class				= ACLUTIL_CXTYPE_FUNC;
	TAR.m_info.m_fn.m_type			= c_type_compound;
	TAR.m_info.m_fn.m_cursor		= c_cursor_compound;

	{
		const struct __ret ID = __ret(h_ctx, &TAR);
		ae2f_expected_but_else(ID.m_id) return 0;
		unless(ID.m_isfirst)		return ID.m_id;

		{
			int ARGC			= clang_getNumArgTypes(c_type_compound);
			aclspv_wrd_t	ARGC_IDX	= (aclspv_wrd_t)ARGC;
			const aclspv_wrd_t	ANCHOUR	= h_ctx->m_count.m_types;
			aclspv_wrd_t* WORDS;

			ae2f_unexpected_but_if(ARGC <= 0) {
				const CXType RET	= clang_getResultType(c_type_compound);
				const aclspv_id_t RETID	= aclutil_mk_cxtp_by_cursor(
						c_cursor_compound
						,RET 
						, h_ctx);

				ae2f_expected_but_else(RETID) return 0;
				return aclutil_mk_cxtp_fn_no_prm(RETID, h_ctx);
			}	/** fallback: argc was under 0 */

			ae2f_expected_but_else(h_ctx->m_count.m_types = util_emit_wrd(
						&h_ctx->m_section.m_types
						, h_ctx->m_count.m_types + (aclspv_wrd_t)ARGC + 2
						, 0)) return 0;


			WORDS = ((aclspv_wrd_t* ae2f_restrict)h_ctx->m_section.m_types.m_p);
			{ ae2f_assume(WORDS); }

			WORDS[ANCHOUR] = SpvOpTypeFunction;
			WORDS[ANCHOUR + 1] = ID.m_id;
			{
				const CXType RET	= clang_getResultType(c_type_compound);
				const aclspv_id_t RETID	= aclutil_mk_cxtp_by_cursor(
						c_cursor_compound
						,RET 
						, h_ctx);

				ae2f_expected_but_else(RETID) {
					h_ctx->m_err = ACLSPV_COMPILE_CXTP_ZERO;
					return 0;
				}
				WORDS[ANCHOUR + 2] = RETID;
			}

			while(ARGC_IDX--) {
				const CXType ARG		= clang_getArgType(
						c_type_compound
						, ARGC_IDX
						);
				const aclspv_wrd_t ARG_ID	= aclutil_mk_cxtp_by_cursor(
						c_cursor_compound
						, ARG
						, h_ctx
						);

				ae2f_expected_but_else(ARG_ID)	{
					h_ctx->m_err = ACLSPV_COMPILE_CXTP_ZERO;
					return 0;
				}

				WORDS[ARGC_IDX + ANCHOUR + 3] = ARG_ID;
			}
		}

		return ID.m_id;
	}
}

ae2f_inline ae2f_ccconst static aclspv_id_t __cxtp_cursor_default(const CXType c_type) {
	switch((uintmax_t)c_type.kind) {
		default:
			return 0;

		case CXType_Void:
			return ID_DEFAULT_VOID;
		case CXType_SChar:
			return ID_DEFAULT_I8;
		case CXType_Short:
			return ID_DEFAULT_I16;
		case CXType_Int:
			return ID_DEFAULT_I32;
		case CXType_LongLong:
		case CXType_Long:
			return ID_DEFAULT_I64;
		case CXType_UInt:
			return ID_DEFAULT_U32;
		case CXType_UChar:
			return ID_DEFAULT_U8;
		case CXType_UShort:
			return ID_DEFAULT_U16;
		case CXType_ULongLong:
		case CXType_ULong:
			return ID_DEFAULT_U64;

		case CXType_Half:
			return ID_DEFAULT_F16;
		case CXType_Float:
			return ID_DEFAULT_F32;
		case CXType_Double:
			return ID_DEFAULT_F64;
	}
}

ae2f_extern aclspv_id_t aclutil_mk_cxtp_by_cursor(
		const CXCursor		c_cursor,
		const CXType		TYPE,
		const h_aclspv_ctx_t	h_ctx
		)
{
	const CXCursor	TYPEDEF	= clang_getTypeDeclaration(TYPE);

	{
		const aclspv_id_t R = __cxtp_cursor_default(TYPE);
		const aclspv_id_t RR = aclid_mk_default_id(R, h_ctx);
		ae2f_unexpected_but_if(RR) return RR;
	}

	switch((uintmax_t)TYPE.kind) {
		case CXType_Void:
		case CXType_SChar:
		case CXType_Short:
		case CXType_Int:
		case CXType_LongLong:
		case CXType_Long:
		case CXType_UInt:
		case CXType_UChar:
		case CXType_UShort:
		case CXType_ULongLong:
		case CXType_ULong:
		case CXType_Half:
		case CXType_Float:
		case CXType_Double:
			assert(0 && "Known by __cxtp_cursor_default");
			ae2f_unreachable();

		default:
			return 0;

		case CXType_FunctionProto:
			{
				const CXType CANONICAL	= clang_getCanonicalType(TYPE);
				return aclutil_mk_cxtp_fn_with_prm(
						c_cursor
						, CANONICAL
						, h_ctx);
			}

		case CXType_FunctionNoProto:
			{
				const CXType RET	= clang_getResultType(TYPE);
				const aclspv_id_t RETID	= aclutil_mk_cxtp_by_cursor(c_cursor, RET, h_ctx);

				return aclutil_mk_cxtp_fn_no_prm(RETID, h_ctx);
			}

		case CXType_Pointer:
			{
				const CXType ELM	= clang_getPointeeType(TYPE);
				const aclspv_id_t ELMID	= aclutil_mk_cxtp_by_cursor(c_cursor, ELM, h_ctx);
				enum SpvStorageClass_	SCLASS = SpvStorageClassMax;

				clang_visitChildren(TYPEDEF, attr_storage_class, &SCLASS);
				clang_visitChildren(c_cursor, attr_storage_class, &SCLASS);

				if(SCLASS == SpvStorageClassMax)
					SCLASS = SpvStorageClassFunction;	/** default fallback */

				return aclutil_mk_cxtp_ptr(ELMID, SCLASS, h_ctx);
			}

		case CXType_ConstantArray:	/** array or vector */
			{
				enum ACLUTIL_CXTYPE_ E		= ACLUTIL_CXTYPE_ARRAY;
				const CXType ELM		= clang_getArrayElementType(TYPE);
				const intmax_t _ELMCOUNT	= clang_getNumElements(TYPE);
				const aclspv_wrd_t ELMCOUNT	= (umax)(_ELMCOUNT) & ~(umax)ACLSPV_MASK_WRD
					? 0 : (aclspv_wrd_t)_ELMCOUNT;

				const aclspv_id_t	ELMID = aclutil_mk_cxtp_by_cursor(c_cursor, ELM, h_ctx);

				aclspv_wrd_t	SPECID = 0xFFFFFFFF;

				clang_visitChildren(TYPEDEF, __is_vector_visit, &E);
				clang_visitChildren(c_cursor, __is_vector_visit, &E);

				if(E == ACLUTIL_CXTYPE_VECTOR) {
					return aclutil_mk_cxtp_vec(
							ELMID
							, ELMCOUNT
							, h_ctx);
				}

				clang_visitChildren(TYPEDEF, attr_specid, &SPECID);
				clang_visitChildren(c_cursor, attr_specid, &SPECID);

				if(SPECID != 0xFFFFFFFF) {
					/** specid */
					aclspv_id_t SPECID_WRD = aclutil_mk_constant_spec_id(
							SPECID, ELMCOUNT, h_ctx);
					unless(SPECID_WRD)
						SPECID_WRD = aclutil_mk_constant_spec64_id(
								SPECID, ELMCOUNT, h_ctx);

					ae2f_expected_but_else(SPECID_WRD) {
						h_ctx->m_err = ACLSPV_COMPILE_CXTP_ZERO;
						return 0; /** not found */
					}

					return aclutil_mk_cxtp_arr(
							ELMID, SPECID_WRD
							, h_ctx);
				}

				return aclutil_mk_cxtp_arr(ELMID, ELMCOUNT, h_ctx);
			}
	}
}
