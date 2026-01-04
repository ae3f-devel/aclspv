/**
 * @brief base opencl c header this compiler supports
 * */

#ifndef ACLSPV_H
#define ACLSPV_H
#ifndef ACLSPV_PTR64
#define ACLSPV_PTR64	1
#endif
#define	ACLSPV_I(t,b)	typedef t i##b;typedef unsigned t u##b, u##t
ACLSPV_I(char,8);ACLSPV_I(short,16);ACLSPV_I(int,32);ACLSPV_I(long,64);
#undef	ACLSPV_I
#define f16	half
#define f32	float
#define	f64	double
#define	uptr	size_t
#define	iptr	ptrdiff_t
#define intptr_t	iptr
#define uintptr_t	uptr
#if	ACLSPV_PTR64
typedef	u64 uptr;typedef i64 iptr;
#else
typedef	u32 uptr;typedef i32 iptr;
#endif
#endif
