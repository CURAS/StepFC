#include <stdio.h>
#include "sfc_famicom.h"
#include "sfc_cpu.h"

int main(int argc, char const* argv[])
{
	const char* file_path = ".\\nestest.nes";
	if (argc > 1)
		file_path = argv[1];

	sfc_famicom_t famicom;
	if (sfc_famicom_init(&famicom, file_path, NULL) == SFC_ERROR_OK)
	{
		printf("ROM: PRG-ROM: %d x 16kb\nCHR-ROM %d x 8kb\nMapper: %03d\n",
			famicom.rom.info.count_prgrom_16kb,
			famicom.rom.info.count_chrrom_8kb,
			famicom.rom.info.mapper_number);

		uint16_t v0 = sfc_read_cpu_address(SFC_VERCTOR_NMI + 0, &famicom);
		v0 |= sfc_read_cpu_address(SFC_VERCTOR_NMI + 1, &famicom) << 8;
		uint16_t v1 = sfc_read_cpu_address(SFC_VERCTOR_RESET + 0, &famicom);
		v1 |= sfc_read_cpu_address(SFC_VERCTOR_RESET + 1, &famicom) << 8;
		uint16_t v2 = sfc_read_cpu_address(SFC_VERCTOR_IRQBRK + 0, &famicom);
		v2 |= sfc_read_cpu_address(SFC_VERCTOR_IRQBRK + 1, &famicom) << 8;

		printf("ROM: NMI: $%04X  RESET: $%04X  IRQ/BRK: $%04X\n",
			v0, v1, v2);

		sfc_famicom_uninit(&famicom);
	}

	getchar();

	return 0;
}
