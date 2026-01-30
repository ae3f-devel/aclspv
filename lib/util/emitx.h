/** @file emitx.h @brief advanced emission */

#ifndef lib_util_emitx
#define lib_util_emitx

#include "./wrdemit.h"
#include <spirv/unified1/spirv.h>

ae2f_inline static spvsz_t	util_emitx_2(
		x_aclspv_vec* ae2f_restrict const	h_wrds,
		const spvsz_t				c_wrdcount,
		const aclspv_wrd_t			c_opcode,
		const aclspv_wrd_t			c_oprand
		) 
{
	ae2f_expected_but_else(util_emit_wrd(h_wrds, c_wrdcount + 1, c_oprand))
		return 0;

	get_wrd_of_vec(h_wrds)[c_wrdcount]	= opcode_to_wrd(c_opcode, 1);

	return c_wrdcount + 2;
}

ae2f_inline static spvsz_t	util_emitx_3(
		x_aclspv_vec* ae2f_restrict const	h_wrds,
		const spvsz_t				c_wrdcount,
		const aclspv_wrd_t			c_opcode,
		const aclspv_wrd_t			c_opr_0,
		const aclspv_wrd_t			c_opr_1
		) 
{
	ae2f_expected_but_else(util_emit_wrd(h_wrds, c_wrdcount + 2, c_opr_1))
		return 0;

	get_wrd_of_vec(h_wrds)[c_wrdcount + 1]	= c_opr_0;
	get_wrd_of_vec(h_wrds)[c_wrdcount]	= opcode_to_wrd(c_opcode, 2);

	return c_wrdcount + 3;
}

ae2f_inline static spvsz_t	util_emitx_4(
		x_aclspv_vec* ae2f_restrict const	h_wrds,
		const spvsz_t				c_wrdcount,
		const aclspv_wrd_t			c_opcode,
		const aclspv_wrd_t			c_opr_0,
		const aclspv_wrd_t			c_opr_1,
		const aclspv_wrd_t			c_opr_2
		) 
{
	ae2f_expected_but_else(util_emit_wrd(h_wrds, c_wrdcount + 3, c_opr_2))
		return 0;

	get_wrd_of_vec(h_wrds)[c_wrdcount + 2] 	= c_opr_1;
	get_wrd_of_vec(h_wrds)[c_wrdcount + 1]	= c_opr_0;
	get_wrd_of_vec(h_wrds)[c_wrdcount]	= opcode_to_wrd(c_opcode, 3); 

	return c_wrdcount + 4;
}

ae2f_inline static spvsz_t	util_emitx_5(
		x_aclspv_vec* ae2f_restrict const	h_wrds,
		const spvsz_t				c_wrdcount,
		const aclspv_wrd_t			c_opcode,
		const aclspv_wrd_t			c_opr_0,
		const aclspv_wrd_t			c_opr_1,
		const aclspv_wrd_t			c_opr_2,
		const aclspv_wrd_t			c_opr_3
		) 
{
	ae2f_expected_but_else(util_emit_wrd(h_wrds, c_wrdcount + 4, c_opr_3))
		return 0;

	get_wrd_of_vec(h_wrds)[c_wrdcount + 3] 	= c_opr_2;
	get_wrd_of_vec(h_wrds)[c_wrdcount + 2] 	= c_opr_1;
	get_wrd_of_vec(h_wrds)[c_wrdcount + 1]	= c_opr_0;
	get_wrd_of_vec(h_wrds)[c_wrdcount]	= opcode_to_wrd(c_opcode, 4);

	return c_wrdcount + 5;
}

ae2f_inline static spvsz_t	util_emitx_6(
		x_aclspv_vec* ae2f_restrict const	h_wrds,
		const spvsz_t				c_wrdcount,
		const aclspv_wrd_t			c_opcode,
		const aclspv_wrd_t			c_opr_0,
		const aclspv_wrd_t			c_opr_1,
		const aclspv_wrd_t			c_opr_2,
		const aclspv_wrd_t			c_opr_3,
		const aclspv_wrd_t			c_opr_4
		) 
{
	ae2f_expected_but_else(util_emit_wrd(h_wrds, c_wrdcount + 5, c_opr_4))
		return 0;

	get_wrd_of_vec(h_wrds)[c_wrdcount + 4] 	= c_opr_3;
	get_wrd_of_vec(h_wrds)[c_wrdcount + 3] 	= c_opr_2;
	get_wrd_of_vec(h_wrds)[c_wrdcount + 2] 	= c_opr_1;
	get_wrd_of_vec(h_wrds)[c_wrdcount + 1]	= c_opr_0;
	get_wrd_of_vec(h_wrds)[c_wrdcount]	= opcode_to_wrd(c_opcode, 5);

	return c_wrdcount + 6;
}

ae2f_inline static spvsz_t	util_emitx_7(
		x_aclspv_vec* ae2f_restrict const	h_wrds,
		const spvsz_t				c_wrdcount,
		const aclspv_wrd_t			c_opcode,
		const aclspv_wrd_t			c_opr_0,
		const aclspv_wrd_t			c_opr_1,
		const aclspv_wrd_t			c_opr_2,
		const aclspv_wrd_t			c_opr_3,
		const aclspv_wrd_t			c_opr_4,
		const aclspv_wrd_t			c_opr_5
		) 
{
	ae2f_expected_but_else(util_emit_wrd(h_wrds, c_wrdcount + 6, c_opr_5))
		return 0;

	get_wrd_of_vec(h_wrds)[c_wrdcount + 5] 	= c_opr_4;
	get_wrd_of_vec(h_wrds)[c_wrdcount + 4] 	= c_opr_3;
	get_wrd_of_vec(h_wrds)[c_wrdcount + 3] 	= c_opr_2;
	get_wrd_of_vec(h_wrds)[c_wrdcount + 2] 	= c_opr_1;
	get_wrd_of_vec(h_wrds)[c_wrdcount + 1]	= c_opr_0;
	get_wrd_of_vec(h_wrds)[c_wrdcount]	= opcode_to_wrd(c_opcode, 6);

	return c_wrdcount + 7;
}

ae2f_inline static spvsz_t	util_emitx_8(
		x_aclspv_vec* ae2f_restrict const	h_wrds,
		const spvsz_t				c_wrdcount,
		const aclspv_wrd_t			c_opcode,
		const aclspv_wrd_t			c_opr_0,
		const aclspv_wrd_t			c_opr_1,
		const aclspv_wrd_t			c_opr_2,
		const aclspv_wrd_t			c_opr_3,
		const aclspv_wrd_t			c_opr_4,
		const aclspv_wrd_t			c_opr_5,
		const aclspv_wrd_t			c_opr_6
		) 
{
	ae2f_expected_but_else(util_emit_wrd(h_wrds, c_wrdcount + 7, c_opr_6))
		return 0;

	get_wrd_of_vec(h_wrds)[c_wrdcount + 6] 	= c_opr_5;
	get_wrd_of_vec(h_wrds)[c_wrdcount + 5] 	= c_opr_4;
	get_wrd_of_vec(h_wrds)[c_wrdcount + 4] 	= c_opr_3;
	get_wrd_of_vec(h_wrds)[c_wrdcount + 3] 	= c_opr_2;
	get_wrd_of_vec(h_wrds)[c_wrdcount + 2] 	= c_opr_1;
	get_wrd_of_vec(h_wrds)[c_wrdcount + 1]	= c_opr_0;
	get_wrd_of_vec(h_wrds)[c_wrdcount]	= opcode_to_wrd(c_opcode, 7);

	return c_wrdcount + 8;
}

#define	util_emitx_variable(h_wrds, c_wrdcount, c_type, c_retid, c_storage_class)	\
	util_emitx_4(h_wrds, c_wrdcount, SpvOpVariable, c_type, c_retid, c_storage_class)

#define	util_emitx_type_pointer(h_wrds, c_wrdcount, c_retid, c_storage_class, c_elm_type_id)	\
	util_emitx_4(h_wrds, c_wrdcount, SpvOpTypePointer, c_retid, c_storage_class, c_elm_type_id)

#define util_emitx_type_array(h_wrds, c_wrdcount, c_retid, c_elm_type_id, c_arrcount_id)	\
	util_emitx_4(h_wrds, c_wrdcount, SpvOpTypeArray, c_retid, c_elm_type_id, c_arrcount_id)

#define util_emitx_spec_constant(h_wrds, c_wrdcount, c_ty, c_retid, c_val)	\
	util_emitx_4(h_wrds, c_wrdcount, SpvOpSpecConstant, c_ty, c_retid, c_val)

#define	util_emitx_spec_constant_op(h_wrds, c_wrdcount, c_ty, c_retid, c_operator, c_operand)	\
	util_emitx_5(h_wrds, c_wrdcount, SpvOpSpecConstantOp, c_ty, c_retid, c_operator, c_operand)

#define	util_emitx_spec_constant_op2(h_wrds, c_wrdcount, c_retid, c_ty, c_operator, c_opr_0, c_opr_1)	\
	util_emitx_6(h_wrds, c_wrdcount, SpvOpSpecConstantOp, c_retid, c_ty, c_operator, c_opr_0, c_opr_1)

#endif
