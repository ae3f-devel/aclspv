
#include <ae2f/Keys.h>
#include <clang-c/Index.h>
#include <assert.h>

#include <util/ctx.h>

ae2f_inline static enum CXChildVisitResult iter_root(CXCursor h_cur, CXCursor h_cur_parent, CXClientData _h_ctx) {
#define h_ctx	((x_aclspv_ctx* ae2f_restrict)_h_ctx)
	assert(h_ctx); assert(h_cur.kind); assert(h_cur_parent.kind);
#undef	h_ctx
	return CXChildVisit_Recurse;
}
