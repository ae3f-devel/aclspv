#ifndef util_u32_to_hex8_h
#define util_u32_to_hex8_h

#include <ae2f/Macro.h>

#if	!__ae2f_MACRO_GENERATED
void	util_u32_to_hex8(const unsigned c_wrd, char  ret[8]);
#endif

#define		util_u32_to_hex_lookup(a)	\
	((a) == 0x0 ? '0' :			\
	(a) == 0x1 ? '1' :			\
	(a) == 0x2 ? '2' :			\
	(a) == 0x3 ? '3' :			\
	(a) == 0x4 ? '4' :			\
	(a) == 0x5 ? '5' :			\
	(a) == 0x6 ? '6' :			\
	(a) == 0x7 ? '7' :			\
	(a) == 0x8 ? '8' :			\
	(a) == 0x9 ? '9' :			\
	(a) == 0xA ? 'a' :			\
	(a) == 0xB ? 'b' :			\
	(a) == 0xC ? 'c' :			\
	(a) == 0xD ? 'd' :			\
	(a) == 0xE ? 'e' : 'f')

ae2f_MAC() util_u32_to_hex8(const unsigned c_u32, char* ret) {
	(ret)[7] = util_u32_to_hex_lookup(0xF & (c_u32) >> (0 << 2));
	(ret)[6] = util_u32_to_hex_lookup(0xF & (c_u32) >> (1 << 2));
	(ret)[5] = util_u32_to_hex_lookup(0xF & (c_u32) >> (2 << 2));
	(ret)[4] = util_u32_to_hex_lookup(0xF & (c_u32) >> (3 << 2));
	(ret)[3] = util_u32_to_hex_lookup(0xF & (c_u32) >> (4 << 2));
	(ret)[2] = util_u32_to_hex_lookup(0xF & (c_u32) >> (5 << 2));
	(ret)[1] = util_u32_to_hex_lookup(0xF & (c_u32) >> (6 << 2));
	(ret)[0] = util_u32_to_hex_lookup(0xF & (c_u32) >> (7 << 2));
}

#endif
