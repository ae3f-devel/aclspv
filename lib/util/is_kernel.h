#ifndef util_is_kernel_h
#define util_is_kernel_h


#include <clang-c/Index.h>
#include <ae2f/Keys.h>
#include <string.h>


ae2f_inline static unsigned util_is_kernel(const CXCursor h_fndecl) {
	CXSourceRange	RANGE;
	CXTranslationUnit TU;
	CXToken*   ae2f_restrict TOKS;
	unsigned NUM_TOKEN, IDX;
	if (clang_getCursorKind(h_fndecl) != CXCursor_FunctionDecl)
		return 0;

	RANGE = clang_getCursorExtent(h_fndecl);
	TU = clang_Cursor_getTranslationUnit(h_fndecl);

	TOKS = NULL;
	NUM_TOKEN = 0;

	clang_tokenize(TU, RANGE, (CXToken**)&TOKS, &NUM_TOKEN);


	IDX = NUM_TOKEN;

	while (IDX--) {
		CXString spelling = clang_getTokenSpelling(TU, TOKS[IDX]);
		if (strstr(clang_getCString(spelling), "__kernel")) {
			clang_disposeString(spelling);
			break;
		}


		clang_disposeString(spelling);
	}

	clang_disposeTokens(TU, TOKS, NUM_TOKEN);
	return IDX < NUM_TOKEN;
}

#endif
