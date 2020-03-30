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
#include <os/fbtext.h>
#include <rcp/vi.h>
#include "rdp.h"

static pixel_t pixmem[160*144] __attribute__((aligned(16)));
static pixel_t colormem[160*144] __attribute__((aligned(16)));

static pixel_t myCachedPalettes[8][8] __attribute__((aligned(16)));
static pixel_t pixelscolors[2][8] __attribute__((aligned(32)));
static pixel_t myPalette[8] __attribute__((aligned(16)));
char inval_palette = 1;

static uint32_t rdp_start, rdp_end;

__attribute__((hot))
static int vid_drawSpanCommon(pixel_t *palette, int vramAddr, int x, int y, int vramBank,
        int xFlip, int *lineStart, int *spanStart, int *spanEnd) {

    // Is this span off the left of the screen?
    if(x<-7)
        return 1;

    // Is this span off the top of the screen?
    if(y<0)
        return 1;

    if((x-8)>160)
        return 1;

    *lineStart = 160 * y;
    //   uint32_t *pixmem = (uint32_t*) screen->pixels;

    // fill pixel array with span
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

    // pixel 7 at the left, pixel 0 at the right
    __builtin_mips_cache(0xD, pixelscolors[0]);
    __builtin_mips_cache(0xD, pixelscolors[1]);

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
        pixelscolors[0][p] = palette[color];
        pixelscolors[1][p] = color;
    }

    // Is the span partially offscreen?
    *spanStart = 0;
    *spanEnd = 7;
    // Partially off the left side of the screen
    if( x < 0 )
        *spanStart = -x;
    // Partially off the right side of the screen
    if( x > (160-8) )
        *spanEnd = 159 - x;

    // xFlip?
    if( xFlip )
    {
        // Flip the span.
        pixel_t temp_pixelscolors[2][8];

        for(p=0; p<8; ++p)
        {
            // Copy pixels to a temporary place, in reverse order.
            temp_pixelscolors[0][p] = pixelscolors[0][7-p];
            temp_pixelscolors[1][p] = pixelscolors[1][7-p];
        }
        for(p=0; p<8; ++p)
        {
            pixelscolors[0][p] = temp_pixelscolors[0][p];
            pixelscolors[1][p] = temp_pixelscolors[1][p];
        }
    }

    return 0;
}


__attribute__((hot))
static void vid_drawOpaqueSpan( uint8_t pal, uint16_t vramAddr, int x, int y, int vramBank, int xFlip, int updateColormem ) {
    int lineStart, spanStart, spanEnd;
    if (vid_drawSpanCommon(&myCachedPalettes[pal][0], vramAddr, x, y, vramBank, xFlip, &lineStart, &spanStart, &spanEnd))
      return;

    // Draw the span from left to right.
    unsigned didx = lineStart + x + spanStart;
    unsigned sidx = 7 - spanStart;

    for( int p=spanStart; p<=spanEnd; ++p )
    {
        if( updateColormem ) {
          colormem[ didx ] = pixelscolors[1][ sidx ];
        }

        pixmem[ didx++ ] = pixelscolors[0][ sidx-- ];
    }

    __builtin_mips_cache(0x11, pixelscolors[0]);
    __builtin_mips_cache(0x11, pixelscolors[1]);
}

__attribute__((hot))
static void vid_drawTransparentSpan( uint8_t pal, uint16_t vramAddr, int x, int y, int vramBank, int xFlip, int priority ) {
    int lineStart, spanStart, spanEnd;
    if (vid_drawSpanCommon(&myCachedPalettes[pal][4], vramAddr, x, y, vramBank, xFlip, &lineStart, &spanStart, &spanEnd))
      return;

    // Draw the span from left to right.
    int p;
    for( p=spanStart; p<=spanEnd; ++p )
    {
        if( pixelscolors[1][7-p] != 0 )
        {
            if( priority )
            {
                if( colormem[ lineStart + x + p ] == 0 )
                {
                    pixmem[ lineStart + x + p ] = pixelscolors[0][ 7-p ];
                } else {
                    // Uncomment the next line to highlight the
                    // sprite-behind-background case in bright red.
                    //           pixmem[ lineStart + x + p ] = 0x001f; // red
                }
            } else {
                pixmem[ lineStart + x + p ] = pixelscolors[0][ 7-p ];
            }
        }
    }

    __builtin_mips_cache(0x11, pixelscolors[0]);
    __builtin_mips_cache(0x11, pixelscolors[1]);
}

__attribute__((hot))
void vid_render_line()
{
    // If the LCD is off, then return.
    // We should probably blank the line instead...
    if( (state.lcdc & LCDC_LCD_ENABLE) == 0 )
        return;

    // Set up cached palettes.
    if ( inval_palette ) {
    if( state.caps & 0x04 )
    {
        __builtin_mips_cache(0xD, myPalette);

        // DMG mode
        // colors need to be translated through BOTH the DMG and CGB palettes
        for (int i = 0; i < 8; i++) {
          __builtin_mips_cache(0xD, myCachedPalettes[i]);

          myPalette[4] = state.bgpd[(i*8)+0] + (state.bgpd[(i*8)+1]<<8);
          myPalette[5] = state.bgpd[(i*8)+2] + (state.bgpd[(i*8)+3]<<8);
          myPalette[6] = state.bgpd[(i*8)+4] + (state.bgpd[(i*8)+5]<<8);
          myPalette[7] = state.bgpd[(i*8)+6] + (state.bgpd[(i*8)+7]<<8);

          myCachedPalettes[i][0] = myPalette[4 + ((state.bgp)      & 0x3) ];
          myCachedPalettes[i][1] = myPalette[4 + ((state.bgp >> 2) & 0x3) ];
          myCachedPalettes[i][2] = myPalette[4 + ((state.bgp >> 4) & 0x3) ];
          myCachedPalettes[i][3] = myPalette[4 + ((state.bgp >> 6) & 0x3) ];

          myPalette[4] = state.obpd[(i*8)+0] + (state.obpd[(i*8)+1]<<8);
          myPalette[5] = state.obpd[(i*8)+2] + (state.obpd[(i*8)+3]<<8);
          myPalette[6] = state.obpd[(i*8)+4] + (state.obpd[(i*8)+5]<<8);
          myPalette[7] = state.obpd[(i*8)+6] + (state.obpd[(i*8)+7]<<8);

          int dmgPalette = (i==0) ? state.obp0 : state.obp1;

          myCachedPalettes[i][4] = myPalette[4 + ((dmgPalette)      & 0x3) ];
          myCachedPalettes[i][5] = myPalette[4 + ((dmgPalette >> 2) & 0x3) ];
          myCachedPalettes[i][6] = myPalette[4 + ((dmgPalette >> 4) & 0x3) ];
          myCachedPalettes[i][7] = myPalette[4 + ((dmgPalette >> 6) & 0x3) ];

          myCachedPalettes[i][0] = ((myCachedPalettes[i][0] & 0x001F) << 11) + ((myCachedPalettes[i][0] & 0x03E0) << 1) + ((myCachedPalettes[i][0] & 0x7C00) >> 9);
          myCachedPalettes[i][1] = ((myCachedPalettes[i][1] & 0x001F) << 11) + ((myCachedPalettes[i][1] & 0x03E0) << 1) + ((myCachedPalettes[i][1] & 0x7C00) >> 9);
          myCachedPalettes[i][2] = ((myCachedPalettes[i][2] & 0x001F) << 11) + ((myCachedPalettes[i][2] & 0x03E0) << 1) + ((myCachedPalettes[i][2] & 0x7C00) >> 9);
          myCachedPalettes[i][3] = ((myCachedPalettes[i][3] & 0x001F) << 11) + ((myCachedPalettes[i][3] & 0x03E0) << 1) + ((myCachedPalettes[i][3] & 0x7C00) >> 9);
          myCachedPalettes[i][4] = ((myCachedPalettes[i][4] & 0x001F) << 11) + ((myCachedPalettes[i][4] & 0x03E0) << 1) + ((myCachedPalettes[i][4] & 0x7C00) >> 9);
          myCachedPalettes[i][5] = ((myCachedPalettes[i][5] & 0x001F) << 11) + ((myCachedPalettes[i][5] & 0x03E0) << 1) + ((myCachedPalettes[i][5] & 0x7C00) >> 9);
          myCachedPalettes[i][6] = ((myCachedPalettes[i][6] & 0x001F) << 11) + ((myCachedPalettes[i][6] & 0x03E0) << 1) + ((myCachedPalettes[i][6] & 0x7C00) >> 9);
          myCachedPalettes[i][7] = ((myCachedPalettes[i][7] & 0x001F) << 11) + ((myCachedPalettes[i][7] & 0x03E0) << 1) + ((myCachedPalettes[i][7] & 0x7C00) >> 9);
        }
    } else {
        // CGB mode
        for (int i = 0; i < 8; i++) {
          __builtin_mips_cache(0xD, myCachedPalettes[i]);

          myCachedPalettes[i][0] = state.bgpd[(i*8)+0] + (state.bgpd[(i*8)+1]<<8);
          myCachedPalettes[i][1] = state.bgpd[(i*8)+2] + (state.bgpd[(i*8)+3]<<8);
          myCachedPalettes[i][2] = state.bgpd[(i*8)+4] + (state.bgpd[(i*8)+5]<<8);
          myCachedPalettes[i][3] = state.bgpd[(i*8)+6] + (state.bgpd[(i*8)+7]<<8);
          myCachedPalettes[i][4] = state.obpd[(i*8)+0] + (state.obpd[(i*8)+1]<<8);
          myCachedPalettes[i][5] = state.obpd[(i*8)+2] + (state.obpd[(i*8)+3]<<8);
          myCachedPalettes[i][6] = state.obpd[(i*8)+4] + (state.obpd[(i*8)+5]<<8);
          myCachedPalettes[i][7] = state.obpd[(i*8)+6] + (state.obpd[(i*8)+7]<<8);

          myCachedPalettes[i][0] = ((myCachedPalettes[i][0] & 0x001F) << 11) + ((myCachedPalettes[i][0] & 0x03E0) << 1) + ((myCachedPalettes[i][0] & 0x7C00) >> 9);
          myCachedPalettes[i][1] = ((myCachedPalettes[i][1] & 0x001F) << 11) + ((myCachedPalettes[i][1] & 0x03E0) << 1) + ((myCachedPalettes[i][1] & 0x7C00) >> 9);
          myCachedPalettes[i][2] = ((myCachedPalettes[i][2] & 0x001F) << 11) + ((myCachedPalettes[i][2] & 0x03E0) << 1) + ((myCachedPalettes[i][2] & 0x7C00) >> 9);
          myCachedPalettes[i][3] = ((myCachedPalettes[i][3] & 0x001F) << 11) + ((myCachedPalettes[i][3] & 0x03E0) << 1) + ((myCachedPalettes[i][3] & 0x7C00) >> 9);
          myCachedPalettes[i][4] = ((myCachedPalettes[i][4] & 0x001F) << 11) + ((myCachedPalettes[i][4] & 0x03E0) << 1) + ((myCachedPalettes[i][4] & 0x7C00) >> 9);
          myCachedPalettes[i][5] = ((myCachedPalettes[i][5] & 0x001F) << 11) + ((myCachedPalettes[i][5] & 0x03E0) << 1) + ((myCachedPalettes[i][5] & 0x7C00) >> 9);
          myCachedPalettes[i][6] = ((myCachedPalettes[i][6] & 0x001F) << 11) + ((myCachedPalettes[i][6] & 0x03E0) << 1) + ((myCachedPalettes[i][6] & 0x7C00) >> 9);
          myCachedPalettes[i][7] = ((myCachedPalettes[i][7] & 0x001F) << 11) + ((myCachedPalettes[i][7] & 0x03E0) << 1) + ((myCachedPalettes[i][7] & 0x7C00) >> 9);
        }
    }
    inval_palette = 0;
    }

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
static const vi_state_t vi_state = {
  0x00003202, // status
  0x00200000, // origin
  0x000000A0, // width
  0x00000002, // intr
  0x00000000, // current
  0x03E52239, // burst
  0x0000020D, // v_sync
  0x00000C15, // h_sync
  0x0C150C15, // leap
  0x006C02EC, // h_start
  0x002501FF, // v_start
  0x000E0204, // v_burst
  0x00000100, // x_scale
  0x00000266, // y_scale
};

struct libn64_fbtext_context fbtext;

void vid_init()
{
  DPC.STATUS = 1; // Clear XBUS Bit To Enable RDP On CPU

  // Setup RDP buffer
  static uint32_t dp_list[] __attribute__((aligned(64))) = { // Draw screen in 160x12 texture rectangle strips
    rdp_set_scissor(0.0,0.0, 160.0,144.0, SCISSOR_FIELD_DISABLE,SCISSOR_EVEN), // Set Scissor: XH,YH, XL,YL, Scissor Field Enable,Field
    rdp_set_color_image(IMAGE_DATA_FORMAT_RGBA,SIZE_OF_PIXEL_16B, 160, vi_state.origin), // Set Color Image: Format,Size, Width, DRAM Address
    rdp_set_other_modes(CYCLE_TYPE_COPY|ALPHA_DITHER_SEL_NO_DITHER|RGB_DITHER_SEL_NO_DITHER), // Set Other Modes

    rdp_set_texture_image(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 160, (uint32_t)pixmem), // Set Texture Image: Format,Size, Width, DRAM Address
    rdp_set_tile(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 40, 0x000, 0,0, 0,0,0,0, 0,0,0,0), // Set Tile: Format,Size, Tile Line Size, TMEM Address, Tile,Palette, CT,MT,MaskT,ShiftT, CS,MS,MaskS,ShiftS
    rdp_load_tile(0.0,0.0, 159.0,11.0, 0), // Load Tile: SL,TL, SH,TH, Tile
    rdp_texture_rectangle(0.0,0.0, 159.0,11.0, 0.0,0.0, 4.0,1.0, 0), // Texture Rectangle: XH,YH, XL,YL, S,T, DSDX,DTDY, Tile

    rdp_sync_tile, // Sync Tile
    rdp_set_texture_image(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 160, (uint32_t)pixmem+3840), // Set Texture Image: Format,Size, Width, DRAM Address
    rdp_set_tile(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 40, 0x000, 0,0, 0,0,0,0, 0,0,0,0), // Set Tile: Format,Size, Tile Line Size, TMEM Address, Tile,Palette, CT,MT,MaskT,ShiftT, CS,MS,MaskS,ShiftS
    rdp_load_tile(0.0,0.0, 159.0,11.0, 0), // Load Tile: SL,TL, SH,TH, Tile
    rdp_texture_rectangle(0.0,12.0, 159.0,23.0, 0.0,0.0, 4.0,1.0, 0), // Texture Rectangle: XH,YH, XL,YL, S,T, DSDX,DTDY, Tile

    rdp_sync_tile, // Sync Tile
    rdp_set_texture_image(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 160, (uint32_t)pixmem+7680), // Set Texture Image: Format,Size, Width, DRAM Address
    rdp_set_tile(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 40, 0x000, 0,0, 0,0,0,0, 0,0,0,0), // Set Tile: Format,Size, Tile Line Size, TMEM Address, Tile,Palette, CT,MT,MaskT,ShiftT, CS,MS,MaskS,ShiftS
    rdp_load_tile(0.0,0.0, 159.0,11.0, 0), // Load Tile: SL,TL, SH,TH, Tile
    rdp_texture_rectangle(0.0,24.0, 159.0,35.0, 0.0,0.0, 4.0,1.0, 0), // Texture Rectangle: XH,YH, XL,YL, S,T, DSDX,DTDY, Tile

    rdp_sync_tile, // Sync Tile
    rdp_set_texture_image(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 160, (uint32_t)pixmem+11520), // Set Texture Image: Format,Size, Width, DRAM Address
    rdp_set_tile(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 40, 0x000, 0,0, 0,0,0,0, 0,0,0,0), // Set Tile: Format,Size, Tile Line Size, TMEM Address, Tile,Palette, CT,MT,MaskT,ShiftT, CS,MS,MaskS,ShiftS
    rdp_load_tile(0.0,0.0, 159.0,11.0, 0), // Load Tile: SL,TL, SH,TH, Tile
    rdp_texture_rectangle(0.0,36.0, 159.0,47.0, 0.0,0.0, 4.0,1.0, 0), // Texture Rectangle: XH,YH, XL,YL, S,T, DSDX,DTDY, Tile

    rdp_sync_tile, // Sync Tile
    rdp_set_texture_image(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 160, (uint32_t)pixmem+15360), // Set Texture Image: Format,Size, Width, DRAM Address
    rdp_set_tile(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 40, 0x000, 0,0, 0,0,0,0, 0,0,0,0), // Set Tile: Format,Size, Tile Line Size, TMEM Address, Tile,Palette, CT,MT,MaskT,ShiftT, CS,MS,MaskS,ShiftS
    rdp_load_tile(0.0,0.0, 159.0,11.0, 0), // Load Tile: SL,TL, SH,TH, Tile
    rdp_texture_rectangle(0.0,48.0, 159.0,59.0, 0.0,0.0, 4.0,1.0, 0), // Texture Rectangle: XH,YH, XL,YL, S,T, DSDX,DTDY, Tile

    rdp_sync_tile, // Sync Tile
    rdp_set_texture_image(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 160, (uint32_t)pixmem+19200), // Set Texture Image: Format,Size, Width, DRAM Address
    rdp_set_tile(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 40, 0x000, 0,0, 0,0,0,0, 0,0,0,0), // Set Tile: Format,Size, Tile Line Size, TMEM Address, Tile,Palette, CT,MT,MaskT,ShiftT, CS,MS,MaskS,ShiftS
    rdp_load_tile(0.0,0.0, 159.0,11.0, 0), // Load Tile: SL,TL, SH,TH, Tile
    rdp_texture_rectangle(0.0,60.0, 159.0,71.0, 0.0,0.0, 4.0,1.0, 0), // Texture Rectangle: XH,YH, XL,YL, S,T, DSDX,DTDY, Tile

    rdp_sync_tile, // Sync Tile
    rdp_set_texture_image(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 160, (uint32_t)pixmem+23040), // Set Texture Image: Format,Size, Width, DRAM Address
    rdp_set_tile(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 40, 0x000, 0,0, 0,0,0,0, 0,0,0,0), // Set Tile: Format,Size, Tile Line Size, TMEM Address, Tile,Palette, CT,MT,MaskT,ShiftT, CS,MS,MaskS,ShiftS
    rdp_load_tile(0.0,0.0, 159.0,11.0, 0), // Load Tile: SL,TL, SH,TH, Tile
    rdp_texture_rectangle(0.0,72.0, 159.0,83.0, 0.0,0.0, 4.0,1.0, 0), // Texture Rectangle: XH,YH, XL,YL, S,T, DSDX,DTDY, Tile

    rdp_sync_tile, // Sync Tile
    rdp_set_texture_image(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 160, (uint32_t)pixmem+26880), // Set Texture Image: Format,Size, Width, DRAM Address
    rdp_set_tile(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 40, 0x000, 0,0, 0,0,0,0, 0,0,0,0), // Set Tile: Format,Size, Tile Line Size, TMEM Address, Tile,Palette, CT,MT,MaskT,ShiftT, CS,MS,MaskS,ShiftS
    rdp_load_tile(0.0,0.0, 159.0,11.0, 0), // Load Tile: SL,TL, SH,TH, Tile
    rdp_texture_rectangle(0.0,84.0, 159.0,95.0, 0.0,0.0, 4.0,1.0, 0), // Texture Rectangle: XH,YH, XL,YL, S,T, DSDX,DTDY, Tile

    rdp_sync_tile, // Sync Tile
    rdp_set_texture_image(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 160, (uint32_t)pixmem+30720), // Set Texture Image: Format,Size, Width, DRAM Address
    rdp_set_tile(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 40, 0x000, 0,0, 0,0,0,0, 0,0,0,0), // Set Tile: Format,Size, Tile Line Size, TMEM Address, Tile,Palette, CT,MT,MaskT,ShiftT, CS,MS,MaskS,ShiftS
    rdp_load_tile(0.0,0.0, 159.0,11.0, 0), // Load Tile: SL,TL, SH,TH, Tile
    rdp_texture_rectangle(0.0,96.0, 159.0,107.0, 0.0,0.0, 4.0,1.0, 0), // Texture Rectangle: XH,YH, XL,YL, S,T, DSDX,DTDY, Tile

    rdp_sync_tile, // Sync Tile
    rdp_set_texture_image(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 160, (uint32_t)pixmem+34560), // Set Texture Image: Format,Size, Width, DRAM Address
    rdp_set_tile(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 40, 0x000, 0,0, 0,0,0,0, 0,0,0,0), // Set Tile: Format,Size, Tile Line Size, TMEM Address, Tile,Palette, CT,MT,MaskT,ShiftT, CS,MS,MaskS,ShiftS
    rdp_load_tile(0.0,0.0, 159.0,11.0, 0), // Load Tile: SL,TL, SH,TH, Tile
    rdp_texture_rectangle(0.0,108.0, 159.0,119.0, 0.0,0.0, 4.0,1.0, 0), // Texture Rectangle: XH,YH, XL,YL, S,T, DSDX,DTDY, Tile

    rdp_sync_tile, // Sync Tile
    rdp_set_texture_image(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 160, (uint32_t)pixmem+38400), // Set Texture Image: Format,Size, Width, DRAM Address
    rdp_set_tile(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 40, 0x000, 0,0, 0,0,0,0, 0,0,0,0), // Set Tile: Format,Size, Tile Line Size, TMEM Address, Tile,Palette, CT,MT,MaskT,ShiftT, CS,MS,MaskS,ShiftS
    rdp_load_tile(0.0,0.0, 159.0,11.0, 0), // Load Tile: SL,TL, SH,TH, Tile
    rdp_texture_rectangle(0.0,120.0, 159.0,131.0, 0.0,0.0, 4.0,1.0, 0), // Texture Rectangle: XH,YH, XL,YL, S,T, DSDX,DTDY, Tile

    rdp_sync_tile, // Sync Tile
    rdp_set_texture_image(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 160, (uint32_t)pixmem+42240), // Set Texture Image: Format,Size, Width, DRAM Address
    rdp_set_tile(IMAGE_DATA_FORMAT_RGBA, SIZE_OF_PIXEL_16B, 40, 0x000, 0,0, 0,0,0,0, 0,0,0,0), // Set Tile: Format,Size, Tile Line Size, TMEM Address, Tile,Palette, CT,MT,MaskT,ShiftT, CS,MS,MaskS,ShiftS
    rdp_load_tile(0.0,0.0, 159.0,11.0, 0), // Load Tile: SL,TL, SH,TH, Tile
    rdp_texture_rectangle(0.0,132.0, 159.0,143.0, 0.0,0.0, 4.0,1.0, 0), // Texture Rectangle: XH,YH, XL,YL, S,T, DSDX,DTDY, Tile

    rdp_sync_full // Ensure Entire Scene Is Fully Drawn
  };

  rdp_start = (uint32_t)dp_list;
  rdp_end = (uint32_t)dp_list + sizeof(dp_list);

  vi_flush_state(&vi_state);

  for (unsigned i = 0; i < 160 * 144 * 2; i += 16) {
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
}

void vid_waitForNextFrame()
{
  while(VI.V_CURRENT_LINE != 144); // Wait For VSync
}

void vid_frame()
{
  rdp_run(rdp_start, rdp_end); // Run RDP buffer: Start, End
}