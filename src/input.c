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

void input_init()
{
  // set up event filtering
  // TODO
  state.joyp = 0xFF;
  state.joyp_buttons = 0xFF;
  state.joyp_directions = 0xFF;
  state.joyp_select = INPUT_SELECT_BUTTONS;
}

void input_handle()
{
}
