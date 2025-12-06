#include <aclspv.h>

#if	ae2f_stdcc_v < 201100L
#error	"Following source must be compiled with at least C++11"
#endif

#include "./compiler-args.h"
#include "./compiler.auto.h"

ae2f_extern ACLSPV_ABI_IMPL ae2f_retnew
h_aclspv_obj_t	aclspv_compile(

		const char* ae2f_restrict const	rd_srcpath,
		const  struct CXUnsavedFile* const ae2f_restrict	rd_unsaved,
		const size_t						c_unsaved_len,
		const char* ae2f_restrict const * ae2f_restrict const	rd_argv_opt,
		const size_t						c_argc
		)
{
	size_t	 			t_sz0;

	clang::CompilerInstance	t_cc;
	clang::DiagnosticOptions	t_diag_opts;
	h_aclspv_obj_t					ret;
	std::unique_ptr<llvm::LLVMContext>		rc_ctx(new llvm::LLVMContext());


	if(rd_argv_opt && c_argc) {
		std::vector<const char*>
			rd_argv(compiler_argc + c_argc + 1);

		rd_argv[0] = compiler_clang;
		rd_argv[1] = rd_srcpath;
		rd_argv[2] = compiler_arg0;
		rd_argv[3] = compiler_arg1;
		rd_argv[4] = compiler_arg2;
		rd_argv[5] = compiler_arg3;

		memcpy(rd_argv.data() + compiler_argc + 1, rd_argv_opt, c_argc * sizeof(void*));

		_aclspv_compile_imp(
				C_vfs, C_fm, C_diagptr, C_action
				, t_sz0, t_cc, t_diag_opts 
				, ret, rc_ctx 
				, rd_unsaved, c_unsaved_len
				, (rd_argv.data()), c_argc + 1 + compiler_argc
				);
	} else {
		const char* rd_argv[compiler_argc + 1] = {
			compiler_clang,
			rd_srcpath,
			compiler_arg0,
			compiler_arg1,
			compiler_arg2,
			compiler_arg3
		};

		_aclspv_compile_imp(
				C_vfs, C_fm, C_diagptr, C_action
				, t_sz0, t_cc, t_diag_opts
				, ret, rc_ctx 
				, rd_unsaved, c_unsaved_len
				, rd_argv, sizeof(rd_argv) / sizeof(rd_argv[0])
				);
	}

	assert(ret);
	assert(ret->m_act);
	assert(ret->m_modctx);

	return ret;
}
