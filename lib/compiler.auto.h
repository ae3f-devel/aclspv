#undef	__ae2f_MACRO_GENERATED
#define	__ae2f_MACRO_GENERATED	1
#ifndef aclspv_compiler_auto_h
#define aclspv_compiler_auto_h

#include <aclspv/compiler.h>
#undef __ae2f_MACRO_GENERATED
#define __ae2f_MACRO_GENERATED 1
#include <ae2f/Macro.h>
#undef __ae2f_MACRO_GENERATED
#define __ae2f_MACRO_GENERATED 1

#define _aclspv_compile_unsaved_imp( \
	/** tparam */ \
		 \
 \
	/** param */ \
		/*    , const x_aclspv_compiler */ c_compiler, \
		/*     constchar*  const */ rd_srcpath, \
		/*      const structCXUnsavedFile*  ae2f_restrict */ rd_unsaved, \
		/*     const     size_t */ c_unsaved_len, \
		/*      constchar*  ae2f_restrictconst*  ae2f_restrict const */ rd_argv, \
		/*     const       size_t    */ c_argc \
)  \
{ \
	CXTranslationUnit	tu; \
 \
 \
	clang_parseTranslationUnit2( \
			(c_compiler).mh_idx \
			, rd_srcpath, rd_argv, c_argc \
			, rd_unsaved, c_unsaved_len \
			, CXTranslationUnit_None \
			, &(tu) \
			); \
 \
	CXCursor cursor = clang_getTranslationUnitCursor(tu); \
	CXModule mod = clang_Cursor_getModule(cursor); \
}

#endif

#undef	__ae2f_MACRO_GENERATED

#define	__ae2f_MACRO_GENERATED	0

