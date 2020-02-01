/*
 * lcd.c
 *
 *  Created on: 2020. 1. 31.
 *      Author: Baram
 */




#include "lcd.h"
#include "pwm.h"
#include "ili9225.h"
#include "hangul/PHan_Lib.h"


#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif


#define PIXEL_FORMAT_RGB565          0x00000002U   /*!< RGB565 LTDC pixel format   */




static bool is_init = false;
static uint8_t backlight_value = 100;
static uint8_t frame_index = 0;
static volatile bool is_double_buffer = true;
static int32_t text_bg_color = -1;

static uint16_t *p_draw_frame_buf = NULL;
static __attribute__((section(".sram_d1"))) uint16_t frame_buffer[2][HW_LCD_WIDTH * HW_LCD_HEIGHT];




void lcdFillBuffer(void * pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex);
void lcdSwapFrameBuffer(void);
void disHanFont(int x, int y, PHAN_FONT_OBJ *FontPtr, uint16_t textcolor);


bool lcdInit(void)
{
  is_init = true;


  backlight_value = 100;


  __HAL_RCC_DMA2D_CLK_ENABLE();



  for (int i=0; i<HW_LCD_WIDTH*HW_LCD_HEIGHT; i++)
  {
    frame_buffer[0][i] = black;
    frame_buffer[1][i] = black;
  }

  if (is_double_buffer == true)
  {
    p_draw_frame_buf = frame_buffer[frame_index ^ 1];
  }
  else
  {
    p_draw_frame_buf = frame_buffer[frame_index];
  }

  lcdClear(black);

  lcdSetBackLight(backlight_value);

  return true;
}

bool lcdIsInit(void)
{
  return is_init;
}

void lcdReset(void)
{

}

uint8_t lcdGetBackLight(void)
{
  return backlight_value;
}

void lcdSetBackLight(uint8_t value)
{
  pwmWrite(0, map(value, 0, 100, 0, 255));

  if (value != backlight_value)
  {
    backlight_value = value;
  }
}

uint32_t lcdReadPixel(uint16_t x_pos, uint16_t y_pos)
{
  return p_draw_frame_buf[y_pos * HW_LCD_WIDTH + x_pos];
}

void lcdDrawPixel(uint16_t x_pos, uint16_t y_pos, uint32_t rgb_code)
{
  p_draw_frame_buf[y_pos * HW_LCD_WIDTH + x_pos] = rgb_code;
}

void lcdClear(uint32_t rgb_code)
{
  lcdClearBuffer(rgb_code);

  lcdUpdateDraw();
}

void lcdClearBuffer(uint32_t rgb_code)
{
  lcdFillBuffer((void *)lcdGetFrameBuffer(), lcdGetWidth(), lcdGetHeight(), 0, rgb_code);
}

void lcdSetDoubleBuffer(bool enable)
{
  is_double_buffer = enable;

  if (enable == true)
  {
    p_draw_frame_buf = frame_buffer[frame_index^1];
  }
  else
  {
    p_draw_frame_buf = frame_buffer[frame_index];
  }
}

bool lcdGetDoubleBuffer(void)
{
  return is_double_buffer;
}

bool lcdDrawAvailable(void)
{
  return ili9225DrawAvailable();
}

void lcdRequestDraw(void)
{
  lcdSwapFrameBuffer();

  ili9225RequestDraw();
}

void lcdUpdateDraw(void)
{
  lcdRequestDraw();
  while(lcdDrawAvailable() != true);
}

void lcdSwapFrameBuffer(void)
{
  if (is_double_buffer == true)
  {
    frame_index ^= 1;
    ili9225SetFrameBuffer(frame_buffer[frame_index]);
    p_draw_frame_buf = frame_buffer[frame_index ^ 1];
  }
  else
  {
    ili9225SetFrameBuffer(frame_buffer[frame_index]);
    p_draw_frame_buf = frame_buffer[frame_index];
  }
}

uint16_t *lcdGetFrameBuffer(void)
{
  return p_draw_frame_buf;
}

uint16_t *lcdGetCurrentFrameBuffer(void)
{
  return frame_buffer[frame_index];;
}

void lcdFillBuffer(void * pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex)
{
  /* Set up mode */
  DMA2D->CR      = 0x00030000UL | (1 << 9);
  DMA2D->OCOLR   = ColorIndex;

  /* Set up pointers */
  DMA2D->OMAR    = (uint32_t)pDst;

  /* Set up offsets */
  DMA2D->OOR     = OffLine;

  /* Set up pixel format */
  DMA2D->OPFCCR  = PIXEL_FORMAT_RGB565;

  /*  Set up size */
  DMA2D->NLR     = (uint32_t)(xSize << 16) | (uint32_t)ySize;

  DMA2D->CR     |= DMA2D_CR_START;

  /* Wait until transfer is done */
  while (DMA2D->CR & DMA2D_CR_START)
  {
  }
}

void lcdDisplayOff(void)
{
  pwmWrite(0, 0);
}

void lcdDisplayOn(void)
{
  pwmWrite(0, backlight_value);
}

int32_t lcdGetWidth(void)
{
  return ili9225GetWidth();
}

int32_t lcdGetHeight(void)
{
  return ili9225GetHeight();
}


void lcdDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color)
{
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);

  if (steep)
  {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1)
  {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++)
  {
    if (steep)
    {
      lcdDrawPixel(y0, x0, color);
    } else
    {
      lcdDrawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

void lcdDrawVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  lcdDrawLine(x, y, x, y+h-1, color);
}

void lcdDrawHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  lcdDrawLine(x, y, x+w-1, y, color);
}

void lcdDrawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  for (int16_t i=x; i<x+w; i++)
  {
    lcdDrawVLine(i, y, h, color);
  }
}

void lcdDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  lcdDrawHLine(x, y, w, color);
  lcdDrawHLine(x, y+h-1, w, color);
  lcdDrawVLine(x, y, h, color);
  lcdDrawVLine(x+w-1, y, h, color);
}

void lcdDrawFillScreen(uint16_t color)
{
  lcdDrawFillRect(0, 0, HW_LCD_WIDTH, HW_LCD_HEIGHT, color);
}

void lcdSetBgColor(int32_t bg_color)
{
  text_bg_color = bg_color;
}

void lcdPrintf(int x, int y, uint16_t color,  const char *fmt, ...)
{
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  char print_buffer[256];
  int Size_Char;
  int i, x_Pre = x;
  PHAN_FONT_OBJ FontBuf;


  len = vsnprintf(print_buffer, 255, fmt, arg);
  va_end (arg);

  for( i=0; i<len; i+=Size_Char )
  {
    PHan_FontLoad( &print_buffer[i], &FontBuf );


    disHanFont( x, y, &FontBuf, color);

    Size_Char = FontBuf.Size_Char;
    if (Size_Char >= 2)
    {
        x += 2*8;
    }
    else
    {
        x += 1*8;
    }

    if( HW_LCD_WIDTH < x )
    {
        x  = x_Pre;
        y += 16;
    }

    if( FontBuf.Code_Type == PHAN_END_CODE ) break;
  }
}

uint32_t lcdGetStrWidth(const char *fmt, ...)
{
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  char print_buffer[256];
  int Size_Char;
  int i;
  PHAN_FONT_OBJ FontBuf;
  uint32_t str_len;


  len = vsnprintf(print_buffer, 255, fmt, arg);
  va_end (arg);

  str_len = 0;

  for( i=0; i<len; i+=Size_Char )
  {
    PHan_FontLoad( &print_buffer[i], &FontBuf );

    Size_Char = FontBuf.Size_Char;

    str_len += (Size_Char * 8);

    if( FontBuf.Code_Type == PHAN_END_CODE ) break;
  }

  return str_len;
}

void disHanFont(int x, int y, PHAN_FONT_OBJ *FontPtr, uint16_t textcolor)
{
  uint16_t    i, j, Loop;
  uint16_t  FontSize = FontPtr->Size_Char;
  uint16_t index_x;

  if (FontSize > 2)
  {
    FontSize = 2;
  }

  for ( i = 0 ; i < 16 ; i++ )        // 16 Lines per Font/Char
  {
    index_x = 0;
    for ( j = 0 ; j < FontSize ; j++ )      // 16 x 16 (2 Bytes)
    {
      for( Loop=0; Loop<8; Loop++ )
      {
        if( FontPtr->FontBuffer[i*FontSize +j] & (0x80>>Loop))
        {
          lcdDrawPixel(x + index_x, y + i, textcolor);
        }
        else if (text_bg_color >= 0)
        {
          lcdDrawPixel(x + index_x, y + i, text_bg_color & 0xFFFF);
        }

        index_x++;
      }
    }
  }
}
