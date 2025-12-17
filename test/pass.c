#include <assert.h>

#include <aclspv.h>
#include <aclspv/lnker.h>
#include <aclspv/pass.h>

#include <llvm-c/BitWriter.h>

#include <stdio.h>

#define content0	\
	"__kernel void __kernel_name_0(__global int* _glob1, __global int* _glob2) {"	\
		"*(_glob1) = *(_glob2);"	\
	"}"

#define content1	\
	"double sin(double);\n"	\
	"unsigned int get_global_id(unsigned int dimindx);\n"	\
	"unsigned int get_local_id(unsigned int dimindx);\n"	\
	"__kernel void __kernel_name_1(__global float* f, __global int* _glob1, __global int* _glob2, const int _pushconstant) {"	\
		"(_glob1)[get_local_id(0)] = (_glob2)[get_global_id(0)] + _pushconstant;\n"	\
		"*f = sin(3);"										\
	"}"

#define ZERO(a) 0

int main(void) {
	struct CXUnsavedFile	files[2];

	h_aclspv_obj_t		obj[2];
	x_aclspv_lnker		lnk;

	const char* args[] = {
		"-std=CL1.2"
	};

	(void)args;


	aclspv_init_global();
	if(aclspv_init_lnker(&lnk)) {
		assert(ZERO("aclspv_init_lnker"));
	}

	files[0].Contents = content0;
	files[0].Filename = "main.cl";
	files[0].Length = sizeof(content0);

	files[1].Contents = content1;
	files[1].Filename = "_main.cl";
	files[1].Length = sizeof(content1);

	obj[0] = aclspv_compile(
			"main.cl"
			, files, 1
			, args, 1
			);

	obj[1] = aclspv_compile(
			"_main.cl"
			, files + 1, 1
			, args, 1
			);

	assert(obj[0]);
	assert(obj[1]);

#if 1
	if(aclspv_add_obj_to_lnker(&lnk, obj, 2)) {
		assert(ZERO("aclspv_add_obj_to_lnker"));
	}
#endif

	do {
		char*  final_llvm = LLVMPrintModuleToString(lnk.m_module);
		puts("OLD LLVM OUTPUT START\n");
		puts(final_llvm);
		puts("\nOLD LLVM OUTPUT END");
		LLVMDisposeMessage(final_llvm);

	} while(0);


	LLVMWriteBitcodeToFile(lnk.m_module, "NPassed.bc");

#if 1
	do {
		e_aclspv_passes		pass_progress;
		e_fn_aclspv_pass	pass_result;
		pass_progress = aclspv_runall_module_passes(
				lnk.m_module
				, ae2f_NIL
				, &pass_result
				);

		printf(
				"pass_progress: %u, pass_result: %u\n"
				, pass_progress
				, pass_result
		      );

		assert(!pass_progress);
		assert(!pass_result);
	} while(0);
#endif

	LLVMWriteBitcodeToFile(lnk.m_module, "Passed.bc");

	do {
		char*  final_llvm = LLVMPrintModuleToString(lnk.m_module);
		puts("\nNEW LLVM OUTPUT START\n");
		puts(final_llvm);
		puts("\nNEW LLVM OUTPUT END");
		LLVMDisposeMessage(final_llvm);
	} while(0);

	aclspv_stop_lnker(&lnk);

#if 1
	aclspv_free_obj(obj[0]);
	aclspv_free_obj(obj[1]);
#endif

	aclspv_stop_global();
	return 0;
}
