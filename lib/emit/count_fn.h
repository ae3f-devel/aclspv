#include <ae2f/Keys.h>

#include <clang-c/CXString.h>
#include <clang-c/Index.h>

#include <stdio.h>
#include <assert.h>

#include <util/ctx.h>
#include <util/emitx.h>
#include <util/is_kernel.h>

#include <ae2f/c90/StdBool.h>
#include <spirv/1.0/spirv.h>

static enum CXChildVisitResult emit_count_fn_wrdgroup_attr(CXCursor h_cur, CXCursor h_parent, CXClientData wr_data) {
	CXString	ATTR_KIND;
	const char* NIDDLE;

	ATTR_KIND = clang_getCursorSpelling(h_cur);
	if((NIDDLE = strstr(ATTR_KIND.data, "reqd_work_group_size"))) {
		sscanf(NIDDLE, "reqd_work_group_size ( %u , %u , %u )"
				, ((unsigned* ae2f_restrict)wr_data)
				, ((unsigned* ae2f_restrict)wr_data) + 1
				, ((unsigned* ae2f_restrict)wr_data) + 2
		      );

		clang_disposeString(ATTR_KIND);
		return CXChildVisit_Break;
	} else if ((NIDDLE = strstr(ATTR_KIND.data, "aclspv_wrkgroup_size"))) {
		sscanf(NIDDLE, "aclspv_wrkgroup_size ( %u , %u , %u )"
				, ((unsigned* ae2f_restrict)wr_data)
				, ((unsigned* ae2f_restrict)wr_data) + 1
				, ((unsigned* ae2f_restrict)wr_data) + 2
		      );

		clang_disposeString(ATTR_KIND);
		return CXChildVisit_Break;
	}

	clang_disposeString(ATTR_KIND);
	return CXChildVisit_Recurse;
	(void)h_parent;
}

static enum CXChildVisitResult emit_count_fn(CXCursor h_cur, CXCursor h_parent, CXClientData _h_ctx) {
#define h_ctx	((x_aclspv_ctx* ae2f_restrict)_h_ctx)
	assert(h_ctx);

	unless(h_cur.kind == CXCursor_CompoundStmt && h_parent.kind == CXCursor_FunctionDecl)
		return CXChildVisit_Recurse;

	if(util_is_kernel(h_parent)) {
		const CXString NAME = clang_getCursorSpelling(h_parent);
		unsigned	XYZ[3] = { 1, 1, 1 };
		aclspv_wrdcount_t	POS = h_ctx->m_count.m_entp;
		h_ctx->m_state = ACLSPV_COMPILE_ALLOC_FAILED;

		clang_visitChildren(h_parent, emit_count_fn_wrdgroup_attr, XYZ);

		/** OpEntryPoint */
		unless((h_ctx->m_count.m_entp = emit_opcode(
						&h_ctx->m_section.m_entp
						, h_ctx->m_count.m_entp
						, SpvOpEntryPoint, 0)))
			goto LBL_ABRT;
		unless((h_ctx->m_count.m_entp = util_emit_wrd(
						&h_ctx->m_section.m_entp
						, h_ctx->m_count.m_entp
						, SpvExecutionModelGLCompute)))
			goto LBL_ABRT;
		unless((h_ctx->m_count.m_entp = util_emit_wrd(
						&h_ctx->m_section.m_entp
						, h_ctx->m_count.m_entp
						, h_ctx->m_id + h_ctx->m_fnlist.m_num_entp)))
			goto LBL_ABRT;
		unless((h_ctx->m_count.m_entp = util_emit_str(
						&h_ctx->m_section.m_entp
						, h_ctx->m_count.m_entp
						, NAME.data))) 
			goto LBL_ABRT;

		set_oprnd_count_for_opcode(get_wrd_of_vec(&h_ctx->m_section.m_entp)[POS], h_ctx->m_count.m_entp - POS - 1);

		/** OpExecMode */
		ae2f_expected_but_else(h_ctx->m_count.m_execmode = util_emitx_6(
				&h_ctx->m_section.m_execmode
				, h_ctx->m_count.m_execmode
				, SpvOpExecutionMode
				, h_ctx->m_id + h_ctx->m_fnlist.m_num_entp
				, SpvExecutionModeLocalSize
				, XYZ[0], XYZ[1], XYZ[2]
			    )) goto LBL_ABRT;

#if !defined(NDEBUG) || !NDEBUG
		POS = h_ctx->m_count.m_name;
		unless((h_ctx->m_count.m_name = emit_opcode(&h_ctx->m_section.m_name, h_ctx->m_count.m_name, SpvOpName, 0)))			goto LBL_ABRT;
		unless((h_ctx->m_count.m_name = util_emit_wrd(&h_ctx->m_section.m_name, h_ctx->m_count.m_name, h_ctx->m_id + h_ctx->m_fnlist.m_num_entp)))				goto LBL_ABRT;
		unless((h_ctx->m_count.m_name = util_emit_str(&h_ctx->m_section.m_name, h_ctx->m_count.m_name, NAME.data)))	goto LBL_ABRT;
		set_oprnd_count_for_opcode(get_wrd_of_vec(&h_ctx->m_section.m_name)[POS], h_ctx->m_count.m_name - POS - 1);
#endif

		++h_ctx->m_fnlist.m_num_entp;
		h_ctx->m_state = ACLSPV_COMPILE_OK;
LBL_ABRT:
		clang_disposeString(NAME);
	} else {
		++h_ctx->m_fnlist.m_num_fn;
	}

	return CXChildVisit_Continue;

	return CXChildVisit_Break;



#undef	h_ctx
}
