/*
 * ili9225.c
 *
 *  Created on: 2020. 1. 30.
 *      Author: Baram
 */




#include "ili9225.h"
#include "spi.h"



enum class_color {
 white     = 0xFFFF,
 gray      = 0x8410,
 darkgray  = 0xAD55,
 black     = 0x0000,
 purple    = 0x8010,
 pink      = 0xFE19,
 red       = 0xF800,
 orange    = 0xFD20,
 brown     = 0xA145,
 beige     = 0xF7BB,
 yellow    = 0xFFE0,
 lightgreen= 0x9772,
 green     = 0x0400,
 darkblue  = 0x0011,
 blue      = 0x001F,
 lightblue = 0xAEDC,
};




#define _LCD_WIDTH      220
#define _LCD_HEIGHT     176

#define _PIN_DEF_DC     0
#define _PIN_DEF_CS     1
#define _PIN_DEF_RST    2



typedef struct
{
  GPIO_TypeDef *p_port;
  uint16_t      pin_num;
} ili9225_pin_t;


static ili9225_pin_t pin_tbl[3] =
  {
  {GPIOC, GPIO_PIN_1},
  {GPIOC, GPIO_PIN_0},
  {GPIOC, GPIO_PIN_2}
  };



static uint16_t _width  = _LCD_WIDTH;
static uint16_t _height = _LCD_HEIGHT;
static uint8_t   spi_ch = _DEF_SPI1;
static bool      swap_color = false;

static void ili9225InitRegs(void);

static void _writePin(uint8_t pin_num, uint8_t pin_state);
static void _writeCommand16(uint16_t cmd);
static void _writeData16(uint16_t data);
static void _writeRegister(uint16_t cmd, uint16_t data);


static __attribute__((section(".sram_d1"))) uint16_t lcd_buf[_LCD_WIDTH * _LCD_HEIGHT];

bool ili9225Init()
{
  GPIO_InitTypeDef  GPIO_InitStruct;


  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

  GPIO_InitStruct.Pin = pin_tbl[_PIN_DEF_DC].pin_num;           // DC
  HAL_GPIO_Init(pin_tbl[_PIN_DEF_DC].p_port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = pin_tbl[_PIN_DEF_RST].pin_num;         // RST
  HAL_GPIO_Init(pin_tbl[_PIN_DEF_RST].p_port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = pin_tbl[_PIN_DEF_CS].pin_num;          // CS
  HAL_GPIO_Init(pin_tbl[_PIN_DEF_CS].p_port, &GPIO_InitStruct);

  _writePin(_PIN_DEF_CS,  _DEF_HIGH);
  _writePin(_PIN_DEF_DC,  _DEF_HIGH);
  _writePin(_PIN_DEF_RST, _DEF_HIGH);


  _writePin(_PIN_DEF_RST, _DEF_HIGH);
  delay(50);
  _writePin(_PIN_DEF_RST, _DEF_LOW);
  delay(50);
  _writePin(_PIN_DEF_RST, _DEF_HIGH);
  delay(50);


  spiBegin(_DEF_SPI1);
  spiSetBitOrder(_DEF_SPI1, SPI_FIRSTBIT_MSB);
  spiSetClockDivider(_DEF_SPI1, SPI_BAUDRATEPRESCALER_8);
  spiSetDataMode(_DEF_SPI1, SPI_MODE0);

  ili9225InitRegs();


  ili9225SetAddrWindow(0, 0, _width-1, _height-1);
  for (int i=0; i<_width*_height; i++)
  {
    _writeData16(0x0000);
  }

  ili9225SetAddrWindow(0, 0, _width-1, _height-1);
  for (int i=0; i<_width*_height; i++)
  {
    //_writeData16(blue);
  }


  ili9225SetAddrWindow(0, 0, _width-1, _height-1);
  for (int i=0; i<_width*_height; i++)
  {
    lcd_buf[i] = blue;
  }

  uint32_t pre_time;
  pre_time = millis();
  _writePin(_PIN_DEF_DC, _DEF_HIGH);
  _writePin(_PIN_DEF_CS, _DEF_LOW);
  spiDmaTransfer(spi_ch, (uint8_t *)lcd_buf, _width*_height, 100);
  _writePin(_PIN_DEF_CS, _DEF_HIGH);
  logPrintf("%d ms\n", millis()-pre_time);

  return true;
}

void ili9225InitRegs(void)
{
  _writeRegister(ILI9225_POWER_CTRL1, 0x0000); // Set SAP,DSTB,STB
  _writeRegister(ILI9225_POWER_CTRL2, 0x0000); // Set APON,PON,AON,VCI1EN,VC
  _writeRegister(ILI9225_POWER_CTRL3, 0x0000); // Set BT,DC1,DC2,DC3
  _writeRegister(ILI9225_POWER_CTRL4, 0x0000); // Set GVDD
  _writeRegister(ILI9225_POWER_CTRL5, 0x0000); // Set VCOMH/VCOML voltage
  delay(40);

  // Power-on sequence
  _writeRegister(ILI9225_POWER_CTRL2, 0x0018); // Set APON,PON,AON,VCI1EN,VC
  _writeRegister(ILI9225_POWER_CTRL3, 0x6121); // Set BT,DC1,DC2,DC3
  _writeRegister(ILI9225_POWER_CTRL4, 0x006F); // Set GVDD   /*007F 0088 */
  _writeRegister(ILI9225_POWER_CTRL5, 0x495F); // Set VCOMH/VCOML voltage
  _writeRegister(ILI9225_POWER_CTRL1, 0x0800); // Set SAP,DSTB,STB
  delay(10);

  _writeRegister(ILI9225_POWER_CTRL2, 0x103B); // Set APON,PON,AON,VCI1EN,VC
  delay(50);

  _writeRegister(ILI9225_DRIVER_OUTPUT_CTRL, 0x011C); // set the display line number and display direction
  _writeRegister(ILI9225_LCD_AC_DRIVING_CTRL, 0x0100); // set 1 line inversion
  _writeRegister(ILI9225_ENTRY_MODE, 0x1038); // set GRAM write direction and BGR=1.
  _writeRegister(ILI9225_DISP_CTRL1, 0x0000); // Display off
  _writeRegister(ILI9225_BLANK_PERIOD_CTRL1, 0x0808); // set the back porch and front porch
  _writeRegister(ILI9225_FRAME_CYCLE_CTRL, 0x1100); // set the clocks number per line
  _writeRegister(ILI9225_INTERFACE_CTRL, 0x0000); // CPU interface
  _writeRegister(ILI9225_OSC_CTRL, 0x0D01); // Set Osc  /*0e01*/
  _writeRegister(ILI9225_VCI_RECYCLING, 0x0020); // Set VCI recycling
  _writeRegister(ILI9225_RAM_ADDR_SET1, 0x0000); // RAM Address
  _writeRegister(ILI9225_RAM_ADDR_SET2, 0x0000); // RAM Address

  /* Set GRAM area */
  _writeRegister(ILI9225_GATE_SCAN_CTRL, 0x0000);
  _writeRegister(ILI9225_VERTICAL_SCROLL_CTRL1, 0x00DB);
  _writeRegister(ILI9225_VERTICAL_SCROLL_CTRL2, 0x0000);
  _writeRegister(ILI9225_VERTICAL_SCROLL_CTRL3, 0x0000);
  _writeRegister(ILI9225_PARTIAL_DRIVING_POS1, 0x00DB);
  _writeRegister(ILI9225_PARTIAL_DRIVING_POS2, 0x0000);
  _writeRegister(ILI9225_HORIZONTAL_WINDOW_ADDR1, 0x00AF);
  _writeRegister(ILI9225_HORIZONTAL_WINDOW_ADDR2, 0x0000);
  _writeRegister(ILI9225_VERTICAL_WINDOW_ADDR1, 0x00DB);
  _writeRegister(ILI9225_VERTICAL_WINDOW_ADDR2, 0x0000);

  /* Set GAMMA curve */
  _writeRegister(ILI9225_GAMMA_CTRL1, 0x0000);
  _writeRegister(ILI9225_GAMMA_CTRL2, 0x0808);
  _writeRegister(ILI9225_GAMMA_CTRL3, 0x080A);
  _writeRegister(ILI9225_GAMMA_CTRL4, 0x000A);
  _writeRegister(ILI9225_GAMMA_CTRL5, 0x0A08);
  _writeRegister(ILI9225_GAMMA_CTRL6, 0x0808);
  _writeRegister(ILI9225_GAMMA_CTRL7, 0x0000);
  _writeRegister(ILI9225_GAMMA_CTRL8, 0x0A00);
  _writeRegister(ILI9225_GAMMA_CTRL9, 0x0710);
  _writeRegister(ILI9225_GAMMA_CTRL10, 0x0710);

  _writeRegister(ILI9225_DISP_CTRL1, 0x0012);
  delay(50);

  _writeRegister(ILI9225_DISP_CTRL1, 0x1017);


}

void ili9225SetRotation(uint8_t mode)
{
}

void ili9225SetAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{

  _writeRegister(ILI9225_ENTRY_MODE, 0x1000 | (1<<3) | (1<<4));


  _writeRegister(ILI9225_HORIZONTAL_WINDOW_ADDR2,y0);
  _writeRegister(ILI9225_HORIZONTAL_WINDOW_ADDR1,y1);

  _writeRegister(ILI9225_VERTICAL_WINDOW_ADDR1,  x1);
  _writeRegister(ILI9225_VERTICAL_WINDOW_ADDR2,  x0);

  _writeRegister(ILI9225_RAM_ADDR_SET1,y0); // 175
  _writeRegister(ILI9225_RAM_ADDR_SET2,x1); // 219

  _writeCommand16( ILI9225_GRAM_DATA_REG );
}


uint16_t ili9225GetWidth(void)
{
  return _width;
}

uint16_t ili9225GetHeight(void)
{
  return _height;
}


void _writePin(uint8_t pin_num, uint8_t pin_state)
{
  if (pin_state == _DEF_HIGH)
  {
    HAL_GPIO_WritePin(pin_tbl[pin_num].p_port, pin_tbl[pin_num].pin_num, GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(pin_tbl[pin_num].p_port, pin_tbl[pin_num].pin_num, GPIO_PIN_RESET);
  }
}

void _writeCommand16(uint16_t cmd)
{
  _writePin(_PIN_DEF_DC, _DEF_LOW);
  _writePin(_PIN_DEF_CS, _DEF_LOW);

  spiTransfer16(spi_ch, cmd);

  _writePin(_PIN_DEF_CS, _DEF_HIGH);

}

void _writeData16(uint16_t data)
{
  _writePin(_PIN_DEF_DC, _DEF_HIGH);
  _writePin(_PIN_DEF_CS, _DEF_LOW);

  spiTransfer16(spi_ch, data);

  _writePin(_PIN_DEF_CS, _DEF_HIGH);
}

void _writeRegister(uint16_t cmd, uint16_t data)
{
  _writeCommand16(cmd);
  _writeData16(data);
}
