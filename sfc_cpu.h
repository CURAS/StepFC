#pragma once
#include <stdint.h>

struct sfc_famicom;
typedef struct sfc_famicom sfc_famicom_t;

enum sfc_cpu_vector {
    SFC_VERCTOR_NMI = 0xFFFA,
    SFC_VERCTOR_RESET = 0xFFFC,
    SFC_VERCTOR_IRQBRK = 0xFFFE,
};

uint8_t sfc_read_cpu_address(uint16_t, const sfc_famicom_t*);

void sfc_write_cpu_address(uint16_t, uint8_t, sfc_famicom_t*);
