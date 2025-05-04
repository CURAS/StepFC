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

		char b0[SFC_DISASSEMBLY_BUF_LEN2];
		char b1[SFC_DISASSEMBLY_BUF_LEN2];
		char b2[SFC_DISASSEMBLY_BUF_LEN2];

		sfc_fc_disassembly(v0, &famicom, b0);
		sfc_fc_disassembly(v1, &famicom, b1);
		sfc_fc_disassembly(v2, &famicom, b2);

		printf(
			"NMI:     %s\n"
			"RESET:   %s\n"
			"IRQ/BRK: %s\n",
			b0, b1, b2
		);

		sfc_famicom_uninit(&famicom);
	}

	getchar();

	return 0;
}
