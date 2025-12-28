#include <aclspv.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define maincontent	\
	"__kernel void __kernel_name(__global int* _glob1, __global int* _glob2) {"	\
		"*(_glob1) = *(_glob2);"	\
	"}"

int main(void) {
	struct CXUnsavedFile	files[1];
	const char* ARG[] = { "main.cl" };
	aclspv_wrd_t* spirv0;
	aclspv_wrdcount_t spirv_count;

	files[0].Contents = maincontent;
	files[0].Filename = "main.cl";
	files[0].Length = sizeof(maincontent) - 1;

	aclspv_compile(
			files, 1
			, ARG, 1
			, &spirv_count, &spirv0, ae2f_NIL
			);

	assert(spirv0);

	if(spirv0) {
		FILE* const fp = fopen("result.spv", "wb");
		unless(fp) goto FPOPEN_FAILED;

		fwrite(spirv0, sizeof(aclspv_wrd_t), (size_t)spirv_count, fp);

		if(fp) fclose(fp);
FPOPEN_FAILED:
		;
	}

	free(spirv0);

	return 0;
}
