#include "./count_fn.h"


#include <attr/storage_class.h>
#include <attr/execmodel.h>

#include <assert.h>

#include <util/ctx.h>
#include <util/emitx.h>
#include <util/is_kernel.h>
#include <util/fn.h>
#include <util/entp.h>

#include <spirv/unified1/spirv.h>

static enum CXChildVisitResult __attr_workgroup(CXCursor h_cur, CXCursor h_parent, CXClientData wr_data) {
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
	return CXChildVisit_Continue;
	(void)h_parent;
}



ACLSPV_ABI_IMPL enum CXChildVisitResult aclemit_iter_entry_point(CXCursor h_cur, CXCursor h_parent, CXClientData CTX) {
#define CTX	((x_aclspv_ctx* ae2f_restrict)CTX)
	unless(h_cur.kind == CXCursor_CompoundStmt && h_parent.kind == CXCursor_FunctionDecl)
		return CXChildVisit_Recurse;

	if(util_is_kernel(h_parent)) {
		const CXString NAME = clang_getCursorSpelling(h_parent);
		unsigned	XYZ[3] = { 1, 1, 1 };
		enum SpvExecutionModel_	EXEC_MODEL = SpvExecutionModelMax;

		aclspv_wrdcount_t	POS = CTX->m_count.m_entp;
		aclspv_wrd_t		IOCOUNT = 0;
		CTX->m_err = ACLSPV_COMPILE_ALLOC_FAILED;

		clang_visitChildren(h_parent, attr_execmodel, &EXEC_MODEL);

		if(EXEC_MODEL == SpvExecutionModelMax)
			EXEC_MODEL = SpvExecutionModelGLCompute;	/** fallback */

		if(EXEC_MODEL == SpvExecutionModelGLCompute || EXEC_MODEL == SpvExecutionModelKernel)
			clang_visitChildren(h_parent, __attr_workgroup, XYZ);

		{
			int ARGI	= clang_Cursor_getNumArguments(h_parent);
			while(ARGI --> 0) {	/** All-Capturing Iterating Annonymous Routine Syntax (ACIARS) */
				CXCursor ARG	= clang_Cursor_getArgument(h_parent, (unsigned)ARGI);
				aclspv_wrd_t	STORAGE = SpvStorageClassMax;
				clang_visitChildren(ARG, attr_storage_class, &STORAGE);

				if(STORAGE == SpvStorageClassInput || STORAGE == SpvStorageClassOutput) {
					++IOCOUNT;
				}
			}
		}


#define THIS_ENTRY_POINT	((util_entp_t* ae2f_restrict)CTX->m_fnlist.m_entp.m_p)[CTX->m_tmp.m_w0]
		THIS_ENTRY_POINT.m_fn	= h_parent;
		THIS_ENTRY_POINT.m_id	= CTX->m_tmp.m_w3 + CTX->m_tmp.m_w0;

		if(IOCOUNT) {
			THIS_ENTRY_POINT.m_io.m_anchour = CTX->m_id;
			THIS_ENTRY_POINT.m_io.m_num	= IOCOUNT;
			CTX->m_id += IOCOUNT;
		}

		/** OpEntryPoint */
		unless((CTX->m_count.m_entp = emit_opcode(
						&CTX->m_section.m_entp
						, CTX->m_count.m_entp
						, SpvOpEntryPoint, 0)))
			goto LBL_ABRT;
		unless((CTX->m_count.m_entp = util_emit_wrd(
						&CTX->m_section.m_entp
						, CTX->m_count.m_entp
						, EXEC_MODEL)))
			goto LBL_ABRT;
		unless((CTX->m_count.m_entp = util_emit_wrd(
						&CTX->m_section.m_entp
						, CTX->m_count.m_entp
						, THIS_ENTRY_POINT.m_id)))
			goto LBL_ABRT;
		unless((CTX->m_count.m_entp = util_emit_str(
						&CTX->m_section.m_entp
						, CTX->m_count.m_entp
						, NAME.data))) 
			goto LBL_ABRT;

		if(IOCOUNT) {
			aclspv_wrd_t	IO_IDX = 0;
			while(IO_IDX < IOCOUNT) {
				unless((CTX->m_count.m_entp = util_emit_wrd(
								&CTX->m_section.m_entp
								, CTX->m_count.m_entp
								, THIS_ENTRY_POINT.m_io.m_anchour + IO_IDX)))
					goto LBL_ABRT;
				++IO_IDX;
			}
		}

		set_oprnd_count_for_opcode(
				get_wrd_of_vec(&CTX->m_section.m_entp)[POS]
				, CTX->m_count.m_entp - POS - 1
				);

		/** OpExecMode */
		if(EXEC_MODEL == SpvExecutionModelGLCompute || EXEC_MODEL == SpvExecutionModelKernel)
			ae2f_expected_but_else(CTX->m_count.m_execmode = util_emitx_6(
						&CTX->m_section.m_execmode
						, CTX->m_count.m_execmode
						, SpvOpExecutionMode
						, THIS_ENTRY_POINT.m_id
						, SpvExecutionModeLocalSize
						, XYZ[0], XYZ[1], XYZ[2]
						)) goto LBL_ABRT;

#if !defined(NDEBUG) || !NDEBUG
		POS = CTX->m_count.m_name;
		unless((CTX->m_count.m_name = emit_opcode(&CTX->m_section.m_name, CTX->m_count.m_name, SpvOpName, 0)))			goto LBL_ABRT;
		unless((CTX->m_count.m_name = util_emit_wrd(&CTX->m_section.m_name, CTX->m_count.m_name
						, THIS_ENTRY_POINT.m_id)))				goto LBL_ABRT;
		unless((CTX->m_count.m_name = util_emit_str(&CTX->m_section.m_name, CTX->m_count.m_name, NAME.data)))	goto LBL_ABRT;
		set_oprnd_count_for_opcode(get_wrd_of_vec(&CTX->m_section.m_name)[POS], CTX->m_count.m_name - POS - 1);
#endif
		CTX->m_err = ACLSPV_COMPILE_OK;
		++CTX->m_tmp.m_w0;

		clang_disposeString(NAME);
		return CXChildVisit_Continue;
		ae2f_unreachable();

LBL_ABRT:
		clang_disposeString(NAME);
		return CXChildVisit_Break;
	} else {
#define	THIS_FUNCTION	((util_fn_t* ae2f_restrict)CTX->m_fnlist.m_fn.m_p)[CTX->m_tmp.m_w1]
		THIS_FUNCTION.m_fn = h_parent;
		THIS_FUNCTION.m_id = CTX->m_id++;
	}
#undef	CTX
#undef	THIS_ENTRY_POINT

	return CXChildVisit_Continue;
}

ACLSPV_ABI_IMPL enum CXChildVisitResult aclemit_count_fn(CXCursor h_cur, CXCursor h_parent, CXClientData h_ctx) {
#define CTX	((x_aclspv_ctx* ae2f_restrict)h_ctx)
	assert(CTX);

	unless(h_cur.kind == CXCursor_CompoundStmt && h_parent.kind == CXCursor_FunctionDecl)
		return CXChildVisit_Recurse;

	if(util_is_kernel(h_parent)) {
		++CTX->m_fnlist.m_num_entp;
	} else {
		++CTX->m_fnlist.m_num_fn;
	}

	return CXChildVisit_Continue;
#undef	CTX
}
