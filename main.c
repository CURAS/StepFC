#define SFC_NO_SL

#include <stdio.h>
#include "common/d2d_interface.h"
#include "sfc_famicom.h"
#include "sfc_cpu.h"
#include "sfc_ppu.h"
#include "sfc_6502.h"

sfc_famicom_t* g_famicom = NULL;
uint32_t palette_data[16];

uint32_t get_pixel(unsigned x, unsigned y, const uint8_t* name_table, const uint8_t* pattern_table)
{
	unsigned name_table_pos = (x >> 3) + (y >> 3) * 32;
	unsigned name = name_table[name_table_pos];

	const uint8_t* pattern_bit0 = pattern_table + 16 * name;
	const uint8_t* pattern_bit1 = pattern_bit0 + 8;

	uint8_t offset = y & 0x7;
	uint8_t shift = 7 - (x & 0x7);

	uint8_t result = (pattern_bit0[offset] & (1 << shift)) >> shift;
	result |= (((pattern_bit1[offset] & (1 << shift)) >> shift) << 1);

	unsigned attr_table_pos = (x >> 5) + (y >> 5) * 8;
	uint8_t attr = name_table[32 * 30 + attr_table_pos];

	uint8_t shift2 = ((x >> 4) & 0x1) + ((y >> 4) & 0x1) * 2;
	uint8_t pattern_bit32 = (attr >> (shift2 * 2)) & 0x3;

	result |= (pattern_bit32 << 2);

	return palette_data[result];
}

void main_render(void* rgba)
{
	uint32_t* data = rgba;

	for (int i = 0; i != 10000; ++i)
		sfc_cpu_execute_one(g_famicom);

	sfc_do_vblank(g_famicom);

	for (int i = 0; i < 16; ++i)
		palette_data[i] = sfc_stdpalette[g_famicom->ppu.palette_indexes[i]].data;
	palette_data[4] = palette_data[8] = palette_data[12] = palette_data[0];

	const uint8_t* name_table = g_famicom->ppu.banks[8];
	const uint8_t* pattern_table = (!(g_famicom->ppu.registers.ctrl & SFC_PPU_FLAG_BGPATN)) ? g_famicom->ppu.banks[0] : g_famicom->ppu.banks[4];
	for (unsigned i = 0; i <= 256 * 240; ++i)
		data[i] = get_pixel(i & 0xff, i >> 8, name_table, pattern_table);
}

int sub_render(void* rgba)
{
	uint32_t* data = rgba;

	for (int i = 0; i < 16; ++i)
		palette_data[i] = sfc_stdpalette[g_famicom->ppu.palette_indexes[i + 16]].data;
	palette_data[4] = palette_data[8] = palette_data[12] = palette_data[0];

	for (unsigned i = 0; i <= 256 * 240; ++i)
		data[i] = palette_data[0];

	const uint8_t* pattern_table = (!(g_famicom->ppu.registers.ctrl & SFC_PPU_FLAG_SPPATN)) ? g_famicom->ppu.banks[0] : g_famicom->ppu.banks[4];
	for (int i = 63; i >= 0; --i)
	{
		uint8_t* ptr = g_famicom->ppu.sprite_ram + i * 4;
		uint8_t xx = ptr[3];
		uint8_t yy = ptr[0];
		uint8_t ii = ptr[1];
		uint8_t aa = ptr[2];

		if (yy >= 0xEF)
			continue;

		const uint8_t* pattern_bit0 = pattern_table + 16 * ii;
		const uint8_t* pattern_bit1 = pattern_bit0 + 8;

		uint8_t pattern_bit32 = aa & 0x3;

		const uint8_t forward[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
		const uint8_t reverse[] = { 7, 6, 5, 4, 3, 2, 1, 0 };
		const uint8_t* x_dir = forward;
		const uint8_t* y_dir = forward;
		if (aa & 0x40)
			x_dir = reverse;
		if (aa & 0x80)
			y_dir = reverse;

		for (uint8_t i = 0; i < 8; i++)
		{
			for (uint8_t j = 0; j < 8; j++)
			{
				uint8_t offset = i;
				uint8_t shift = 7 - j;
				uint8_t result = (pattern_bit0[offset] & (1 << shift)) >> shift;
				result |= (((pattern_bit1[offset] & (1 << shift)) >> shift) << 1);
				result |= (pattern_bit32 << 2);

				data[(yy + 1 + y_dir[i]) * 256 + (xx + x_dir[j])] = palette_data[result];
			}
		}
	}

	return 1;
}

void user_input(int index, unsigned char data)
{
	g_famicom->button_states[index] = data;
}

int main(int argc, char const* argv[])
{
	const char* file_path = ".\\nestest.nes";
	if (argc > 1)
		file_path = argv[1];

	sfc_famicom_t famicom;
	g_famicom = &famicom;
	if (sfc_famicom_init(&famicom, (void*)file_path, NULL) == SFC_ERROR_OK)
	{
		printf("ROM: PRG-ROM: %d x 16kb\nCHR-ROM %d x 8kb\nMapper: %03d\n",
			famicom.rom.info.count_prgrom_16kb,
			famicom.rom.info.count_chrrom_8kb,
			famicom.rom.info.mapper_number);

		main_cpp();

		sfc_famicom_uninit(&famicom);
	}

	return 0;
}
