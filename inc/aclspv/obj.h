/** 
 * @file obj.h 
 * @brief object which is compilerd.
 * */

#ifndef aclspv_obj_h
#define aclspv_obj_h

#include <llvm-c/Core.h>
#include "./abi.h"
#include <ae2f/Keys.h>
#include <ae2f/LangVer.h>

#undef ON
#undef OFF

/** 
 * @struct	x_aclspv_obj
 * @brief	a structure which contains obj's data, not available on C
 * */
typedef struct x_aclspv_obj x_aclspv_obj;

/**
 * @def		ACLSPV_OBJ_DATA_LAYOUT_DEFAULT
 * @brief	default data layout for spir-vulkan
 * */
#define ACLSPV_OBJ_DATA_LAYOUT_DEFAULT						\
			"e-p:64:64-p1:64:64-p2:64:64-p3:64:64-p4:64:64-"	\
			"i64:64-i128:128-n8:16:32:64"

#if ae2f_stdcc_v
#include <llvm/IR/Module.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <memory>
#include <assert.h>

struct x_aclspv_obj {
	std::unique_ptr<llvm::LLVMContext>	m_modctx;
	std::unique_ptr<clang::EmitLLVMAction>	m_act;
	std::unique_ptr<llvm::Module>		m_module;
	inline x_aclspv_obj(
			std::unique_ptr<llvm::LLVMContext>&& s_modctx,
			std::unique_ptr<clang::EmitLLVMAction>&& s_act
			) : 
		m_modctx(std::move(s_modctx)),
		m_act(std::move(s_act)),
		m_module(this->m_act ? std::move(this->m_act->takeModule()) : ae2f_NIL)
	{
		/** validations */
		assert(this->m_modctx);
		assert(this->m_act);
		assert(this->m_module);

		LLVMSetDataLayout(
				ae2f_reinterpret_cast(LLVMModuleRef, (this)->m_module.get())
				, ACLSPV_OBJ_DATA_LAYOUT_DEFAULT
				);
	}

	inline ~x_aclspv_obj() {
		m_module.reset();
		m_act.reset();
		m_modctx.reset();
	}
};

#endif

/** 
 * @class	h_aclspv_obj_t
 * @brief	an object compiled by a `x_aclspv_compiler`.
 * */
typedef x_aclspv_obj* h_aclspv_obj_t;

/**
 * @method	aclspv_get_module_from_obj
 * @memberof	h_aclspv_obj_t
 * @brief	get `LLVMModuleRef` from an object
 * @param	h_module	<HANDLE>	an object
 * */
ae2f_extern ACLSPV_ABI_DECL ae2f_ccconst
LLVMModuleRef	aclspv_get_module_from_obj(x_aclspv_obj* ae2f_restrict const	h_obj);

/** 
 * @method	aclspv_obj_stop 
 * @memberof	h_aclspv_obj_t
 * @brief	stop the life span of `hs`.
 * @param	hs	<HANDLE> <STOP>
 * */
ae2f_extern ACLSPV_ABI_DECL
void	aclspv_free_obj(const h_aclspv_obj_t hs);

#endif
