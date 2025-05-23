#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "sfc_famicom.h"
#include "sfc_cpu.h"

extern uint8_t* sfc_get_dma_address(uint16_t address, sfc_famicom_t* famicom)
{
	switch (address >> 13)
	{
	case 0:
		return famicom->main_memory + (address & (uint16_t)0x07FF);
	case 1:
		assert(!"PPU REG!");
		return NULL;
	case 2:
		assert(!"TODO");
		return NULL;
	case 3:
		return famicom->save_memory + (address & (uint16_t)0x1FFF);
	case 4:
	case 5:
	case 6:
	case 7:
		return famicom->prg_banks[address >> 13] + (address & (uint16_t)0x1FFF);
	default:
		assert(!"invalid address");
		return NULL;
	}
}

extern uint8_t sfc_read_cpu_address4020(uint16_t address, sfc_famicom_t* famicom)
{
	uint8_t data = 0;
	switch (address & 0x1F)
	{
	case 0x16:
		if (famicom->button_index_1 >= 0)
			data = famicom->button_states[(famicom->button_index_1++) & 0xF];
		break;
	case 0x17:
		if (famicom->button_index_2 >= 0)
			data = famicom->button_states[(8 + famicom->button_index_2++) & 0xF];
		break;
	}

	return data;
}

extern void sfc_write_cpu_address4020(uint16_t address, uint8_t data, sfc_famicom_t* famicom)
{
	switch (address & 0x1F)
	{
	case 0x14:
		if (famicom->ppu.registers.sprite_pt)
		{
			int count = famicom->ppu.registers.sprite_pt;
			memcpy(famicom->ppu.sprite_ram + count, (const void*)sfc_get_dma_address((uint16_t)data << 8, famicom), 256 - count);
			memcpy(famicom->ppu.sprite_ram, (const void*)(sfc_get_dma_address((uint16_t)data << 8, famicom) + count), count);
		}
		else
		{
			memcpy(famicom->ppu.sprite_ram, (const void*)sfc_get_dma_address((uint16_t)data << 8, famicom), 256);
		}
		famicom->cpu_cycle_count += 513;
		famicom->cpu_cycle_count += (famicom->cpu_cycle_count & 1) ? 1 : 0;
		break;
	case 0x16:
		if (data & 0x1)
			famicom->button_index_1 = famicom->button_index_2 = -1;
		else
			famicom->button_index_1 = famicom->button_index_2 = 0;
		break;
	}
}