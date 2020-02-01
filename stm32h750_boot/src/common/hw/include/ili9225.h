/*
 * ili9225.h
 *
 *  Created on: 2020. 1. 30.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_ILI9225_H_
#define SRC_COMMON_HW_INCLUDE_ILI9225_H_



#ifdef __cplusplus
 extern "C" {
#endif



#include "hw_def.h"


#ifdef _USE_HW_ILI9225

#include "ili9225_regs.h"



#define ILI9225_LCD_WIDTH      220
#define ILI9225_LCD_HEIGHT     176




bool ili9225Init(void);
bool ili9225DrawAvailable(void);
bool ili9225RequestDraw(void);
void ili9225SetFrameBuffer(uint16_t *p_buf);
void ili9225SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

uint32_t ili9225GetFps(void);
uint32_t ili9225GetFpsTime(void);

uint16_t ili9225GetWidth(void);
uint16_t ili9225GetHeight(void);


#endif

#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_ILI9225_H_ */
