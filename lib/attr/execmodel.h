/** @file execmodel.h */

#ifndef	attr_execmodel_h
#define attr_execmodel_h

#include <clang-c/Index.h>
#include <ae2f/cc.h>
#include <stdio.h>
#include <string.h>
#include <spirv/unified1/spirv.h>

/** @param enum SpvExecutionModel_* ae2f_restrict wr_data */
static enum CXChildVisitResult attr_execmodel(CXCursor h_cur, CXCursor h_parent, CXClientData ae2f_restrict wr_data) {
	CXString	ATTR_KIND;
	const char*	NIDDLE;

	if(*((enum SpvExecutionModel_* ae2f_restrict)wr_data) != SpvExecutionModelMax) {
		return CXChildVisit_Break;
	}

	ATTR_KIND = clang_getCursorSpelling(h_cur);

	if((NIDDLE = strstr(ATTR_KIND.data, "aclspv_execmodel"))) {
		unsigned DATA;

		sscanf(NIDDLE
				, "aclspv_execmodel ( %u )"
				, &DATA
		      );

		*((enum SpvExecutionModel_* ae2f_restrict)wr_data) = (enum SpvExecutionModel_)DATA;

		clang_disposeString(ATTR_KIND);
		return CXChildVisit_Break;
	}

	clang_disposeString(ATTR_KIND);
	return CXChildVisit_Recurse;
	(void)h_parent;
}

#endif
