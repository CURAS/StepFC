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

		do {
			for (int i = 0; i < 1000; ++i)
				sfc_cpu_execute_one(&famicom);
		} while (getchar() != -1);

		sfc_famicom_uninit(&famicom);
	}

	return 0;
}
