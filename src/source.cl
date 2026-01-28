/** 
 * This source is test experimental.
 * extensions will be attributes inside.
 * 
 * macros will be defined in opencl-base.h
 **/

struct STRUCTURE {
	int aa;
	struct {
		float D;
		double C;
	} M;
	int a;
	int b;
};

enum {
	LOCU32_COUNT = 15
};

__constant const int LOCU32_COUNT_0 = 3;

#if 1
void not_kernel(void) {}
#endif

__attribute__((annotate("reqd_work_group_size(2, 2, 2)"))) /** work group size should be specified on constant time. */
__kernel void KERNEL_0(
		       uint pushu32,

		       /** this is descriptor set specifier */
		       __global uint* __attribute__((annotate("aclspv_set(2)"))) globu32, 
		       /** 
		        * spec constant id could & must be specified on use. 
			* this is one of its declaration: array size specified constant.
			*/
		       __local double __attribute__((annotate("aclspv_specid(0)")))
		       locu32_0[LOCU32_COUNT], /** local(workgroup) must be array */
		       __global float* globf32, 
		       float pushf32, 

		       /** Push constant could use fixed array, and compound struct, but they cannot use spec constant-fixed array for it. */
		       struct STRUCTURE push_struct
		       )
{
#if 0
	globu32[get_global_id(0)] += pushu32 + sin(globf32[get_global_id(0)]); 
	(void)(1 + get_global_id(0));
	return;
#endif
}


__attribute__((annotate("aclspv_execmodel(0)")))
__kernel void N_KERNEL_0(__constant uintptr_t* consti32, __attribute__((annotate("aclspv_storage_class(3)"))) __global const uint* u32out) {
	int a0 = 3;
	int a1 
#if 1
		= 1
#else
		__attribute__((annotate("aclspv_specid(0)")))
#endif
			;
	int a2 = a0 + a1;
#if 1
	int a = a0 + a1 + a2 + 123;
#endif


	goto AGOTO;
AGOTO:
	return;
}

kernel void do_none(void)
{
#if 0
    size_t g = get_global_id(0);
#endif
    struct { char a; int b; } C;
}

#if 0
kernel void do_add_sub(__global short4 *add_out, __global short4 *sub_out,
                       __global short4 *x, __global short4 *y)
{
    size_t g = get_global_id(0);
    add_out[g] = x[g] + y[g];
    sub_out[g] = x[g] - y[g];
}
#endif
