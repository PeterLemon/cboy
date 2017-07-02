#include "audio.h"

const int sample_rate = 48000;
void fill_audio( void *userdata, uint8_t *stream, int len );

int audio_init()
{
  return 0;
}

void fill_audio( void *userdata, uint8_t *stream, int len )
{
}

void audio_cleanup()
{
}

uint8_t audio_read( uint16_t address )
{
  return 0;
}

void audio_write( uint16_t address, uint8_t data )
{
}

void audio_frame()
{
}

