#include "rdp.h"

/*** VARIABLES ***/
uint32_t __bitdepth = BPP16; // FIX WITH VIDEO CHECK (needs to know if 16 or 32bit mode)
static uint32_t memory_pos = 0; // RDP DRAM LIST

/*** RDP COMMANDS ***/

// Create RDP commands
void rdp_command( uint32_t data )
{
    *(uintptr_t *)(0xA0300000 | memory_pos) = data;
    memory_pos += 4; // 32 bit / 8
    memory_pos = memory_pos % 131072; // TOP LIMIT
}

// Run RDP Command List (From Start Address To End Address)
void rdp_run( uint32_t start, uint32_t end )
{
    // Store DPC Command Start Address To DP Start Register (0xA4100000)
    *(uintptr_t *)0xA4100000 = 0xA0300000 | start;

    // Store DPC Command End Address To DP End Register (0xA4100004)
    *(uintptr_t *)0xA4100004 = 0xA0300000 | end;
}

// No Op (No Operation)
void rdp_no_op( void )
{
    rdp_command( 0x00000000 );
    rdp_command( 0x00000000 );
}

// Fill Triangle (Flat Non-Shaded) Edge Coefficients
void rdp_fill_triangle( uint8_t lft, uint8_t level, uint8_t tile, float yl, float ym, float yh, float xl, float dxldy, float xh, float dxhdy, float xm, float dxmdy )
{
    rdp_command( 0x08000000 | lft << 23 | level << 19 | tile << 16 | ((int)(yl * 4.0) & 0x3FFF) );
    rdp_command( ((int)(ym * 4.0) & 0x3FFF) << 16 | ((int)(yh * 4.0) & 0x3FFF) );
    rdp_command( (int)(xl * 65536.0) );
    rdp_command( (int)(dxldy * 65536.0) );
    rdp_command( (int)(xh * 65536.0) );
    rdp_command( (int)(dxhdy * 65536.0) );
    rdp_command( (int)(xm * 65536.0) );
    rdp_command( (int)(dxmdy * 65536.0) );
}

// Fill Z-Buffer Triangle (Flat Non-Shaded Z-Buffered) Edge Coefficients
void rdp_fill_zbuffer_triangle( uint8_t lft, uint8_t level, uint8_t tile, float yl, float ym, float yh, float xl, float dxldy, float xh, float dxhdy, float xm, float dxmdy )
{
    rdp_command( 0x09000000 | lft << 23 | level << 19 | tile << 16 | ((int)(yl * 4.0) & 0x3FFF) );
    rdp_command( ((int)(ym * 4.0) & 0x3FFF) << 16 | ((int)(yh * 4.0) & 0x3FFF) );
    rdp_command( (int)(xl * 65536.0) );
    rdp_command( (int)(dxldy * 65536.0) );
    rdp_command( (int)(xh * 65536.0) );
    rdp_command( (int)(dxhdy * 65536.0) );
    rdp_command( (int)(xm * 65536.0) );
    rdp_command( (int)(dxmdy * 65536.0) );
}

// Texture Triangle (Textured Non-Shaded) Edge Coefficients
void rdp_texture_triangle( uint8_t lft, uint8_t level, uint8_t tile, float yl, float ym, float yh, float xl, float dxldy, float xh, float dxhdy, float xm, float dxmdy )
{
    rdp_command( 0x0A000000 | lft << 23 | level << 19 | tile << 16 | ((int)(yl * 4.0) & 0x3FFF) );
    rdp_command( ((int)(ym * 4.0) & 0x3FFF) << 16 | ((int)(yh * 4.0) & 0x3FFF) );
    rdp_command( (int)(xl * 65536.0) );
    rdp_command( (int)(dxldy * 65536.0) );
    rdp_command( (int)(xh * 65536.0) );
    rdp_command( (int)(dxhdy * 65536.0) );
    rdp_command( (int)(xm * 65536.0) );
    rdp_command( (int)(dxmdy * 65536.0) );
}

// Texture Z-Buffer Triangle (Textured Non-Shaded Z-Buffered) Edge Coefficients
void rdp_texture_zbuffer_triangle( uint8_t lft, uint8_t level, uint8_t tile, float yl, float ym, float yh, float xl, float dxldy, float xh, float dxhdy, float xm, float dxmdy )
{
    rdp_command( 0x0B000000 | lft << 23 | level << 19 | tile << 16 | ((int)(yl * 4.0) & 0x3FFF) );
    rdp_command( ((int)(ym * 4.0) & 0x3FFF) << 16 | ((int)(yh * 4.0) & 0x3FFF) );
    rdp_command( (int)(xl * 65536.0) );
    rdp_command( (int)(dxldy * 65536.0) );
    rdp_command( (int)(xh * 65536.0) );
    rdp_command( (int)(dxhdy * 65536.0) );
    rdp_command( (int)(xm * 65536.0) );
    rdp_command( (int)(dxmdy * 65536.0) );
}

// Shade Triangle (Goraud Shaded) Edge Coefficients
void rdp_shade_triangle( uint8_t lft, uint8_t level, uint8_t tile, float yl, float ym, float yh, float xl, float dxldy, float xh, float dxhdy, float xm, float dxmdy )
{
    rdp_command( 0x0C000000 | lft << 23 | level << 19 | tile << 16 | ((int)(yl * 4.0) & 0x3FFF) );
    rdp_command( ((int)(ym * 4.0) & 0x3FFF) << 16 | ((int)(yh * 4.0) & 0x3FFF) );
    rdp_command( (int)(xl * 65536.0) );
    rdp_command( (int)(dxldy * 65536.0) );
    rdp_command( (int)(xh * 65536.0) );
    rdp_command( (int)(dxhdy * 65536.0) );
    rdp_command( (int)(xm * 65536.0) );
    rdp_command( (int)(dxmdy * 65536.0) );
}

// Shade Z-Buffer Triangle (Goraud Shaded Z-Buffered) Edge Coefficients
void rdp_shade_zbuffer_triangle( uint8_t lft, uint8_t level, uint8_t tile, float yl, float ym, float yh, float xl, float dxldy, float xh, float dxhdy, float xm, float dxmdy )
{
    rdp_command( 0x0D000000 | lft << 23 | level << 19 | tile << 16 | ((int)(yl * 4.0) & 0x3FFF) );
    rdp_command( ((int)(ym * 4.0) & 0x3FFF) << 16 | ((int)(yh * 4.0) & 0x3FFF) );
    rdp_command( (int)(xl * 65536.0) );
    rdp_command( (int)(dxldy * 65536.0) );
    rdp_command( (int)(xh * 65536.0) );
    rdp_command( (int)(dxhdy * 65536.0) );
    rdp_command( (int)(xm * 65536.0) );
    rdp_command( (int)(dxmdy * 65536.0) );
}

// Shade Texture Triangle (Goraud Shaded Textured) Edge Coefficients
void rdp_shade_texture_triangle( uint8_t lft, uint8_t level, uint8_t tile, float yl, float ym, float yh, float xl, float dxldy, float xh, float dxhdy, float xm, float dxmdy )
{
    rdp_command( 0x0E000000 | lft << 23 | level << 19 | tile << 16 | ((int)(yl * 4.0) & 0x3FFF) );
    rdp_command( ((int)(ym * 4.0) & 0x3FFF) << 16 | ((int)(yh * 4.0) & 0x3FFF) );
    rdp_command( (int)(xl * 65536.0) );
    rdp_command( (int)(dxldy * 65536.0) );
    rdp_command( (int)(xh * 65536.0) );
    rdp_command( (int)(dxhdy * 65536.0) );
    rdp_command( (int)(xm * 65536.0) );
    rdp_command( (int)(dxmdy * 65536.0) );
}

// Shade Texture Z-Buffer Triangle (Goraud Shaded Textured Z-Buffered) Edge Coefficients
void rdp_shade_texture_zbuffer_triangle( uint8_t lft, uint8_t level, uint8_t tile, float yl, float ym, float yh, float xl, float dxldy, float xh, float dxhdy, float xm, float dxmdy )
{
    rdp_command( 0x0F000000 | lft << 23 | level << 19 | tile << 16 | ((int)(yl * 4.0) & 0x3FFF) );
    rdp_command( ((int)(ym * 4.0) & 0x3FFF) << 16 | ((int)(yh * 4.0) & 0x3FFF) );
    rdp_command( (int)(xl * 65536.0) );
    rdp_command( (int)(dxldy * 65536.0) );
    rdp_command( (int)(xh * 65536.0) );
    rdp_command( (int)(dxhdy * 65536.0) );
    rdp_command( (int)(xm * 65536.0) );
    rdp_command( (int)(dxmdy * 65536.0) );
}

// Shade Coefficients (Concat With Triangle Edge Coefficients Commands)
void rdp_shade_coefficients( float r, float g, float b, float a, float drdx, float dgdx, float dbdx, float dadx, float drde, float dgde, float dbde, float dade, float drdy, float dgdy, float dbdy, float dady )
{
    rdp_command( (int)(r) << 16 | ((int)(g) & 0xFFFF) );
    rdp_command( (int)(b) << 16 | ((int)(a) & 0xFFFF) );
    rdp_command( (int)(drdx) << 16 | ((int)(dgdx) & 0xFFFF) );
    rdp_command( (int)(dbdx) << 16 | ((int)(dadx) & 0xFFFF) );
    rdp_command( (int)(r * 65536.0) << 16 | ((int)(g * 65536.0) & 0xFFFF) );
    rdp_command( (int)(b * 65536.0) << 16 | ((int)(a * 65536.0) & 0xFFFF) );
    rdp_command( (int)(drdx * 65536.0) << 16 | ((int)(dgdx * 65536.0) & 0xFFFF) );
    rdp_command( (int)(dbdx * 65536.0) << 16 | ((int)(dadx * 65536.0) & 0xFFFF) );
    rdp_command( (int)(drde) << 16 | ((int)(dgde) & 0xFFFF) );
    rdp_command( (int)(dbde) << 16 | ((int)(dade) & 0xFFFF) );
    rdp_command( (int)(drdy) << 16 | ((int)(dgdy) & 0xFFFF) );
    rdp_command( (int)(dbdy) << 16 | ((int)(dady) & 0xFFFF) );
    rdp_command( (int)(drde * 65536.0) << 16 | ((int)(dgde * 65536.0) & 0xFFFF) );
    rdp_command( (int)(dbde * 65536.0) << 16 | ((int)(dade * 65536.0) & 0xFFFF) );
    rdp_command( (int)(drdy * 65536.0) << 16 | ((int)(dgdy * 65536.0) & 0xFFFF) );
    rdp_command( (int)(dbdy * 65536.0) << 16 | ((int)(dady * 65536.0) & 0xFFFF) );
}

// Texture Coefficients (Concat With Triangle Edge Coefficients Commands)
void rdp_texture_coefficients( float s, float t, float w, float dsdx, float dtdx, float dwdx, float dsde, float dtde, float dwde, float dsdy, float dtdy, float dwdy )
{
    rdp_command( (int)(s) << 16 | ((int)(t) & 0xFFFF) );
    rdp_command( (int)(w) << 16 );
    rdp_command( (int)(dsdx) << 16 | ((int)(dtdx) & 0xFFFF) );
    rdp_command( (int)(dwdx) << 16 );
    rdp_command( (int)(s * 65536.0) << 16 | ((int)(t * 65536.0) & 0xFFFF) );
    rdp_command( (int)(w * 65536.0) << 16 );
    rdp_command( (int)(dsdx * 65536.0) << 16 | ((int)(dtdx * 65536.0) & 0xFFFF) );
    rdp_command( (int)(dwdx * 65536.0) << 16 );
    rdp_command( (int)(dsde) << 16 | ((int)(dtde) & 0xFFFF) );
    rdp_command( (int)(dwde) << 16 );
    rdp_command( (int)(dsdy) << 16 | ((int)(dtdy) & 0xFFFF) );
    rdp_command( (int)(dwdy) << 16 );
    rdp_command( (int)(dsde * 65536.0) << 16 | ((int)(dtde * 65536.0) & 0xFFFF) );
    rdp_command( (int)(dwde * 65536.0) << 16 );
    rdp_command( (int)(dsdy * 65536.0) << 16 | ((int)(dtdy * 65536.0) & 0xFFFF) );
    rdp_command( (int)(dwdy * 65536.0) << 16 );
}

// Z-Buffer Coefficients (Concat With Triangle Edge Coefficients Commands)
void rdp_zbuffer_coefficients( float z, float dzdx, float dzde, float dzdy )
{
    rdp_command( (int)(z * 65536.0) );
    rdp_command( (int)(dzdx * 65536.0) );
    rdp_command( (int)(dzde * 65536.0) );
    rdp_command( (int)(dzdy * 65536.0) );
}

// Texture Rectangle (Top Left To Bottom Right)
void rdp_texture_rectangle( float xh, float yh, float xl, float yl, float s, float t, float dsdx, float dtdy, uint8_t tile )
{
    rdp_command( 0x24000000 | ((int)(xl * 4.0) & 0xFFF) << 12 | ((int)(yl * 4.0) & 0xFFF) ); 
    rdp_command( tile << 24 | ((int)(xh * 4.0) & 0xFFF) << 12 | ((int)(yh * 4.0) & 0xFFF) );
    rdp_command( (int)(s * 32.0) << 16 | ((int)(t * 32.0) & 0xFFFF) );
    rdp_command( (int)(dsdx * 1024.0) << 16 | ((int)(dtdy * 1024.0) & 0xFFFF) );
}

// Texture Rectangle Flip (Top Left To Bottom Right)
void rdp_texture_rectangle_flip( float xh, float yh, float xl, float yl, float s, float t, float dsdx, float dtdy, uint8_t tile )
{
    rdp_command( 0x25000000 | ((int)(xl * 4.0) & 0xFFF) << 12 | ((int)(yl * 4.0) & 0xFFF) ); 
    rdp_command( tile << 24 | ((int)(xh * 4.0) & 0xFFF) << 12 | ((int)(yh * 4.0) & 0xFFF) );
    rdp_command( (int)(s * 32.0) << 16 | ((int)(t * 32.0) & 0xFFFF) );
    rdp_command( (int)(dsdx * 1024.0) << 16 | ((int)(dtdy * 1024.0) & 0xFFFF) );
}

// Sync Load
void rdp_sync_load( void )
{
    rdp_command( 0x26000000 );
    rdp_command( 0x00000000 );
}

// Sync Pipe
void rdp_sync_pipe( void )
{
    rdp_command( 0x27000000 );
    rdp_command( 0x00000000 );
}

// Sync Tile
void rdp_sync_tile( void )
{
    rdp_command( 0x28000000 );
    rdp_command( 0x00000000 );
}

// Sync Full
void rdp_sync_full( void )
{
    rdp_command( 0x29000000 );
    rdp_command( 0x00000000 );
}

// Set Key GB (Coefficients Used For Green/Blue Keying)
void rdp_set_key_gb( float widthg, uint8_t centerg, uint8_t scaleg, float widthb, uint8_t centerb, uint8_t scaleb )
{
    rdp_command( 0x2A000000 | ((int)(widthg * 16.0) & 0xFFF) << 12 | ((int)(widthb * 16.0) & 0xFFF) );
    rdp_command( centerg << 24 | scaleg << 16 | centerb << 8 | scaleb );
}

// Set Key R (Coefficients Used For Red Keying)
void rdp_set_key_r( float width, uint8_t center, uint8_t scale )
{
    rdp_command( 0x2B000000 );
    rdp_command( (int)(width * 16.0) << 16 | center << 8 | scale );
}

// Set Convert (Coefficients For Converting YUV Pixels To RGB)
void rdp_set_convert( float k0, float k1, float k2, float k3, float k4, float k5 )
{
    rdp_command( 0x2C000000 | ((int)(k0 * 128.0) & 0x1FF) << 13 | ((int)(k1 * 128.0) & 0x1FF) << 4 | ((int)(k2 * 128.0) & 0x1FF) >> 5 );
    rdp_command( (int)(k2 * 128.0) << 27 | ((int)(k3 * 128.0) & 0x1FF) << 18 | ((int)(k4 * 128.0) & 0x1FF) << 9 | ((int)(k5 * 128.0) & 0x1FF) );
}

// Set Scissor (Top Left To Bottom Right)
void rdp_set_scissor( float xh, float yh, float xl, float yl, uint8_t f, uint8_t o )
{
    rdp_command( 0x2D000000 | ((int)(xh * 4.0) & 0xFFF) << 12 | ((int)(yh * 4.0) & 0xFFF) );
    rdp_command( f << 25 | o << 24 | ((int)(xl * 4.0) & 0xFFF) << 12 | ((int)(yl * 4.0) & 0xFFF) );
}

// Set Primitive Depth (Primitive Z, Primitive Delta Z)
void rdp_set_prim_depth( float z, float dz )
{
    rdp_command( 0x2E000000 );
    rdp_command( (int)(z) << 16 | ((int)(dz) & 0xFFFF) );
}

// Set Other Modes
void rdp_set_other_modes( uint64_t mode )
{
    uint32_t mode_hi = mode >> 32;
    uint32_t mode_lo = mode & 0xFFFFFFFF;

    rdp_command( 0x2F000000 | mode_hi );
    rdp_command( mode_lo );
}

// Load TLUT (Top Left To Bottom Right)
void rdp_load_tlut( float sl, float tl, float sh, float th, uint8_t tile )
{
    rdp_command( 0x30000000 | ((int)(sl * 4.0) & 0xFFF) << 12 | ((int)(tl * 4.0) & 0xFFF) ); 
    rdp_command( tile << 24 | ((int)(sh * 4.0) & 0xFFF) << 12 | ((int)(th * 4.0) & 0xFFF) );
}

// Set Tile Size (Top Left To Bottom Right)
void rdp_set_tile_size( float sl, float tl, float sh, float th, uint8_t tile )
{
    rdp_command( 0x32000000 | ((int)(sl * 4.0) & 0xFFF) << 12 | ((int)(tl * 4.0) & 0xFFF) ); 
    rdp_command( tile << 24 | ((int)(sh * 4.0) & 0xFFF) << 12 | ((int)(th * 4.0) & 0xFFF) );
}

// Load Block (Top Left To Bottom Right)
void rdp_load_block( float sl, float tl, float sh, float dxt, uint8_t tile )
{
    rdp_command( 0x33000000 | ((int)(sl) & 0xFFF) << 12 | ((int)(tl) & 0xFFF) ); 
    rdp_command( tile << 24 | ((int)(sh) & 0xFFF) << 12 | ((int)(dxt * 2048.0) & 0xFFF) );
}

// Load Tile (Top Left To Bottom Right)
void rdp_load_tile( float sl, float tl, float sh, float th, uint8_t tile )
{
    rdp_command( 0x34000000 | ((int)(sl * 4.0) & 0xFFF) << 12 | ((int)(tl * 4.0) & 0xFFF) ); 
    rdp_command( tile << 24 | ((int)(sh * 4.0) & 0xFFF) << 12 | ((int)(th * 4.0) & 0xFFF) );
}

// Set Tile (Command Format)
void rdp_set_tile( uint8_t format, uint8_t size, uint16_t line, uint16_t tmem, uint8_t tile, uint8_t palette, uint8_t ct, uint8_t mt, uint8_t maskt, uint8_t shiftt, uint8_t cs, uint8_t ms, uint8_t masks, uint8_t shifts )
{
    rdp_command( 0x35000000 | format << 21 | size << 19 | line << 9 | tmem ); 
    rdp_command( tile << 24 | palette << 20 | ct << 19 | mt << 18 | maskt << 14 | shiftt << 10 | cs << 9 | ms << 8 | masks << 4 | shifts );
}

// Fill Rectangle (Top Left To Bottom Right)
void rdp_fill_rectangle( float xh, float yh, float xl, float yl )
{
    rdp_command( 0x36000000 | ((int)(xl * 4.0) & 0xFFF) << 12 | ((int)(yl * 4.0) & 0xFFF) ); 
    rdp_command( ((int)(xh * 4.0) & 0xFFF) << 12 | ((int)(yh * 4.0) & 0xFFF) );
}

// Set Fill Color (R,G,B,A)
void rdp_set_fill_color( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
    uint32_t color;
	
    if( __bitdepth == BPP16 )
    {
        // 8 to 5 bit (RGB555)
        r = r >> 3;
        g = g >> 3;
        b = b >> 3;
        a = a >> 7;
		
        // Pack Color Twice For 16BPP Mode
        color = r << 11 | g << 6 | b << 1 | a;
        color = color | color << 16;
    }
    else
        color = r << 24 | g << 16 | b << 8 | a;
	
    // Set Fill Color
    rdp_command( 0x37000000 );
    rdp_command( color );
}

// Set Fog Color (R,G,B,A)
void rdp_set_fog_color( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
    rdp_command( 0x38000000 );
    rdp_command( r << 24 | g << 16 | b << 8 | a );
}

// Set Blend Color (R,G,B,A)
void rdp_set_blend_color( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
    rdp_command( 0x39000000 );
    rdp_command( r << 24 | g << 16 | b << 8 | a );
}

// Set Primitive Color (R,G,B,A)
void rdp_set_prim_color( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
    rdp_command( 0x3A000000 );
    rdp_command( r << 24 | g << 16 | b << 8 | a );
}

// Set Environment Color (R,G,B,A)
void rdp_set_env_color( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
    rdp_command( 0x3B000000 );
    rdp_command( r << 24 | g << 16 | b << 8 | a );
}

// Set Combine Mode
void rdp_set_combine_mode( uint8_t sub_a_r0, uint8_t mul_r0, uint8_t sub_a_a0, uint8_t mul_a0, uint8_t sub_a_r1, uint8_t mul_r1, uint8_t sub_b_r0, uint8_t sub_b_r1, uint8_t sub_a_a1, uint8_t mul_a1, uint8_t add_r0, uint8_t sub_b_a0, uint8_t add_a0, uint8_t add_r1, uint8_t sub_b_a1, uint8_t add_a1 )
{
    rdp_command( 0x3C000000 | sub_a_r0 << 20 | mul_r0 << 15 | sub_a_a0 << 12 | mul_a0 << 9 | sub_a_r1 << 5 | mul_r1 );
    rdp_command( sub_b_r0 << 28 | sub_b_r1 << 24 | sub_a_a1 << 21 | mul_a1 << 18 | add_r0 << 15 | sub_b_a0 << 12 | add_a0 << 9 | add_r1 << 6 | sub_b_a1 << 3 | add_a1);
}

// Set Texture Image
void rdp_set_texture_image( uint8_t format, uint8_t size, uint16_t width, uint32_t address )
{
    rdp_command( 0x3D000000 | format << 21 | size << 19 | (width - 1) );
    rdp_command( address );
}

// Set Z Image
void rdp_set_z_image( uint32_t address )
{
    rdp_command( 0x3E000000 );
    rdp_command( address );
}

// Set Color Image
void rdp_set_color_image( uint8_t format, uint8_t size, uint16_t width, uint32_t address )
{
    rdp_command( 0x3F000000 | format << 21 | size << 19 | (width - 1) );
    rdp_command( address );
}