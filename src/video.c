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
#include "video.h"
#include "memory.h"
#include "cpu.h"
#include <fbtext.h>
#include <rcp/vi.h>

pixel_t pixmem[160*144] __attribute__((aligned(16)));
pixel_t colormem[160*144];

void vid_drawOpaqueSpan( uint8_t pal, uint16_t vramAddr, int x, int y, int vramBank, int xFlip, int updateColormem ) {

    // Is this span off the left of the screen?
    if(x<-7)
        return;

    // Is this span off the top of the screen?
    if(y<0)
        return;

    if((x-8)>160)
        return;

    // Set up local palette.
    // FIXME The names of these variables are fucking confusing
    pixel_t myPalette[4];
    if( state.caps & 0x04 )
    {
        // DMG mode
        // colors need to be translated through BOTH the DMG and CGB palettes
        pixel_t cgbPalette[4];
        cgbPalette[0] = state.bgpd[(pal*8)+0] + (state.bgpd[(pal*8)+1]<<8);
        cgbPalette[1] = state.bgpd[(pal*8)+2] + (state.bgpd[(pal*8)+3]<<8);
        cgbPalette[2] = state.bgpd[(pal*8)+4] + (state.bgpd[(pal*8)+5]<<8);
        cgbPalette[3] = state.bgpd[(pal*8)+6] + (state.bgpd[(pal*8)+7]<<8);

        myPalette[0] = cgbPalette[ (state.bgp)      & 0x3 ];
        myPalette[1] = cgbPalette[ (state.bgp >> 2) & 0x3 ];
        myPalette[2] = cgbPalette[ (state.bgp >> 4) & 0x3 ];
        myPalette[3] = cgbPalette[ (state.bgp >> 6) & 0x3 ];
    } else {
        // CGB mode
        myPalette[0] = state.bgpd[(pal*8)+0] + (state.bgpd[(pal*8)+1]<<8);
        myPalette[1] = state.bgpd[(pal*8)+2] + (state.bgpd[(pal*8)+3]<<8);
        myPalette[2] = state.bgpd[(pal*8)+4] + (state.bgpd[(pal*8)+5]<<8);
        myPalette[3] = state.bgpd[(pal*8)+6] + (state.bgpd[(pal*8)+7]<<8);
    }


    int lineStart = 160 * y;
    //   uint32_t *pixmem = (uint32_t*) screen->pixels;


    // fill pixel array with span
    pixel_t pixels[8];    // pixel 7 at the left, pixel 0 at the right
    pixel_t colors[8];
    int lowBits, highBits;
    if( vramBank == 0 )
    {
        lowBits = vram_bank_zero[vramAddr];
        highBits = vram_bank_zero[vramAddr + 1];
    }
    else
    {
        lowBits = vram_bank_one[vramAddr];
        highBits = vram_bank_one[vramAddr + 1];
    }

    int p;
    for( p=0; p<8; ++p )
    {
        int color;
        int mask;
        color = 0;
        mask = 1<<p;
        if( lowBits & mask )
            color = 1;
        if( highBits & mask )
            color += 2;
        pixels[p] = myPalette[color];
        colors[p] = color;
    }

    // Is the span partially offscreen?
    int spanStart = 0;
    int spanEnd = 7;
    // Partially off the left side of the screen
    if( x < 0 )
        spanStart = -x;
    // Partially off the right side of the screen
    if( x > (160-8) )
        spanEnd = 159 - x;

    // xFlip?
    if( xFlip )
    {
        // Flip the span.
        pixel_t temp_pixels[8];
        pixel_t temp_colors[8];

        int i;
        for(i=0; i<8; ++i)
        {
            // Copy pixels to a temporary place, in reverse order.
            temp_pixels[i] = pixels[7-i];
            temp_colors[i] = colors[7-i];
        }
        for(i=0; i<8; ++i)
        {
            pixels[i] = temp_pixels[i];
            colors[i] = temp_colors[i];
        }
    }

    // Draw the span from left to right.
    for( p=spanStart; p<=spanEnd; ++p )
    {
        pixmem[ lineStart + x + p ] = pixels[ 7-p ];
    }
    if( updateColormem )
    {
        for( p=spanStart; p<=spanEnd; ++p )
        {
            colormem[ lineStart + x + p ] = colors[ 7-p ];
        }
    }

}

void vid_drawTransparentSpan( uint8_t pal, uint16_t vramAddr, int x, int y, int vramBank, int xFlip, int priority ) {

    // Is this span off the left of the screen?
    if(x<-7)
        return;

    // Is this span off the top of the screen?
    if(y<0)
        return;

    if((x-8)>160)
        return;

    // Set up local palette.
    // FIXME The names of these variables are fucking confusing
    pixel_t myPalette[4];
    if( state.caps & 0x04 )
    {
        // DMG mode
        // colors need to be translated through BOTH the DMG and CGB palettes
        pixel_t cgbPalette[4];
        cgbPalette[0] = state.obpd[(pal*8)+0] + (state.obpd[(pal*8)+1]<<8);
        cgbPalette[1] = state.obpd[(pal*8)+2] + (state.obpd[(pal*8)+3]<<8);
        cgbPalette[2] = state.obpd[(pal*8)+4] + (state.obpd[(pal*8)+5]<<8);
        cgbPalette[3] = state.obpd[(pal*8)+6] + (state.obpd[(pal*8)+7]<<8);

        int dmgPalette = (pal==0) ? state.obp0 : state.obp1;

        myPalette[0] = cgbPalette[ (dmgPalette)      & 0x3 ];
        myPalette[1] = cgbPalette[ (dmgPalette >> 2) & 0x3 ];
        myPalette[2] = cgbPalette[ (dmgPalette >> 4) & 0x3 ];
        myPalette[3] = cgbPalette[ (dmgPalette >> 6) & 0x3 ];
    } else {
        // CGB mode
        myPalette[0] = state.obpd[(pal*8)+0] + (state.obpd[(pal*8)+1]<<8);
        myPalette[1] = state.obpd[(pal*8)+2] + (state.obpd[(pal*8)+3]<<8);
        myPalette[2] = state.obpd[(pal*8)+4] + (state.obpd[(pal*8)+5]<<8);
        myPalette[3] = state.obpd[(pal*8)+6] + (state.obpd[(pal*8)+7]<<8);
    }

    int lineStart = 160 * y;


    // fill pixel array with span
    pixel_t pixels[8];    // pixel 7 at the left, pixel 0 at the right
    pixel_t colors[8];
    int lowBits, highBits;
    if( vramBank == 0 )
    {
        lowBits = vram_bank_zero[vramAddr];
        highBits = vram_bank_zero[vramAddr + 1];
    }
    else
    {
        lowBits = vram_bank_one[vramAddr];
        highBits = vram_bank_one[vramAddr + 1];
    }

    int p;
    for( p=0; p<8; ++p )
    {
        int color;
        int mask;
        color = 0;
        mask = 1<<p;
        if( lowBits & mask )
            color = 1;
        if( highBits & mask )
            color += 2;
        colors[p] = color;
        pixels[p] = myPalette[color];
    }

    // Is the span partially offscreen?
    int spanStart = 0;
    int spanEnd = 7;
    // Partially off the left side of the screen
    if( x < 0 )
        spanStart = -x;
    // Partially off the right side of the screen
    if( x > (160-8) )
        spanEnd = 159 - x;

    // xFlip?
    if( xFlip )
    {
        // Flip the span.
        pixel_t temp_pixels[8];
        pixel_t temp_colors[8];

        int i;
        for(i=0; i<8; ++i)
        {
            // Copy pixels to a temporary place, in reverse order.
            temp_pixels[i] = pixels[7-i];
            temp_colors[i] = colors[7-i];
        }
        for(i=0; i<8; ++i)
        {
            pixels[i] = temp_pixels[i];
            colors[i] = temp_colors[i];
        }
    }


    // Draw the span from left to right.
    for( p=spanStart; p<=spanEnd; ++p )
    {
        if( colors[7-p] != 0 )
        {
            if( priority )
            {
                if( colormem[ lineStart + x + p ] == 0 )
                {
                    pixmem[ lineStart + x + p ] = pixels[ 7-p ];
                } else {
                    // Uncomment the next line to highlight the
                    // sprite-behind-background case in bright red.
                    //           pixmem[ lineStart + x + p ] = 0x001f; // red
                }
            } else {
                pixmem[ lineStart + x + p ] = pixels[ 7-p ];
            }
        }
    }

}

void vid_render_line()
{
    // If the LCD is off, then return.
    // We should probably blank the line instead...
    if( (state.lcdc & LCDC_LCD_ENABLE) == 0 )
        return;

    int backLineToRender = ((int)state.ly + (int)state.scy) % 256;
    int backTileRow = backLineToRender / 8;
    int backLineInTile = backLineToRender % 8;

    int i, tileNum, tileAddress, tileDataAddress, tile, spanAddress, vramBank;

    // Render the background.
    if( state.lcdc & LCDC_BG_ENABLE )
        for( i=-1; i<21; ++i )
        {
            tileNum = backTileRow*32 + (i + (state.scx >> 3))%32;


            if( state.lcdc & LCDC_BG_TILE_MAP_SELECT )
                tileAddress = 0x1C00 + tileNum;
            else
                tileAddress = 0x1800 + tileNum;


            if( state.lcdc & LCDC_TILE_DATA_SELECT )
            {
                tile = vram_bank_zero[tileAddress];
                tileDataAddress = 0x0000 + tile*16;
            }
            else
            {
                tile = vram_bank_zero[tileAddress];
                if(tile < 0x80)
                    tileDataAddress = 0x1000 + tile*16;
                else
                    tileDataAddress = tile*16;
            }


            uint8_t attributes = vram_bank_one[tileAddress];
            int xFlip = (attributes&BG_XFLIP)?1:0;
            int yFlip = (attributes&BG_YFLIP)?1:0;

            if( yFlip )
                spanAddress = tileDataAddress + ((7-backLineInTile) * 2);
            else
                spanAddress = tileDataAddress + (backLineInTile * 2);

            // Set the VRAM bank.
            if( state.caps == 0x04 )
                vramBank = 0;
            else
                vramBank = (attributes & BG_VRAM_BANK)?1:0;

            // Set the palette.
            uint8_t pal;
            if( state.caps == 0x04 )
                pal = 0;    // dmg mode
            else
                pal = attributes & 0x07;    // cgb mode

            vid_drawOpaqueSpan( pal, spanAddress, i*8 - (state.scx%8), state.ly, vramBank, xFlip, 1 );
        }

    // Render the window.
    int windowLineToRender = (int)state.ly - (int)state.wy;
    int windowTileRow = windowLineToRender / 8;
    int windowLineInTile = windowLineToRender % 8;
    if( (state.lcdc & LCDC_WINDOW_ENABLE) && (state.ly >= state.wy) )
    {
        for( i=0; i<20; ++i )
        {
            tileNum = windowTileRow*32 + i;

            if( state.lcdc & LCDC_WINDOW_TILE_MAP_SELECT )
                tileAddress = 0x1C00 + tileNum;
            else
                tileAddress = 0x1800 + tileNum;


            if( state.lcdc & LCDC_TILE_DATA_SELECT )
            {
                tile = vram_bank_zero[tileAddress];
                tileDataAddress = 0x0000 + tile*16;
            }
            else
            {
                tile = vram_bank_zero[tileAddress];
                if(tile < 0x80)
                    tileDataAddress = 0x1000 + tile*16;
                else
                    tileDataAddress = tile*16;
            }

            uint8_t attributes = vram_bank_one[tileAddress];
            int xFlip = (attributes&BG_XFLIP)?1:0;
            int yFlip = (attributes&BG_YFLIP)?1:0;

            if( yFlip )
                spanAddress = tileDataAddress + ((7-windowLineInTile) * 2);
            else
                spanAddress = tileDataAddress + (windowLineInTile * 2);

            // Set the VRAM bank.
            if( state.caps == 0x04 )
                vramBank = 0;
            else
                vramBank = (attributes & BG_VRAM_BANK)?1:0;

            // Set the palette.
            uint8_t pal;
            if( state.caps == 0x04 )
                pal = 0;        // dmg mode
            else
                pal = attributes & 0x07;        // cgb mode

            vid_drawOpaqueSpan( pal, spanAddress, i*8 + state.wx - 7, state.ly, vramBank, xFlip, 1 );
        }
    }

    int objLineToRender = ((int)state.ly) % 256;

    // Render sprites.
    if( state.lcdc & LCDC_OBJ_DISPLAY )
        for( i=0; i<40; ++i )
        {
            uint8_t y          = oam[(i<<2)    ];
            uint8_t x          = oam[(i<<2) + 1];
            uint8_t tileNum    = oam[(i<<2) + 2];
            uint8_t attributes = oam[(i<<2) + 3];

            // Set object height.
            int objHeight;
            if( state.lcdc & LCDC_OBJ_SIZE )
                objHeight = 16;
            else
                objHeight = 8;

            if(objHeight == 16)
            {
                tileNum &= 0xfe;
            }

            // Is this sprite on this line? If not, let's skip it.
            int reject = 0;
            int lineOfSpriteToRender = objLineToRender - y + 16;
            // yFlip is handled here
            if( attributes & SPRITE_YFLIP )
                lineOfSpriteToRender = objHeight - lineOfSpriteToRender - 1;
            if( lineOfSpriteToRender >= objHeight || lineOfSpriteToRender < 0  )
                reject = 1;
            if( reject == 1 )
                continue;

            // Set the palette.
            uint8_t pal;
            if( state.caps == 0x04 )
                if( attributes & SPRITE_DMG_PAL )   // dmg mode
                    pal = 1;
                else
                    pal = 0;
            else
                pal = attributes & 0x07;    // cgb mode

            // Set the VRAM bank.
            if( state.caps == 0x04 )
                vramBank = 0;   // dmg mode
            else
                vramBank = (attributes & SPRITE_VRAM_BANK)?1:0; // cgb mode

            // Set the priority. 0=sprite above background, 1=sprite behind bg color 1-3
            int priority = attributes & SPRITE_PRIORITY;

            // Calculate the span address, relative to 0x8000.
            spanAddress = (tileNum * 16) + (lineOfSpriteToRender*2);

            int xFlip = attributes & SPRITE_XFLIP;
            vid_drawTransparentSpan( pal, spanAddress, x-8, state.ly, vramBank, xFlip, priority );
        }


}

// These pre-defined values are suitable for NTSC.
// TODO: Add support for PAL and PAL-M televisions.
static vi_state_t vi_state = {
  0x0000324E, // status
  0x00200000, // origin
  0x00000140, // width
  0x00000002, // intr
  0x00000000, // current
  0x03E52239, // burst
  0x0000020D, // v_sync
  0x00000C15, // h_sync
  0x0C150C15, // leap
  0x006C02EC, // h_start
  0x002501FF, // v_start
  0x000E0204, // v_burst
  0x00000200, // x_scale
  0x00000400, // y_scale
};

struct libn64_fbtext_context fbtext;

void vid_init()
{
  vi_flush_state(&vi_state);

  for (unsigned i = 0; i < 320 * 240 * 2; i += 16) {
    __asm__ __volatile__(
      ".set gp=64\n\t"
      "cache 0xD, 0x0(%0)\n\t"
      "sd $zero, 0x0(%0)\n\t"
      "sd $zero, 0x8(%0)\n\t"
      "cache 0x19, 0x0(%0)\n\t"
      ".set gp=default\n\t"

      :: "r" (0x80000000 | (vi_state.origin + i))
      : "memory"
    );
  }

  libn64_fbtext_init(&fbtext, 0x200000, LIBN64_FBTEXT_COLOR_WHITE,
      LIBN64_FBTEXT_COLOR_BLACK, 0x140, LIBN64_FBTEXT_16BPP);

  fbtext.x = 13; fbtext.y = 1;
  libn64_fbtext_puts(&fbtext, "cboy by jrra\n");
  fbtext.x = 6; fbtext.y = 13;
  libn64_fbtext_puts(&fbtext, "n64chain port by marathonm\n");
}

void vid_waitForNextFrame()
{
}

void vid_frame()
{
  unsigned i, j;

  for (i = 0; i < 160; i++) {
    for (j = 0; j < 144; j += 8) {
      uint32_t clh1, clh2;

      __asm__ __volatile__(
        ".set noat\n\t"
        ".set gp=64\n\t"
        "ld %0, 0x0(%3)\n\t"
        "ld %1, 0x8(%3)\n\t"
        "cache 0xD, 0x0(%2)\n\t"
        "sd %0, 0x0(%2)\n\t"
        "sd %1, 0x8(%2)\n\t"
        ".set gp=default\n\t"
        ".set at\n\t"

        : "=&r" (clh1), "=&r" (clh2)
        : "r" (0x80000000 | (vi_state.origin + ((i + 48) * 320 * 2) + (j + 80) * 2)),
          "r" (pixmem + (i * 160) + j)
        : "memory"
      );
    }
  }
}
