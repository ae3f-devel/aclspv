#include <stdio.h>
#include <aclspv.h>

int main(void) {
	aclspv_init_global();

	struct CXUnsavedFile	main_c;
	const char KERNEL[] =
		"__kernel void add(__constant int* a, __global int* b) {"
			"*(b) = *(a);"
		"}";
	main_c.Filename = "main.cl";
	main_c.Length = sizeof(KERNEL);
	main_c.Contents = KERNEL;

	aclspv_compile(
			"main.cl", &main_c, 1
			, ae2f_NIL, 0
			);

	aclspv_stop_global();
	return 0;
}
