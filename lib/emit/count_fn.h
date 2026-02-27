#ifndef	aclemit_count_fn_h
#define	aclemit_count_fn_h

/** @file count_fn.h */

#include <aclspv/abi.h>
#include <ae2f/Keys.h>

#include <clang-c/CXString.h>
#include <clang-c/Index.h>

ACLSPV_ABI_DECL ae2f_extern enum CXChildVisitResult aclemit_iter_entry_point(
		CXCursor h_cur, CXCursor h_parent, CXClientData h_ctx
		);

ACLSPV_ABI_DECL ae2f_extern enum CXChildVisitResult aclemit_count_fn(
		CXCursor h_cur, CXCursor h_parent, CXClientData h_ctx
		);


#endif
