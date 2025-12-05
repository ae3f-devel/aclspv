#ifndef aclspv_compiler_auto_h
#define aclspv_compiler_auto_h

#include <ae2f/Macro.h>
#undef	ON
#undef	OFF

#include <aclspv/obj.h>

#undef	ON
#undef	OFF

#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Lex/PreprocessorOptions.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/CompilerInstance.h>

ae2f_MAC((C_vfs, C_fm, C_diagptr, C_action, )) aclspv_compile_imp(
		size_t&	 ae2f_restrict		t_sz0,

		clang::CompilerInstance&	t_cc,
		clang::DiagnosticOptions&	t_diag_opts,

		h_aclspv_obj_t&					ret,
		std::unique_ptr<llvm::LLVMContext>&		rc_ctx,

		const char* ae2f_restrict const	rd_srcpath,
		const struct CXUnsavedFile* ae2f_restrict	rd_unsaved,
		const size_t					c_unsaved_len,

		const char* ae2f_restrict const* ae2f_restrict const	rd_argv,
		const size_t		 				c_argc
		) 
{
	llvm::IntrusiveRefCntPtr<llvm::vfs::InMemoryFileSystem>
		C_vfs(new llvm::vfs::InMemoryFileSystem());

	llvm::IntrusiveRefCntPtr<clang::FileManager> C_fm(
			new clang::FileManager(clang::FileSystemOptions(), C_vfs));

	(t_cc).setFileManager((C_fm.get()));

	llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine> C_diagptr 
		= (t_cc).createDiagnostics(*(C_vfs).get(), t_diag_opts, nullptr, true);

	(t_cc).setDiagnostics(C_diagptr.get());

	clang::CompilerInvocation::CreateFromArgs(
			(t_cc).getInvocation()
			, clang::ArrayRef<const char*>((rd_argv), (rd_argv) + (c_argc))
			, (t_cc).getDiagnostics()
			);

	(t_cc).getFrontendOpts().Inputs.clear();
	for((t_sz0) = (c_unsaved_len); (t_sz0)--;) {
		C_vfs.get()->addFile(
				(rd_unsaved)->Filename
				, 0, (llvm::MemoryBuffer::getMemBuffer(
					clang::StringRef(
						(rd_unsaved)->Contents
						, (rd_unsaved->Length) - !((rd_unsaved)->Contents[(rd_unsaved)->Length - 1])
						)
					))
				);
		(t_cc).getFrontendOpts().Inputs.emplace_back((rd_unsaved)->Filename, clang::Language::OpenCL);
	}

	(t_cc).createSourceManager(*C_fm.get());

	std::unique_ptr<clang::EmitLLVMAction>	C_action(new clang::EmitLLVMAction(rc_ctx.get()));
	if((t_cc).ExecuteAction(*C_action)) {
		/** succesful */
		(ret) = new x_aclspv_obj(
				std::move(rc_ctx)
				, std::move(C_action)
				);
	} else {
		assert(!"ExecuteAction has failed");
		(ret) = ae2f_NIL;
	}
}

#endif
