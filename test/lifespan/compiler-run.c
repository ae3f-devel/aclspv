#include <aclspv.h>
#include <assert.h>

#define maincontent	\
	"__kernel void __kernel_name(__global int* _glob1, __global int* _glob2) {"	\
		"*(_glob1) = *(_glob2);"	\
	"}"

int main(void) {
	struct CXUnsavedFile	files[1];
	const char* argv[] = { "-x", "cl", "-triple", "spir64-unknown-unkn" };

	h_aclspv_obj_t		obj, obj2;

	aclspv_init_global();

	files[0].Contents = maincontent;
	files[0].Filename = "main.cl";
	files[0].Length = sizeof(maincontent) - 1;

	obj = aclspv_compile(
			"main.cl"
			, files, 1
			, argv, sizeof(argv) / sizeof(argv[0])
			);

	obj2 = aclspv_compile(
			"main.cl"
			, files, 1
			, argv, sizeof(argv) / sizeof(argv[0])
			);

	assert(obj);
	assert(obj2);

	aclspv_free_obj(obj);
	aclspv_free_obj(obj2);

	aclspv_stop_global();
	return 0;
}
