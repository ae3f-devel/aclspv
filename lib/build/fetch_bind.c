#include <build.h>
#include <assert.h>
#include <stdio.h>

#include <pass/md.h>
#include <pass/argknd.h>

#include <llvm-c/Core.h>
#include <llvm-c/Types.h>
#include <spirv/unified1/spirv.h>

#include "./wrdemit.h"
#include "./entp.h"
#include "./bind.h"
#include "./scale.h"
#include "./constant.h"
#include "./llvmtospv.h"
#include "aclspv/pass.h"

ACLSPV_ABI_IMPL ae2f_noexcept e_fn_aclspv_pass	aclspv_build_fetch_bind(
		const LLVMModuleRef		M,
		const h_aclspv_build_ctx_t	CTX
		)
{
	aclspv_wrdcount_t   i_fn = CTX->m_fnlist.m_num_entp;
	const LLVMContextRef C = LLVMGetModuleContext(M);
	const unsigned layout_md_id = LLVMGetMDKindIDInContext(C, ACLSPV_MD_PIPELINE_LAYOUT,
			sizeof(ACLSPV_MD_PIPELINE_LAYOUT)-1);
	const unsigned arg_kind_md_id = LLVMGetMDKindIDInContext(C, ACLSPV_MD_ARGKND,
			sizeof(ACLSPV_MD_ARGKND)-1);

	unsigned	num_vars = 0;

	assert(CTX);
	assert(CTX->m_scale_vars.m_p);
	assert(CTX->m_scale_vars.m_sz);
	assert(layout_md_id);
	unless(layout_md_id) {
		return FN_ACLSPV_PASS_MET_INVAL;
	}

	while(i_fn--) {
#define	kernel	((lib_build_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)[i_fn]
		const LLVMValueRef layout_md = LLVMGetMetadata(kernel.m_fn, layout_md_id);
		LLVMValueRef		set_layouts_node;
		int			num_sets, i_set;
		aclspv_wrdcount_t	interface_count = 0;

		_aclspv_grow_vec(
				_aclspv_malloc, _aclspv_free
				, CTX->m_tmp.m_v0
				, (size_t)(LLVMCountParams(kernel.m_fn) * sizeof(LLVMTypeRef))
				);
		unless(CTX->m_tmp.m_v0.m_p) return FN_ACLSPV_PASS_ALLOC_FAILED;
		LLVMGetParamTypes(
				LLVMGlobalGetValueType(kernel.m_fn)
				, CTX->m_tmp.m_v0.m_p);


		if (!layout_md || !LLVMIsAMDNode(layout_md) || LLVMGetNumOperands(layout_md) < 2) {
			goto LBL_NEXT_KRNL;
		}

		set_layouts_node = LLVMGetOperand(layout_md, 1);
		if (!set_layouts_node || !LLVMIsAMDNode(set_layouts_node)) {
			goto LBL_NEXT_KRNL;
		}

		num_sets = LLVMGetNumOperands(set_layouts_node);
		for(i_set = 0; i_set < num_sets; ++i_set) {
			const LLVMValueRef set_layout_node = LLVMGetOperand(set_layouts_node, (unsigned)i_set);
			LLVMValueRef	set_num_val, bindings_node;
			aclspv_wrd_t	set;
			int	i_binding;

			if (!set_layout_node || !LLVMIsAMDNode(set_layout_node) || LLVMGetNumOperands(set_layout_node) < 2) {
				continue;
			}

			set_num_val = LLVMGetOperand(set_layout_node, 0);
			set = (aclspv_wrd_t)LLVMConstIntGetZExtValue(set_num_val);

			bindings_node = LLVMGetOperand(set_layout_node, 1);
			if (!bindings_node || !LLVMIsAMDNode(bindings_node)) {
				continue;
			}

			i_binding = LLVMGetNumOperands(bindings_node);

			/* Iterate over bindings */

#define var_infos	((lib_build_bind* ae2f_restrict)CTX->m_vecid_vars.m_p)
#define interface_ids_mgr	get_last_scale_from_vec(&CTX->m_scale_vars)
#define	interface_ids		((aclspv_wrd_t* ae2f_restrict)get_buf_from_scale(&CTX->m_scale_vars, *interface_ids_mgr))
			while (i_binding --> 0) {
				const LLVMValueRef binding_node = LLVMGetOperand(bindings_node, (unsigned)i_binding);
				LLVMValueRef			arg_idx_val, binding_val;
				lib_build_bind* ae2f_restrict	info;

				assert(CTX->m_scale_vars.m_p);
				assert(interface_ids_mgr);

				if (!binding_node || !LLVMIsAMDNode(binding_node) || LLVMGetNumOperands(binding_node) < 5) {
					continue;
				}


				if (CTX->m_vecid_vars.m_sz <= ((size_t)sizeof(lib_build_bind) * (size_t)num_vars)) {
					_aclspv_grow_vec_with_copy(
							_aclspv_malloc, _aclspv_free, _aclspv_memcpy, L_new
							, CTX->m_vecid_vars
							, CTX->m_vecid_vars.m_sz 
							? CTX->m_vecid_vars.m_sz << 1 
							: (size_t)(sizeof(lib_build_bind) << 3)
							);

					unless(var_infos)
						return FN_ACLSPV_PASS_ALLOC_FAILED;
				}

				info = &var_infos[num_vars];

				arg_idx_val = LLVMGetOperand(binding_node, 0);
				binding_val = LLVMGetOperand(binding_node, 1);


				info->m_arg_idx = (aclspv_wrd_t)LLVMConstIntGetZExtValue(arg_idx_val);

				info->m_var_id = CTX->m_id++;
				/** 
				 * NOTICE:
				 * size matters in context of alignment, not the actual size of element shall be.
				 * */
				info->m_struct_id = lib_build_mk_constant_struct_id(1, CTX);
				unless(info->m_struct_id) return FN_ACLSPV_PASS_MET_INVAL;
				info->m_binding = (aclspv_wrd_t)LLVMConstIntGetZExtValue(binding_val);

				assert(info->m_arg_idx == info->m_binding);
				info->m_set = set;
				info->m_entp_idx = i_fn;

				/* Determine storage class based on argument kind */
				{
					const LLVMValueRef arg_kinds_node = LLVMGetMetadata(kernel.m_fn, arg_kind_md_id);
					if (arg_kinds_node && LLVMGetMDNodeNumOperands(arg_kinds_node) > (unsigned)info->m_arg_idx) {
						LLVMValueRef arg_kind_md = LLVMGetOperand(arg_kinds_node, (unsigned)info->m_arg_idx);
						if (arg_kind_md) {
							unsigned len;
							const char* kind_str = LLVMGetMDString(arg_kind_md, &len);
							if (kind_str) {
								if (strcmp(kind_str, ACLSPV_ARGKND_BUFF_UBO) == 0) {
									info->m_storage_class = SpvStorageClassUniform;
									info->m_struct_id = lib_build_mk_constant_struct128_id(1, CTX);
									info->m_ptr_struct_id = lib_build_mk_constant_ptr_uniform_id(
											1, CTX
											);
								} else {
									info->m_storage_class = SpvStorageClassStorageBuffer;
									info->m_ptr_struct_id = lib_build_mk_constant_ptr_storage_id(
										1	
											, CTX
											);
								}
							} else {
								info->m_storage_class = SpvStorageClassStorageBuffer;
								info->m_ptr_struct_id = lib_build_mk_constant_ptr_storage_id(
									1	
										, CTX);
							}
						} else {
							info->m_storage_class = SpvStorageClassStorageBuffer;
							info->m_ptr_struct_id = lib_build_mk_constant_ptr_storage_id(
								1	
									, CTX);
						}
					} else {
						info->m_storage_class = SpvStorageClassStorageBuffer;
						info->m_ptr_struct_id = lib_build_mk_constant_ptr_storage_id(
							1	
								, CTX);
					}
				}

				assert(CTX->m_scale_vars.m_p);


				/** store id */
				if(grow_last_scale(&CTX->m_scale_vars, (size_t)count_to_sz(interface_count + 1))) {
					return FN_ACLSPV_PASS_ALLOC_FAILED;
				}

				assert(info);
				assert(interface_ids);
				assert(interface_ids_mgr);
				interface_ids[interface_count++] = info->m_var_id;
				++num_vars;
			}
		}

LBL_NEXT_KRNL:
		assert(get_last_scale_from_vec(&CTX->m_scale_vars)->m_sz >= (size_t)count_to_sz(interface_count));
		get_last_scale_from_vec(&CTX->m_scale_vars)->m_sz = (size_t)count_to_sz(interface_count);

		unless(mk_scale_from_vec(&CTX->m_scale_vars, 0)) {
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		}
	}

	CTX->m_vecid_vars.m_sz = (size_t)count_to_sz(num_vars);
	return FN_ACLSPV_PASS_OK;
}
