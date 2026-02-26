/** @file expr.h */

#ifndef	lib_emit_expr_h
#define	lib_emit_expr_h
#include <aclspv/abi.h>
#include <aclspv/spvty.h>
#include <util/literal.h>

typedef enum EMIT_EXPR_ {

	EMIT_EXPR_FAILURE,

	/** succeed, value was not constant */
	EMIT_EXPR_SUCCESS,

	/** succeed, value was constant. */
	EMIT_EXPR_SUCCESS_CONSTANT,

	/** succeed, value was literal. */
	EMIT_EXPR_SUCCESS_LITERAL,

	EMIT_EXPR_NOT_THE_CASE
} e_emit_expr_bin_1_t;

typedef util_literal emit_expr_literal;

ACLSPV_ABI_DECL enum EMIT_EXPR_ aclexpr_arithmetic_cast_non_literal(
		const aclspv_id_t	c_old_id,
		const aclspv_id_t	c_old_type,
		const B_util_literal_t	c_flag_literal,
		aclspv_id_t* ae2f_restrict		r_newid,
		aclspv_id_t				c_new_type,
		const h_util_ctx_t			h_ctx
		);

ACLSPV_ABI_DECL enum EMIT_EXPR_ aclexpr_arithmetic_cast(
		const aclspv_id_t	c_old_id,
		const aclspv_id_t	c_old_type,
		const B_util_literal_t	c_flag_literal,
		const emit_expr_literal	c_old_literal,

		aclspv_id_t* ae2f_restrict	r_newid,
		aclspv_id_t		c_new_type,
		emit_expr_literal* ae2f_restrict wr_new_literal_opt,
		const h_util_ctx_t	h_ctx
		);

ACLSPV_ABI_DECL enum EMIT_EXPR_ aclspv_get_expr(
		aclspv_id_t* ae2f_restrict const ret_id
		, const aclspv_id_t c_type_req
		, const CXCursor c_cur
		, util_literal* ae2f_restrict	wr_literal_opt
		, h_util_ctx_t h_ctx
		);

#endif
