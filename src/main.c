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

int stop = 0;
int pause = 0;

/*  If you want to add a new command:
        1. Declare a function that takes argc and argv as arguments.
        2. Add an entry to the cmds[] array.
        3. Implement the function at the end of this file.
*/

struct command {
    const char *name;
    int (*func)(int, char**);
    const char *help_short;
    const char *help_long;
    int end;    // 0 (default) for commands, 1 for end-of-array sentinel
};
void print_usage();
int main(int, char**);
int cmd_help(int, char**);
int cmd_exit(int, char**);
int cmd_info(int, char**);

struct command cmds[] = {
    {
        .name = "help",
        .func = cmd_help,
        .help_short = "Get help for a command.",
        .help_long = "help <command>\n\tget help for a command",
    },
    {
        .name = "run",
        .func = main,
        .help_short = "Run a Game Boy ROM in the emulator.",
        .help_long = "run <romfile>\n\trun romfile in the Game Boy emulator",
    },
    {
        .name = "info",
        .func = cmd_info,
        .help_short = "Print ROM header info.",
        .help_long = "info <romfile>\n\tprint meaning of header fields in romfile",
    },
    {
        .name = "",
        .func = cmd_exit,
        .help_short = "",
        .help_long = "",
        .end = 1, // end-of-array sentinel
    },
};

int get_cmd_index_for_name( char *needle )
{
    int cmds_index = 0;
    while( !cmds[cmds_index].end )
    {
        if( !strcmp( cmds[cmds_index].name, needle ) )
        {
            return cmds_index;
        }
        cmds_index++;
    }

    return -1;
}

void print_usage()
{
    int cmds_index=0;
    while( !cmds[cmds_index].end )
    {
        cmds_index++;
    }
}

int cmd_help( int argc, char *argv[] )
{
    if( argc < 3 )
    {
        return 1;
    }

    char *command_name = argv[2];
    int cmds_index=0;
    while( !cmds[cmds_index].end )
    {
        if( !strcmp( cmds[cmds_index].name, command_name ) )
        {
            return 0;
        }
        cmds_index++;
    }
    return 1;
}

int main( int argc, char *argv[] )
{
    mem_init();
    audio_init();
    if( !cpu_init() )
    {
        return 1;
    }
    cart_init( NULL, NULL );
    vid_init();
    input_init();
    while( !stop )
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

    cart_cleanup();
    audio_cleanup();

    return 0;
}

int cmd_exit( int argc, char *argv[] )
{
    return 0;
}

int cmd_info( int argc, char *argv[] )
{
    // implemented in gbinfo.c
    extern int cmd_info_impl(int, char**);
    return cmd_info_impl( argc, argv );
}
