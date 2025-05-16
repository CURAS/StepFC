#include <assert.h>
#include <string.h>
#include "sfc_cpu.h"
#include "sfc_ppu.h"
#include "sfc_famicom.h"
#include "sfc_6502.h"

extern inline void sfc_btoh(char[], uint8_t);

void sfc_fc_disassembly(uint16_t address, sfc_famicom_t* famicom, char buf[SFC_DISASSEMBLY_BUF_LEN2])
{
	sfc_6502_code_t code;
	code.data = 0;
	code.op = sfc_read_cpu_address(address, famicom);
	if (inslen[code.op] >= 2)
		code.a1 = sfc_read_cpu_address(address + 1, famicom);
	if (inslen[code.op] >= 3)
		code.a2 = sfc_read_cpu_address(address + 2, famicom);

	memset(buf, ' ', SFC_DISASSEMBLY_BUF_LEN2);
	buf[SFC_DISASSEMBLY_BUF_LEN2 - 1] = '\0';

	buf[0] = '$';
	sfc_btoh(buf + 1, address >> 8);
	sfc_btoh(buf + 3, address & 0xFF);

	sfc_6502_disassembly(code, buf + 8);
}

uint8_t sfc_read_cpu_address(uint16_t address, sfc_famicom_t* famicom)
{
	switch (address >> 13)
	{
	case 0:
		return famicom->main_memory[address & (uint16_t)0x07FF];
	case 1:
		return sfc_read_ppu_register_via_cpu(address, &famicom->ppu);
	case 2:
		if (address < 0x4020)
			;
		else
			assert(!"NOT IMPL");
		return 0;
	case 3:
		return famicom->save_memory[address & (uint16_t)0x1FFF];
	case 4:
	case 5:
	case 6:
	case 7:
		return famicom->prg_banks[address >> 13][address & (uint16_t)0x1FFF];
	default:
		assert(!"invalid address");
		return 0;
	}
}

void sfc_write_cpu_address(uint16_t address, uint8_t data, sfc_famicom_t* famicom)
{
	switch (address >> 13)
	{
	case 0:
		famicom->main_memory[address & (uint16_t)0x07FF] = data;
		return;
	case 1:
		sfc_write_ppu_register_via_cpu(address, data, &famicom->ppu);
		return;
	case 2:
		if (address < 0x4020)
			;
		else
			assert(!"NOT IMPL");
		return;
	case 3:
		famicom->save_memory[address & (uint16_t)0x1FFF] = data;
		return;
	case 4:
	case 5:
	case 6:
	case 7:
		famicom->prg_banks[address >> 13][address & (uint16_t)0x1FFF] = data;
		return;
	default:
		assert(!"invalid address");
		return;
	}
}
