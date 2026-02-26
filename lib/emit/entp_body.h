/** @file entp_body.h */

#ifndef	lib_emit_entp_body_h
#define	lib_emit_entp_body_h


#include <clang-c/Index.h>
#include <aclspv/abi.h>

/**
 * @details
 * 		CTX->m_tmp.m_v0: name maker
 * */

ACLSPV_ABI_DECL enum CXChildVisitResult aclemit_entp_body(CXCursor h_cur, CXCursor h_parent, CXClientData rdwr_data);

#endif
