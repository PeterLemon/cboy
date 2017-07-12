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
#include "input.h"
#include "cpu.h"
#include "memory.h"
#include "audio.h"
#include "video.h"
#include "cart.h"
#include "main.h"
#include "audio.h"
#include "libc.h"

int pause = 0;

void main() {
    mem_init();
    audio_init();
    if( !cpu_init() )
    {
        return;
    }
    cart_init();
    vid_init();
    input_init();
    while( 1 )
    {
        vid_waitForNextFrame();
        input_handle();
        if( !pause )
        {
            cpu_do_one_frame();
            vid_frame();
            audio_frame();
        }
    }
    __builtin_unreachable();
}

