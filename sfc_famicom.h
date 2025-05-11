#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "sfc_rom.h"
#include "sfc_cpu.h"

struct sfc_famicom;
typedef struct sfc_famicom sfc_famicom_t;

typedef enum sfc_error_code {
	SFC_ERROR_OK = 0,
	SFC_ERROR_FAILED,
	SFC_ERROR_MAPPER_NOT_FOUND,
	SFC_ERROR_FILE_NOT_FOUND,
	SFC_ERROR_ILLEGAL_FILE,
	SFC_ERROR_OUT_OF_MEMORY
} sfc_ecode;

typedef struct {
	sfc_ecode(*load_rom)(void*, sfc_rom_t*);
	sfc_ecode(*free_rom)(void*, sfc_rom_t*);
	void(*before_execute)(void*, sfc_famicom_t*);
} sfc_interface_t;

typedef struct {
	sfc_ecode(*reset)(sfc_famicom_t*);
} sfc_mapper_t;

struct sfc_famicom {
	sfc_rom_t rom;
	void* argument;
	sfc_interface_t interfaces;
	sfc_mapper_t mapper;
	sfc_cpu_register_t registers;
	uint8_t* prg_banks[0x10000 >> 13];
	uint8_t save_memory[8 * 1024];
	uint8_t main_memory[2 * 1024];
};

sfc_ecode sfc_famicom_init(sfc_famicom_t*, void*, const sfc_interface_t*);

sfc_ecode sfc_famicom_uninit(sfc_famicom_t*);

sfc_ecode sfc_famicom_reset(sfc_famicom_t*);