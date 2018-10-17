#ifndef __CONTROLLER_H
#define __CONTROLLER_H

#define JOY_CRIGHT 0x0001 // PIF HWORD: CAMERA RIGHT
#define JOY_CLEFT  0x0002 // PIF HWORD: CAMERA LEFT
#define JOY_CDOWN  0x0004 // PIF HWORD: CAMERA DOWN
#define JOY_CUP    0x0008 // PIF HWORD: CAMERA UP
#define JOY_R      0x0010 // PIF HWORD: R (PAN RIGHT)
#define JOY_L      0x0020 // PIF HWORD: L (PAN LEFT)
#define JOY_RIGHT  0x0100 // PIF HWORD: RIGHT
#define JOY_LEFT   0x0200 // PIF HWORD: LEFT
#define JOY_DOWN   0x0400 // PIF HWORD: DOWN
#define JOY_UP     0x0800 // PIF HWORD: UP
#define JOY_START  0x1000 // PIF HWORD: START
#define JOY_Z      0x2000 // PIF HWORD: Z
#define JOY_B      0x4000 // PIF HWORD: B
#define JOY_A      0x8000 // PIF HWORD: A

void init_controller(void);
uint32_t read_controller(void);

#endif
