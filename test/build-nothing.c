#include <assert.h>

#include <aclspv.h>
#include <aclspv/lnker.h>
#include <aclspv/pass.h>
#include <aclspv/build.h>

#include <llvm-c/BitWriter.h>

#include <stdio.h>
#include <stdlib.h>

#define content0	""

#define content1	""

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
			FILE* const file = fopen("result-build-nothing.spv", "wb");
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
