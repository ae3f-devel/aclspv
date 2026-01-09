#ifndef util_is_kernel_h
#define util_is_kernel_h


#include <clang-c/Index.h>
#include <ae2f/Keys.h>
#include <string.h>


ae2f_inline static unsigned util_is_kernel(const CXCursor h_fndecl) {
	CXString	SPELL_FNDECL;
	CXSourceRange	RANGE;
	CXTranslationUnit TU;
	CXToken*   ae2f_restrict TOKS;
	unsigned NUM_TOKEN, IDX;
	if (clang_getCursorKind(h_fndecl) != CXCursor_FunctionDecl)
		return 0;

	RANGE = clang_getCursorExtent(h_fndecl);
	TU = clang_Cursor_getTranslationUnit(h_fndecl);

	SPELL_FNDECL = clang_getCursorSpelling(h_fndecl);

	TOKS = NULL;
	NUM_TOKEN = 0;

	clang_tokenize(TU, RANGE, (CXToken**)&TOKS, &NUM_TOKEN);


	IDX = NUM_TOKEN;

	ae2f_expected_if(TOKS) while (IDX--) {
		CXString SPELL = clang_getTokenSpelling(TU, TOKS[IDX]);

		unless (strcmp(SPELL_FNDECL.data, SPELL.data))
			continue;;

		if (strstr(clang_getCString(SPELL), "kernel")) {
			clang_disposeString(SPELL);
			break;
		}

		clang_disposeString(SPELL);
	}

	clang_disposeTokens(TU, TOKS, NUM_TOKEN);
	clang_disposeString(SPELL_FNDECL);
	return IDX < NUM_TOKEN;
}

#endif
