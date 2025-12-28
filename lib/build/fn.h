#ifndef lib_build_fn_h
#define lib_build_fn_h

#include <aclspv/spvty.h>
#include <llvm-c/Core.h>

typedef struct {
	/* @brief Result ID for function */
	aclspv_id_t       m_id;
	/* @brief OpTypeFunction ID */
	aclspv_id_t       m_type_id;

	/* @brief LLVM function reference */
	LLVMValueRef      m_fn;
} lib_build_fn_t;

#endif
