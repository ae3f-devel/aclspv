/** @file storage_class.h */

#ifndef lib_attr_storage_class_h
#define lib_attr_storage_class_h

#include <clang-c/Index.h>
#include <ae2f/Keys.h>
#include <aclspv/spvty.h>
#include <stdio.h>
#include <string.h>
#include <spirv/unified1/spirv.h>

/** @param enum SpvStorageClass_* wr_storage_class */
static enum CXChildVisitResult	attr_storage_class(CXCursor h_cur, CXCursor h_parent, CXClientData ae2f_restrict wr_storage_class) {
	CXString TEXT;
	enum CXChildVisitResult RES = CXChildVisit_Break;
	const char* NEEDLE;
	unsigned INP;

	unless(h_cur.kind == CXCursor_AnnotateAttr) return CXChildVisit_Recurse;
	TEXT = clang_getCursorSpelling(h_cur);

	unless(NEEDLE = strstr(TEXT.data, "aclspv_storage_class")) {
		RES = CXChildVisit_Continue;
		goto LBL_FINI;
	}

	sscanf(NEEDLE, "aclspv_storage_class ( %u )", &INP);

	if(INP == SpvStorageClassMax)
		*((enum SpvStorageClass_* ae2f_restrict)wr_storage_class) = (enum SpvStorageClass_)INP;

LBL_FINI:
	clang_disposeString(TEXT);
	return RES;

	(void)h_parent;
}

#endif
