/**
 * @file ocl_bltin_lower.c
 * @brief Lowers OpenCL builtins to LLVM intrinsics / SPIR-V equivalents
 */

#include "./ctx.h"
#include "./bltin_lowered.h"		/* g_aclspv_ocl_bltin_map */
#include <llvm-c/Core.h>
#include <llvm-c/Target.h>		/* for LLVMGetIntrinsicID */
#include <llvm-c/Types.h>
#include <llvm-c/DataTypes.h>
#include <llvm-c/DebugInfo.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define	PREFIX	ACLSPV_PREFIX_BLTIN

#define ZERO(a)	0


#define __IntrinsicBltIn_if(c_tar)	unless(strcmp(name, PREFIX c_tar))

#define __IntrinsicBltIn1(c_llvm_intrinsic_name, L_num, L_Fn, L_Ty)	\
{									\
	LLVMValueRef	L_num = LLVMGetOperand(I, 0);			\
	const LLVMValueRef	L_Fn = LLVMGetIntrinsicDeclaration(	\
			M, LLVMLookupIntrinsicID(			\
				c_llvm_intrinsic_name			\
				, sizeof(c_llvm_intrinsic_name) - 1	\
				)					\
				, ae2f_NIL, 0);				\
	const LLVMTypeRef L_Ty = LLVMGlobalGetValueType(L_Fn);		\
	assert(L_Fn);							\
	assert(LLVMGetTypeKind(L_Ty) == LLVMFunctionTypeKind);		\
	replacement = LLVMBuildCall2(builder, L_Ty, L_Fn		\
			, &L_num, 1, "aclspv." c_llvm_intrinsic_name	\
			);						\
}

#define __IntrinsicBltIn2(c_llvm_intrinsic_name, L_nums, L_Fn, L_Ty)	\
{									\
	LLVMValueRef	L_nums[2];					\
	const LLVMValueRef	L_Fn = LLVMGetIntrinsicDeclaration(	\
			M, LLVMLookupIntrinsicID(			\
				c_llvm_intrinsic_name			\
				, sizeof(c_llvm_intrinsic_name) - 1	\
				)					\
				, ae2f_NIL, 0);				\
	const LLVMTypeRef L_Ty = LLVMGlobalGetValueType(L_Fn);		\
	L_nums[0] = LLVMGetOperand(I, 0);				\
	L_nums[1] = LLVMGetOperand(I, 1);				\
	assert(LLVMGetTypeKind(L_Ty) == LLVMFunctionTypeKind);		\
	replacement = LLVMBuildCall2(builder, L_Ty, L_Fn		\
			, L_nums, 2, "aclspv." c_llvm_intrinsic_name	\
			);						\
}

IMPL_PASS_RET aclspv_pass_ocl_bltin_lower(
		const LLVMModuleRef M,
		const h_aclspv_pass_ctx CTX
		)
{
	LLVMBuilderRef builder = LLVMCreateBuilder();
	LLVMContextRef C = LLVMGetModuleContext(M);
	LLVMValueRef F;

	/* Precompute some types */
	const LLVMTypeRef i32_ty = LLVMInt32TypeInContext(C);
	const LLVMTypeRef void_ty = LLVMVoidTypeInContext(C);


	IGNORE(CTX);
	IGNORE(void_ty);

	for (F = LLVMGetFirstFunction(M); F; F = LLVMGetNextFunction(F)) {
		LLVMBasicBlockRef BB;
		if (LLVMCountBasicBlocks(F) == 0) continue;  /* Skip declarations */

		for (BB = LLVMGetFirstBasicBlock(F); BB; BB = LLVMGetNextBasicBlock(BB)) {
			LLVMValueRef I = LLVMGetFirstInstruction(BB);

			while (I) {
				LLVMValueRef next_I = LLVMGetNextInstruction(I);
				LLVMValueRef			callee;
				const char* ae2f_restrict	name;
				LLVMValueRef replacement = NULL;

				unless (LLVMIsACallInst(I)) {
					goto LOOP_CONTINUE;
				}

				callee = LLVMGetCalledValue(I);
				unless (callee && LLVMIsAFunction(callee)) {
					goto LOOP_CONTINUE;
				}

				name = LLVMGetValueName(callee);
				unless (name && name[0]) {
					goto LOOP_CONTINUE;
				}

				if (strncmp(name, PREFIX, sizeof(PREFIX)-1)) {
					goto LOOP_CONTINUE;
				}

				/* Search in your map */

				LLVMPositionBuilderBefore(builder, I);


				/* Work-item builtins → SPIR-V BuiltIn */
				unless (
						strcmp(name, PREFIX "get_global_id")		&&
						strcmp(name, PREFIX "get_local_id")		&&
						strcmp(name, PREFIX "get_group_id")		&&
						strcmp(name, PREFIX "get_global_size")		&&
						strcmp(name, PREFIX "get_local_size")		&&
						strcmp(name, PREFIX "get_num_groups")		&&
						strcmp(name, PREFIX "get_global_offset")	&&
						strcmp(name, PREFIX "get_work_dim")
				       ) 
				{
					LLVMTypeRef			i32_prmty = i32_ty;
					const LLVMTypeRef		func_ty = LLVMFunctionType(i32_ty, &i32_prmty, 1, 0);
					LLVMValueRef			func;
					LLVMValueRef			prm = LLVMGetOperand(I, 0);

#define					uef	else unless
#define					uif	unless

#define	__WorkGroupRetU32PrmU32(bltin_name)	\
					func = LLVMAddFunction(M, bltin_name, func_ty);	\
					replacement = LLVMBuildCall2(builder, func_ty, func, &prm, 1, "aclspv." bltin_name);

					uif(strcmp(name, PREFIX "get_global_id")) {
						__WorkGroupRetU32PrmU32("llvm.spv.thread.id.i32");
					}
					uef(strcmp(name, PREFIX "get_local_id")) {
						__WorkGroupRetU32PrmU32("llvm.spv.thread.id.in.group.i32");
					}
					uef (strcmp(name, PREFIX "get_group_id"))	{
						__WorkGroupRetU32PrmU32("llvm.spv.group.id.i32");
					}
					uef (strcmp(name, PREFIX "get_global_size"))	{
						__WorkGroupRetU32PrmU32("llvm.spv.global.size.i32");
					}
					uef (strcmp(name, PREFIX "get_local_size"))	{
						__WorkGroupRetU32PrmU32("llvm.spv.workgroup.size.i32");
					}
					uef (strcmp(name, PREFIX "get_num_groups"))	{
						__WorkGroupRetU32PrmU32("llvm.spv.num.workgroups.i32");
					}
					uef (strcmp(name, PREFIX "get_global_offset"))	{
						__WorkGroupRetU32PrmU32("llvm.spv.global.offset.i32");
					}
					uef (strcmp(name, PREFIX "get_work_dim"))	{
						return FN_ACLSPV_PASS_NO_SUPPORT;
					}

#undef					uef
#undef					uif


				}
				/* Barrier and fences */
				else if (strcmp(name, PREFIX "barrier") == 0 ||
						strcmp(name, PREFIX "mem_fence") == 0 ||
						strcmp(name, PREFIX "read_mem_fence") == 0 ||
						strcmp(name, PREFIX "write_mem_fence") == 0) {
					const LLVMValueRef flags = LLVMGetOperand(I, 0);

					/* Decode OpenCL flags: CLK_LOCAL_MEM_FENCE = 1, CLK_GLOBAL_MEM_FENCE = 2 */
					const LLVMValueRef local_flag  = LLVMConstInt(i32_ty, 1, 0);
					const LLVMValueRef global_flag = LLVMConstInt(i32_ty, 2, 0);

					const LLVMValueRef has_local  = LLVMBuildICmp(builder, LLVMIntNE,
							LLVMBuildAnd(builder, flags, local_flag, ""), 
							LLVMConstNull(i32_ty), "");
					const LLVMValueRef has_global = LLVMBuildICmp(builder, LLVMIntNE,
							LLVMBuildAnd(builder, flags, global_flag, ""), 
							LLVMConstNull(i32_ty), "");

					/* SPIR-V Scope constants */
					const LLVMValueRef scope_device     = LLVMConstInt(i32_ty, 1, 0);  /* Device */
					const LLVMValueRef scope_workgroup  = LLVMConstInt(i32_ty, 2, 0);  /* Workgroup */

					/* SPIR-V MemorySemantics constants */
					const LLVMValueRef sem_none            = LLVMConstInt(i32_ty, 0,   0);
					const LLVMValueRef sem_acquire_release = LLVMConstInt(i32_ty, 10,  0);  /* Acquire | Release = 0xA */
					const LLVMValueRef sem_seq_cst         = LLVMConstInt(i32_ty, 16,  0);  /* SequentiallyConsistent = 0x10 */

					/* Determine execution scope (control barrier always uses Workgroup for sync) */
					LLVMValueRef exec_scope = scope_workgroup;

					/* Determine memory scope and semantics */
					LLVMValueRef mem_scope = scope_device;  /* default to Device for global */
					LLVMValueRef mem_sem   = sem_acquire_release;

					IGNORE(sem_none);
					IGNORE(sem_seq_cst);

					/* Optimize scope only if the flags argument is a constant */
					if (LLVMIsAConstantInt(flags)) {
						const uintmax_t flag_val = LLVMConstIntGetZExtValue(flags);

						if (flag_val == 1) {  /* CLK_LOCAL_MEM_FENCE only */
							mem_scope = scope_workgroup;
						}
					}

					/* Emit __spirv_MemoryBarrier(mem_scope, mem_sem) if any memory fence */
					if (strcmp(name, PREFIX "mem_fence") == 0 ||
							strcmp(name, PREFIX "read_mem_fence") == 0 ||
							strcmp(name, PREFIX "write_mem_fence") == 0 ||
							(strcmp(name, PREFIX "barrier") == 0 && 
							 (LLVMIsConstant(has_local) || LLVMIsConstant(has_global)))) {

						LLVMTypeRef mb_params[2], mb_ty;
						LLVMValueRef mb_func, mb_args[2];
						mb_params[0] = i32_ty;
						mb_params[1] = i32_ty;
						mb_ty = LLVMFunctionType(void_ty, mb_params, 2, 0);
						mb_func = LLVMAddFunction(M, "__spirv_MemoryBarrier", mb_ty);
						mb_args[0] = mem_scope;
						mb_args[1] = mem_sem;

						LLVMBuildCall2(builder, mb_ty, mb_func, mb_args, 2, "");
					}

					/* Emit __spirv_ControlBarrier only for actual barrier() (needs control sync) */
					if (strcmp(name, PREFIX "barrier") == 0) {
						LLVMTypeRef	cb_params[3];
						LLVMValueRef	cb_args[3];
						LLVMTypeRef	cb_ty;
						LLVMValueRef	cb_func;
						cb_params[0] = i32_ty;
						cb_params[1] = i32_ty;
						cb_params[2] = i32_ty;

						cb_ty = LLVMFunctionType(void_ty, cb_params, 3, 0);
						cb_func = LLVMAddFunction(M, "__spirv_ControlBarrier", cb_ty);

						cb_args[0] = exec_scope;
						cb_args[1] = mem_scope;
						cb_args[2] = mem_sem;

						LLVMBuildCall2(builder, cb_ty, cb_func, cb_args, 3, "");
					}

					replacement = LLVMConstNull(void_ty);
				}
				/* Atomics → LLVM atomic intrinsics */
				else if (strstr(name, PREFIX "atomic_") == name) {
					const char* op = name + strlen(PREFIX "atomic_");
					LLVMAtomicRMWBinOp atomic_op;
					LLVMValueRef	ptr, val;

					if (strcmp(op, "add") == 0)		atomic_op = LLVMAtomicRMWBinOpAdd;
					else if (strcmp(op, "sub") == 0)	atomic_op = LLVMAtomicRMWBinOpSub;
					else if (strcmp(op, "xchg") == 0)	atomic_op = LLVMAtomicRMWBinOpXchg;
					else if (strcmp(op, "inc") == 0) {
						/* inc = add 1 */
						atomic_op = LLVMAtomicRMWBinOpAdd; 
						val = LLVMConstInt(i32_ty, 1, 0);
						goto ATOM_REP_AUTO;
					}
					else if (strcmp(op, "dec") == 0) {
						/* dec = sub 1 */
						atomic_op = LLVMAtomicRMWBinOpSub; 
						val = LLVMConstInt(i32_ty, 1, 0);
						goto ATOM_REP_AUTO;
					}
					else if (strcmp(op, "min") == 0)	atomic_op = LLVMAtomicRMWBinOpMin;
					else if (strcmp(op, "max") == 0)	atomic_op = LLVMAtomicRMWBinOpMax;
					else if (strcmp(op, "umin") == 0)	atomic_op = LLVMAtomicRMWBinOpUMin;
					else if (strcmp(op, "umax") == 0)	atomic_op = LLVMAtomicRMWBinOpUMax;
					else if (strcmp(op, "and") == 0)	atomic_op = LLVMAtomicRMWBinOpAnd;
					else if (strcmp(op, "or") == 0)		atomic_op = LLVMAtomicRMWBinOpOr;
					else if (strcmp(op, "xor") == 0)	atomic_op = LLVMAtomicRMWBinOpXor;
					else goto LOOP_CONTINUE;

					val = LLVMGetOperand(I, 1);

ATOM_REP_AUTO:
					ptr = LLVMGetOperand(I, 0);
					replacement = LLVMBuildAtomicRMW(
							builder, atomic_op, ptr, val
							, LLVMAtomicOrderingSequentiallyConsistent
							, LLVMAtomicOrderingNotAtomic);
				}

				/* Math builtins → LLVM intrinsics */

				else __IntrinsicBltIn_if("sin") { __IntrinsicBltIn1("llvm.sin", num, Fn, Ty); }
				else __IntrinsicBltIn_if("cos") { __IntrinsicBltIn1("llvm.cos", num, Fn, Ty); }
				else __IntrinsicBltIn_if("fabs") { __IntrinsicBltIn1("llvm.fabs", num, Fn, Ty); }
				else __IntrinsicBltIn_if("sqrt") { __IntrinsicBltIn1("llvm.sqrt", num, Fn, Ty); }
				else __IntrinsicBltIn_if("log") { __IntrinsicBltIn1("llvm.log", num, Fn, Ty); }
				else __IntrinsicBltIn_if("exp") { __IntrinsicBltIn2("llvm.exp", num, Fn, Ty); }
				else __IntrinsicBltIn_if("pow") { __IntrinsicBltIn2("llvm.pow", num, Fn, Ty); }
				/* ... add more math as needed */


LOOP_CONTINUE:
				if (replacement) {
					LLVMReplaceAllUsesWith(I, replacement);
					LLVMInstructionEraseFromParent(I);
					replacement = ae2f_NIL;
				}

				I = next_I;
			}
		}
	}

	LLVMDisposeBuilder(builder);
	return FN_ACLSPV_PASS_OK;
}
