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
  state.joyp_buttons = 0xFF;
  state.joyp_directions = 0xFF;

  switch(read_controller()) {
    case JOY_START:
      state.joyp_buttons &= ~INPUT_BUTTONS_START;
      state.iflag |= IMASK_JOYPAD;
      state.halt = 0;
      break;
    case JOY_Z:
      state.joyp_buttons &= ~INPUT_BUTTONS_SELECT;
      state.iflag |= IMASK_JOYPAD;
      state.halt = 0;
      break;
    case JOY_A:
      state.joyp_buttons &= ~INPUT_BUTTONS_A;
      state.iflag |= IMASK_JOYPAD;
      state.halt = 0;
      break;
    case JOY_B:
      state.joyp_buttons &= ~INPUT_BUTTONS_B;
      state.iflag |= IMASK_JOYPAD;
      state.halt = 0;
      break;
    case JOY_UP:
      state.joyp_directions &= ~INPUT_DIRECTIONS_UP;
      state.iflag |= IMASK_JOYPAD;
      state.halt = 0;
      break;
    case JOY_DOWN:
      state.joyp_directions &= ~INPUT_DIRECTIONS_DOWN;
      state.iflag |= IMASK_JOYPAD;
      state.halt = 0;
      break;
    case JOY_LEFT:
      state.joyp_directions &= ~INPUT_DIRECTIONS_LEFT;
      state.iflag |= IMASK_JOYPAD;
      state.halt = 0;
      break;
    case JOY_RIGHT:
      state.joyp_directions &= ~INPUT_DIRECTIONS_RIGHT;
      state.iflag |= IMASK_JOYPAD;
      state.halt = 0;
      break;
    default:
      break;
  }
}
