#include <string.h>
#include "sfc_cpu.h"
#include "sfc_famicom.h"

static sfc_ecode sfc_mapper_00_reset(sfc_famicom_t* famicom)
{
	famicom->prg_banks[4] = famicom->rom.prg_rom;
	famicom->prg_banks[5] = famicom->rom.prg_rom + 8 * 1024;
	famicom->prg_banks[6] = famicom->rom.prg_rom + (famicom->rom.info.count_prgrom_16kb - 1) * 16 * 1024;
	famicom->prg_banks[7] = famicom->rom.prg_rom + (famicom->rom.info.count_prgrom_16kb - 1) * 16 * 1024 + 8 * 1024;

	return SFC_ERROR_OK;
}

sfc_ecode sfc_load_mapper_00(sfc_famicom_t* famicom)
{
	famicom->mapper.reset = sfc_mapper_00_reset;
	return SFC_ERROR_OK;
}

#define SFC_CASE_LOAD_MAPPER(id) case 0x##id: return sfc_load_mapper_##id(famicom);

extern sfc_ecode sfc_load_mapper(sfc_famicom_t* famicom, uint8_t id)
{
	switch (id)
	{
		SFC_CASE_LOAD_MAPPER(00)
	}
	return SFC_ERROR_MAPPER_NOT_FOUND;
}