#include "n64.h"

//--------------------
// N64 VI Definitions
//--------------------
#define BPP0  0x0000 // VI Status/Control: Color Depth Blank (No Data Or Sync) (Bit 0..1)
//*RESERVED*  0x0001 // VI Status/Control: Color Depth Reserved (Bit 0..1)
#define BPP16 0x0002 // VI Status/Control: Color Depth 16BPP R5/G5/B5/A1 (Bit 0..1)
#define BPP32 0x0003 // VI Status/Control: Color Depth 32BPP R8/G8/B8/A8 (Bit 0..1)
#define GAMMA_DITHER_EN 0x00004 // VI Status/Control: Gamma Dither Enable (Requires: Gamma Enable) (Bit 2)
#define GAMMA_EN 0x00008 // VI Status/Control: Gamma Enable (Gamma Boost For YUV Images) (Bit 3)
#define DIVOT_EN 0x00010 // VI Status/Control: Divot Enable (Used With Anti-alias) (Bit 4)
#define VBUS_CLK_EN 0x00020 // VI Status/Control: Video Bus Clock Enable (Bit 5)
#define INTERLACE 0x00040 // VI Status/Control: Interlace/Serrate (Used With Interlaced Display) (Bit 6)
#define TST_MODE 0x00080  // VI Status/Control: Test Mode (Bit 7)
#define AA_MODE_0 0x00000 // VI Status/Control: AA Mode 0 = Anti­-alias & Resample (Always Fetch Extra Lines) (Bit 8..9)
#define AA_MODE_1 0x00100 // VI Status/Control: AA Mode 1 = Anti­-alias & Resample (Fetch Extra Lines When Needed) (Bit 8..9)
#define AA_MODE_2 0x00200 // VI Status/Control: AA Mode 2 = Resample Only (Bit 8..9)
#define AA_MODE_3 0x00300 // VI Status/Control: AA Mode 3 = Replicate Pixels & No Interpolation (Bit 8..9)
#define DIAG_0 0x00400 // VI Status/Control: Diagnotic 0 (Bit 10..11)
#define DIAG_1 0x00800 // VI Status/Control: Diagnotic 1 (Bit 10..11)
#define PIXEL_ADV_0 0x00000 // VI Status/Control: Pixel Advance 0 (Bit 12..15)
#define PIXEL_ADV_1 0x01000 // VI Status/Control: Pixel Advance 1 (Bit 12..15)
#define PIXEL_ADV_2 0x02000 // VI Status/Control: Pixel Advance 2 (Bit 12..15)
#define PIXEL_ADV_3 0x03000 // VI Status/Control: Pixel Advance 3 (Bit 12..15)
#define PIXEL_ADV_4 0x04000 // VI Status/Control: Pixel Advance 4 (Bit 12..15)
#define PIXEL_ADV_5 0x05000 // VI Status/Control: Pixel Advance 5 (Bit 12..15)
#define PIXEL_ADV_6 0x06000 // VI Status/Control: Pixel Advance 6 (Bit 12..15)
#define PIXEL_ADV_7 0x07000 // VI Status/Control: Pixel Advance 7 (Bit 12..15)
#define PIXEL_ADV_8 0x08000 // VI Status/Control: Pixel Advance 8 (Bit 12..15)
#define PIXEL_ADV_9 0x09000 // VI Status/Control: Pixel Advance 9 (Bit 12..15)
#define PIXEL_ADV_A 0x0A000 // VI Status/Control: Pixel Advance A (Bit 12..15)
#define PIXEL_ADV_B 0x0B000 // VI Status/Control: Pixel Advance B (Bit 12..15)
#define PIXEL_ADV_C 0x0C000 // VI Status/Control: Pixel Advance C (Bit 12..15)
#define PIXEL_ADV_D 0x0D000 // VI Status/Control: Pixel Advance D (Bit 12..15)
#define PIXEL_ADV_E 0x0E000 // VI Status/Control: Pixel Advance E (Bit 12..15)
#define PIXEL_ADV_F 0x0F000 // VI Status/Control: Pixel Advance F (Bit 12..15)
#define DITHER_FILTER_EN 0x10000 // VI Status/Control: Dither Filter Enable (Used With 16BPP Display) (Bit 16)

//---------------------
// N64 RDP Definitions
//---------------------
// RDP FUNCTIONS

// Run RDP Command List (From Start Address To End Address)
void rdp_run( uint32_t start, uint32_t end )
{
  // Store DPC Command Start Address To DP Start Register (0xA4100000)
  DPC.START = start;
  // Store DPC Command End Address To DP End Register (0xA4100004)
  DPC.END = end;
}

// RDP COMMANDS

// No Op (No Operation)
#define rdp_no_op \
  0x00000000, \
  0x00000000 \

// Fill Triangle (Flat Non-Shaded) Edge Coefficients
#define rdp_fill_triangle( lft, level, tile, yl, ym, yh, xl, dxldy, xh, dxhdy, xm, dxmdy ) \
  0x08000000 | (uint8_t)(lft) << 23 | (uint8_t)(level) << 19 | (uint8_t)(tile) << 16 | ((int)((float)(yl) * 4.0) & 0x3FFF), \
  ((int)((float)(ym) * 4.0) & 0x3FFF) << 16 | ((int)((float)(yh) * 4.0) & 0x3FFF), \
  (int)((float)(xl) * 65536.0), \
  (int)((float)(dxldy) * 65536.0), \
  (int)((float)(xh) * 65536.0), \
  (int)((float)(dxhdy) * 65536.0), \
  (int)((float)(xm) * 65536.0), \
  (int)((float)(dxmdy) * 65536.0) \

// Fill Z-Buffer Triangle (Flat Non-Shaded Z-Buffered) Edge Coefficients
#define rdp_fill_zbuffer_triangle( lft, level, tile, yl, ym, yh, xl, dxldy, xh, dxhdy, xm, dxmdy ) \
  0x09000000 | (uint8_t)(lft) << 23 | (uint8_t)(level) << 19 | (uint8_t)(tile) << 16 | ((int)((float)(yl) * 4.0) & 0x3FFF), \
  ((int)((float)(ym) * 4.0) & 0x3FFF) << 16 | ((int)((float)(yh) * 4.0) & 0x3FFF), \
  (int)((float)(xl) * 65536.0), \
  (int)((float)(dxldy) * 65536.0), \
  (int)((float)(xh) * 65536.0), \
  (int)((float)(dxhdy) * 65536.0), \
  (int)((float)(xm) * 65536.0), \
  (int)((float)(dxmdy) * 65536.0) \

// Texture Triangle (Textured Non-Shaded) Edge Coefficients
#define rdp_texture_triangle( lft, level, tile, yl, ym, yh, xl, dxldy, xh, dxhdy, xm, dxmdy ) \
  0x0A000000 | (uint8_t)(lft) << 23 | (uint8_t)(level) << 19 | (uint8_t)(tile) << 16 | ((int)((float)(yl) * 4.0) & 0x3FFF), \
  ((int)((float)(ym) * 4.0) & 0x3FFF) << 16 | ((int)((float)(yh) * 4.0) & 0x3FFF), \
  (int)((float)(xl) * 65536.0), \
  (int)((float)(dxldy) * 65536.0), \
  (int)((float)(xh) * 65536.0), \
  (int)((float)(dxhdy) * 65536.0), \
  (int)((float)(xm) * 65536.0), \
  (int)((float)(dxmdy) * 65536.0) \

// Texture Z-Buffer Triangle (Textured Non-Shaded Z-Buffered) Edge Coefficients
#define rdp_texture_zbuffer_triangle( lft, level, tile, yl, ym, yh, xl, dxldy, xh, dxhdy, xm, dxmdy ) \
  0x0B000000 | (uint8_t)(lft) << 23 | (uint8_t)(level) << 19 | (uint8_t)(tile) << 16 | ((int)((float)(yl) * 4.0) & 0x3FFF), \
  ((int)((float)(ym) * 4.0) & 0x3FFF) << 16 | ((int)((float)(yh) * 4.0) & 0x3FFF), \
  (int)((float)(xl) * 65536.0), \
  (int)((float)(dxldy) * 65536.0), \
  (int)((float)(xh) * 65536.0), \
  (int)((float)(dxhdy) * 65536.0), \
  (int)((float)(xm) * 65536.0), \
  (int)((float)(dxmdy) * 65536.0) \

// Shade Triangle (Goraud Shaded) Edge Coefficients
#define rdp_shade_triangle( lft, level, tile, yl, ym, yh, xl, dxldy, xh, dxhdy, xm, dxmdy ) \
  0x0C000000 | (uint8_t)(lft) << 23 | (uint8_t)(level) << 19 | (uint8_t)(tile) << 16 | ((int)((float)(yl) * 4.0) & 0x3FFF), \
  ((int)((float)(ym) * 4.0) & 0x3FFF) << 16 | ((int)((float)(yh) * 4.0) & 0x3FFF), \
  (int)((float)(xl) * 65536.0), \
  (int)((float)(dxldy) * 65536.0), \
  (int)((float)(xh) * 65536.0), \
  (int)((float)(dxhdy) * 65536.0), \
  (int)((float)(xm) * 65536.0), \
  (int)((float)(dxmdy) * 65536.0) \

// Shade Z-Buffer Triangle (Goraud Shaded Z-Buffered) Edge Coefficients
#define rdp_shade_zbuffer_triangle( lft, level, tile, yl, ym, yh, xl, dxldy, xh, dxhdy, xm, dxmdy ) \
  0x0D000000 | (uint8_t)(lft) << 23 | (uint8_t)(level) << 19 | (uint8_t)(tile) << 16 | ((int)((float)(yl) * 4.0) & 0x3FFF), \
  ((int)((float)(ym) * 4.0) & 0x3FFF) << 16 | ((int)((float)(yh) * 4.0) & 0x3FFF), \
  (int)((float)(xl) * 65536.0), \
  (int)((float)(dxldy) * 65536.0), \
  (int)((float)(xh) * 65536.0), \
  (int)((float)(dxhdy) * 65536.0), \
  (int)((float)(xm) * 65536.0), \
  (int)((float)(dxmdy) * 65536.0) \

// Shade Texture Triangle (Goraud Shaded Textured) Edge Coefficients
#define rdp_shade_texture_triangle( lft, level, tile, yl, ym, yh, xl, dxldy, xh, dxhdy, xm, dxmdy ) \
  0x0E000000 | (uint8_t)(lft) << 23 | (uint8_t)(level) << 19 | (uint8_t)(tile) << 16 | ((int)((float)(yl) * 4.0) & 0x3FFF), \
  ((int)((float)(ym) * 4.0) & 0x3FFF) << 16 | ((int)((float)(yh) * 4.0) & 0x3FFF), \
  (int)((float)(xl) * 65536.0), \
  (int)((float)(dxldy) * 65536.0), \
  (int)((float)(xh) * 65536.0), \
  (int)((float)(dxhdy) * 65536.0), \
  (int)((float)(xm) * 65536.0), \
  (int)((float)(dxmdy) * 65536.0) \

// Shade Texture Z-Buffer Triangle (Goraud Shaded Textured Z-Buffered) Edge Coefficients
#define rdp_shade_texture_zbuffer_triangle( lft, level, tile, yl, ym, yh, xl, dxldy, xh, dxhdy, xm, dxmdy ) \
  0x0F000000 | (uint8_t)(lft) << 23 | (uint8_t)(level) << 19 | (uint8_t)(tile) << 16 | ((int)((float)(yl) * 4.0) & 0x3FFF), \
  ((int)((float)(ym) * 4.0) & 0x3FFF) << 16 | ((int)((float)(yh) * 4.0) & 0x3FFF), \
  (int)((float)(xl) * 65536.0), \
  (int)((float)(dxldy) * 65536.0), \
  (int)((float)(xh) * 65536.0), \
  (int)((float)(dxhdy) * 65536.0), \
  (int)((float)(xm) * 65536.0), \
  (int)((float)(dxmdy) * 65536.0) \

// Shade Coefficients (Concat With Triangle Edge Coefficients Commands)
#define rdp_shade_coefficients( r, g, b, a, drdx, dgdx, dbdx, dadx, drde, dgde, dbde, dade, drdy, dgdy, dbdy, dady ) \
  (int)((float)(r)) << 16 | ((int)((float)(g)) & 0xFFFF), \
  (int)((float)(b)) << 16 | ((int)((float)(a)) & 0xFFFF), \
  (int)((float)(drdx)) << 16 | ((int)((float)(dgdx)) & 0xFFFF), \
  (int)((float)(dbdx)) << 16 | ((int)((float)(dadx)) & 0xFFFF), \
  (int)((float)(r) * 65536.0) << 16 | ((int)((float)(g) * 65536.0) & 0xFFFF), \
  (int)((float)(b) * 65536.0) << 16 | ((int)((float)(a) * 65536.0) & 0xFFFF), \
  (int)((float)(drdx) * 65536.0) << 16 | ((int)((float)(dgdx) * 65536.0) & 0xFFFF), \
  (int)((float)(dbdx) * 65536.0) << 16 | ((int)((float)(dadx) * 65536.0) & 0xFFFF), \
  (int)((float)(drde)) << 16 | ((int)((float)(dgde)) & 0xFFFF), \
  (int)((float)(dbde)) << 16 | ((int)((float)(dade)) & 0xFFFF), \
  (int)((float)(drdy)) << 16 | ((int)((float)(dgdy)) & 0xFFFF), \
  (int)((float)(dbdy)) << 16 | ((int)((float)(dady)) & 0xFFFF), \
  (int)((float)(drde) * 65536.0) << 16 | ((int)((float)(dgde) * 65536.0) & 0xFFFF), \
  (int)((float)(dbde) * 65536.0) << 16 | ((int)((float)(dade) * 65536.0) & 0xFFFF), \
  (int)((float)(drdy) * 65536.0) << 16 | ((int)((float)(dgdy) * 65536.0) & 0xFFFF), \
  (int)((float)(dbdy) * 65536.0) << 16 | ((int)((float)(dady) * 65536.0) & 0xFFFF) \

// Texture Coefficients (Concat With Triangle Edge Coefficients Commands)
#define rdp_texture_coefficients( s, t, w, dsdx, dtdx, dwdx, dsde, dtde, dwde, dsdy, dtdy, dwdy ) \
  (int)((float)(s)) << 16 | ((int)((float)(t)) & 0xFFFF), \
  (int)((float)(w)) << 16, \
  (int)((float)(dsdx)) << 16 | ((int)((float)(dtdx)) & 0xFFFF), \
  (int)((float)(dwdx)) << 16, \
  (int)((float)(s) * 65536.0) << 16 | ((int)((float)(t) * 65536.0) & 0xFFFF), \
  (int)((float)(w) * 65536.0) << 16, \
  (int)((float)(dsdx) * 65536.0) << 16 | ((int)((float)(dtdx) * 65536.0) & 0xFFFF), \
  (int)((float)(dwdx) * 65536.0) << 16, \
  (int)((float)(dsde)) << 16 | ((int)((float)(dtde)) & 0xFFFF), \
  (int)((float)(dwde)) << 16, \
  (int)((float)(dsdy)) << 16 | ((int)((float)(dtdy)) & 0xFFFF), \
  (int)((float)(dwdy)) << 16, \
  (int)((float)(dsde) * 65536.0) << 16 | ((int)((float)(dtde) * 65536.0) & 0xFFFF), \
  (int)((float)(dwde) * 65536.0) << 16, \
  (int)((float)(dsdy) * 65536.0) << 16 | ((int)((float)(dtdy) * 65536.0) & 0xFFFF), \
  (int)((float)(dwdy) * 65536.0) << 16 \

// Z-Buffer Coefficients (Concat With Triangle Edge Coefficients Commands)
#define rdp_zbuffer_coefficients( z, dzdx, dzde, dzdy ) \
  (int)((float)(z) * 65536.0), \
  (int)((float)(dzdx) * 65536.0), \
  (int)((float)(dzde) * 65536.0), \
  (int)((float)(dzdy) * 65536.0) \

// Texture Rectangle (Top Left To Bottom Right)
#define rdp_texture_rectangle( xh, yh, xl, yl, s, t, dsdx, dtdy, tile ) \
  0x24000000 | ((int)((float)(xl) * 4.0) & 0xFFF) << 12 | ((int)((float)(yl) * 4.0) & 0xFFF), \
  (uint8_t)(tile) << 24 | ((int)((float)(xh) * 4.0) & 0xFFF) << 12 | ((int)((float)(yh) * 4.0) & 0xFFF), \
  (int)((float)(s) * 32.0) << 16 | ((int)((float)(t) * 32.0) & 0xFFFF), \
  (int)((float)(dsdx) * 1024.0) << 16 | ((int)((float)(dtdy) * 1024.0) & 0xFFFF) \

// Texture Rectangle Flip (Top Left To Bottom Right)
#define rdp_texture_rectangle_flip( xh, yh, xl, yl, s, t, dsdx, dtdy, tile ) \
  0x25000000 | ((int)((float)(xl) * 4.0) & 0xFFF) << 12 | ((int)((float)(yl) * 4.0) & 0xFFF), \
  (uint8_t)(tile) << 24 | ((int)((float)(xh) * 4.0) & 0xFFF) << 12 | ((int)((float)(yh) * 4.0) & 0xFFF), \
  (int)((float)(s) * 32.0) << 16 | ((int)((float)(t) * 32.0) & 0xFFFF), \
  (int)((float)(dsdx) * 1024.0) << 16 | ((int)((float)(dtdy) * 1024.0) & 0xFFFF) \

// Sync Load
#define rdp_sync_load \
  0x26000000, \
  0x00000000 \

// Sync Pipe
#define rdp_sync_pipe \
  0x27000000, \
  0x00000000 \

// Sync Tile
#define rdp_sync_tile \
  0x28000000, \
  0x00000000 \

// Sync Full
#define rdp_sync_full \
  0x29000000, \
  0x00000000 \

// Set Key GB (Coefficients Used For Green/Blue Keying)
#define rdp_set_key_gb( widthg, centerg, scaleg, widthb, centerb, scaleb ) \
  0x2A000000 | ((int)((float)(widthg) * 16.0) & 0xFFF) << 12 | ((int)((float)(widthb) * 16.0) & 0xFFF), \
  (uint8_t)(centerg) << 24 | (uint8_t)(scaleg) << 16 | (uint8_t)(centerb) << 8 | (uint8_t)(scaleb) \

// Set Key R (Coefficients Used For Red Keying)
#define rdp_set_key_r( width, center, scale ) \
  0x2B000000, \
  (int)((float)(width) * 16.0) << 16 | (uint8_t)(center) << 8 | (uint8_t)(scale) \

// Set Convert (Coefficients For Converting YUV Pixels To RGB)
#define rdp_set_convert( k0, k1, k2, k3, k4, k5 ) \
  0x2C000000 | ((int)((float)(k0) * 128.0) & 0x1FF) << 13 | ((int)((float)(k1) * 128.0) & 0x1FF) << 4 | ((int)((float)(k2) * 128.0) & 0x1FF) >> 5, \
  (int)((float)(k2) * 128.0) << 27 | ((int)((float)(k3) * 128.0) & 0x1FF) << 18 | ((int)((float)(k4) * 128.0) & 0x1FF) << 9 | ((int)((float)(k5) * 128.0) & 0x1FF) \

// Set Scissor Word
#define SCISSOR_EVEN 0 // Set_Scissor O: Field Even (Bit 24)
#define SCISSOR_ODD 1  // Set_Scissor O: Field Odd (Bit 24)
#define SCISSOR_FIELD_DISABLE 0 // Set_Scissor F: Scissor Field Disable (Bit 25)
#define SCISSOR_FIELD_ENABLE 1  // Set_Scissor F: Scissor Field Enable (Bit 25)

// Set Scissor (Top Left To Bottom Right)
#define rdp_set_scissor( xh, yh, xl, yl, f, o ) \
  0x2D000000 | ((int)((float)(xh) * 4.0) & 0xFFF) << 12 | ((int)((float)(yh) * 4.0) & 0xFFF), \
  (uint8_t)(f) << 25 | (uint8_t)(o) << 24 | ((int)((float)(xl) * 4.0) & 0xFFF) << 12 | ((int)((float)(yl) * 4.0) & 0xFFF) \

// Set Primitive Depth (Primitive Z, Primitive Delta Z)
#define rdp_set_prim_depth( z, dz ) \
  0x2E000000, \
  (int)((float)(z)) << 16 | ((int)((float)(dz)) & 0xFFFF) \

// Set Other Modes LO Word
#define ALPHA_COMPARE_EN 0x00000000000001 // Set_Other_Modes A: Conditional Color Write On Alpha Compare (Bit 0)
#define DITHER_ALPHA_EN 0x00000000000002 // Set_Other_Modes B: Use Random Noise In Alpha Compare, Otherwise Use Blend Alpha In Alpha Compare (Bit 1)
#define Z_SOURCE_SEL 0x00000000000004 // Set_Other_Modes C: Choose Between Primitive Z And Pixel Z (Bit 2)
#define ANTIALIAS_EN 0x00000000000008 // Set_Other_Modes D: If Not Force Blend, Allow Blend Enable - Use CVG Bits (Bit 3)
#define Z_COMPARE_EN 0x00000000000010 // Set_Other_Modes E: Conditional Color Write Enable On Depth Comparison (Bit 4)
#define Z_UPDATE_EN 0x00000000000020 // Set_Other_Modes F: Enable Writing Of Z If Color Write Enabled (Bit 5)
#define IMAGE_READ_EN 0x00000000000040 // Set_Other_Modes G: Enable Color/CVG Read/Modify/Write Memory Access (Bit 6)
#define COLOR_ON_CVG 0x00000000000080 // Set_Other_Modes H: Only Update Color On Coverage Overflow (Transparent Surfaces) (Bit 7)
#define CVG_DEST_CLAMP 0x00000000000000 // Set_Other_Modes I: CVG Destination Clamp (Normal) (Bit 8..9)
#define CVG_DEST_WRAP 0x00000000000100  // Set_Other_Modes I: CVG Destination Wrap (WAS Assume Full CVG) (Bit 8..9)
#define CVG_DEST_ZAP 0x00000000000200   // Set_Other_Modes I: CVG Destination Zap (Force To Full CVG) (Bit 8..9)
#define CVG_DEST_SAVE 0x00000000000300  // Set_Other_Modes I: CVG Destination Save (Don't Overwrite Memory CVG) (Bit 8..9)
#define Z_MODE_OPAQUE 0x00000000000000           // Set_Other_Modes J: Z Mode Opaque (Bit 10..11)
#define Z_MODE_INTERPENETRATING 0x00000000000400 // Set_Other_Modes J: Z Mode Interpenetrating (Bit 10..11)
#define Z_MODE_TRANSPARENT 0x00000000000800      // Set_Other_Modes J: Z Mode Transparent (Bit 10..11)
#define Z_MODE_DECAL 0x00000000000C00            // Set_Other_Modes J: Z Mode Decal (Bit 10..11)
#define CVG_TIMES_ALPHA 0x00000000001000 // Set_Other_Modes K: Use CVG Times Alpha For Pixel Alpha And Coverage (Bit 12)
#define ALPHA_CVG_SELECT 0x00000000002000 // Set_Other_Modes L: Use CVG (Or CVG*Alpha) For Pixel Alpha (Bit 13)
#define FORCE_BLEND 0x00000000004000 // Set_Other_Modes M: Force Blend Enable (Bit 14)
//*RESERVED* 0x00000000008000 // Set_Other_Modes N: This Mode Bit Is Not Currently Used, But May Be In The Future (Bit 15)
#define B_M2B_1_0 0x00000000000000 // Set_Other_Modes O: Blend Modeword, Multiply 2b Input Select 0, Cycle 1 (Bit 16..17)
#define B_M2B_1_1 0x00000000010000 // Set_Other_Modes O: Blend Modeword, Multiply 2b Input Select 1, Cycle 1 (Bit 16..17)
#define B_M2B_1_2 0x00000000020000 // Set_Other_Modes O: Blend Modeword, Multiply 2b Input Select 2, Cycle 1 (Bit 16..17)
#define B_M2B_1_3 0x00000000030000 // Set_Other_Modes O: Blend Modeword, Multiply 2b Input Select 3, Cycle 1 (Bit 16..17)
#define B_M2B_0_0 0x00000000000000 // Set_Other_Modes P: Blend Modeword, Multiply 2b Input Select 0, Cycle 0 (Bit 18..19)
#define B_M2B_0_1 0x00000000040000 // Set_Other_Modes P: Blend Modeword, Multiply 2b Input Select 1, Cycle 0 (Bit 18..19)
#define B_M2B_0_2 0x00000000080000 // Set_Other_Modes P: Blend Modeword, Multiply 2b Input Select 2, Cycle 0 (Bit 18..19)
#define B_M2B_0_3 0x000000000C0000 // Set_Other_Modes P: Blend Modeword, Multiply 2b Input Select 3, Cycle 0 (Bit 18..19)
#define B_M2A_1_0 0x00000000000000 // Set_Other_Modes Q: Blend Modeword, Multiply 2a Input Select 0, Cycle 1 (Bit 20..21)
#define B_M2A_1_1 0x00000000100000 // Set_Other_Modes Q: Blend Modeword, Multiply 2a Input Select 1, Cycle 1 (Bit 20..21)
#define B_M2A_1_2 0x00000000200000 // Set_Other_Modes Q: Blend Modeword, Multiply 2a Input Select 2, Cycle 1 (Bit 20..21)
#define B_M2A_1_3 0x00000000300000 // Set_Other_Modes Q: Blend Modeword, Multiply 2a Input Select 3, Cycle 1 (Bit 20..21)
#define B_M2A_0_0 0x00000000000000 // Set_Other_Modes R: Blend Modeword, Multiply 2a Input Select 0, Cycle 0 (Bit 22..23)
#define B_M2A_0_1 0x00000000400000 // Set_Other_Modes R: Blend Modeword, Multiply 2a Input Select 1, Cycle 0 (Bit 22..23)
#define B_M2A_0_2 0x00000000800000 // Set_Other_Modes R: Blend Modeword, Multiply 2a Input Select 2, Cycle 0 (Bit 22..23)
#define B_M2A_0_3 0x00000000C00000 // Set_Other_Modes R: Blend Modeword, Multiply 2a Input Select 3, Cycle 0 (Bit 22..23)
#define B_M1B_1_0 0x00000000000000 // Set_Other_Modes S: Blend Modeword, Multiply 1b Input Select 0, Cycle 1 (Bit 24..25)
#define B_M1B_1_1 0x00000001000000 // Set_Other_Modes S: Blend Modeword, Multiply 1b Input Select 1, Cycle 1 (Bit 24..25)
#define B_M1B_1_2 0x00000002000000 // Set_Other_Modes S: Blend Modeword, Multiply 1b Input Select 2, Cycle 1 (Bit 24..25)
#define B_M1B_1_3 0x00000003000000 // Set_Other_Modes S: Blend Modeword, Multiply 1b Input Select 3, Cycle 1 (Bit 24..25)
#define B_M1B_0_0 0x00000000000000 // Set_Other_Modes T: Blend Modeword, Multiply 1b Input Select 0, Cycle 0 (Bit 26..27)
#define B_M1B_0_1 0x00000004000000 // Set_Other_Modes T: Blend Modeword, Multiply 1b Input Select 1, Cycle 0 (Bit 26..27)
#define B_M1B_0_2 0x00000008000000 // Set_Other_Modes T: Blend Modeword, Multiply 1b Input Select 2, Cycle 0 (Bit 26..27)
#define B_M1B_0_3 0x0000000C000000 // Set_Other_Modes T: Blend Modeword, Multiply 1b Input Select 3, Cycle 0 (Bit 26..27)
#define B_M1A_1_0 0x00000000000000 // Set_Other_Modes U: Blend Modeword, Multiply 1a Input Select 0, Cycle 1 (Bit 28..29)
#define B_M1A_1_1 0x00000010000000 // Set_Other_Modes U: Blend Modeword, Multiply 1a Input Select 1, Cycle 1 (Bit 28..29)
#define B_M1A_1_2 0x00000020000000 // Set_Other_Modes U: Blend Modeword, Multiply 1a Input Select 2, Cycle 1 (Bit 28..29)
#define B_M1A_1_3 0x00000030000000 // Set_Other_Modes U: Blend Modeword, Multiply 1a Input Select 3, Cycle 1 (Bit 28..29)
#define B_M1A_0_0 0x00000000000000 // Set_Other_Modes V: Blend Modeword, Multiply 1a Input Select 0, Cycle 0 (Bit 30..31)
#define B_M1A_0_1 0x00000040000000 // Set_Other_Modes V: Blend Modeword, Multiply 1a Input Select 1, Cycle 0 (Bit 30..31)
#define B_M1A_0_2 0x00000080000000 // Set_Other_Modes V: Blend Modeword, Multiply 1a Input Select 2, Cycle 0 (Bit 30..31)
#define B_M1A_0_3 0x000000C0000000 // Set_Other_Modes V: Blend Modeword, Multiply 1a Input Select 3, Cycle 0 (Bit 30..31)
// Set Other Modes HI Word
//*RESERVED* 0x00000F00000000 // Set_Other_Modes: Reserved For Future Use, Default Value Is 0xF (Bit 32..35)
#define ALPHA_DITHER_SEL_PATTERN 0x00000000000000   // Set_Other_Modes V1: Alpha Dither Selection Pattern (Bit 36..37)
#define ALPHA_DITHER_SEL_PATTERNB 0x00001000000000  // Set_Other_Modes V1: Alpha Dither Selection ~Pattern (Bit 36..37)
#define ALPHA_DITHER_SEL_NOISE 0x00002000000000     // Set_Other_Modes V1: Alpha Dither Selection Noise (Bit 36..37)
#define ALPHA_DITHER_SEL_NO_DITHER 0x00003000000000 // Set_Other_Modes V1: Alpha Dither Selection No Dither (Bit 36..37)
#define RGB_DITHER_SEL_MAGIC_SQUARE_MATRIX 0x00000000000000   // Set_Other_Modes V2: RGB Dither Selection Magic Square Matrix (Preferred If Filtered) (Bit 38..39)
#define RGB_DITHER_SEL_STANDARD_BAYER_MATRIX 0x00004000000000 // Set_Other_Modes V2: RGB Dither Selection Standard Bayer Matrix (Preferred If Not Filtered) (Bit 38..39)
#define RGB_DITHER_SEL_NOISE 0x00008000000000                 // Set_Other_Modes V2: RGB Dither Selection Noise (As Before) (Bit 38..39)
#define RGB_DITHER_SEL_NO_DITHER 0x0000C000000000             // Set_Other_Modes V2: RGB Dither Selection No Dither (Bit 38..39)
#define KEY_EN 0x00010000000000 // Set_Other_Modes W: Enables Chroma Keying (Bit 40)
#define CONVERT_ONE 0x00020000000000 // Set_Other_Modes X: Color Convert Texel That Was The Ouput Of The Texture Filter On Cycle0, Used To Qualify BI_LERP_1 (Bit 41)
#define BI_LERP_1 0x00040000000000 // Set_Other_Modes Y: 1=BI_LERP, 0=Color Convert Operation In Texture Filter. Used In Cycle 1 (Bit 42)
#define BI_LERP_0 0x00080000000000 // Set_Other_Modes Z: 1=BI_LERP, 0=Color Convert Operation In Texture Filter. Used In Cycle 0 (Bit 43)
#define MID_TEXEL 0x00100000000000 // Set_Other_Modes a: Indicates Texture Filter Should Do A 2x2 Half Texel Interpolation, Primarily Used For MPEG Motion Compensation Processing (Bit 44)
#define SAMPLE_TYPE 0x00200000000000 // Set_Other_Modes b: Determines How Textures Are Sampled: 0=1x1 (Point Sample), 1=2x2. Note That Copy (Point Sample 4 Horizontally Adjacent Texels) Mode Is Indicated By CYCLE_TYPE (Bit 45)
#define TLUT_TYPE 0x00400000000000 // Set_Other_Modes c: Type Of Texels In Table, 0=16b RGBA(5/5/5/1), 1=IA(8/8) (Bit 46)
#define EN_TLUT 0x00800000000000 // Set_Other_Modes d: Enable Lookup Of Texel Values From TLUT. Meaningful If Texture Type Is Index, Tile Is In Low TMEM, TLUT Is In High TMEM, And Color Image Is RGB (Bit 47)
#define TEX_LOD_EN 0x01000000000000 // Set_Other_Modes e: Enable Texture Level Of Detail (LOD) (Bit 48)
#define SHARPEN_TEX_EN 0x02000000000000 // Set_Other_Modes f: Enable Sharpened Texture (Bit 49)
#define DETAIL_TEX_EN 0x04000000000000 // Set_Other_Modes g: Enable Detail Texture (Bit 50)
#define PERSP_TEX_EN 0x08000000000000 // Set_Other_Modes h: Enable Perspective Correction On Texture (Bit 51)
#define CYCLE_TYPE_1_CYCLE 0x00000000000000 // Set_Other_Modes i: Display Pipeline Cycle Control Mode 1 Cycle (Bit 52..53)
#define CYCLE_TYPE_2_CYCLE 0x10000000000000 // Set_Other_Modes i: Display Pipeline Cycle Control Mode 2 Cycle (Bit 52..53)
#define CYCLE_TYPE_COPY 0x20000000000000    // Set_Other_Modes i: Display Pipeline Cycle Control Mode Copy (Bit 52..53)
#define CYCLE_TYPE_FILL 0x30000000000000    // Set_Other_Modes i: Display Pipeline Cycle Control Mode Fill (Bit 52..53)
//*RESERVED* 0x40000000000000 // Set_Other_Modes j: This Mode Bit Is Not Currently Used, But May Be In The Future (Bit 54)
#define ATOMIC_PRIM 0x80000000000000 // Set_Other_Modes k: Force Primitive To Be Written To Frame Buffer Before Read Of Following Primitive

// Set Other Modes
#define rdp_set_other_modes( mode ) \
  0x2F000000 | (uint32_t)((mode) >> 32), \
  (uint32_t)(mode) \

// Load TLUT (Top Left To Bottom Right)
#define rdp_load_tlut( sl, tl, sh, th, tile ) \
  0x30000000 | ((int)((float)(sl) * 4.0) & 0xFFF) << 12 | ((int)((float)(tl) * 4.0) & 0xFFF), \
  (uint8_t)(tile) << 24 | ((int)((float)(sh) * 4.0) & 0xFFF) << 12 | ((int)((float)(th) * 4.0) & 0xFFF) \

// Set Tile Size (Top Left To Bottom Right)
#define rdp_set_tile_size( sl, tl, sh, th, tile ) \
  0x32000000 | ((int)((float)(sl) * 4.0) & 0xFFF) << 12 | ((int)((float)(tl) * 4.0) & 0xFFF), \
  (uint8_t)(tile) << 24 | ((int)((float)(sh) * 4.0) & 0xFFF) << 12 | ((int)((float)(th) * 4.0) & 0xFFF) \

// Load Block (Top Left To Bottom Right)
#define rdp_load_block( sl, tl, sh, dxt, tile ) \
  0x33000000 | ((int)((float)(sl)) & 0xFFF) << 12 | ((int)((float)(tl)) & 0xFFF), \
  (uint8_t)(tile) << 24 | ((int)((float)(sh)) & 0xFFF) << 12 | ((int)((float)(dxt) * 2048.0) & 0xFFF) \

// Load Tile (Top Left To Bottom Right)
#define rdp_load_tile( sl, tl, sh, th, tile ) \
  0x34000000 | ((int)((float)(sl) * 4.0) & 0xFFF) << 12 | ((int)((float)(tl) * 4.0) & 0xFFF), \
  (uint8_t)(tile) << 24 | ((int)((float)(sh) * 4.0) & 0xFFF) << 12 | ((int)((float)(th) * 4.0) & 0xFFF) \

// Set Tile LO Word
#define SHIFT_S_0 0x0 // Set_Tile: Shift 0 Level Of Detail Shift For S Addresses (Bit 0..3)
#define SHIFT_S_1 0x1 // Set_Tile: Shift 1 Level Of Detail Shift For S Addresses (Bit 0..3)
#define SHIFT_S_2 0x2 // Set_Tile: Shift 2 Level Of Detail Shift For S Addresses (Bit 0..3)
#define SHIFT_S_3 0x3 // Set_Tile: Shift 3 Level Of Detail Shift For S Addresses (Bit 0..3)
#define SHIFT_S_4 0x4 // Set_Tile: Shift 4 Level Of Detail Shift For S Addresses (Bit 0..3)
#define SHIFT_S_5 0x5 // Set_Tile: Shift 5 Level Of Detail Shift For S Addresses (Bit 0..3)
#define SHIFT_S_6 0x6 // Set_Tile: Shift 6 Level Of Detail Shift For S Addresses (Bit 0..3)
#define SHIFT_S_7 0x7 // Set_Tile: Shift 7 Level Of Detail Shift For S Addresses (Bit 0..3)
#define SHIFT_S_8 0x8 // Set_Tile: Shift 8 Level Of Detail Shift For S Addresses (Bit 0..3)
#define SHIFT_S_9 0x9 // Set_Tile: Shift 9 Level Of Detail Shift For S Addresses (Bit 0..3)
#define SHIFT_S_A 0xA // Set_Tile: Shift A Level Of Detail Shift For S Addresses (Bit 0..3)
#define SHIFT_S_B 0xB // Set_Tile: Shift B Level Of Detail Shift For S Addresses (Bit 0..3)
#define SHIFT_S_C 0xC // Set_Tile: Shift C Level Of Detail Shift For S Addresses (Bit 0..3)
#define SHIFT_S_D 0xD // Set_Tile: Shift D Level Of Detail Shift For S Addresses (Bit 0..3)
#define SHIFT_S_E 0xE // Set_Tile: Shift E Level Of Detail Shift For S Addresses (Bit 0..3)
#define SHIFT_S_F 0xF // Set_Tile: Shift F Level Of Detail Shift For S Addresses (Bit 0..3)
#define MASK_S_0 0x0 // Set_Tile: Mask 0 For Wrapping/Mirroring In S Direction, Zero = Clamp (Bit 14..17)
#define MASK_S_1 0x1 // Set_Tile: Mask 1 For Wrapping/Mirroring In S Direction, Pass (Mask) LSBs Of S Address (Bit 4..7)
#define MASK_S_2 0x2 // Set_Tile: Mask 2 For Wrapping/Mirroring In S Direction, Pass (Mask) LSBs Of S Address (Bit 4..7)
#define MASK_S_3 0x3 // Set_Tile: Mask 3 For Wrapping/Mirroring In S Direction, Pass (Mask) LSBs Of S Address (Bit 4..7)
#define MASK_S_4 0x4 // Set_Tile: Mask 4 For Wrapping/Mirroring In S Direction, Pass (Mask) LSBs Of S Address (Bit 4..7)
#define MASK_S_5 0x5 // Set_Tile: Mask 5 For Wrapping/Mirroring In S Direction, Pass (Mask) LSBs Of S Address (Bit 4..7)
#define MASK_S_6 0x6 // Set_Tile: Mask 6 For Wrapping/Mirroring In S Direction, Pass (Mask) LSBs Of S Address (Bit 4..7)
#define MASK_S_7 0x7 // Set_Tile: Mask 7 For Wrapping/Mirroring In S Direction, Pass (Mask) LSBs Of S Address (Bit 4..7)
#define MASK_S_8 0x8 // Set_Tile: Mask 8 For Wrapping/Mirroring In S Direction, Pass (Mask) LSBs Of S Address (Bit 4..7)
#define MASK_S_9 0x9 // Set_Tile: Mask 9 For Wrapping/Mirroring In S Direction, Pass (Mask) LSBs Of S Address (Bit 4..7)
#define MASK_S_A 0xA // Set_Tile: Mask A For Wrapping/Mirroring In S Direction, Pass (Mask) LSBs Of S Address (Bit 4..7)
#define MASK_S_B 0xB // Set_Tile: Mask B For Wrapping/Mirroring In S Direction, Pass (Mask) LSBs Of S Address (Bit 4..7)
#define MASK_S_C 0xC // Set_Tile: Mask C For Wrapping/Mirroring In S Direction, Pass (Mask) LSBs Of S Address (Bit 4..7)
#define MASK_S_D 0xD // Set_Tile: Mask D For Wrapping/Mirroring In S Direction, Pass (Mask) LSBs Of S Address (Bit 4..7)
#define MASK_S_E 0xE // Set_Tile: Mask E For Wrapping/Mirroring In S Direction, Pass (Mask) LSBs Of S Address (Bit 4..7)
#define MASK_S_F 0xF // Set_Tile: Mask F For Wrapping/Mirroring In S Direction, Pass (Mask) LSBs Of S Address (Bit 4..7)
#define MIRROR_S 1 // Set_Tile: Mirror Enable For S Direction (Bit 8)
#define CLAMP_S 1  // Set_Tile:  Clamp Enable For S Direction (Bit 9)
#define SHIFT_T_0 0x0 // Set_Tile: Shift 0 Level Of Detail Shift For T Addresses (Bit 10..13)
#define SHIFT_T_1 0x1 // Set_Tile: Shift 1 Level Of Detail Shift For T Addresses (Bit 10..13)
#define SHIFT_T_2 0x2 // Set_Tile: Shift 2 Level Of Detail Shift For T Addresses (Bit 10..13)
#define SHIFT_T_3 0x3 // Set_Tile: Shift 3 Level Of Detail Shift For T Addresses (Bit 10..13)
#define SHIFT_T_4 0x4 // Set_Tile: Shift 4 Level Of Detail Shift For T Addresses (Bit 10..13)
#define SHIFT_T_5 0x5 // Set_Tile: Shift 5 Level Of Detail Shift For T Addresses (Bit 10..13)
#define SHIFT_T_6 0x6 // Set_Tile: Shift 6 Level Of Detail Shift For T Addresses (Bit 10..13)
#define SHIFT_T_7 0x7 // Set_Tile: Shift 7 Level Of Detail Shift For T Addresses (Bit 10..13)
#define SHIFT_T_8 0x8 // Set_Tile: Shift 8 Level Of Detail Shift For T Addresses (Bit 10..13)
#define SHIFT_T_9 0x9 // Set_Tile: Shift 9 Level Of Detail Shift For T Addresses (Bit 10..13)
#define SHIFT_T_A 0xA // Set_Tile: Shift A Level Of Detail Shift For T Addresses (Bit 10..13)
#define SHIFT_T_B 0xB // Set_Tile: Shift B Level Of Detail Shift For T Addresses (Bit 10..13)
#define SHIFT_T_C 0xC // Set_Tile: Shift C Level Of Detail Shift For T Addresses (Bit 10..13)
#define SHIFT_T_D 0xD // Set_Tile: Shift D Level Of Detail Shift For T Addresses (Bit 10..13)
#define SHIFT_T_E 0xE // Set_Tile: Shift E Level Of Detail Shift For T Addresses (Bit 10..13)
#define SHIFT_T_F 0xF // Set_Tile: Shift F Level Of Detail Shift For T Addresses (Bit 10..13)
#define MASK_T_0 0x0 // Set_Tile: Mask 0 For Wrapping/Mirroring In T Direction, Zero = Clamp (Bit 14..17)
#define MASK_T_1 0x1 // Set_Tile: Mask 1 For Wrapping/Mirroring In T Direction, Pass (Mask) LSBs Of T Address (Bit 14..17)
#define MASK_T_2 0x2 // Set_Tile: Mask 2 For Wrapping/Mirroring In T Direction, Pass (Mask) LSBs Of T Address (Bit 14..17)
#define MASK_T_3 0x3 // Set_Tile: Mask 3 For Wrapping/Mirroring In T Direction, Pass (Mask) LSBs Of T Address (Bit 14..17)
#define MASK_T_4 0x4 // Set_Tile: Mask 4 For Wrapping/Mirroring In T Direction, Pass (Mask) LSBs Of T Address (Bit 14..17)
#define MASK_T_5 0x5 // Set_Tile: Mask 5 For Wrapping/Mirroring In T Direction, Pass (Mask) LSBs Of T Address (Bit 14..17)
#define MASK_T_6 0x6 // Set_Tile: Mask 6 For Wrapping/Mirroring In T Direction, Pass (Mask) LSBs Of T Address (Bit 14..17)
#define MASK_T_7 0x7 // Set_Tile: Mask 7 For Wrapping/Mirroring In T Direction, Pass (Mask) LSBs Of T Address (Bit 14..17)
#define MASK_T_8 0x8 // Set_Tile: Mask 8 For Wrapping/Mirroring In T Direction, Pass (Mask) LSBs Of T Address (Bit 14..17)
#define MASK_T_9 0x9 // Set_Tile: Mask 9 For Wrapping/Mirroring In T Direction, Pass (Mask) LSBs Of T Address (Bit 14..17)
#define MASK_T_A 0xA // Set_Tile: Mask A For Wrapping/Mirroring In T Direction, Pass (Mask) LSBs Of T Address (Bit 14..17)
#define MASK_T_B 0xB // Set_Tile: Mask B For Wrapping/Mirroring In T Direction, Pass (Mask) LSBs Of T Address (Bit 14..17)
#define MASK_T_C 0xC // Set_Tile: Mask C For Wrapping/Mirroring In T Direction, Pass (Mask) LSBs Of T Address (Bit 14..17)
#define MASK_T_D 0xD // Set_Tile: Mask D For Wrapping/Mirroring In T Direction, Pass (Mask) LSBs Of T Address (Bit 14..17)
#define MASK_T_E 0xE // Set_Tile: Mask E For Wrapping/Mirroring In T Direction, Pass (Mask) LSBs Of T Address (Bit 14..17)
#define MASK_T_F 0xF // Set_Tile: Mask F For Wrapping/Mirroring In T Direction, Pass (Mask) LSBs Of T Address (Bit 14..17)
#define MIRROR_T 1 // Set_Tile: Mirror Enable For T Direction (Bit 18)
#define CLAMP_T 1  // Set_Tile:  Clamp Enable For T Direction (Bit 19)
#define PALETTE_0 0x0 // Set_Tile: Palette Number 0 For 4Bit Color Indexed Texels, This Number Is The MS 4Bits Of An 8Bit Index (Bit 20..23)
#define PALETTE_1 0x1 // Set_Tile: Palette Number 1 For 4Bit Color Indexed Texels, This Number Is The MS 4Bits Of An 8Bit Index (Bit 20..23)
#define PALETTE_2 0x2 // Set_Tile: Palette Number 2 For 4Bit Color Indexed Texels, This Number Is The MS 4Bits Of An 8Bit Index (Bit 20..23)
#define PALETTE_3 0x3 // Set_Tile: Palette Number 3 For 4Bit Color Indexed Texels, This Number Is The MS 4Bits Of An 8Bit Index (Bit 20..23)
#define PALETTE_4 0x4 // Set_Tile: Palette Number 4 For 4Bit Color Indexed Texels, This Number Is The MS 4Bits Of An 8Bit Index (Bit 20..23)
#define PALETTE_5 0x5 // Set_Tile: Palette Number 5 For 4Bit Color Indexed Texels, This Number Is The MS 4Bits Of An 8Bit Index (Bit 20..23)
#define PALETTE_6 0x6 // Set_Tile: Palette Number 6 For 4Bit Color Indexed Texels, This Number Is The MS 4Bits Of An 8Bit Index (Bit 20..23)
#define PALETTE_7 0x7 // Set_Tile: Palette Number 7 For 4Bit Color Indexed Texels, This Number Is The MS 4Bits Of An 8Bit Index (Bit 20..23)
#define PALETTE_8 0x8 // Set_Tile: Palette Number 8 For 4Bit Color Indexed Texels, This Number Is The MS 4Bits Of An 8Bit Index (Bit 20..23)
#define PALETTE_9 0x9 // Set_Tile: Palette Number 9 For 4Bit Color Indexed Texels, This Number Is The MS 4Bits Of An 8Bit Index (Bit 20..23)
#define PALETTE_A 0xA // Set_Tile: Palette Number A For 4Bit Color Indexed Texels, This Number Is The MS 4Bits Of An 8Bit Index (Bit 20..23)
#define PALETTE_B 0xB // Set_Tile: Palette Number B For 4Bit Color Indexed Texels, This Number Is The MS 4Bits Of An 8Bit Index (Bit 20..23)
#define PALETTE_C 0xC // Set_Tile: Palette Number C For 4Bit Color Indexed Texels, This Number Is The MS 4Bits Of An 8Bit Index (Bit 20..23)
#define PALETTE_D 0xD // Set_Tile: Palette Number D For 4Bit Color Indexed Texels, This Number Is The MS 4Bits Of An 8Bit Index (Bit 20..23)
#define PALETTE_E 0xE // Set_Tile: Palette Number E For 4Bit Color Indexed Texels, This Number Is The MS 4Bits Of An 8Bit Index (Bit 20..23)
#define PALETTE_F 0xF // Set_Tile: Palette Number F For 4Bit Color Indexed Texels, This Number Is The MS 4Bits Of An 8Bit Index (Bit 20..23)
// Set Tile/Set Texture Image/Set Color Image HI Word
#define SIZE_OF_PIXEL_4B 0  // Set_Tile/Set_Texture_Image/Set_Color_Image: Size Of Pixel/Texel Color Element 4B (Bit 51..52)
#define SIZE_OF_PIXEL_8B 1  // Set_Tile/Set_Texture_Image/Set_Color_Image: Size Of Pixel/Texel Color Element 8B (Bit 51..52)
#define SIZE_OF_PIXEL_16B 2 // Set_Tile/Set_Texture_Image/Set_Color_Image: Size Of Pixel/Texel Color Element 16B (Bit 51..52)
#define SIZE_OF_PIXEL_32B 3 // Set_Tile/Set_Texture_Image/Set_Color_Image: Size Of Pixel/Texel Color Element 32B (Bit 51..52)
#define IMAGE_DATA_FORMAT_RGBA 0       // Set_Tile/Set_Texture_Image/Set_Color_Image: Image Data Format RGBA (Bit 53..55)
#define IMAGE_DATA_FORMAT_YUV 1        // Set_Tile/Set_Texture_Image/Set_Color_Image: Image Data Format YUV (Bit 53..55)
#define IMAGE_DATA_FORMAT_COLOR_INDX 2 // Set_Tile/Set_Texture_Image/Set_Color_Image: Image Data Format COLOR_INDX (Bit 53..55)
#define IMAGE_DATA_FORMAT_IA 3         // Set_Tile/Set_Texture_Image/Set_Color_Image: Image Data Format IA (Bit 53..55)
#define IMAGE_DATA_FORMAT_I 4          // Set_Tile/Set_Texture_Image/Set_Color_Image: Image Data Format I (Bit 53..55)

// Set Tile (Command Format)
#define rdp_set_tile( format, size, line, tmem, tile, palette, ct, mt, maskt, shiftt, cs, ms, masks, shifts ) \
  0x35000000 | (uint8_t)(format) << 21 | (uint8_t)(size) << 19 | (uint16_t)(line) << 9 | (uint16_t)(tmem), \
  (uint8_t)(tile) << 24 | (uint8_t)(palette) << 20 | (uint8_t)(ct) << 19 | (uint8_t)(mt) << 18 | (uint8_t)(maskt) << 14 | (uint8_t)(shiftt) << 10 | (uint8_t)(cs) << 9 | (uint8_t)(ms) << 8 | (uint8_t)(masks) << 4 | (uint8_t)(shifts) \

// Fill Rectangle (Top Left To Bottom Right)
#define rdp_fill_rectangle( xh, yh, xl, yl ) \
  0x36000000 | ((int)((float)(xl) * 4.0) & 0xFFF) << 12 | ((int)((float)(yl) * 4.0) & 0xFFF), \
  ((int)((float)(xh) * 4.0) & 0xFFF) << 12 | ((int)((float)(yh) * 4.0) & 0xFFF) \

// Set Fill Color 16-Bit (R,G,B,A)
#define rdp_set_fill_color_16( r, g, b, a ) \
  0x37000000, \
  ((uint8_t)(r) >> 3) << 27 | ((uint8_t)(g) >> 3) << 22 | ((uint8_t)(b) >> 3) << 17 | ((uint8_t)(a) >> 7) << 16 \
  | ((uint8_t)(r) >> 3) << 11 | ((uint8_t)(g) >> 3) << 6 | ((uint8_t)(b) >> 3) << 1 | ((uint8_t)(a) >> 7) \

// Set Fill Color 32-Bit (R,G,B,A)
#define rdp_set_fill_color_32( r, g, b, a ) \
  0x37000000, \
  ((uint8_t)(r) << 24) | ((uint8_t)(g) << 16) | ((uint8_t)(b) << 8) | (uint8_t)(a) \

// Set Fog Color (R,G,B,A)
#define rdp_set_fog_color( r, g, b, a ) \
  0x38000000, \
  (uint8_t)(r) << 24 | (uint8_t)(g) << 16 | (uint8_t)(b) << 8 | (uint8_t)(a) \

// Set Blend Color (R,G,B,A)
#define rdp_set_blend_color( r, g, b, a ) \
  0x39000000, \
  (uint8_t)(r) << 24 | (uint8_t)(g) << 16 | (uint8_t)(b) << 8 | (uint8_t)(a) \

// Set Primitive Color (R,G,B,A)
#define rdp_set_prim_color( r, g, b, a ) \
  0x3A000000, \
  (uint8_t)(r) << 24 | (uint8_t)(g) << 16 | (uint8_t)(b) << 8 | (uint8_t)(a) \

// Set Environment Color (R,G,B,A)
#define rdp_set_env_color( r, g, b, a ) \
  0x3B000000, \
  (uint8_t)(r) << 24 | (uint8_t)(g) << 16 | (uint8_t)(b) << 8 | (uint8_t)(a) \

// Set Combine Mode
#define rdp_set_combine_mode( sub_a_r0, mul_r0, sub_a_a0, mul_a0, sub_a_r1, mul_r1, sub_b_r0, sub_b_r1, sub_a_a1, mul_a1, add_r0, sub_b_a0, add_a0, add_r1, sub_b_a1, add_a1 ) \
  0x3C000000 | (uint8_t)(sub_a_r0) << 20 | (uint8_t)(mul_r0) << 15 | (uint8_t)(sub_a_a0) << 12 | (uint8_t)(mul_a0) << 9 | (uint8_t)(sub_a_r1) << 5 | (uint8_t)(mul_r1), \
  (uint8_t)(sub_b_r0) << 28 | (uint8_t)(sub_b_r1) << 24 | (uint8_t)(sub_a_a1) << 21 | (uint8_t)(mul_a1) << 18 | (uint8_t)(add_r0) << 15 | (uint8_t)(sub_b_a0) << 12 | (uint8_t)(add_a0) << 9 | (uint8_t)(add_r1) << 6 | (uint8_t)(sub_b_a1) << 3 | (uint8_t)(add_a1) \

// Set Texture Image
#define rdp_set_texture_image( format, size, width, address ) \
  0x3D000000 | (uint8_t)(format) << 21 | (uint8_t)(size) << 19 | ((uint16_t)(width) - 1), \
  (uint32_t)(address) \

// Set Z Image
#define rdp_set_z_image( address ) \
  0x3E000000, \
  (uint32_t)(address) \

// Set Color Image
#define rdp_set_color_image( format, size, width, address ) \
  0x3F000000 | (uint8_t)(format) << 21 | (uint8_t)(size) << 19 | ((uint16_t)(width) - 1), \
  (uint32_t)(address) \
