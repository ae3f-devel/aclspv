/** @file decl_glob_obj.h */

#ifndef	aclemit_decl_glob_obj_h
#define aclemit_decl_glob_obj_h

#include <aclspv/abi.h>
#include <clang-c/Index.h>


ACLSPV_ABI_DECL enum CXChildVisitResult	aclemit_decl_glob_obj_visit_attr_set(
		CXCursor h_cur, CXCursor h_parent, CXClientData ae2f_restrict h_data);


ACLSPV_ABI_DECL enum CXChildVisitResult aclemit_decl_glob_obj(
		CXCursor h_cur, CXCursor h_cur_parent, CXClientData h_ctx);

#endif
