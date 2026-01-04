#include "aclspv.h"
#include "aclspv/spvty.h"
#include "util/id.h"
#include "util/iddef.h"
#include <assert.h>
#include <clang-c/CXString.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <util/ctx.h>
#include <util/emitx.h>
#include <util/is_kernel.h>
#include <util/fn.h>
#include <util/entp.h>
#include <util/bind.h>
#include <util/constant.h>
#include <util/scale.h>

#include <ae2f/Keys.h>
#include <ae2f/c90/StdBool.h>
#include <ae2f/c90/StdInt.h>

#include <spirv/1.0/spirv.h>
#include <clang-c/Index.h>

static enum CXChildVisitResult	emit_decl_glob_obj_visit_attr_set(CXCursor h_cur, CXCursor h_parent, CXClientData ae2f_restrict h_data) {
	CXString TEXT;
	enum CXChildVisitResult RES = CXChildVisit_Break;
	char* NEEDLE;
	unsigned SET;

	unless(h_cur.kind == CXCursor_AnnotateAttr) return CXChildVisit_Recurse;
	TEXT = clang_getCursorSpelling(h_cur);

	unless(NEEDLE = strstr(TEXT.data, "aclspv_set")) {
		RES = CXChildVisit_Continue;
		goto LBL_FINI;
	}


	sscanf(NEEDLE, "aclspv_set ( %u )", &SET);

	*((aclspv_wrd_t* ae2f_restrict)h_data) = (aclspv_wrd_t)SET;

LBL_FINI:
	clang_disposeString(TEXT);
	return RES;

	(void)h_parent;
}

static enum CXChildVisitResult	emit_decl_glob_obj_visit_attr_specid(CXCursor h_cur, CXCursor h_parent, CXClientData ae2f_restrict h_data) {
	CXString TEXT;
	enum CXChildVisitResult RES = CXChildVisit_Break;
	char* NEEDLE;
	unsigned SPECID;

	unless(h_cur.kind == CXCursor_AnnotateAttr) return CXChildVisit_Recurse;
	TEXT = clang_getCursorSpelling(h_cur);

	unless(NEEDLE = strstr(TEXT.data, "aclspv_specid")) {
		RES = CXChildVisit_Continue;
		goto LBL_FINI;
	}


	sscanf(NEEDLE, "aclspv_specid ( %u )", &SPECID);

	*((aclspv_wrd_t* ae2f_restrict)h_data) = (aclspv_wrd_t)SPECID;

LBL_FINI:
	clang_disposeString(TEXT);
	return RES;

	(void)h_parent;
}

static enum CXChildVisitResult emit_decl_glob_obj_visit_fetch(CXCursor h_cur, CXCursor h_parent, CXClientData h_data) {
#if	!defined(NDEBUG) || !NDEBUG

	CXString	PARAM_NAME, FUNC_NAME;
	aclspv_wrdcount_t	POS_FOR_LOCAL;
#endif
	CXString	PARAM_TY_NAME;
	CXType		PARAM_TY;

#define	PUSH_SIZE	((uintptr_t* ae2f_restrict)h_data)[0]
#define BIND_IDX	((uintptr_t* ae2f_restrict)h_data)[1]
#define CTX		((h_aclspv_ctx_t)((uintptr_t* ae2f_restrict)h_data)[2])
#define	ARG_IDX		((uintptr_t* ae2f_restrict)h_data)[3]
#define	INTERFACE_COUNT	((uintptr_t* ae2f_restrict)h_data)[4]

	unless(h_cur.kind == CXCursor_ParmDecl) {
		return CXChildVisit_Continue;
	}

	PARAM_TY	= clang_getCursorType(h_cur);
	ae2f_unexpected_but_if(ARG_IDX > UINT32_MAX) {
		CTX->m_state = ACLSPV_COMPILE_TOO_BIG;
		return CXChildVisit_Break;
	}

	/** non-pointer type */
	unless(PARAM_TY.kind == CXType_Pointer || PARAM_TY.kind == CXType_ConstantArray) {
		PUSH_SIZE += sz_to_count(clang_Type_getSizeOf(PARAM_TY));
		return CXChildVisit_Continue;
	}

	PARAM_TY_NAME	= clang_getTypeSpelling(PARAM_TY);


	/** FETCH BIND */

#define INTERFACES_MGR		get_last_scale_from_vec(&CTX->m_scale_vars)
#define	INTERFACES		((util_bind* ae2f_restrict)get_buf_from_scale(&CTX->m_scale_vars, INTERFACES_MGR[0]))
#define	INFO			(&((INTERFACES + ARG_IDX))->m_bindable)

	assert(INTERFACES_MGR->m_id == (size_t)CTX->m_tmp.m_w0);
	{ ae2f_assume(INTERFACES_MGR->m_id == (size_t)CTX->m_tmp.m_w0); }

	ae2f_unexpected_but_if(
			grow_last_scale(&CTX->m_scale_vars, (size_t)(sizeof(util_bind) * (ARG_IDX + 1)))
			) {
		CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;
		return CXChildVisit_Break;
	}

	ae2f_expected_but_else(CTX->m_scale_vars.m_p) {
		CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;
		return CXChildVisit_Break;
	}

	INFO->m_arg_idx		= (aclspv_wrd_t)ARG_IDX;
	INFO->m_binding		= (aclspv_wrd_t)BIND_IDX;
	INFO->m_var_id		= CTX->m_id++;
	INFO->m_var_elm_type_id	= util_mk_constant_struct_id(1, CTX);
	INFO->m_var_type_id	= util_mk_constant_ptr_storage_id(1, CTX);
	INFO->m_set		= 0;

	/** FETCH BIND END */
#if	!defined(NDEBUG) || !NDEBUG
	FUNC_NAME	= clang_getCursorSpelling(h_parent);
	PARAM_NAME	= clang_getCursorSpelling(h_cur);

	{
		char* ae2f_restrict	NAME_MERGE = malloc(strlen(FUNC_NAME.data) + strlen(PARAM_NAME.data) + 3);
		const aclspv_wrdcount_t	POS = CTX->m_count.m_name;
		unless(NAME_MERGE) {
LBL_ABRT_NALLOC:
			CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;
			free(NAME_MERGE);
			clang_disposeString(FUNC_NAME);
			clang_disposeString(PARAM_NAME);
			clang_disposeString(PARAM_TY_NAME);
			return CXChildVisit_Break;
		}

		strcpy(NAME_MERGE, FUNC_NAME.data);
		strcat(NAME_MERGE, "::");
		strcat(NAME_MERGE, PARAM_NAME.data);

#define EMIT_POS	CTX->m_count.m_name
		unless((POS_FOR_LOCAL = EMIT_POS = emit_opcode(&CTX->m_section.m_name, EMIT_POS, SpvOpName, 0))) 
			goto LBL_ABRT_NALLOC;
		unless((EMIT_POS = util_emit_wrd(&CTX->m_section.m_name, EMIT_POS, INFO->m_var_id)))
			goto LBL_ABRT_NALLOC;
		unless((EMIT_POS = util_emit_str(&CTX->m_section.m_name, EMIT_POS, NAME_MERGE))) 
			goto LBL_ABRT_NALLOC;
		set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_section.m_name)[POS], EMIT_POS - POS - 1);
#undef	EMIT_POS


		free(NAME_MERGE);
	}
#endif

#if	!defined(NDEBUG) || !NDEBUG
	clang_disposeString(FUNC_NAME);
	clang_disposeString(PARAM_NAME);
#endif
	if(PARAM_TY.kind == CXType_ConstantArray)
		goto LBL_WORKGROUP; 

	if(strstr(PARAM_TY_NAME.data, "__global")) {
		INFO->m_storage_class = SpvStorageClassStorageBuffer;
	} else if (strstr(PARAM_TY_NAME.data, "__constant")) {
		INFO->m_storage_class = SpvStorageClassUniform;
	} else {
		CTX->m_state = ACLSPV_COMPILE_MET_INVAL;
		return CXChildVisit_Break;
	}

	clang_disposeString(PARAM_TY_NAME);
	clang_visitChildren(h_cur, emit_decl_glob_obj_visit_attr_set, &INFO->m_set);

	CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;

	ae2f_expected_but_else(CTX->m_count.m_decorate = util_emitx_4(
				/** Decorations::Descriptorsets */
				&CTX->m_section.m_decorate
				, CTX->m_count.m_decorate
				, SpvOpDecorate
				, INFO->m_var_id
				, SpvDecorationDescriptorSet
				, INFO->m_set
				)) return CXChildVisit_Break;



	ae2f_expected_but_else(CTX->m_count.m_decorate = util_emitx_4(
				/** Decorations::Binding */
				&CTX->m_section.m_decorate
				, CTX->m_count.m_decorate
				, SpvOpDecorate
				, INFO->m_var_id
				, SpvDecorationBinding
				, INFO->m_binding
				)) return CXChildVisit_Break;

	if(INFO->m_storage_class == SpvStorageClassUniform) {
		INFO->m_var_type_id = util_mk_constant_ptr_uniform_id(1, CTX);
		assert(INFO->m_var_type_id);
	}


	ae2f_expected_but_else(CTX->m_count.m_vars = util_emitx_variable(
				&CTX->m_section.m_vars
				, CTX->m_count.m_vars
				, INFO->m_var_type_id
				, INFO->m_var_id
				, INFO->m_storage_class
				)) return CXChildVisit_Break;


	CTX->m_state = ACLSPV_COMPILE_OK;
	INFO->m_entp_idx	= CTX->m_tmp.m_w0;

	++BIND_IDX;

LBL_WORKGROUP_FINI:
	++ARG_IDX;
	++INTERFACE_COUNT;
	return CXChildVisit_Continue;

LBL_WORKGROUP:
#undef	INFO
#define	INFO			(&((INTERFACES + BIND_IDX))->m_work)
	unless(strstr(PARAM_TY_NAME.data, "__local")) {
		CTX->m_state = ACLSPV_COMPILE_MET_INVAL;
		return CXChildVisit_Break;
	}


	clang_disposeString(PARAM_TY_NAME);

	{
		aclspv_wrd_t	SPECID_WORK = 0xFFFFFFFF;
		const aclspv_wrd_t	VAL_2 = util_mk_constant_val_id(
				2
				, CTX
				);

		const aclspv_wrd_t	VAL_3 = util_mk_constant_val_id(
				3, CTX
				);
		const intmax_t NUM_ARR_ORIG	= clang_getArraySize(PARAM_TY);
		const aclspv_wrd_t NUM_ARR	= NUM_ARR_ORIG < 0 || NUM_ARR_ORIG > UINT32_MAX ? 0 : (aclspv_wrd_t)NUM_ARR_ORIG;
		ae2f_expected_but_else(VAL_2)		return CXChildVisit_Break;


		clang_visitChildren(
				h_cur
				, emit_decl_glob_obj_visit_attr_specid
				, &SPECID_WORK
				);

		ae2f_expected_but_else(NUM_ARR) {
			CTX->m_state = ACLSPV_COMPILE_MET_INVAL;
			return CXChildVisit_Break;
		}

		ae2f_expected_if(SPECID_WORK != 0xFFFFFFFF) {
			util_constant* CONST_NODE = util_mk_constant_node(SPECID_WORK, CTX);

			CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;

			unless(CONST_NODE) {
				return CXChildVisit_Break;
			}

			unless(util_get_default_id(ID_DEFAULT_U32, CTX))
				return CXChildVisit_Break;


			unless(CONST_NODE->m_const_spec_id) {
				const aclspv_wrdcount_t RETCOUNT_TY = util_emitx_spec_constant(
						&CTX->m_section.m_types
						, CTX->m_count.m_types
						, ID_DEFAULT_U32
						, CTX->m_id
						, NUM_ARR 
						);

				const aclspv_wrdcount_t RETCOUNT_DC = 
					RETCOUNT_TY ? util_emitx_4(&CTX->m_section.m_decorate
							, CTX->m_count.m_decorate
							, SpvOpDecorate
							, CTX->m_id
							, SpvDecorationSpecId
							, SPECID_WORK
							) : 0;


				ae2f_expected_but_else(RETCOUNT_TY && RETCOUNT_DC) {
					return CXChildVisit_Break;
				}

				CONST_NODE->m_const_spec_id = CTX->m_id++;
				CONST_NODE->m_const_spec_type_id = ID_DEFAULT_U32;
				CTX->m_count.m_types = RETCOUNT_TY;
				CTX->m_count.m_decorate = RETCOUNT_DC;
			} else ae2f_expected_but_else(util_is_default_id_int(CONST_NODE->m_const_spec_type_id)) {
				CTX->m_state = ACLSPV_COMPILE_MET_INVAL;
				return CXChildVisit_Break;
			}

			INFO->m_arr_count_id = CONST_NODE->m_const_spec_id;
		} else {
			INFO->m_arr_count_id = util_mk_constant_val_id(NUM_ARR, CTX);
		}


		ae2f_expected_but_else(CTX->m_count.m_types = util_emitx_spec_constant_op2(
					&CTX->m_section.m_types
					, CTX->m_count.m_types
					, ID_DEFAULT_U32
					, CTX->m_id
					, SpvOpIAdd
					, CTX->m_id - 1
					, VAL_3
					))
			return CXChildVisit_Break;

		ae2f_expected_but_else(CTX->m_count.m_types = util_emitx_spec_constant_op2(
					&CTX->m_section.m_types
					, CTX->m_count.m_types
					, ID_DEFAULT_U32
					, CTX->m_id + 1
					, SpvOpShiftRightLogical
					, CTX->m_id
					, VAL_2
					)) return CXChildVisit_Break;



		ae2f_expected_but_else(CTX->m_count.m_types = util_emitx_type_array(
					&CTX->m_section.m_types
					, CTX->m_count.m_types
					, CTX->m_id + 2
					, ID_DEFAULT_U32
					, CTX->m_id + 1
					)) return CXChildVisit_Break;
		ae2f_expected_but_else(CTX->m_count.m_types = util_emitx_type_pointer(
					&CTX->m_section.m_types
					, CTX->m_count.m_types
					, CTX->m_id + 3
					, SpvStorageClassWorkgroup /* c_storage_class */
					, CTX->m_id + 2
					)) return CXChildVisit_Break;

		INFO->m_var_id		= CTX->m_id + 4;
		INFO->m_var_type_id	= CTX->m_id + 3;
		INFO->m_var_elm_type_id	= CTX->m_id + 2;
		INFO->m_arg_idx		= (aclspv_wrd_t)ARG_IDX;
		INFO->m_storage_class	= SpvStorageClassWorkgroup;
		INFO->m_entp_idx	= CTX->m_tmp.m_w0;

		CTX->m_id	+= 5;
		CTX->m_state	 = ACLSPV_COMPILE_OK;
		((aclspv_wrd_t* ae2f_restrict)CTX->m_section.m_name.m_p)[POS_FOR_LOCAL] = INFO->m_var_id;

		goto LBL_WORKGROUP_FINI;
	}
#undef	PUSH_SIZE
#undef	BIND_IDX
#undef	CTX
#undef	ARG_IDX
#undef	INTERFACE_COUNT
#undef	VAR_INFOS
#undef	INTERFACES_MGR
#undef	INTERFACES
#undef	INFO

	ae2f_unreachable();
	(void)h_parent;
}

static enum CXChildVisitResult emit_decl_glob_obj(CXCursor h_cur, CXCursor h_cur_parent, CXClientData h_ctx) {
#define CTX	((x_aclspv_ctx* ae2f_restrict)h_ctx)
	assert(CTX);


	unless(h_cur.kind == CXCursor_CompoundStmt && h_cur_parent.kind == CXCursor_FunctionDecl) 
		return CXChildVisit_Recurse;

	if(util_is_kernel(h_cur_parent)) {
		uintptr_t BUFF[6] = {
			0 /* push constant size */ , 
			0 /* bind index */, 
			0 /* ctx */,
			0 /* argument index */,
			0 /* interface count */,
		};

		const int NPARAMS = clang_Cursor_getNumArguments(h_cur_parent);

		CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;

		if(NPARAMS < 0) {
			CTX->m_state = ACLSPV_COMPILE_MET_INVAL;
			return CXChildVisit_Break;
		}

		((util_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)[CTX->m_tmp.m_w0]
			.m_fn = h_cur;
		((util_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)[CTX->m_tmp.m_w0]
			.m_id = CTX->m_tmp.m_w3 + CTX->m_fnlist.m_num_entp;
		((util_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)[CTX->m_tmp.m_w0]
			.m_id = CTX->m_tmp.m_w3 + CTX->m_fnlist.m_num_entp;

		BUFF[2] = (uintptr_t)CTX;

		_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_tmp.m_v0, (size_t)((unsigned)NPARAMS * sizeof(CXType)));
		ae2f_expected_but_else(CTX->m_tmp.m_v0.m_p) {
			CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;
			return CXChildVisit_Break;
		}

		clang_visitChildren(h_cur_parent, emit_decl_glob_obj_visit_fetch, BUFF);
		ae2f_unexpected_but_if(CTX->m_state) return CXChildVisit_Break;
		ae2f_unexpected_but_if(BUFF[0] > UINT32_MAX) {
			CTX->m_state = ACLSPV_COMPILE_TOO_BIG;
			return CXChildVisit_Break;
		}


		if(CTX->m_tmp.m_w4 < BUFF[0]) {
			CTX->m_tmp.m_w4 = (aclspv_wrd_t)BUFF[0];
		}

		if(BUFF[0]) {
			const aclspv_id_t PSH_PTR_ID 	= util_mk_constant_ptr_psh_id((aclspv_wrd_t)BUFF[0], CTX);
			const aclspv_id_t PSH_VAR_ID	= CTX->m_id++;

#if	!defined(NDEBUG) || !NDEBUG
			const aclspv_wrdcount_t	POS = CTX->m_count.m_name;
			CXString		NAME_STR = clang_getCursorSpelling(h_cur_parent);
			char* ae2f_restrict	NAME = NAME_STR.data ? malloc(strlen(NAME_STR.data) + (size_t)sizeof(":::push")) : 0;
#endif

			ae2f_expected_but_else(PSH_PTR_ID) {
				CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;
				goto LBL_DBG_STR_DISPOSE_FAIL;
			}

			CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;

			ae2f_expected_but_else(CTX->m_count.m_vars = util_emitx_4(
						&CTX->m_section.m_vars
						, CTX->m_count.m_vars
						, SpvOpVariable
						, PSH_PTR_ID
						, PSH_VAR_ID
						, SpvStorageClassPushConstant
						)) goto LBL_DBG_STR_DISPOSE_FAIL;


#if	!defined(NDEBUG) || !NDEBUG
#define EMIT_POS	CTX->m_count.m_name
			ae2f_expected_but_else(NAME) goto LBL_DBG_STR_DISPOSE_FAIL;
			strcpy(NAME, NAME_STR.data);
			strcat(NAME, ":::push");


			ae2f_expected_but_else((EMIT_POS = emit_opcode(&CTX->m_section.m_name, EMIT_POS, SpvOpName, 0))) 
				goto LBL_DBG_STR_DISPOSE_FAIL;
			ae2f_expected_but_else((EMIT_POS = util_emit_wrd(&CTX->m_section.m_name, EMIT_POS, PSH_VAR_ID)))
				goto LBL_DBG_STR_DISPOSE_FAIL;
			ae2f_expected_but_else((EMIT_POS = util_emit_str(&CTX->m_section.m_name, EMIT_POS, NAME))) 
				goto LBL_DBG_STR_DISPOSE_FAIL;
			set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_section.m_name)[POS], EMIT_POS - POS - 1);
			clang_disposeString(NAME_STR);
			free(NAME);

			if(0) {
				ae2f_unreachable();
LBL_DBG_STR_DISPOSE_FAIL:
				clang_disposeString(NAME_STR);
				free(NAME);
				return CXChildVisit_Break;
			}
#undef	EMIT_POS
#endif


			CTX->m_state = ACLSPV_COMPILE_OK;
#if 1 
			((util_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)[CTX->m_tmp.m_w0].m_push_ids.m_push_ptr
				= PSH_PTR_ID;
			((util_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)[CTX->m_tmp.m_w0].m_push_ids.m_push_var
				= PSH_VAR_ID;
			((util_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)[CTX->m_tmp.m_w0].m_push_ids.m_push_struct
				= util_mk_constant_struct_id((aclspv_wrd_t)BUFF[0], CTX);


			((util_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)[CTX->m_tmp.m_w0].m_nprm = (aclspv_wrd_t)BUFF[3];
#endif
		}


		mk_scale_from_vec(&CTX->m_scale_vars, 0);

		++CTX->m_tmp.m_w0;
	} else {
		((lib_build_fn_t* ae2f_restrict)CTX->m_fnlist.m_fn.m_p)[CTX->m_tmp.m_w1]
			.m_fn = h_cur_parent;
		((lib_build_fn_t* ae2f_restrict)CTX->m_fnlist.m_fn.m_p)[CTX->m_tmp.m_w1]
			.m_id = CTX->m_tmp.m_w3
			+ (aclspv_wrd_t)((aclspv_wrd_t)CTX->m_fnlist.m_entp.m_sz / sizeof(util_entp_t))
			+ CTX->m_tmp.m_w1;

		++CTX->m_tmp.m_w1;
	}

	return CXChildVisit_Continue;

#undef	CTX
	ae2f_unreachable();
	return CXChildVisit_Recurse;

	ae2f_unreachable();

#if	!defined(NDEBUG) || !NDEBUG
#else
LBL_DBG_STR_DISPOSE_FAIL:
	return CXChildVisit_Break;
#endif
}
