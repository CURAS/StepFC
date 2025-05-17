#include <assert.h>
#include <string.h>
#include "sfc_6502.h"
#include "sfc_famicom.h"

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

uint16_t sfc_addressing_UNK(sfc_famicom_t* famicom)
{
	return 0;
}
uint16_t sfc_addressing_ACC(sfc_famicom_t* famicom)
{
	return 0;
}
uint16_t sfc_addressing_IMP(sfc_famicom_t* famicom)
{
	return 0;
}
uint16_t sfc_addressing_IMM(sfc_famicom_t* famicom)
{
	uint16_t address = SFC_PC++;
	return address;
}
uint16_t sfc_addressing_ABS(sfc_famicom_t* famicom)
{
	uint16_t address = SFC_READ(SFC_PC++);
	address |= SFC_READ(SFC_PC++) << 8;
	return address;
}
uint16_t sfc_addressing_ABX(sfc_famicom_t* famicom)
{
	return sfc_addressing_ABS(famicom) + SFC_X;
}
uint16_t sfc_addressing_ABY(sfc_famicom_t* famicom)
{
	return sfc_addressing_ABS(famicom) + SFC_Y;
}
uint16_t sfc_addressing_ZPG(sfc_famicom_t* famicom)
{
	uint16_t address = SFC_READ(SFC_PC++);
	return address;
}
uint16_t sfc_addressing_ZPX(sfc_famicom_t* famicom)
{
	return (sfc_addressing_ZPG(famicom) + SFC_X) & 0xFF;
}
uint16_t sfc_addressing_ZPY(sfc_famicom_t* famicom)
{
	return (sfc_addressing_ZPG(famicom) + SFC_Y) & 0xFF;
}
uint16_t sfc_addressing_IND(sfc_famicom_t* famicom)
{
	uint16_t ind_address = SFC_READ(SFC_PC++);
	ind_address |= SFC_READ(SFC_PC++) << 8;
	uint16_t ind_address2 = (ind_address & 0xFF00) | ((ind_address + 1) & 0x00FF);

	uint16_t address = SFC_READ(ind_address);
	address |= SFC_READ(ind_address2) << 8;
	return address;
}
uint16_t sfc_addressing_INX(sfc_famicom_t* famicom)
{
	uint8_t ind_address1 = SFC_READ(SFC_PC++);
	ind_address1 += SFC_X;
	uint8_t ind_address2 = ind_address1+1;
	uint16_t address = SFC_READ(ind_address1);
	address |= SFC_READ(ind_address2) << 8;
	return address;
}
uint16_t sfc_addressing_INY(sfc_famicom_t* famicom)
{
	uint8_t ind_address1 = SFC_READ(SFC_PC++);
	uint8_t ind_address2 = ind_address1 + 1;
	uint16_t address = SFC_READ(ind_address1);
	address |= SFC_READ(ind_address2) << 8;
	address += SFC_Y;
	return address;
}
uint16_t sfc_addressing_REL(sfc_famicom_t* famicom)
{
	int8_t step = (int8_t)SFC_READ(SFC_PC++);
	uint16_t address = SFC_PC + step;
	return address;
}

void sfc_operation_ADC(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	uint16_t result = (uint16_t)data + SFC_A + (SFC_CF_CHK ? 1 : 0);
	(result > 0xFF) ? SFC_CF_SET : SFC_CF_CLR;
	((result & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(!((data ^ SFC_A) & 0x80) && (((uint8_t)result ^ SFC_A) & 0x80)) ? SFC_VF_SET : SFC_VF_CLR;
	(result & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
	SFC_A = (uint8_t)result;
}
void sfc_operation_AHX(uint16_t address, sfc_famicom_t* famicom)
{
	assert(!"NOT IMPL");
	return;
}
void sfc_operation_ALR(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	data &= SFC_A;
	(data & 0x01) ? SFC_CF_SET : SFC_CF_CLR;
	((data & 0xFE) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	SFC_SF_CLR;
	SFC_A = data >> 1;
}
void sfc_operation_ANC(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	uint8_t result = data & SFC_A;
	((result & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(result & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
	SFC_A = (uint8_t)result;
	SFC_SF_CHK ? SFC_CF_SET : SFC_CF_CLR;
}
void sfc_operation_AND(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	uint8_t result = data & SFC_A;
	((result & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(result & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
	SFC_A = (uint8_t)result;
}
void sfc_operation_ARR(uint16_t address, sfc_famicom_t* famicom)
{
	uint16_t data = SFC_READ(address);
	data = data & SFC_A;
	data |= (SFC_CF_CHK ? 0x100 : 0);
	((data & 0x1FE) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(data & 0x100) ? SFC_SF_SET : SFC_SF_CLR;
	data >>= 1;
	(data & 0x40) ? SFC_CF_SET : SFC_CF_CLR;
	(((data >> 6) ^ (data >> 5)) & 0x01) ? SFC_VF_SET : SFC_VF_CLR;
	SFC_A = (uint8_t)data;
}
void sfc_operation_ASL(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	(data & 0x80) ? SFC_CF_SET : SFC_CF_CLR;
	((data & 0x7F) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(data & 0x40) ? SFC_SF_SET : SFC_SF_CLR;
	data <<= 1;
	SFC_WRITE(address, data);
}
void sfc_operation_ASLA(uint16_t address, sfc_famicom_t* famicom)
{
	(SFC_A & 0x80) ? SFC_CF_SET : SFC_CF_CLR;
	((SFC_A & 0x7F) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(SFC_A & 0x40) ? SFC_SF_SET : SFC_SF_CLR;
	SFC_A <<= 1;
}
void sfc_operation_AXS(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	uint16_t result = SFC_X & SFC_A;
	result -= data;
	(result & 0x100) ? SFC_CF_SET : SFC_CF_CLR;
	((result & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(result & 0x40) ? SFC_SF_SET : SFC_SF_CLR;
	SFC_X = (uint8_t)result;
}
void sfc_operation_BCC(uint16_t address, sfc_famicom_t* famicom)
{
	if (!SFC_CF_CHK)
		SFC_PC = address;
}
void sfc_operation_BCS(uint16_t address, sfc_famicom_t* famicom)
{
	if (SFC_CF_CHK)
		SFC_PC = address;
}
void sfc_operation_BEQ(uint16_t address, sfc_famicom_t* famicom)
{
	if (SFC_ZF_CHK)
		SFC_PC = address;
}
void sfc_operation_BIT(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	((SFC_A & data) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(data & 0x40) ? SFC_VF_SET : SFC_VF_CLR;
	(data & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_BMI(uint16_t address, sfc_famicom_t* famicom)
{
	if (SFC_SF_CHK)
		SFC_PC = address;
}
void sfc_operation_BNE(uint16_t address, sfc_famicom_t* famicom)
{
	if (!SFC_ZF_CHK)
		SFC_PC = address;
}
void sfc_operation_BPL(uint16_t address, sfc_famicom_t* famicom)
{
	if (!SFC_SF_CHK)
		SFC_PC = address;
}
void sfc_operation_BRK(uint16_t address, sfc_famicom_t* famicom)
{
	++SFC_PC;
	SFC_PUSH(SFC_PC >> 8);
	SFC_PUSH(SFC_PC & 0xFF);
	uint8_t data = SFC_P;
	data |= (SFC_FLAG_R | SFC_FLAG_B);
	SFC_PUSH(data);
	SFC_IF_SET;
	uint16_t irql = SFC_READ(SFC_VECTOR_IRQBRK);
	uint16_t irqh = SFC_READ(SFC_VECTOR_IRQBRK + 1);
	SFC_PC = irql | (irqh << 8);
}
void sfc_operation_BVC(uint16_t address, sfc_famicom_t* famicom)
{
	if (!SFC_VF_CHK)
		SFC_PC = address;
}
void sfc_operation_BVS(uint16_t address, sfc_famicom_t* famicom)
{
	if (SFC_VF_CHK)
		SFC_PC = address;
}
void sfc_operation_CLC(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_CF_CLR;
}
void sfc_operation_CLD(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_DF_CLR;
}
void sfc_operation_CLI(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_IF_CLR;
}
void sfc_operation_CLV(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_VF_CLR;
}
void sfc_operation_CMP(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	uint16_t result = (uint16_t)SFC_A - data;
	(result < 0x100) ? SFC_CF_SET : SFC_CF_CLR;
	((result & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(result & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_CPX(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	uint16_t result = (uint16_t)SFC_X - data;
	(result < 0x100) ? SFC_CF_SET : SFC_CF_CLR;
	((result & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(result & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_CPY(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	uint16_t result = (uint16_t)SFC_Y - data;
	(result < 0x100) ? SFC_CF_SET : SFC_CF_CLR;
	((result & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(result & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_DCP(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	data--;
	SFC_WRITE(address, data);
	uint16_t result = (uint16_t)SFC_A - data;
	(result < 0x100) ? SFC_CF_SET : SFC_CF_CLR;
	((result & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(result & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_DEC(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	data--;
	SFC_WRITE(address, data);
	((data & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(data & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_DEX(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_X--;
	((SFC_X & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(SFC_X & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_DEY(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_Y--;
	((SFC_Y & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(SFC_Y & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_EOR(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	uint8_t result = data ^ SFC_A;
	((result & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(result & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
	SFC_A = (uint8_t)result;
}
void sfc_operation_INC(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	data++;
	SFC_WRITE(address, data);
	((data & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(data & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_INX(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_X++;
	((SFC_X & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(SFC_X & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_INY(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_Y++;
	((SFC_Y & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(SFC_Y & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_ISC(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	data++;
	SFC_WRITE(address, data);
	uint16_t result = (uint16_t)SFC_A - data - (SFC_CF_CHK ? 0 : 1);
	(result > 0xFF) ? SFC_CF_SET : SFC_CF_CLR;
	((result & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(((data ^ SFC_A) & 0x80) && (((uint8_t)result ^ SFC_A) & 0x80)) ? SFC_VF_SET : SFC_VF_CLR;
	(result & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
	SFC_A = (uint8_t)result;
}
void sfc_operation_JMP(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_PC = address;
}
void sfc_operation_JSR(uint16_t address, sfc_famicom_t* famicom)
{
	--SFC_PC;
	SFC_PUSH(SFC_PC >> 8);
	SFC_PUSH(SFC_PC & 0xFF);
	SFC_PC = address;
}
void sfc_operation_LAS(uint16_t address, sfc_famicom_t* famicom)
{
	assert(!"NOT IMPL");
	return;
}
void sfc_operation_LAX(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	((data & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(data & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
	SFC_X = SFC_A = data;
}
void sfc_operation_LDA(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	SFC_A = data;
	(data == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(data & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_LDX(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	SFC_X = data;
	(data == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(data & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_LDY(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	SFC_Y = data;
	(data == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(data & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_LSR(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	(data & 0x01) ? SFC_CF_SET : SFC_CF_CLR;
	((data & 0xFE) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	SFC_SF_CLR;
	data >>= 1;
	SFC_WRITE(address, data);
}
void sfc_operation_LSRA(uint16_t address, sfc_famicom_t* famicom)
{
	(SFC_A & 0x01) ? SFC_CF_SET : SFC_CF_CLR;
	((SFC_A & 0xFE) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	SFC_SF_CLR;
	SFC_A >>= 1;
}
void sfc_operation_NOP(uint16_t address, sfc_famicom_t* famicom)
{
}
void sfc_operation_ORA(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	uint8_t result = data | SFC_A;
	((result & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(result & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
	SFC_A = (uint8_t)result;
}
void sfc_operation_PHA(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_PUSH(SFC_A);
}
void sfc_operation_PHP(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_P;
	data |= (SFC_FLAG_R | SFC_FLAG_B);
	SFC_PUSH(data);
}
void sfc_operation_PLA(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_A = SFC_POP();
	((SFC_A & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(SFC_A & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_PLP(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_P = SFC_POP();
	SFC_BF_CLR;
}
void sfc_operation_RLA(uint16_t address, sfc_famicom_t* famicom)
{
	uint16_t data = SFC_READ(address);
	data <<= 1;
	data |= (SFC_CF_CHK ? 1 : 0);
	(data & 0x100) ? SFC_CF_SET : SFC_CF_CLR;
	SFC_WRITE(address, (uint8_t)data);
	uint8_t result = (uint8_t)data & SFC_A;
	((result & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(result & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
	SFC_A = (uint8_t)result;
}
void sfc_operation_ROL(uint16_t address, sfc_famicom_t* famicom)
{
	uint16_t data = SFC_READ(address);
	data <<= 1;
	data |= (SFC_CF_CHK ? 1 : 0);
	(data & 0x100) ? SFC_CF_SET : SFC_CF_CLR;
	((data & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(data & 0xF0) ? SFC_SF_SET : SFC_SF_CLR;
	SFC_WRITE(address, (uint8_t)data);
}
void sfc_operation_ROLA(uint16_t address, sfc_famicom_t* famicom)
{
	uint16_t data = SFC_A;
	data <<= 1;
	data |= (SFC_CF_CHK ? 1 : 0);
	(data & 0x100) ? SFC_CF_SET : SFC_CF_CLR;
	((data & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(data & 0xF0) ? SFC_SF_SET : SFC_SF_CLR;
	SFC_A = (uint8_t)data;
}
void sfc_operation_ROR(uint16_t address, sfc_famicom_t* famicom)
{
	uint16_t data = SFC_READ(address);
	data |= (SFC_CF_CHK ? 0x100 : 0);
	(data & 0x01) ? SFC_CF_SET : SFC_CF_CLR;
	((data & 0x1FE) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(data & 0x100) ? SFC_SF_SET : SFC_SF_CLR;
	data >>= 1;
	SFC_WRITE(address, (uint8_t)data);
}
void sfc_operation_RORA(uint16_t address, sfc_famicom_t* famicom)
{
	uint16_t data = SFC_A;
	data |= (SFC_CF_CHK ? 0x100 : 0);
	(data & 0x01) ? SFC_CF_SET : SFC_CF_CLR;
	((data & 0x1FE) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(data & 0x100) ? SFC_SF_SET : SFC_SF_CLR;
	data >>= 1;
	SFC_A = (uint8_t)data;
}
void sfc_operation_RRA(uint16_t address, sfc_famicom_t* famicom)
{
	uint16_t data = SFC_READ(address);
	data |= (SFC_CF_CHK ? 0x100 : 0);
	data >>= 1;
	SFC_WRITE(address, (uint8_t)data);
	uint16_t result = (uint16_t)data + SFC_A + (SFC_CF_CHK ? 1 : 0);
	(result > 0xFF) ? SFC_CF_SET : SFC_CF_CLR;
	((result & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(!(((uint8_t)data ^ SFC_A) & 0x80) && (((uint8_t)result ^ SFC_A) & 0x80)) ? SFC_VF_SET : SFC_VF_CLR;
	(result & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
	SFC_A = (uint8_t)result;
}
void sfc_operation_RTI(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_POP();
	data |= SFC_FLAG_R;
	SFC_P = data;
	SFC_PC = SFC_POP();
	SFC_PC |= SFC_POP() << 8;
}
void sfc_operation_RTS(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_PC = SFC_POP();
	SFC_PC |= SFC_POP() << 8;
	++SFC_PC;
}
void sfc_operation_SAX(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_A & SFC_X;
	SFC_WRITE(address, data);
}
void sfc_operation_SBC(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	uint16_t result = (uint16_t)SFC_A - data - (SFC_CF_CHK ? 0 : 1);
	(!(result > 0xFF)) ? SFC_CF_SET : SFC_CF_CLR;
	((result & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(((data ^ SFC_A) & 0x80) && (((uint8_t)result ^ SFC_A) & 0x80)) ? SFC_VF_SET : SFC_VF_CLR;
	(result & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
	SFC_A = (uint8_t)result;
}
void sfc_operation_SEC(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_CF_SET;
}
void sfc_operation_SED(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_DF_SET;
}
void sfc_operation_SEI(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_IF_SET;
}
void sfc_operation_SHX(uint16_t address, sfc_famicom_t* famicom)
{
	assert(!"NOT IMPL");
	return;
}
void sfc_operation_SHY(uint16_t address, sfc_famicom_t* famicom)
{
	assert(!"NOT IMPL");
	return;
}
void sfc_operation_SLO(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	(data & 0x80) ? SFC_CF_SET : SFC_CF_CLR;
	data <<= 1;
	SFC_WRITE(address, data);
	uint8_t result = data | SFC_A;
	((result & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(result & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
	SFC_A = (uint8_t)result;
}
void sfc_operation_SRE(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = SFC_READ(address);
	(data & 0x01) ? SFC_CF_SET : SFC_CF_CLR;
	data >>= 1;
	SFC_WRITE(address, data);
	uint8_t result = data ^ SFC_A;
	((result & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(result & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
	SFC_A = (uint8_t)result;
}
void sfc_operation_STA(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_WRITE(address, SFC_A);
}
void sfc_operation_STP(uint16_t address, sfc_famicom_t* famicom)
{
	assert(!"NOT IMPL");
	return;
}
void sfc_operation_STX(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_WRITE(address, SFC_X);
}
void sfc_operation_STY(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_WRITE(address, SFC_Y);
}
void sfc_operation_TAS(uint16_t address, sfc_famicom_t* famicom)
{
	assert(!"NOT IMPL");
	return;
}
void sfc_operation_TAX(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_X = SFC_A;
	((SFC_X & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(SFC_X & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_TAY(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_Y = SFC_A;
	((SFC_Y & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(SFC_Y & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_TSX(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_X = SFC_S;
	((SFC_X & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(SFC_X & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_TXA(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_A = SFC_X;
	((SFC_A & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(SFC_A & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_TXS(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_S = SFC_X;
}
void sfc_operation_TYA(uint16_t address, sfc_famicom_t* famicom)
{
	SFC_A = SFC_Y;
	((SFC_A & 0xFF) == 0) ? SFC_ZF_SET : SFC_ZF_CLR;
	(SFC_A & 0x80) ? SFC_SF_SET : SFC_SF_CLR;
}
void sfc_operation_XAA(uint16_t address, sfc_famicom_t* famicom)
{
	assert(!"NOT IMPL");
	return;
}

#define OP(n, o, a)\
case 0x##n:\
{\
	uint16_t address = sfc_addressing_##a(famicom);\
	sfc_operation_##o(address, famicom);\
	break;\
}

void sfc_cpu_execute_one(sfc_famicom_t* famicom)
{
	famicom->interfaces.before_execute(famicom->argument, famicom);
	uint8_t opcode = SFC_READ(SFC_PC++);
	switch (opcode)
	{
		OP(00, BRK, IMP)  OP(01, ORA, INX)  OP(02, STP, UNK)  OP(03, SLO, INX)  OP(04, NOP, ZPG)  OP(05, ORA, ZPG)  OP(06, ASL, ZPG)  OP(07, SLO, ZPG)  OP(08, PHP, IMP)  OP(09, ORA, IMM)  OP(0A, ASLA, ACC)  OP(0B, ANC, IMM)  OP(0C, NOP, ABS)  OP(0D, ORA, ABS)  OP(0E, ASL, ABS)  OP(0F, SLO, ABS)
		OP(10, BPL, REL)  OP(11, ORA, INY)  OP(12, STP, UNK)  OP(13, SLO, INY)  OP(14, NOP, ZPX)  OP(15, ORA, ZPX)  OP(16, ASL, ZPX)  OP(17, SLO, ZPX)  OP(18, CLC, IMP)  OP(19, ORA, ABY)  OP(1A, NOP, IMP)  OP(1B, SLO, ABY)  OP(1C, NOP, ABX)  OP(1D, ORA, ABX)  OP(1E, ASL, ABX)  OP(1F, SLO, ABX)
		OP(20, JSR, ABS)  OP(21, AND, INX)  OP(22, STP, UNK)  OP(23, RLA, INX)  OP(24, BIT, ZPG)  OP(25, AND, ZPG)  OP(26, ROL, ZPG)  OP(27, RLA, ZPG)  OP(28, PLP, IMP)  OP(29, AND, IMM)  OP(2A, ROLA, ACC)  OP(2B, ANC, IMM)  OP(2C, BIT, ABS)  OP(2D, AND, ABS)  OP(2E, ROL, ABS)  OP(2F, RLA, ABS)
		OP(30, BMI, REL)  OP(31, AND, INY)  OP(32, STP, UNK)  OP(33, RLA, INY)  OP(34, NOP, ZPX)  OP(35, AND, ZPX)  OP(36, ROL, ZPX)  OP(37, RLA, ZPX)  OP(38, SEC, IMP)  OP(39, AND, ABY)  OP(3A, NOP, IMP)  OP(3B, RLA, ABY)  OP(3C, NOP, ABX)  OP(3D, AND, ABX)  OP(3E, ROL, ABX)  OP(3F, RLA, ABX)
		OP(40, RTI, IMP)  OP(41, EOR, INX)  OP(42, STP, UNK)  OP(43, SRE, INX)  OP(44, NOP, ZPG)  OP(45, EOR, ZPG)  OP(46, LSR, ZPG)  OP(47, SRE, ZPG)  OP(48, PHA, IMP)  OP(49, EOR, IMM)  OP(4A, LSRA, ACC)  OP(4B, ALR, IMM)  OP(4C, JMP, ABS)  OP(4D, EOR, ABS)  OP(4E, LSR, ABS)  OP(4F, SRE, ABS)
		OP(50, BVC, REL)  OP(51, EOR, INY)  OP(52, STP, UNK)  OP(53, SRE, INY)  OP(54, NOP, ZPX)  OP(55, EOR, ZPX)  OP(56, LSR, ZPX)  OP(57, SRE, ZPX)  OP(58, CLI, IMP)  OP(59, EOR, ABY)  OP(5A, NOP, IMP)  OP(5B, SRE, ABY)  OP(5C, NOP, ABX)  OP(5D, EOR, ABX)  OP(5E, LSR, ABX)  OP(5F, SRE, ABX)
		OP(60, RTS, IMP)  OP(61, ADC, INX)  OP(62, STP, UNK)  OP(63, RRA, INX)  OP(64, NOP, ZPG)  OP(65, ADC, ZPG)  OP(66, ROR, ZPG)  OP(67, RRA, ZPG)  OP(68, PLA, IMP)  OP(69, ADC, IMM)  OP(6A, RORA, ACC)  OP(6B, ARR, IMM)  OP(6C, JMP, IND)  OP(6D, ADC, ABS)  OP(6E, ROR, ABS)  OP(6F, RRA, ABS)
		OP(70, BVS, REL)  OP(71, ADC, INY)  OP(72, STP, UNK)  OP(73, RRA, INY)  OP(74, NOP, ZPX)  OP(75, ADC, ZPX)  OP(76, ROR, ZPX)  OP(77, RRA, ZPX)  OP(78, SEI, IMP)  OP(79, ADC, ABY)  OP(7A, NOP, IMP)  OP(7B, RRA, ABY)  OP(7C, NOP, ABX)  OP(7D, ADC, ABX)  OP(7E, ROR, ABX)  OP(7F, RRA, ABX)
		OP(80, NOP, IMM)  OP(81, STA, INX)  OP(82, NOP, IMM)  OP(83, SAX, INX)  OP(84, STY, ZPG)  OP(85, STA, ZPG)  OP(86, STX, ZPG)  OP(87, SAX, ZPG)  OP(88, DEY, IMP)  OP(89, NOP, IMM)  OP(8A, TXA, IMP)  OP(8B, XAA, IMM)  OP(8C, STY, ABS)  OP(8D, STA, ABS)  OP(8E, STX, ABS)  OP(8F, SAX, ABS)
		OP(90, BCC, REL)  OP(91, STA, INY)  OP(92, STP, UNK)  OP(93, AHX, INY)  OP(94, STY, ZPX)  OP(95, STA, ZPX)  OP(96, STX, ZPY)  OP(97, SAX, ZPY)  OP(98, TYA, IMP)  OP(99, STA, ABY)  OP(9A, TXS, IMP)  OP(9B, TAS, ABY)  OP(9C, SHY, ABX)  OP(9D, STA, ABX)  OP(9E, SHX, ABY)  OP(9F, AHX, ABY)
		OP(A0, LDY, IMM)  OP(A1, LDA, INX)  OP(A2, LDX, IMM)  OP(A3, LAX, INX)  OP(A4, LDY, ZPG)  OP(A5, LDA, ZPG)  OP(A6, LDX, ZPG)  OP(A7, LAX, ZPG)  OP(A8, TAY, IMP)  OP(A9, LDA, IMM)  OP(AA, TAX, IMP)  OP(AB, LAX, IMM)  OP(AC, LDY, ABS)  OP(AD, LDA, ABS)  OP(AE, LDX, ABS)  OP(AF, LAX, ABS)
		OP(B0, BCS, REL)  OP(B1, LDA, INY)  OP(B2, STP, UNK)  OP(B3, LAX, INY)  OP(B4, LDY, ZPX)  OP(B5, LDA, ZPX)  OP(B6, LDX, ZPY)  OP(B7, LAX, ZPY)  OP(B8, CLV, IMP)  OP(B9, LDA, ABY)  OP(BA, TSX, IMP)  OP(BB, LAS, ABY)  OP(BC, LDY, ABX)  OP(BD, LDA, ABX)  OP(BE, LDX, ABY)  OP(BF, LAX, ABY)
		OP(C0, CPY, IMM)  OP(C1, CMP, INX)  OP(C2, NOP, IMM)  OP(C3, DCP, INX)  OP(C4, CPY, ZPG)  OP(C5, CMP, ZPG)  OP(C6, DEC, ZPG)  OP(C7, DCP, ZPG)  OP(C8, INY, IMP)  OP(C9, CMP, IMM)  OP(CA, DEX, IMP)  OP(CB, AXS, IMM)  OP(CC, CPY, ABS)  OP(CD, CMP, ABS)  OP(CE, DEC, ABS)  OP(CF, DCP, ABS)
		OP(D0, BNE, REL)  OP(D1, CMP, INY)  OP(D2, STP, UNK)  OP(D3, DCP, INY)  OP(D4, NOP, ZPX)  OP(D5, CMP, ZPX)  OP(D6, DEC, ZPX)  OP(D7, DCP, ZPX)  OP(D8, CLD, IMP)  OP(D9, CMP, ABY)  OP(DA, NOP, IMP)  OP(DB, DCP, ABY)  OP(DC, NOP, ABX)  OP(DD, CMP, ABX)  OP(DE, DEC, ABX)  OP(DF, DCP, ABX)
		OP(E0, CPX, IMM)  OP(E1, SBC, INX)  OP(E2, NOP, IMM)  OP(E3, ISC, INX)  OP(E4, CPX, ZPG)  OP(E5, SBC, ZPG)  OP(E6, INC, ZPG)  OP(E7, ISC, ZPG)  OP(E8, INX, IMP)  OP(E9, SBC, IMM)  OP(EA, NOP, IMP)  OP(EB, SBC, IMM)  OP(EC, CPX, ABS)  OP(ED, SBC, ABS)  OP(EE, INC, ABS)  OP(EF, ISC, ABS)
		OP(F0, BEQ, REL)  OP(F1, SBC, INY)  OP(F2, STP, UNK)  OP(F3, ISC, INY)  OP(F4, NOP, ZPX)  OP(F5, SBC, ZPX)  OP(F6, INC, ZPX)  OP(F7, ISC, ZPX)  OP(F8, SED, IMP)  OP(F9, SBC, ABY)  OP(FA, NOP, IMP)  OP(FB, ISC, ABY)  OP(FC, NOP, ABX)  OP(FD, SBC, ABX)  OP(FE, INC, ABX)  OP(FF, ISC, ABX)
	}
}

void sfc_cpu_nmi(sfc_famicom_t* famicom)
{
	SFC_PUSH(SFC_PC >> 8);
	SFC_PUSH(SFC_PC & 0xFF);
	SFC_PUSH(SFC_P);
	SFC_IF_SET;
	uint16_t nmil = SFC_READ(SFC_VECTOR_NMI);
	uint16_t nmih = SFC_READ(SFC_VECTOR_NMI + 1);
	SFC_PC = nmil | (nmih << 8);
}