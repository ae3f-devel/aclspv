#include <assert.h>
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

#include <attr/specid.h>
#include <attr/location.h>
#include <attr/storage_class.h>

#include <ae2f/Keys.h>
#include <ae2f/c90/StdBool.h>
#include <ae2f/c90/StdInt.h>

#include <spirv/unified1/spirv.h>
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
#define	IO_ARG_IDX	((uintptr_t* ae2f_restrict)h_data)[5]

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
#define	INFO			(((INTERFACES + ARG_IDX)))

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

	INFO->m_unified.m_arg_idx		= (aclspv_wrd_t)ARG_IDX;
	INFO->m_unified.m_var_id		= CTX->m_id++;
	INFO->m_unified.m_var_elm_type_id	= util_mk_constant_struct_id(1, CTX);
	INFO->m_unified.m_var_type_id		= util_mk_constant_ptr_storage_id(1, CTX);
	INFO->m_unified.m_cursor		= h_cur;
	INFO->m_unified.m_entp_idx		= CTX->m_tmp.m_w0;
	INFO->m_unified.m_arg_idx		= (aclspv_wrd_t)ARG_IDX;

	INFO->m_bindable.m_set			= 0;
	INFO->m_bindable.m_binding		= (aclspv_wrd_t)BIND_IDX;

	/** FETCH BIND END */
#if	!defined(NDEBUG) || !NDEBUG
	FUNC_NAME	= clang_getCursorSpelling(h_parent);
	PARAM_NAME	= clang_getCursorSpelling(h_cur);

	{
		char* ae2f_restrict const	
			NAME_MERGE = malloc(strlen(FUNC_NAME.data) + strlen(PARAM_NAME.data) + 3);
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
		unless((EMIT_POS = util_emit_wrd(&CTX->m_section.m_name, EMIT_POS, INFO->m_unified.m_var_id)))
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
		INFO->m_unified.m_storage_class = SpvStorageClassWorkgroup;
	else if(strstr(PARAM_TY_NAME.data, "global")) {
		INFO->m_unified.m_storage_class = SpvStorageClassStorageBuffer;
	} else if (strstr(PARAM_TY_NAME.data, "constant")) {
		INFO->m_unified.m_storage_class = SpvStorageClassUniform;
	} else {
		INFO->m_unified.m_storage_class = SpvStorageClassMax;

		/** ATTRIBUTE HARD CODED? */
		clang_visitChildren(
				h_cur
				, attr_storage_class
				, &INFO->m_unified.m_storage_class
				);
	}

	clang_disposeString(PARAM_TY_NAME);

	switch(INFO->m_unified.m_storage_class) {
		default:
		case SpvStorageClassMax:
			CTX->m_state = ACLSPV_COMPILE_STORAGE_CLASS_UNDEFINED;
			return CXChildVisit_Break;

			/** BINDABLES */
			ae2f_unexpected_but_if(0) {
				ae2f_unreachable();
				case SpvStorageClassUniform:
				INFO->m_unified.m_var_type_id = util_mk_constant_ptr_uniform_id(1, CTX);
				ae2f_expected_but_else(INFO->m_unified.m_var_type_id) 
					return CXChildVisit_Break;
			}
			ae2f_fallthrough;

		case SpvStorageClassStorageBuffer:
			clang_visitChildren(h_cur
					, emit_decl_glob_obj_visit_attr_set
					, &INFO->m_bindable.m_set
					);

			CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;

			ae2f_expected_but_else(CTX->m_count.m_decorate = util_emitx_4(
						/** Decorations::Descriptorsets */
						&CTX->m_section.m_decorate
						, CTX->m_count.m_decorate
						, SpvOpDecorate
						, INFO->m_unified.m_var_id
						, SpvDecorationDescriptorSet
						, INFO->m_bindable.m_set
						)) return CXChildVisit_Break;

			ae2f_expected_but_else(CTX->m_count.m_decorate = util_emitx_4(
						/** Decorations::Binding */
						&CTX->m_section.m_decorate
						, CTX->m_count.m_decorate
						, SpvOpDecorate
						, INFO->m_unified.m_var_id
						, SpvDecorationBinding
						, INFO->m_bindable.m_binding
						)) return CXChildVisit_Break;

			ae2f_expected_but_else(CTX->m_count.m_vars = util_emitx_variable(
						&CTX->m_section.m_vars
						, CTX->m_count.m_vars
						, INFO->m_unified.m_var_type_id
						, INFO->m_unified.m_var_id
						, INFO->m_unified.m_storage_class
						)) return CXChildVisit_Break;

			++BIND_IDX;
			break;

		case SpvStorageClassWorkgroup:
			PARAM_TY_NAME	= clang_getTypeSpelling(PARAM_TY);
			unless(strstr(PARAM_TY_NAME.data, "local")) {
				clang_disposeString(PARAM_TY_NAME);
				CTX->m_state = ACLSPV_COMPILE_MET_INVAL;
				return CXChildVisit_Break;
			} clang_disposeString(PARAM_TY_NAME);

			unless(PARAM_TY.kind == CXType_ConstantArray) {
				CTX->m_state = ACLSPV_COMPILE_WORKGROUP_MUST_BE_CONSTANT_ARRAY;
				return CXChildVisit_Break;
			}

			{
				aclspv_wrd_t	SPECID_WORK	= 0xFFFFFFFF;
				const intmax_t NUM_ARR_ORIG	= clang_getArraySize(PARAM_TY);
				const intmax_t REAL_ARR_ORIG	= clang_Type_getSizeOf(PARAM_TY);
				const aclspv_wrd_t NUM_ARR	= NUM_ARR_ORIG < 0 || NUM_ARR_ORIG > UINT32_MAX ? 0 : (aclspv_wrd_t)NUM_ARR_ORIG;
				const aclspv_wrd_t REAL_ARR	= REAL_ARR_ORIG < 0 || REAL_ARR_ORIG > UINT32_MAX ? 0 : (aclspv_wrd_t)REAL_ARR_ORIG;
				const aclspv_wrd_t ELM_ARR	= ae2f_expected(NUM_ARR && REAL_ARR) ? REAL_ARR / NUM_ARR : 0;
				const aclspv_id_t VAL_ELM	= util_mk_constant_val_id(ELM_ARR, CTX);
				const aclspv_id_t VAL_2		= util_mk_constant_val_id(2, CTX);

				const aclspv_id_t VAL_3		= util_mk_constant_val_id(3, CTX);

				assert(REAL_ARR >= NUM_ARR && !(REAL_ARR % NUM_ARR));
				{ ae2f_assume(REAL_ARR >= NUM_ARR && !(REAL_ARR % NUM_ARR)); }

				ae2f_expected_but_else(VAL_2)		return CXChildVisit_Break;
				ae2f_expected_but_else(VAL_ELM)		return CXChildVisit_Break;

				clang_visitChildren(
						h_cur
						, attr_specid 
						, &SPECID_WORK
						);

				ae2f_expected_but_else(NUM_ARR) {
					CTX->m_state = ACLSPV_COMPILE_MET_INVAL;
					return CXChildVisit_Break;
				}

				ae2f_expected_if(SPECID_WORK != 0xFFFFFFFF) {
					util_constant* CONST_NODE = util_mk_constant_node(SPECID_WORK, CTX);

					CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;

					ae2f_expected_but_else(CONST_NODE) {
						return CXChildVisit_Break;
					}

					ae2f_expected_but_else(util_get_default_id(ID_DEFAULT_U32, CTX))
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

					INFO->m_work.m_arr_count_id = CONST_NODE->m_const_spec_id;
				} else {
					INFO->m_work.m_arr_count_id = util_mk_constant_val_id(NUM_ARR, CTX);
				}


				ae2f_expected_but_else(CTX->m_count.m_types = util_emitx_spec_constant_op2(
							&CTX->m_section.m_types
							, CTX->m_count.m_types
							, ID_DEFAULT_U32
							, CTX->m_id
							, SpvOpIMul
							, CTX->m_id - 1
							, VAL_ELM
							))
					return CXChildVisit_Break;

				ae2f_expected_but_else(CTX->m_count.m_types = util_emitx_spec_constant_op2(
							&CTX->m_section.m_types
							, CTX->m_count.m_types
							, ID_DEFAULT_U32
							, CTX->m_id + 1
							, SpvOpIAdd
							, CTX->m_id
							, VAL_3
							))
					return CXChildVisit_Break;

				ae2f_expected_but_else(CTX->m_count.m_types = util_emitx_spec_constant_op2(
							&CTX->m_section.m_types
							, CTX->m_count.m_types
							, ID_DEFAULT_U32
							, CTX->m_id + 2
							, SpvOpShiftRightLogical
							, CTX->m_id + 1
							, VAL_2
							)) return CXChildVisit_Break;

				ae2f_expected_but_else(CTX->m_count.m_types = util_emitx_type_array(
							&CTX->m_section.m_types
							, CTX->m_count.m_types
							, CTX->m_id + 3
							, ID_DEFAULT_U32
							, CTX->m_id + 2
							)) return CXChildVisit_Break;
				ae2f_expected_but_else(CTX->m_count.m_types = util_emitx_type_pointer(
							&CTX->m_section.m_types
							, CTX->m_count.m_types
							, CTX->m_id + 4
							, SpvStorageClassWorkgroup /* c_storage_class */
							, CTX->m_id + 3
							)) return CXChildVisit_Break;

				INFO->m_unified.m_var_id		= CTX->m_id + 5;
				INFO->m_unified.m_var_type_id		= CTX->m_id + 4;
				INFO->m_unified.m_var_elm_type_id	= CTX->m_id + 3;
				INFO->m_unified.m_storage_class		= SpvStorageClassWorkgroup;

				CTX->m_id	+= 6;
				CTX->m_state	 = ACLSPV_COMPILE_OK;

				((aclspv_wrd_t* ae2f_restrict)CTX->m_section.m_name.m_p)[POS_FOR_LOCAL] 
					= INFO->m_unified.m_var_id;
			}

			break;

			/** LOCATED */

			ae2f_unexpected_but_if(0) {
				ae2f_unreachable();
				case SpvStorageClassInput:
				INFO->m_unified.m_var_type_id = util_get_default_id(ID_DEFAULT_U32V4_PTR_INP, CTX);
			}

			ae2f_unexpected_but_if(0) {
				ae2f_unreachable();
				case SpvStorageClassOutput:
				INFO->m_unified.m_var_type_id = util_get_default_id(ID_DEFAULT_U32V4_PTR_OUT, CTX);
			}

			clang_visitChildren(h_cur, attr_location, &INFO->m_io.m_location);
#define		THIS_ENTP	((util_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)[CTX->m_tmp.m_w0]
			/** TODO: this will now uintvec4. make this flexible some day */
			assert(IO_ARG_IDX < THIS_ENTP.m_io.m_num);
			INFO->m_unified.m_var_id		= THIS_ENTP.m_io.m_anchour + (aclspv_wrd_t)IO_ARG_IDX;
			INFO->m_unified.m_var_elm_type_id	= util_mk_constant_vec32_id(4, CTX);

			ae2f_expected_but_else(CTX->m_count.m_decorate = util_emitx_4(
						&CTX->m_section.m_decorate
						, CTX->m_count.m_decorate
						, SpvOpDecorate
						, INFO->m_unified.m_var_id
						, SpvDecorationLocation
						, INFO->m_io.m_location
						)) return CXChildVisit_Break;

			ae2f_expected_but_else(CTX->m_count.m_vars = util_emitx_variable(
						&CTX->m_section.m_vars
						, CTX->m_count.m_vars
						, INFO->m_unified.m_var_type_id
						, INFO->m_unified.m_var_id
						, INFO->m_unified.m_storage_class
						)) return CXChildVisit_Break;

			get_wrd_of_vec(&CTX->m_section.m_name)[POS_FOR_LOCAL] = INFO->m_unified.m_var_id;
#undef		THIS_ENTP

			++IO_ARG_IDX;
			break;
	}

	CTX->m_state = ACLSPV_COMPILE_OK;
	++ARG_IDX;
	++INTERFACE_COUNT;
	return CXChildVisit_Continue;


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
			0 /* io argument count */
		};

		const int NPARAMS = clang_Cursor_getNumArguments(h_cur_parent);

		CTX->m_state = ACLSPV_COMPILE_ALLOC_FAILED;

		if(NPARAMS < 0) {
			CTX->m_state = ACLSPV_COMPILE_MET_INVAL;
			return CXChildVisit_Break;
		}

		unless(NPARAMS) goto LBL_HOLLOW_ENTP;

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


		if(CTX->m_tmp.m_w2 < BUFF[0]) {
			CTX->m_tmp.m_w2 = (aclspv_wrd_t)BUFF[0];
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

LBL_HOLLOW_ENTP:
		mk_scale_from_vec(&CTX->m_scale_vars, 0);
		++CTX->m_tmp.m_w0;
	} else {
	}

	return CXChildVisit_Continue;

#undef	CTX
	ae2f_unreachable();

#if	!defined(NDEBUG) || !NDEBUG
#else
LBL_DBG_STR_DISPOSE_FAIL:
	return CXChildVisit_Break;
#endif
}
