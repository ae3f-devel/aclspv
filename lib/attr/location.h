/** 
 * @file	location.h 
 * @brief	location attribute detector
 * */

#ifndef lib_attr_location_h
#define lib_attr_location_h

#include <clang-c/Index.h>
#include <ae2f/Keys.h>
#include <aclspv/spvty.h>
#include <stdio.h>
#include <string.h>

/** 
 * @param aclspv_wrd_t* ae2f_restrict wr_location
 * */
static enum CXChildVisitResult	attr_location(CXCursor h_cur, CXCursor h_parent, CXClientData ae2f_restrict wr_location) {
	CXString TEXT;
	enum CXChildVisitResult RES = CXChildVisit_Break;
	const char* NEEDLE;
	unsigned LOCATION;

	if(*((aclspv_wrd_t* ae2f_restrict)wr_location) != 0xFFFFFFFF)
		return CXChildVisit_Break;

	unless(h_cur.kind == CXCursor_AnnotateAttr) return CXChildVisit_Recurse;
	TEXT = clang_getCursorSpelling(h_cur);

	unless(NEEDLE = strstr(TEXT.data, "aclspv_location")) {
		RES = CXChildVisit_Continue;
		goto LBL_FINI;
	}


	sscanf(NEEDLE, "aclspv_location ( %u )", &LOCATION);

	*((aclspv_wrd_t* ae2f_restrict)wr_location) = (aclspv_wrd_t)LOCATION;

LBL_FINI:
	clang_disposeString(TEXT);
	return RES;

	(void)h_parent;
}

#endif
