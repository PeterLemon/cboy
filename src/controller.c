#include <stdint.h>

#include "controller.h"

extern volatile struct {
  uint32_t DRAM_ADDR;
  uint32_t PIF_ADDR_RD64B;
  uint32_t reserved0;
  uint32_t reserved1;
  uint32_t PIF_ADDR_WR64B;
  uint32_t reserved2;
  uint32_t STATUS;
} SI;

extern const volatile struct {
  unsigned char ROM[0x7C0];
  unsigned char RAM[0x40];
} PIF;

static uint32_t pif_cmds[16] __attribute__ ((aligned(8))) = {
  0xFF010401, 0xFFFFFFFF,
  0xFE000000, 0,
  0, 0,
  0, 0,
  0, 0,
  0, 0,
  0, 0,
  0, 1
};

void init_controller(void) {
  SI.DRAM_ADDR = (uint32_t)&pif_cmds;
  SI.PIF_ADDR_WR64B = (uint32_t)&PIF.RAM;
}

uint32_t read_controller(void) {
  SI.DRAM_ADDR = (uint32_t)&pif_cmds;
  SI.PIF_ADDR_RD64B = (uint32_t)&PIF.RAM;
  while(SI.STATUS & 1);
  return pif_cmds[1] >> 16;
}