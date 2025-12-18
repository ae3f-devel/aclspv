#include "./ctx.h"

#include <llvm-c/Core.h>
#include <string.h>
#include <assert.h>

#include "./argknd.h"
#include "md.h"

#ifndef unless
#define unless(c) if (!(c))
#endif

IMPL_PASS_RET aclspv_pass_cluster_pod_args(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		)
{	const LLVMContextRef C = LLVMGetModuleContext(M);
	const unsigned arg_kind_md_id = LLVMGetMDKindIDInContext(C, ACLSPV_MD_ARGKND, sizeof(ACLSPV_MD_ARGKND) - 1);
	const unsigned pod_cluster_md_id = LLVMGetMDKindIDInContext(C, ACLSPV_MD_POD_CLUSTERS, sizeof(ACLSPV_MD_POD_CLUSTERS) - 1);
	unsigned	i, j;

	LLVMValueRef F, FNxt;

	for (F = LLVMGetFirstFunction(M); F; F = FNxt) {
		LLVMValueRef	arg_kinds_node;
		unsigned	num_args;
		unsigned	num_clusters = 0;

		FNxt = LLVMGetNextFunction(F);

		if (LLVMGetFunctionCallConv(F) != LLVMSPIRKERNELCallConv) {
			continue;
		}

		arg_kinds_node = LLVMGetMetadata(F, arg_kind_md_id);
		unless (arg_kinds_node) continue;
		num_args = LLVMGetMDNodeNumOperands(arg_kinds_node);
		if (num_args == 0) continue;

		_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_v0, (size_t)(sizeof(LLVMValueRef) * 5));
		_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_v1, (size_t)(sizeof(LLVMValueRef) * num_args));
#define	cluster_nodes		ae2f_static_cast(LLVMValueRef* ae2f_restrict, CTX->m_v0.m_p)
#define clusters_capacity	(CTX)->m_v0.m_sz
#define args		ae2f_static_cast(LLVMValueRef* ae2f_restrict, CTX->m_v1.m_p)

		unless(cluster_nodes && args) {
			return FN_ACLSPV_PASS_ALLOC_FAILED;
		}

		LLVMGetMDNodeOperands(arg_kinds_node, args);

		for (i = 0; i < num_args; ) {
			const LLVMValueRef op = args[i];
			const char* ae2f_restrict kind_str;
			unsigned len;

			unless (op) { ++i; continue; }

			kind_str = LLVMGetMDString(op, &len);
			unless(kind_str) { ++i; continue; }

			if (strncmp(kind_str, ACLSPV_ARGKND_POD_PSHCONST, sizeof(ACLSPV_ARGKND_POD_PSHCONST) - 1) == 0) {
				const unsigned cluster_start_idx = i;
				unsigned cluster_count = 0;

				assert(i < num_args);
				for (j = i; j < num_args; ++j) {
#if 1
					LLVMValueRef inner_op = args[j];
#else
					LLVMValueRef inner_op = NULL;
#endif
					const char* ae2f_restrict inner_kind_str;

					unless(inner_op) break;
					inner_kind_str  = LLVMGetMDString(inner_op, &len);
					unless(inner_kind_str) break;

					if (strncmp(inner_kind_str, ACLSPV_ARGKND_POD_PSHCONST, sizeof(ACLSPV_ARGKND_POD_PSHCONST) - 1) == 0) {
						++cluster_count;
					} else {
						break;
					}
				}

				if (cluster_count > 0) {
					LLVMValueRef cluster_ops[2], cluster_node;
					cluster_ops[0] = LLVMConstInt(LLVMInt32TypeInContext(C), cluster_start_idx, 0);
					cluster_ops[1] = LLVMConstInt(LLVMInt32TypeInContext(C), cluster_count, 0);

					cluster_node = LLVMMDNodeInContext(C, cluster_ops, 2);

					if ((size_t)(num_clusters * sizeof(LLVMValueRef)) >= clusters_capacity) {
						_aclspv_grow_vec_with_copy(
								_aclspv_malloc
								, _aclspv_free
								, _aclspv_memcpy
								, L_new, CTX->m_v0, 
								(size_t)((num_clusters << 1) * sizeof(LLVMValueRef))
								);

						unless(cluster_nodes) {
							return FN_ACLSPV_PASS_ALLOC_FAILED;
						}
					}

					assert((size_t)(num_clusters * sizeof(LLVMValueRef)) < (size_t)clusters_capacity);
					cluster_nodes[num_clusters++] = cluster_node;
				}

				i += cluster_count;
			} else {
				++i;
			}
		}

		if (num_clusters > 0) {
			LLVMValueRef all_clusters_node = LLVMMDNodeInContext(C, cluster_nodes, num_clusters);
			LLVMSetMetadata(F, pod_cluster_md_id, all_clusters_node);
		}
	}

	return FN_ACLSPV_PASS_OK;
}
