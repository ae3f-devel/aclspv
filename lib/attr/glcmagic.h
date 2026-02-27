/** 
 * @file glcmagic.h 
 * GLCompute magic (user wants auto input-thingy?)
 * */

#ifndef attr_glcmagic_h
#define attr_glcmagic_h

#include <clang-c/Index.h>
#include <ae2f/Keys.h>
#include <string.h>

/**
 * @brief	attr_glcmagic
 * @param	unsigned* ae2f_restrict wr_ismagic
 * 	Its value must be cleared as 0 when passed.
 * 	0 when the attribute is not found.
 * 	1 when the attribute is found.
 *
 * */
static enum CXChildVisitResult	attr_glcmagic(CXCursor h_cur, CXCursor h_parent, CXClientData ae2f_restrict wr_ismagic) {
	CXString TEXT;
	const char* NEEDLE;
	TEXT = clang_getCursorSpelling(h_cur);

	unless(NEEDLE = strstr(TEXT.data, "aclspv_storage_class")) {
		clang_disposeString(TEXT);
		return CXChildVisit_Recurse;
	}

	*((unsigned* ae2f_restrict)wr_ismagic) |= 1;
	clang_disposeString(TEXT);
	return CXChildVisit_Break;

	(void)h_parent;
}

#endif
