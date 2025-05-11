#pragma once
#include <stdint.h>

#pragma pack(1)
typedef struct {
	uint8_t tag[4];
	uint8_t count_prgrom_16kb;
	uint8_t count_chrrom_8kb;
	uint8_t flags6;
	uint8_t flags7;
	uint8_t reserved[8];
} sfc_rom_header_t;
#pragma pack()

typedef struct {
	uint8_t count_prgrom_16kb;
	uint8_t count_chrrom_8kb;
	uint8_t mapper_number;
	uint8_t four_screen_flag;
	uint8_t trainer_flag;
	uint8_t sram_flag;
	uint8_t mirror_flag;
	uint8_t playchoice10_flag;
	uint8_t vs_unisystem_flag;
} sfc_rom_info_t;

typedef struct {
	sfc_rom_info_t info;
	uint8_t* trainer_rom;
	uint8_t* prg_rom;
	uint8_t* chr_rom;
} sfc_rom_t;

enum flags6_bit {
	SFC_NES_VMIRROR = 0x01,
	SFC_NES_SAVERAM = 0x02,
	SFC_NES_TRAINER = 0x04,
	SFC_NES_4SCREEN = 0x08
};

enum flags7_bit {
	SFC_NES_VS_UNISYSTEM = 0x01,
	SFC_NES_Playchoice10 = 0x02
};