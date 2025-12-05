#include <aclspv.h>
#include <assert.h>

#define maincontent	\
	"__kernel void __kernel_name(__global int* _glob1, __global int* _glob2) {"	\
		"*(_glob1) = *(_glob2);"	\
	"}"

int main(void) {
	struct CXUnsavedFile	files[1];

	h_aclspv_obj_t		obj;

	aclspv_init_global();

	files[0].Contents = maincontent;
	files[0].Filename = "main.cl";
	files[0].Length = sizeof(maincontent);

	obj = aclspv_compile(
			"main.cl"
			, files, 1
			, ae2f_NIL, 0
			);

	assert(obj);

	aclspv_free_obj(obj);
	aclspv_stop_global();
	return 0;
}
