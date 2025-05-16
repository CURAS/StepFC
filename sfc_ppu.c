#include <assert.h>
#include "sfc_ppu.h"
#include "sfc_famicom.h"

uint8_t sfc_read_ppu_address(uint16_t address, sfc_ppu_t* ppu)
{
	address &= 0x3FFF;
	if (address >= 0x3F00)
	{
		uint8_t palette_index = address & 0x1F;
		ppu->registers.vram_cache = ppu->palette_indexes[palette_index];
		return ppu->palette_indexes[palette_index];
	}
	else
	{
		uint8_t data = ppu->registers.vram_cache;
		ppu->registers.vram_cache = ppu->banks[address >> 10][address & 0x3FF];
		return data;
	}
}

void sfc_write_ppu_address(uint16_t address, uint8_t data, sfc_ppu_t* ppu)
{
	address &= 0x3FFF;
	if (address >= 0x3F00)
	{
		uint8_t palette_index = address & 0x1F;
		if ((palette_index & 0x3) == 0)
		{
			ppu->palette_indexes[palette_index & 0x0F] = data;
			ppu->palette_indexes[palette_index | 0x10] = data;
		}
		else
		{
			ppu->palette_indexes[palette_index] = data;
		}
	}
	else
	{
		ppu->banks[address >> 10][address & 0x3FF] = data;
	}
}

uint8_t sfc_read_ppu_register_via_cpu(uint16_t address, sfc_ppu_t* ppu)
{
	uint8_t data = 0;
	switch (address & 0x7)
	{
	case 0:
	case 1:
		assert(!"write only");
		break;
	case 2:
		data = ppu->registers.status;
		ppu->registers.status &= (~SFC_PPU_FLAG_VBLANK);
		break;
	case 3:
		assert(!"write only");
		break;
	case 4:
		data = ppu->sprite_ram[ppu->registers.sprite_pt++];
		break;
	case 5:
	case 6:
		assert(!"write only");
		break;
	case 7:
		data = sfc_read_ppu_address(ppu->registers.vram_pt, ppu);
		(ppu->registers.ctrl & SFC_PPU_FLAG_VINC32) ? (ppu->registers.vram_pt += 32) : (ppu->registers.vram_pt++);
		break;
	}
	return data;
}

void sfc_write_ppu_register_via_cpu(uint16_t address, uint8_t data, sfc_ppu_t* ppu)
{
	switch (address & 0x7)
	{
	case 0:
		ppu->registers.ctrl = data;
		break;
	case 1:
		ppu->registers.mask = data;
		break;
	case 2:
		assert(!"read only");
		break;
	case 3:
		ppu->registers.sprite_pt = data;
		break;
	case 4:
		ppu->sprite_ram[ppu->registers.sprite_pt++] = data;
		break;
	case 5:
		ppu->registers.scroll[ppu->registers.scroll_pos++] = data;
		ppu->registers.scroll_pos &= 0x1;
		break;
	case 6:
		if (ppu->registers.vram_pt_pos == 0)
		{
			ppu->registers.vram_pt &= 0x00FF;
			ppu->registers.vram_pt |= (data << 8);
			ppu->registers.vram_pt_pos = 1;
		}
		else
		{
			ppu->registers.vram_pt &= 0xFF00;
			ppu->registers.vram_pt |= data;
			ppu->registers.vram_pt_pos = 0;
		}
		break;
	case 7:
		sfc_write_ppu_address(ppu->registers.vram_pt, data, ppu);
		(ppu->registers.ctrl & SFC_PPU_FLAG_VINC32) ? (ppu->registers.vram_pt += 32) : (ppu->registers.vram_pt++);
		break;
	}
}
