#pragma once
#include <stdint.h>

struct sfc_famicom;
typedef struct sfc_famicom sfc_famicom_t;

enum sfc_cpu_vector {
	SFC_VECTOR_NMI = 0xFFFA,
	SFC_VECTOR_RESET = 0xFFFC,
	SFC_VECTOR_IRQBRK = 0xFFFE,
};

enum {
	SFC_DISASSEMBLY_BUF_LEN2 = 48
};

enum sfc_status_flag {
	SFC_FLAG_C = 1 << 0,
	SFC_FLAG_Z = 1 << 1,
	SFC_FLAG_I = 1 << 2,
	SFC_FLAG_D = 1 << 3,
	SFC_FLAG_B = 1 << 4,
	SFC_FLAG_R = 1 << 5,
	SFC_FLAG_V = 1 << 6,
	SFC_FLAG_S = 1 << 7,
};

typedef struct {
	uint16_t pc;
	uint8_t a;
	uint8_t x;
	uint8_t y;
	uint8_t s;
	uint8_t p;
	uint8_t reserved;
} sfc_cpu_register_t;

void sfc_fc_disassembly(uint16_t, sfc_famicom_t*, char[SFC_DISASSEMBLY_BUF_LEN2]);

uint8_t sfc_read_cpu_address(uint16_t, sfc_famicom_t*);

void sfc_write_cpu_address(uint16_t, uint8_t, sfc_famicom_t*);
