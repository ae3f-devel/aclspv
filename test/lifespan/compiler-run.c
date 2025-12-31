#include <aclspv.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

char maincontent[] =
"struct STRUCTURE { int aa; struct { float D; double C; } M; int a; int b; };"
"void N_KERNEL_0();"
"__attribute__((annotate(\"reqd_work_group_size(2, 2, 2)\")))"
"__kernel void KERNEL_0(uint pushu32, __global uint* __attribute__((annotate(\"aclspv_set(2)\"))) globu32, __local uint* locu32, float pushf32, struct STRUCTURE push_struct) { }"
"__kernel void N_KERNEL_0(__constant int* consti32) {}";

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
