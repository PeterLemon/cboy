/*************************************************************************
 *   Cboy, a Game Boy emulator
 *   Copyright (C) 2012 jkbenaim
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ************************************************************************/

#include "types.h"
#include "main.h"
#include "memory.h"
#include "input.h"
#include "cpu.h"
#include "controller.h"

void input_init()
{
  init_controller(); // Init N64 Controller

  // set up event filtering
  // TODO
  state.joyp = 0xFF;
  state.joyp_buttons = 0xFF;
  state.joyp_directions = 0xFF;
  state.joyp_select = INPUT_SELECT_BUTTONS;
}

void input_handle()
{
  uint32_t n64controller = read_controller() & 0xFF00;
  state.joyp_buttons = ~( ((n64controller & JOY_A) >> 15) | ((n64controller & JOY_B) >> 13) | ((n64controller & JOY_Z) >> 11) | ((n64controller & JOY_START) >> 9) );
  state.joyp_directions = ~( ((n64controller & JOY_UP) >> 9) | ((n64controller & JOY_DOWN) >> 7) | ((n64controller & (JOY_RIGHT+JOY_LEFT)) >> 8) );
  if(n64controller != 0) {
    state.iflag |= IMASK_JOYPAD;
    state.halt = 0;
  }
}
