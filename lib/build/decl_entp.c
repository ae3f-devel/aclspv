#include <build.h>
#include <limits.h>

#include "./wrdemit.h"
#include "./entp.h"
#include "./constant.h"
#include "./iddef.h"

#include "aclspv/pass.h"

#include <pass/md.h>

#include <spirv/unified1/spirv.h>


#define	BADALLOC	FN_ACLSPV_PASS_ALLOC_FAILED

ACLSPV_ABI_IMPL ae2f_noexcept e_fn_aclspv_pass aclspv_build_decl_entp(
		const LLVMModuleRef		M,
		const h_aclspv_build_ctx_t	CTX
		)
{
	aclspv_wrd_t	i;
	const LLVMContextRef	C = LLVMGetModuleContext(M);

	const unsigned pod_clusters_md_id = C ? LLVMGetMDKindIDInContext(
			(C), ACLSPV_MD_POD_CLUSTERS,
			sizeof(ACLSPV_MD_POD_CLUSTERS) - 1) : 0;

	const unsigned pipelayout_md_id = C ? LLVMGetMDKindIDInContext(
			C, ACLSPV_MD_PIPELINE_LAYOUT
			, sizeof(ACLSPV_MD_PIPELINE_LAYOUT) - 1
			) : 0;

	unless(pipelayout_md_id)	return FN_ACLSPV_PASS_GET_FAILED;
	unless(C)			return FN_ACLSPV_PASS_GET_FAILED;


	for(i = CTX->m_fnlist.m_num_entp; i--;) {
#define entp	((lib_build_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)[i]
		const char* ae2f_restrict	
			name = LLVMGetValueName(entp.m_fn);
		const LLVMValueRef pod_clusters_md =  LLVMGetMetadata(entp.m_fn, pod_clusters_md_id);
		const LLVMValueRef pipelayout_md =  LLVMGetMetadata(entp.m_fn, pipelayout_md_id);

		const unsigned num_pipelayout_md = LLVMGetMDNodeNumOperands(pipelayout_md);

		spvsz_t pos;

		if (pod_clusters_md && LLVMGetNumOperands(pod_clusters_md) <= 0) {
PSHCONSTANT_IS_ZERO:
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

			aclspv_wrd_t array_size_const;
			aclspv_wrd_t array_type_id;
			aclspv_wrd_t push_struct_id;
			aclspv_wrd_t push_ptr_id;
			aclspv_wrd_t push_var_id = next_id++;
			aclspv_wrdcount_t	arrcount;

			unless(num_pipelayout_md) {
				return FN_ACLSPV_PASS_GET_FAILED;
			}
			else {
				unsigned	num_range_nodes;
				LLVMValueRef	range_node;

				_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_tmp.m_v0, (size_t)(num_pipelayout_md * sizeof(LLVMValueRef)));
				unless(CTX->m_tmp.m_v0.m_p)	return FN_ACLSPV_PASS_ALLOC_FAILED;
				LLVMGetMDNodeOperands(pipelayout_md, CTX->m_tmp.m_v0.m_p);
				range_node = (0)[(LLVMValueRef* ae2f_restrict)(CTX)->m_tmp.m_v0.m_p];
				unless(range_node)			return FN_ACLSPV_PASS_GET_FAILED;
				unless(LLVMIsAMDNode(range_node))	return FN_ACLSPV_PASS_MET_INVAL;
				num_range_nodes = LLVMGetMDNodeNumOperands(range_node);


				unless(num_range_nodes) {
					arrcount = 0;
					goto PSHCONSTANT_IS_ZERO;
				}
				else {
					LLVMValueRef	SIZE_VAL;
					uintmax_t	SIZE;

					_aclspv_grow_vec_with_copy(
							_aclspv_malloc, _aclspv_free, _aclspv_memcpy
							, COPY, CTX->m_tmp.m_v0
							, (size_t)((num_range_nodes + 1) * sizeof(LLVMValueRef)));

					unless(CTX->m_tmp.m_v0.m_p)					return FN_ACLSPV_PASS_ALLOC_FAILED; 
					unless(*(LLVMValueRef* ae2f_restrict)CTX->m_tmp.m_v0.m_p)	return FN_ACLSPV_PASS_GET_FAILED;
					LLVMGetMDNodeOperands(
							*(LLVMValueRef* ae2f_restrict)CTX->m_tmp.m_v0.m_p
							, ((LLVMValueRef* ae2f_restrict)CTX->m_tmp.m_v0.m_p) + 1
							);

					SIZE_VAL = ((LLVMValueRef* ae2f_restrict)CTX->m_tmp.m_v0.m_p)[3];

					unless(LLVMIsAConstant(SIZE_VAL)) {
						assert(0 && "VAL was not constant");
						return FN_ACLSPV_PASS_GET_FAILED;
					}

					SIZE = SIZE_VAL ? LLVMConstIntGetZExtValue(SIZE_VAL) : 0;

					if(SIZE > UINT32_MAX)	{
						return FN_ACLSPV_PASS_TOO_BIG;
					}
					unless(SIZE)		{
						return FN_ACLSPV_PASS_MET_INVAL;
					}
					arrcount = sz_to_count(SIZE);
				}
			}

			/** OpConstant for array size (32 = 128 bytes) */
			unless(array_size_const = lib_build_mk_constant_val_id(arrcount, CTX)) return BADALLOC;
			/** OpTypeArray %uint [32] */
			unless(array_type_id = lib_build_mk_constant_arr32_id(arrcount, CTX)) return BADALLOC;
			/** OpTypeStruct { array< uint, 32 > } */
			/** OpDecorate %push_struct Block */
			unless(push_struct_id = lib_build_mk_constant_struct_id(arrcount, CTX)) return BADALLOC;
			/* OpTypePointer PushConstant %push_struct */
			unless(push_ptr_id = lib_build_mk_constant_ptr_psh_id(arrcount, CTX)) return BADALLOC;

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
		unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, entp.m_id)))				return BADALLOC;
		unless((ret_count = emit_str(&CTX->m_ret, ret_count, LLVMGetValueName(entp.m_fn))))	return BADALLOC;
		set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_ret)[pos], ret_count - pos - 1);
#endif
	}

	return FN_ACLSPV_PASS_OK;
}
