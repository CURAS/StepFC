#include <string.h>
#include "sfc_famicom.h"

extern sfc_ecode sfc_load_mapper(sfc_famicom_t*, uint8_t);

static sfc_ecode sfc_default_load_rom(void*, sfc_rom_t*);
static sfc_ecode sfc_default_free_rom(void*, sfc_rom_t*);
static void sfc_default_before_execute(void*, sfc_famicom_t*);
static void sfc_log_before_execute(void*, sfc_famicom_t*);

static sfc_interface_t default_interface = {
	sfc_default_load_rom,
	sfc_default_free_rom,
	sfc_default_before_execute
};

static sfc_interface_t log_interface = {
	sfc_default_load_rom,
	sfc_default_free_rom,
	sfc_log_before_execute
};

sfc_ecode sfc_famicom_init(sfc_famicom_t* famicom, void* argument, const sfc_interface_t* interfaces)
{
	famicom->argument = argument;

	if (interfaces == NULL)
		famicom->interfaces = log_interface;

	memset(&famicom->prg_banks, 0, (0x10000 >> 13) * sizeof(uint8_t*));
	famicom->prg_banks[0] = famicom->main_memory;
	famicom->prg_banks[3] = famicom->save_memory;

	memset(&famicom->rom, 0, sizeof(sfc_rom_t));
	return sfc_load_new_rom(famicom);
}

sfc_ecode sfc_famicom_uninit(sfc_famicom_t* famicom)
{
	return famicom->interfaces.free_rom(famicom->argument, &famicom->rom);
}

sfc_ecode sfc_famicom_reset(sfc_famicom_t* famicom)
{
	sfc_ecode code = famicom->mapper.reset(famicom);
	if (code != SFC_ERROR_OK)
		return code;

	const uint8_t pcl = sfc_read_cpu_address(SFC_VERCTOR_RESET + 0, famicom);
	const uint8_t pch = sfc_read_cpu_address(SFC_VERCTOR_RESET + 1, famicom);
	famicom->registers.pc = (uint16_t)pcl | (uint16_t)(pch << 8);
	famicom->registers.a = 0;
	famicom->registers.x = 0;
	famicom->registers.y = 0;
	famicom->registers.p = SFC_FLAG_I | SFC_FLAG_R;
	famicom->registers.s = 0xfd;

#if 1
	famicom->registers.pc = 0xC000;
#endif

	return SFC_ERROR_OK;
}

sfc_ecode sfc_load_new_rom(sfc_famicom_t* famicom)
{
	sfc_ecode code = famicom->interfaces.free_rom(famicom->argument, &famicom->rom);
	memset(&famicom->rom, 0, sizeof(famicom->rom));

	if (code == SFC_ERROR_OK)
		code = famicom->interfaces.load_rom(famicom->argument, &famicom->rom);

	if (code == SFC_ERROR_OK)
		code = sfc_load_mapper(famicom, famicom->rom.info.mapper_number);

	if (code == SFC_ERROR_OK)
		sfc_famicom_reset(famicom);

	return code;
}

sfc_ecode sfc_default_load_rom(void* argument, sfc_rom_t* rom)
{
	const char* file_path = argument;

	FILE* file = fopen(file_path, "rb");
	if (file == NULL)
		return SFC_ERROR_FILE_NOT_FOUND;

	sfc_rom_header_t header;
	if (fread(&header, sizeof(sfc_rom_header_t), 1, file) < 1)
		return SFC_ERROR_ILLEGAL_FILE;

	if (header.tag[0] != 'N' || header.tag[1] != 'E' || header.tag[2] != 'S' || header.tag[3] != 0x1A)
		return SFC_ERROR_ILLEGAL_FILE;

	sfc_rom_info_t* info = &rom->info;
	info->count_chrrom_8kb = header.count_chrrom_8kb;
	info->count_prgrom_16kb = header.count_prgrom_16kb;
	info->mapper_number = (header.flags7 & 0xF0) | (header.flags6 >> 4);
	info->four_screen_flag = (header.flags6 & SFC_NES_4SCREEN) != 0;
	info->trainer_flag = (header.flags6 & SFC_NES_TRAINER) != 0;
	info->sram_flag = (header.flags6 & SFC_NES_SAVERAM) != 0;
	info->mirror_flag = (header.flags6 & SFC_NES_VMIRROR) != 0;
	info->playchoice10_flag = (header.flags7 & SFC_NES_Playchoice10) != 0;
	info->vs_unisystem_flag = (header.flags7 & SFC_NES_VS_UNISYSTEM) != 0;

	if (info->trainer_flag)
	{
		rom->trainer_rom = (uint8_t*)malloc(512);
		if (rom->trainer_rom == NULL)
			return SFC_ERROR_FAILED;
		fread(rom->trainer_rom, 512, 1, file);
	}

	rom->prg_rom = (uint8_t*)malloc(16 * 1024 * header.count_prgrom_16kb);
	if (rom->prg_rom == NULL)
		return SFC_ERROR_FAILED;
	fread(rom->prg_rom, 16 * 1024, header.count_prgrom_16kb, file);

	rom->chr_rom = (uint8_t*)malloc(8 * 1024 * header.count_chrrom_8kb);
	if (rom->chr_rom == NULL)
		return SFC_ERROR_FAILED;
	fread(rom->chr_rom, 8 * 1024, header.count_chrrom_8kb, file);

	return SFC_ERROR_OK;
}

sfc_ecode sfc_default_free_rom(void* argument, sfc_rom_t* rom)
{
	if (rom->info.trainer_flag)
	{
		free(rom->trainer_rom);
		rom->trainer_rom = NULL;
	}

	free(rom->prg_rom);
	rom->prg_rom = NULL;

	free(rom->chr_rom);
	rom->chr_rom = NULL;

	return SFC_ERROR_OK;
}

void sfc_default_before_execute(void* argument, sfc_famicom_t* info)
{
}

void sfc_log_before_execute(void* arg, sfc_famicom_t* famicom)
{
	static int line = 0;
	++line;

	char buf[SFC_DISASSEMBLY_BUF_LEN2];
	sfc_fc_disassembly(famicom->registers.pc, famicom, buf);
	printf("%4d - %s   A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
		line, buf,
		famicom->registers.a,
		famicom->registers.x,
		famicom->registers.y,
		famicom->registers.p,
		famicom->registers.s
	);
}