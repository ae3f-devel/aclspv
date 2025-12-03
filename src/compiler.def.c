#ifndef aclspv_compiler_auto_h
#define aclspv_compiler_auto_h

#include <aclspv/compiler.h>
#include <ae2f/Macro.h>

ae2f_MAC() aclspv_compile_unsaved_imp(
		const x_aclspv_compiler	c_compiler,
		const char* const	rd_srcpath,

		const struct CXUnsavedFile* ae2f_restrict	rd_unsaved,
		const size_t					c_unsaved_len,

		const char* ae2f_restrict const* ae2f_restrict const	rd_argv,
		const size_t		 				c_argc
		) 
{
	CXTranslationUnit	tu;


	clang_parseTranslationUnit2(
			(c_compiler).mh_idx
			, rd_srcpath, rd_argv, c_argc
			, rd_unsaved, c_unsaved_len
			, CXTranslationUnit_None
			, &(tu)
			);

	CXCursor cursor = clang_getTranslationUnitCursor(tu);
	CXModule mod = clang_Cursor_getModule(cursor);
}

#endif
