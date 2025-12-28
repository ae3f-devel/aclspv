#include <assert.h>

#include <aclspv.h>
#include <aclspv/lnker.h>
#include <aclspv/pass.h>
#include <aclspv/build.h>

#include <llvm-c/BitWriter.h>

#include <stdio.h>
#include <stdlib.h>

#define content0	\
	"union pushconstant_union_lol { int a; float b; };"		\
	"__kernel void __kernel_name_0(__global int* _glob1, __local int* _loc, union pushconstant_union_lol lol) {"	\
		"*(_glob1) = *(_loc);"	\
	"}"				\
	"__kernel void __kernel_name_1(__local int* out, __constant int* asdf, int asdf_push, double asdf_push1, __global const int* dd) {"	\
			"*out = *(asdf) + *(dd);"									\
	"}"

#define content1	\
	"double sin(double);\n"	\
	"int returnthree(void) { return 3; }\n" \
	"#if 1\n"						\
	"unsigned int get_global_id(unsigned int dimindx);\n"	\
	"unsigned int get_local_id(unsigned int dimindx);\n"	\
	"__kernel void __kernel_name_2(__global float* f, __global int* _glob1, __global int* _glob2, const int _pushconstant, const float _pushconstant2) {"	\
		"(_glob1)[get_local_id(0)] = (_glob2)[get_global_id(0)] + _pushconstant;\n"	\
		"*f = sin(3);"										\
	"}\n"	\
	"#endif\n"						\

#define ZERO(a) 0

int main(void) {
	struct CXUnsavedFile	files[2];

	h_aclspv_obj_t		obj[2];
	x_aclspv_lnker		lnk;


	aclspv_init_global();
	if(aclspv_init_lnker(&lnk)) {
		assert(ZERO("aclspv_init_lnker"));
	}

	files[0].Contents = content0;
	files[0].Filename = "main.cl";
	files[0].Length = sizeof(content0) - 1;

	files[1].Contents = content1;
	files[1].Filename = "_main.cl";
	files[1].Length = sizeof(content1) - 1;

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

	if(aclspv_add_obj_to_lnker(&lnk, obj, 2)) {
		assert(ZERO("aclspv_add_obj_to_lnker"));
	}

	{
		e_fn_aclspv_pass	e_pass;
		e_aclspv_passes		e_wh;
		uintptr_t		count;
		uint32_t* ae2f_restrict	const spv = aclspv_build(
				&lnk
				, &e_pass, &e_wh
				, &count
				);

		printf("%p %u %u %lu\n", (void*)spv, e_pass, e_wh, count);

		if(spv) {
			FILE* const file = fopen("result-build.spv", "wb");
			fwrite(spv, sizeof(uint32_t), (size_t)(count), file);
			fclose(file);
			free(spv);
		}
		else {
			perror("aclspv_build has failed.");
			assert(!(const char*)("ASSERT: aclspv_build has failed"));
		}
	}

	aclspv_stop_lnker(&lnk);

	aclspv_free_obj(obj[0]);
	aclspv_free_obj(obj[1]);

	aclspv_stop_global();
	return 0;
}
