#include <assert.h>

#include <aclspv.h>
#include <aclspv/lnker.h>
#include <aclspv/pass.h>

#include <stdio.h>

#define content0	\
	"__kernel void __kernel_name_0(__global int* _glob1, __global int* _glob2) {"	\
		"*(_glob1) = *(_glob2);"	\
	"}"

#define content1	\
	"__kernel void __kernel_name_1(__global int* _glob1, __global int* _glob2) {"	\
		"*(_glob1) = *(_glob2);"	\
	"}"

#define ZERO(a) 0

int main(void) {
	struct CXUnsavedFile	files[2];

	h_aclspv_obj_t		obj[2];
	x_aclspv_lnker		lnk;

	size_t			pass_progress;
	e_fn_aclspv_pass	pass_result;

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
			, ae2f_NIL, 0
			);

	obj[1] = aclspv_compile(
			"_main.cl"
			, files + 1, 1
			, ae2f_NIL, 0
			);

	assert(obj[0]);
	assert(obj[1]);

#if 1
	if(aclspv_add_obj_to_lnker(&lnk, obj, 2)) {
		assert(ZERO("aclspv_add_obj_to_lnker"));
	}
#endif

	pass_progress = aclspv_runall_module_passes(
			lnk.m_module
			, &pass_result
			);

	printf(
			"pass_progress: %lu, pass_result: %u\n"
			, pass_progress
			, pass_result
			);

	aclspv_stop_lnker(&lnk);

#if 1
	aclspv_free_obj(obj[0]);
	aclspv_free_obj(obj[1]);
#endif

	aclspv_stop_global();
	return 0;
}
