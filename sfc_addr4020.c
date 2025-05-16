#include <assert.h>
#include <stdint.h>
#include "sfc_famicom.h"
#include "sfc_cpu.h"

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
	case 0x16:
		if (data & 0x1)
			famicom->button_index_1 = famicom->button_index_2 = -1;
		else
			famicom->button_index_1 = famicom->button_index_2 = 0;
		break;
	}
}