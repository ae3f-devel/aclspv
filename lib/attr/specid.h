#ifndef lib_attr_specid_h
#define lib_attr_specid_h

#include <clang-c/Index.h>
#include <ae2f/Keys.h>
#include <aclspv/spvty.h>
#include <stdio.h>
#include <string.h>

static enum CXChildVisitResult	attr_specid(CXCursor h_cur, CXCursor h_parent, CXClientData ae2f_restrict wr_specid) {
	CXString TEXT;
	enum CXChildVisitResult RES = CXChildVisit_Break;
	char* NEEDLE;
	unsigned SPECID;

	unless(h_cur.kind == CXCursor_AnnotateAttr) return CXChildVisit_Recurse;
	TEXT = clang_getCursorSpelling(h_cur);

	unless(NEEDLE = strstr(TEXT.data, "aclspv_specid")) {
		RES = CXChildVisit_Continue;
		goto LBL_FINI;
	}


	sscanf(NEEDLE, "aclspv_specid ( %u )", &SPECID);

	*((aclspv_wrd_t* ae2f_restrict)wr_specid) = (aclspv_wrd_t)SPECID;

LBL_FINI:
	clang_disposeString(TEXT);
	return RES;

	(void)h_parent;
}

#endif
