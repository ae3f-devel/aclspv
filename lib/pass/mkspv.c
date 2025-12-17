#include "./ctx.h"
#include "./md.h"
#include "./argknd.h"
#include "./scale.h"

#include <ae2f/c90/StdInt.h>
#include <ae2f/c90/StdBool.h>

#include <llvm-c/Core.h>
#include <spirv/unified1/spirv.h>
#include <assert.h>
#include <string.h>

#include <spirv-tools/libspirv.h>
#include <aclspv/spvty.h>

#include <stdio.h>

/** 
 * @def 	sz_to_count 
 * @brief	byte size to word count
 * */
#define sz_to_count(c_sz)	((size_t)(((size_t)(c_sz)) / ((size_t)sizeof(aclspv_wrd_t))))

/**
 * @def		count_to_sz
 * @brief	word count to byte size
 * */
#define count_to_sz(c_count)	((size_t)(((size_t)(c_count)) * (size_t)(sizeof(aclspv_wrd_t))))

/**
 * @def		get_wrd_of_vec
 * @brief	get word buffer from vector
 * */
#define get_wrd_of_vec(vec)	((aclspv_wrd_t* ae2f_restrict)((vec)->m_p))

#define _free(a, b)	free(a)

/** type id */
typedef enum {
	ID_DEFAULT_VOID = 1, 
	ID_DEFAULT_FN_VOID,
	ID_DEFAULT_INT32,
	ID_DEFAULT_PTR_STORAGE,

	ID_DEFAULT_END
} e_id_default;

/**
 * @fn		emit_wrd
 * @brief	try emitting `c_wrd` on `h_vec`
 * @return	new c_wrdcount available. 0 when failed allocating.
 * */
static ae2f_inline size_t emit_wrd(x_aclspv_vec* ae2f_restrict const h_wrds, const size_t c_wrdcount, const aclspv_wrd_t c_wrd)
{
	if(count_to_sz(c_wrdcount + 1) > h_wrds->m_sz)
		_aclspv_grow_vec_with_copy(
				malloc, _free, _aclspv_memcpy 
				, L_new, *h_wrds
				, count_to_sz((c_wrdcount + 1) << 1)
				);

	unless(h_wrds->m_p) return 0;
	if(count_to_sz(c_wrdcount) >= h_wrds->m_sz)
		return 0;

	get_wrd_of_vec(h_wrds)[c_wrdcount] = c_wrd;
	return c_wrdcount + 1;
}


#define	mk_noprnds(c_num_opprm)	\
	((((aclspv_wrd_t)((c_num_opprm) + 1)) << 16) & (aclspv_wrd_t)ACLSPV_MASK_NOPRNDS)

#define	opcode_to_wrd(c_opcode, c_num_opprm)	\
	((aclspv_wrd_t)(((aclspv_wrd_t)(c_opcode) & ACLSPV_MASK_OPCODE) | mk_noprnds(c_num_opprm)))
/**
 * @def		emit_opcode
 * @brief	try emit opcode with num_opprm
 * */
#define	emit_opcode(h_wrds, c_wrdcount, c_opcode, c_num_opprm_opt)	\
	(emit_wrd(h_wrds, c_wrdcount, opcode_to_wrd(c_opcode, c_num_opprm_opt)))

#define set_oprnd_count_for_opcode(cr_wrd, c_num_opprm)	\
	(cr_wrd) = opcode_to_wrd((cr_wrd & ACLSPV_MASK_OPCODE), ((aclspv_num_opprm_t)(c_num_opprm)))


typedef struct {
	aclspv_wrd_t	m_id;
	LLVMValueRef	m_fn;
} func_with_id;

/**
 * @fn		emit_str
 * @brief	emit string with word size padded.
 * @returns	0 when failed.
 * */
ae2f_inline static size_t emit_str(
		x_aclspv_vec* ae2f_restrict const h_wrds, 
		const size_t c_wrdcount, 
		const char* ae2f_restrict const rd_str
		)
{
	/** string len: null included. */
	const size_t	len = strlen(rd_str) + 1;
	const size_t	pad_wrds = (!!((len) & 3)) + ((len) >> 2);

	_aclspv_grow_vec_with_copy(malloc, _free, _aclspv_memcpy, L_new
			, *h_wrds, count_to_sz(pad_wrds + c_wrdcount));

	unless(h_wrds->m_p) return 0;
	assert(count_to_sz(pad_wrds + c_wrdcount) <= h_wrds->m_sz);

	memset(&get_wrd_of_vec(h_wrds)[c_wrdcount], 0, count_to_sz(pad_wrds));
	_aclspv_memcpy(&get_wrd_of_vec(h_wrds)[c_wrdcount], rd_str, len);

	return c_wrdcount + pad_wrds;
}

IMPL_PASS_RET aclspv_pass_mkspv(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		)
{
	size_t	ret_count = 5;
	size_t	pos_tmp0;
	unsigned	num_kernels;

	const LLVMContextRef C = LLVMGetModuleContext(M);
	const unsigned layout_md_id = C ? LLVMGetMDKindIDInContext(C, ACLSPV_MD_PIPELINE_LAYOUT,
			sizeof(ACLSPV_MD_PIPELINE_LAYOUT)-1) : 0;

	/** 
	 * @var		tynxt_id 
	 * @brief	next id
	 * @details	use `next_id++` to get new id
	 * */
	aclspv_wrd_t next_id = ID_DEFAULT_END;

	e_fn_aclspv_pass	status = FN_ACLSPV_PASS_ALLOC_FAILED;

	assert(M);
	assert(CTX);


	/** header */
	_aclspv_grow_vec(malloc, _free, CTX->m_ret, count_to_sz(5));
	unless(CTX->m_ret.m_p)
		goto LBL_FNSTATISBAD;
	unless((CTX->m_ret.m_sz) == count_to_sz(5)) {
		status = FN_ACLSPV_PASS_MET_INVAL;
		goto LBL_FNSTATISBAD;
	}
	unless(C) goto LBL_FNSTATISBAD;
	unless(layout_md_id) {
		status = FN_ACLSPV_PASS_GET_FAILED;
		goto LBL_FNSTATISBAD;
	}

	get_wrd_of_vec(&CTX->m_ret)[0] = SpvMagicNumber;
	get_wrd_of_vec(&CTX->m_ret)[1] = SpvVersion;
	get_wrd_of_vec(&CTX->m_ret)[2] = 0;
	get_wrd_of_vec(&CTX->m_ret)[3] = 0;	/** bound */
	get_wrd_of_vec(&CTX->m_ret)[4] = 0;

	unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpCapability, 1))) {
		goto LBL_FNSTATISBAD;
	}

	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvCapabilityShader))) {
		goto LBL_FNSTATISBAD;
	}

	pos_tmp0 = ret_count;
	unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpExtension, 0))) {
		goto LBL_FNSTATISBAD;
	}

	unless((ret_count = emit_str(&CTX->m_ret, ret_count, "SPV_KHR_storage_buffer_storage_class"))) {
		goto LBL_FNSTATISBAD;
	}

	set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_ret)[pos_tmp0], (aclspv_wrd_t)(ret_count - pos_tmp0 - 1));

	unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpMemoryModel, 2))) {
		goto LBL_FNSTATISBAD;
	}

	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvAddressingModelLogical))) {
		goto LBL_FNSTATISBAD;
	}

	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvMemoryModelGLSL450))) {
		goto LBL_FNSTATISBAD;
	}

	/** OpTypeVoid */
	unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpTypeVoid, 1))) 
		goto LBL_FNSTATISBAD;
	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_VOID))) 
		goto LBL_FNSTATISBAD;


	/** OpTypeInt */
	unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpTypeInt, 3))) 
		goto LBL_FNSTATISBAD;
	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_INT32))) 
		goto LBL_FNSTATISBAD;
	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 32))) 
		goto LBL_FNSTATISBAD;
	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, 1))) 
		goto LBL_FNSTATISBAD;  /* signed */

	/** OpTypeFunction %void () */
	unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpTypeFunction, 2)))
		goto LBL_FNSTATISBAD;
	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_FN_VOID)))
		goto LBL_FNSTATISBAD;
	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_VOID)))
		goto LBL_FNSTATISBAD;

	/** Storage Pointer */
	unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpTypePointer, 3))) goto LBL_FNSTATISBAD;
	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_PTR_STORAGE))) goto LBL_FNSTATISBAD;
	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvStorageClassStorageBuffer))) goto LBL_FNSTATISBAD;
	unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_INT32))) goto LBL_FNSTATISBAD;

	{
		unsigned i = 0;

		{
			LLVMValueRef	func;
			for (func = LLVMGetFirstFunction(M); func; func = LLVMGetNextFunction(func)) {
				if (LLVMGetMetadata(func, LLVMGetMDKindID(
								ACLSPV_MD_PIPELINE_LAYOUT
								, sizeof(ACLSPV_MD_PIPELINE_LAYOUT) - 1
								))) {  /* Has pipelayout? → kernel */
					i++;
				}
			}
		}

		unless (i) {
			goto LBL_NO_KRNL;
		}


		_aclspv_grow_vec(_aclspv_malloc, _aclspv_free, CTX->m_v0, (size_t)(sizeof(LLVMValueRef) * i));

#define kernel_nodes	((func_with_id* ae2f_restrict)CTX->m_v0.m_p)
#define interface_ids_mgr	get_last_scale_from_vec(&CTX->m_v1)
#define	interface_ids		((aclspv_wrd_t* ae2f_restrict)get_buf_from_scale(&CTX->m_v1, *interface_ids_mgr))

		unless (kernel_nodes && init_scale(&CTX->m_v1, 0)) {
			goto LBL_FNSTATISBAD;
		}

		assert(get_first_scale_from_vec(&CTX->m_v1) == get_last_scale_from_vec(&CTX->m_v1));

		{
			LLVMValueRef func;
			i = 0;
			for (func = LLVMGetFirstFunction(M); func; func = LLVMGetNextFunction(func)) {
				if (LLVMGetMetadata(func, LLVMGetMDKindID(
								ACLSPV_MD_PIPELINE_LAYOUT
								, sizeof(ACLSPV_MD_PIPELINE_LAYOUT) - 1))

						&& !strncmp(LLVMGetValueName(func), "__kernel_", sizeof("__kernel_") - 1)
				   )
				{
					kernel_nodes[i++].m_fn = func;
				}
			}

			num_kernels = i;
			unless(num_kernels) goto LBL_NO_KRNL;
#define	num_kernels	((const unsigned)num_kernels)
		}
		typedef struct {
			aclspv_wrd_t	var_id;
			aclspv_wrd_t	set;
			aclspv_wrd_t	binding;
			aclspv_wrd_t	arg_idx;
			LLVMValueRef	kernel_func;
		} s_var_info;

#define var_infos	((s_var_info* ae2f_restrict)CTX->m_v2.m_p)
		size_t	num_vars = 0;
		_aclspv_grow_vec(malloc, _free, CTX->m_v2, sizeof(s_var_info) * num_kernels * 4); // Initial alloc
		unless(var_infos) goto LBL_FNSTATISBAD;

		for(i = 0; i < num_kernels; i++) {
			const LLVMValueRef kernel_func = kernel_nodes[i].m_fn;
			const LLVMValueRef layout_md = LLVMGetMetadata(kernel_func, layout_md_id);
			size_t	interface_count = 0;
			int num_sets;
			int i_set;
			LLVMValueRef set_layouts_node;

			if (!layout_md || !LLVMIsAMDNode(layout_md) || LLVMGetNumOperands(layout_md) < 2) {
				goto LBL_NEXT_KRNL;
			}

			set_layouts_node = LLVMGetOperand(layout_md, 1);
			if (!set_layouts_node || !LLVMIsAMDNode(set_layouts_node)) {
				goto LBL_NEXT_KRNL;
			}

			num_sets = LLVMGetNumOperands(set_layouts_node);
			/* Iterate over descriptor sets */
			for (i_set = 0; i_set < num_sets; ++i_set) {
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

				/* Iterate over bindings */
				for (i_binding = LLVMGetNumOperands(bindings_node); i_binding-- > 0; ) {
					const LLVMValueRef binding_node = LLVMGetOperand(bindings_node, (unsigned)i_binding);
					LLVMValueRef	arg_idx_val, binding_val;
					s_var_info* ae2f_restrict	info;

					if (!binding_node || !LLVMIsAMDNode(binding_node) || LLVMGetNumOperands(binding_node) < 5) {
						continue;
					}

					if (CTX->m_v2.m_sz <= sizeof(s_var_info) * num_vars) {
						_aclspv_grow_vec_with_copy(malloc, _free, _aclspv_memcpy, L_new, CTX->m_v2, CTX->m_v2.m_sz << 1);
						unless(var_infos) goto LBL_FNSTATISBAD;
					}
					info = &var_infos[num_vars];

					arg_idx_val = LLVMGetOperand(binding_node, 0);
					binding_val = LLVMGetOperand(binding_node, 1);

					info->var_id = next_id++;
					info->arg_idx = (aclspv_wrd_t)LLVMConstIntGetZExtValue(arg_idx_val);
					info->binding = (aclspv_wrd_t)LLVMConstIntGetZExtValue(binding_val);
					info->set = set;
					info->kernel_func = kernel_func;

					/** store id */
					if(grow_last_scale(&CTX->m_v1, count_to_sz(interface_count + 1))) {
						goto LBL_FNSTATISBAD;
					}
					interface_ids[interface_count++] = info->var_id;
					num_vars++;
				}
			}

LBL_NEXT_KRNL:
			assert(get_last_scale_from_vec(&CTX->m_v1)->m_sz >= count_to_sz(interface_count));
			get_last_scale_from_vec(&CTX->m_v1)->m_sz = count_to_sz(interface_count);

			unless(mk_scale_from_vec(&CTX->m_v1, 0)) {
				goto LBL_FNSTATISBAD;
			}
		}

		/* Emit decorations and names */
		for (i = 0; i < num_vars; i++) {
			const s_var_info* ae2f_restrict const info = &var_infos[i];
#if		!defined(NDEBUG) || !NDEBUG
			char	var_name[64];
			size_t	name_pos;
#endif
			/** Decorations::Descriptorsets */
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpDecorate, 3))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->var_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvDecorationDescriptorSet))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->set))) goto LBL_FNSTATISBAD;

			/** Decorations::Bind */
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpDecorate, 3))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->var_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvDecorationBinding))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->binding))) goto LBL_FNSTATISBAD;

			/** opname */
#if		!defined(NDEBUG) || !NDEBUG
			snprintf(var_name, sizeof(var_name), "%s.arg%u", LLVMGetValueName(info->kernel_func), (uint32_t)info->arg_idx);
			name_pos = ret_count;
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpName, 0))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->var_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_str(&CTX->m_ret, ret_count, var_name))) goto LBL_FNSTATISBAD;
			set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_ret)[name_pos], ret_count - name_pos - 1);
#endif
		}

		/* Emit variables */
		for (i = 0; i < num_vars; i++) {
			const s_var_info* ae2f_restrict const info = &var_infos[i];
			/* OpVariable */
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpVariable, 3))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_PTR_STORAGE))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, info->var_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvStorageClassStorageBuffer))) 
				goto LBL_FNSTATISBAD;
		}

		for(i = 0; i < num_kernels; i++) {
			LLVMValueRef	kernel_func = kernel_nodes[i].m_fn;
			const char* ae2f_restrict	name = LLVMGetValueName(kernel_func);
			const aclspv_wrd_t	func_id	= next_id++;
			h_scale_t	scale = get_scale_from_vec(&CTX->m_v1, (size_t)i);
			size_t entry_pos = ret_count, name_pos;

			kernel_nodes[i].m_id = func_id;

			unless(scale)	{
				status = FN_ACLSPV_PASS_GET_FAILED;
				goto LBL_FNSTATISBAD;
			}
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpEntryPoint, 0))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvExecutionModelGLCompute))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, func_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_str(&CTX->m_ret, ret_count, name))) goto LBL_FNSTATISBAD;

			for (size_t j = 0; j < sz_to_count(scale->m_sz); ++j) {
				unless((ret_count = emit_wrd(&CTX->m_ret, ret_count
								, ((aclspv_wrd_t* ae2f_restrict)
									get_buf_from_scale(&CTX->m_v1, *scale))[j]))) 
					goto LBL_FNSTATISBAD;
			}

			set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_ret)[entry_pos], ret_count - entry_pos - 1);

			/* OpName for function */
#if !defined(NDEBUG) || !NDEBUG
			name_pos = ret_count;
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpName, 0))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, func_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_str(&CTX->m_ret, ret_count, name))) goto LBL_FNSTATISBAD;
			set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_ret)[name_pos], ret_count - name_pos - 1);
#endif
		}

		/* Emit function stubs */
		for (i = num_kernels; i--; ) {
			aclspv_wrd_t func_id = kernel_nodes[i].m_id; 
			aclspv_wrd_t lbl_id = next_id++;

			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpFunction, 4))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_VOID))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, func_id))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, SpvFunctionControlMaskNone))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, ID_DEFAULT_FN_VOID))) goto LBL_FNSTATISBAD;

			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpLabel, 1))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_wrd(&CTX->m_ret, ret_count, lbl_id))) goto LBL_FNSTATISBAD;

			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpReturn, 0))) goto LBL_FNSTATISBAD;
			unless((ret_count = emit_opcode(&CTX->m_ret, ret_count, SpvOpFunctionEnd, 0))) goto LBL_FNSTATISBAD;
		}
LBL_NO_KRNL:
		;
	}

	goto LBL_FNSTATISGOOD;

LBL_FNSTATISBAD:
	_aclspv_stop_vec(_free, CTX->m_ret);
	_aclspv_init_vec(CTX->m_ret);
	return status;

LBL_FNSTATISGOOD:
	assert(CTX->m_ret.m_p);
	if(CTX->m_ret.m_sz < count_to_sz(ret_count) || (ret_count) > (size_t)UINT32_MAX || !CTX->m_ret.m_p) {
		status = FN_ACLSPV_PASS_TOO_BIG;
		goto LBL_FNSTATISBAD;
	} else {
		/** bound */
		get_wrd_of_vec(&CTX->m_ret)[3] = ((aclspv_wrd_t)ret_count);
		CTX->m_ret.m_sz = count_to_sz((ret_count));
	}

	return FN_ACLSPV_PASS_OK;
}
