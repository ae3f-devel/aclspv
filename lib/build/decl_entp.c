#include <build.h>
#include "./wrdemit.h"
#include "./entp.h"
#include "./id.h"

#include <pass/md.h>

#include <spirv/1.0/spirv.h>

#define	BADALLOC	FN_ACLSPV_PASS_ALLOC_FAILED

ACLSPV_ABI_IMPL ae2f_noexcept e_fn_aclspv_pass aclspv_build_decl_entp(
		const LLVMModuleRef		M,
		const h_aclspv_build_ctx_t	CTX
		)
{
	aclspv_wrd_t	i;
	IGNORE(M);

	for(i = CTX->m_fnlist.m_num_entp; i--;) {
#define entp	((lib_build_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)[i]
		const char* ae2f_restrict	
			name = LLVMGetValueName(entp.m_fn);
		spvsz_t pos;

		const unsigned pod_clusters_md_id = LLVMGetMDKindIDInContext(
				LLVMGetModuleContext(M), ACLSPV_MD_POD_CLUSTERS,
				sizeof(ACLSPV_MD_POD_CLUSTERS) - 1);

		LLVMValueRef pod_clusters_md = LLVMGetMetadata(entp.m_fn, pod_clusters_md_id);

		if (pod_clusters_md && LLVMGetNumOperands(pod_clusters_md) <= 0) {
			((lib_build_entp_t* ae2f_restrict) CTX->m_fnlist.m_entp.m_p)[i].m_push_ids.m_push_ptr
				= 0;

			((lib_build_entp_t* ae2f_restrict) CTX->m_fnlist.m_entp.m_p)[i].m_push_ids.m_push_struct
				= 0;

			((lib_build_entp_t* ae2f_restrict) CTX->m_fnlist.m_entp.m_p)[i].m_push_ids.m_push_var
				= 0;

			((lib_build_entp_t* ae2f_restrict) CTX->m_fnlist.m_entp.m_p)[i].m_push_ids.m_arrsz
				= 0;

			((lib_build_entp_t* ae2f_restrict) CTX->m_fnlist.m_entp.m_p)[i].m_push_ids.m_arrty
				= 0;
		}
		else {
#define	next_id		CTX->m_id
#define	uint_type_id	ID_DEFAULT_INT32

			aclspv_wrd_t array_size_const = next_id++;
			aclspv_wrd_t array_type_id = next_id++;
			aclspv_wrd_t push_struct_id = next_id++;
			aclspv_wrd_t push_ptr_id = next_id++;
			aclspv_wrd_t push_var_id = next_id++;

			/* OpTypeRuntimeArray %uint */
#undef m_ret
#undef ret_count
#define m_ret   m_section.m_types
#define ret_count CTX->m_count.m_types
			/** OpConstant for array size (32 = 128 bytes) */
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpConstant, 3))) return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, uint_type_id))) return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, array_size_const))) return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 32))) return BADALLOC;  

			/** OpTypeArray %uint [32] */
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpTypeArray, 3))) return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, array_type_id))) return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, uint_type_id))) return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, array_size_const))) return BADALLOC;

			/** OpTypeStruct { array< uint, 32 > } */
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpTypeStruct, 2))) return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, push_struct_id))) return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, array_type_id))) return BADALLOC;


			/* OpDecorate %push_struct Block */
#undef m_ret
#undef ret_count
#define m_ret   m_section.m_decorate
#define ret_count CTX->m_count.m_decorate

			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpDecorate, 2))) 
				return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, push_struct_id))) 
				return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvDecorationBlock))) 
				return BADALLOC;

			/* REQUIRED: Member Offset */
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpMemberDecorate, 4))) return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, push_struct_id))) return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 0))) return BADALLOC;  /** member index 0 */
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvDecorationOffset))) return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 0))) return BADALLOC;

#if 1
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpDecorate, 3))) return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, array_type_id))) return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvDecorationArrayStride))) return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 4))) return BADALLOC;
#endif

			/* OpTypePointer PushConstant %push_struct */
#undef m_ret
#undef ret_count
#define m_ret   m_section.m_types
#define ret_count CTX->m_count.m_types

			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpTypePointer, 3))) 
				return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, push_ptr_id))) 
				return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvStorageClassPushConstant))) 
				return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, push_struct_id))) 
				return BADALLOC;

			/* OpVariable */
#undef m_ret
#undef ret_count
#define m_ret   m_section.m_vars
#define ret_count CTX->m_count.m_vars

			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpVariable, 3))) 
				return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, push_ptr_id))) 
				return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, push_var_id))) 
				return BADALLOC;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvStorageClassPushConstant))) 
				return BADALLOC;

			/* Add to OpEntryPoint interface — we'll do this in the next section */
			/* Store push_var_id in a per-kernel list or directly emit later */

			((lib_build_entp_t* ae2f_restrict) CTX->m_fnlist.m_entp.m_p)[i].m_push_ids.m_push_ptr
				= push_ptr_id;

			((lib_build_entp_t* ae2f_restrict) CTX->m_fnlist.m_entp.m_p)[i].m_push_ids.m_push_struct
				= push_struct_id;

			((lib_build_entp_t* ae2f_restrict) CTX->m_fnlist.m_entp.m_p)[i].m_push_ids.m_push_var
				= push_var_id;

			((lib_build_entp_t* ae2f_restrict) CTX->m_fnlist.m_entp.m_p)[i].m_push_ids.m_arrsz
				= array_size_const;

			((lib_build_entp_t* ae2f_restrict) CTX->m_fnlist.m_entp.m_p)[i].m_push_ids.m_arrty
				= array_type_id;
		}

#undef	m_ret
#undef	ret_count
#define m_ret		m_section.m_entp
#define	ret_count	CTX->m_count.m_entp

		pos = ret_count;

		/** OpEntryPoint */
		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpEntryPoint, 0)))
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvExecutionModelGLCompute)))
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, entp.m_id)))
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		unless((ret_count = emit_str(&CTX->m_ret, ret_count, name))) 
			return FN_ACLSPV_PASS_ALLOC_FAILED;

		set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_ret)[pos], ret_count - pos - 1);

#undef	m_ret
#undef	ret_count
#define	m_ret		m_section.m_execmode
#define	ret_count	CTX->m_count.m_execmode

		/** OpExecMode */
		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpExecutionMode, 5)))	return BADALLOC;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, entp.m_id)))			return BADALLOC;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvExecutionModeLocalSize)))	return BADALLOC;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 1)))				return BADALLOC;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 1)))				return BADALLOC;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 1)))				return BADALLOC;

#if !defined(NDEBUG) || !NDEBUG
#undef	m_ret
#undef	ret_count
#define	m_ret		m_section.m_name
#define	ret_count	CTX->m_count.m_name
		pos = ret_count;
		unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpName, 0)))			return BADALLOC;
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, entp.m_id)))			return BADALLOC;
		unless((ret_count = emit_str(&CTX->m_ret, ret_count, LLVMGetValueName(entp.m_fn))))	return BADALLOC;
		set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_ret)[pos], ret_count - pos - 1);
#endif
	}

	return FN_ACLSPV_PASS_OK;
}
