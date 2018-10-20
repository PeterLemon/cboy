#include <stdint.h>
#include "controller.h"
#include "n64.h"

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