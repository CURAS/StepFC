#include <assert.h>
#include <string.h>
#include "sfc_6502.h"

static const char SFC_HEXDATA[] = "0123456789ABCDEF";

extern inline void sfc_btoh(char o[], uint8_t b) {
	o[0] = SFC_HEXDATA[b >> 4];
	o[1] = SFC_HEXDATA[b & 0x0F];
}

extern inline void sfc_btod(char o[], uint8_t b) {
	const int8_t sb = (int8_t)b;
	if (sb < 0) {
		o[0] = '-';
		b = -b;
	}
	else {
		o[0] = '+';
	}
	o[1] = SFC_HEXDATA[(uint8_t)b / 100];
	o[2] = SFC_HEXDATA[(uint8_t)b / 10 % 10];
	o[3] = SFC_HEXDATA[(uint8_t)b % 10];
}

void sfc_6502_disassembly(sfc_6502_code_t code, char buf[SFC_DISASSEMBLY_BUF_LEN])
{
	memset(buf, ' ', SFC_DISASSEMBLY_BUF_LEN);
	buf[SFC_DISASSEMBLY_BUF_LEN - 2] = ';';
	buf[SFC_DISASSEMBLY_BUF_LEN - 1] = '\0';

	memcpy(buf, opnames[code.op], 3);

	switch (opmodes[code.op])
	{
	case SFC_AM_UNK:
		break;
	case SFC_AM_ACC:
		buf[4] = 'A';
		break;
	case SFC_AM_IMP:
		break;
	case SFC_AM_IMM:
		buf[4] = '#';
		sfc_btoh(buf + 5, code.a1);
		break;
	case SFC_AM_ABS:
		buf[4] = '$';
		sfc_btoh(buf + 5, code.a2);
		sfc_btoh(buf + 7, code.a1);
		break;
	case SFC_AM_ABX:
		buf[4] = '$';
		sfc_btoh(buf + 5, code.a2);
		sfc_btoh(buf + 7, code.a1);
		buf[9] = ',';
		buf[10] = 'X';
		break;
	case SFC_AM_ABY:
		buf[4] = '$';
		sfc_btoh(buf + 5, code.a2);
		sfc_btoh(buf + 7, code.a1);
		buf[9] = ',';
		buf[10] = 'Y';
		break;
	case SFC_AM_ZPG:
		buf[4] = '$';
		sfc_btoh(buf + 5, code.a1);
		break;
	case SFC_AM_ZPX:
		buf[4] = '$';
		sfc_btoh(buf + 5, code.a1);
		buf[7] = ',';
		buf[8] = 'X';
		break;
	case SFC_AM_ZPY:
		buf[4] = '$';
		sfc_btoh(buf + 5, code.a1);
		buf[7] = ',';
		buf[8] = 'Y';
		break;
	case SFC_AM_INX:
		buf[4] = '(';
		buf[5] = '$';
		sfc_btoh(buf + 6, code.a1);
		buf[8] = ',';
		buf[9] = 'X';
		buf[10] = ')';
		break;
	case SFC_AM_INY:
		buf[4] = '(';
		buf[5] = '$';
		sfc_btoh(buf + 6, code.a1);
		buf[8] = ')';
		buf[9] = ',';
		buf[10] = 'Y';
		break;
	case SFC_AM_IND:
		buf[4] = '(';
		buf[5] = '$';
		sfc_btoh(buf + 6, code.a2);
		sfc_btoh(buf + 8, code.a1);
		buf[10] = ')';
		break;
	case SFC_AM_REL:
		buf[4] = '*';
		sfc_btod(buf + 5, code.a1);
		break;
	default:
		assert(!"invalid addressing mode");
		break;
	}
}