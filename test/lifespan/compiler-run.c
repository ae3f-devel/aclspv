#include <aclspv.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "./source.auto.h"
#include "./ocl.auto.h"

	char maincontent[
sizeof(A_0)
	+ sizeof(A_1) - 1
	+ sizeof(A_2) - 1
	+ sizeof(A_3) - 1
	+ sizeof(A_4) - 1
	/* + sizeof(A_5) -1 */
	];
	char oclhead[
sizeof(B_0) 
	+ sizeof(B_1) - 1
	];
#if 1
	int main(void) {
		struct CXUnsavedFile	files[2];
		const char* ARG[] = { "main.cl" };
		aclspv_wrd_t* spirv0;
		aclspv_wrdcount_t spirv_count;

		strcpy(maincontent, A_0);
		strcat(maincontent, A_1);
		strcat(maincontent, A_2);
		strcat(maincontent, A_3);
		strcat(maincontent, A_4);
		/** strcat(maincontent, A_5); */

		strcpy(oclhead, B_0);
		strcat(oclhead, B_1);

		files[0].Contents = maincontent;
		files[0].Filename = "main.cl";
		files[0].Length = sizeof(maincontent) - 1;

		files[1].Contents = oclhead;
		files[1].Filename = "./opencl-c-base.h";
		files[1].Length = sizeof(oclhead) - 1;

		aclspv_compile(
				files, 2
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
#else

#endif
