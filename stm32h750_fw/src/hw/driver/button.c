/*
 * button.c
 *
 *  Created on: 2020. 2. 1.
 *      Author: Baram
 */




#include "button.h"
#include "swtimer.h"
#include "cmdif.h"
#include "joypad.h"

typedef struct
{
  GPIO_TypeDef *port;
  uint32_t      pin;
  GPIO_PinState on_state;
  GPIO_PinState off_state;
} button_tbl_t;




button_tbl_t button_port_tbl[BUTTON_MAX_CH] =
{
  {GPIOB, GPIO_PIN_8, GPIO_PIN_RESET, GPIO_PIN_SET},  // 0. BTN_A
  {GPIOB, GPIO_PIN_5, GPIO_PIN_RESET, GPIO_PIN_SET},  // 1. BTN_B
  {GPIOD, GPIO_PIN_4, GPIO_PIN_RESET, GPIO_PIN_SET},  // 2. BTN_SELECT
  {GPIOD, GPIO_PIN_6, GPIO_PIN_RESET, GPIO_PIN_SET},  // 3. BTN_START
  {GPIOD, GPIO_PIN_0, GPIO_PIN_RESET, GPIO_PIN_SET},  // 4. BTN_MENU
  {GPIOA, GPIO_PIN_9, GPIO_PIN_RESET, GPIO_PIN_SET},  // 5. BTN_HOME

  {NULL, },                                           // 6. BTN_LEFT
  {NULL, },                                           // 7. BTN_RIGHT
  {NULL, },                                           // 8. BTN_UP
  {NULL, },                                           // 9. BTN_DOWN
};



typedef struct
{
  bool        pressed;
  bool        pressed_event;
  uint16_t    pressed_cnt;
  uint32_t    pressed_start_time;
  uint32_t    pressed_end_time;

  bool        released;
  bool        released_event;
  uint32_t    released_start_time;
  uint32_t    released_end_time;

} button_t;


static button_t button_tbl[BUTTON_MAX_CH];


#ifdef _USE_HW_CMDIF
void buttonCmdifInit(void);
void buttonCmdif(void);
#endif

static bool is_enable = true;
static bool buttonGetPin(uint8_t ch);


void button_isr(void *arg)
{
  uint8_t i;


  for (i=0; i<BUTTON_MAX_CH; i++)
  {

    if (buttonGetPin(i))
    {
      if (button_tbl[i].pressed == false)
      {
        button_tbl[i].pressed_event = true;
        button_tbl[i].pressed_start_time = millis();
      }

      button_tbl[i].pressed = true;
      button_tbl[i].pressed_cnt++;

      button_tbl[i].pressed_end_time = millis();

      button_tbl[i].released = false;
    }
    else
    {
      if (button_tbl[i].pressed == true)
      {
        button_tbl[i].released_event = true;
        button_tbl[i].released_start_time = millis();
      }

      button_tbl[i].pressed  = false;
      button_tbl[i].released = true;

      button_tbl[i].released_end_time = millis();
    }
  }
}



bool buttonInit(void)
{
  uint32_t i;
  GPIO_InitTypeDef  GPIO_InitStruct;
  swtimer_handle_t h_button_timer;


  GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  for (i=0; i<BUTTON_MAX_CH; i++)
  {
    if (button_port_tbl[i].port != NULL)
    {
      GPIO_InitStruct.Pin = button_port_tbl[i].pin;
      HAL_GPIO_Init(button_port_tbl[i].port, &GPIO_InitStruct);
    }
  }

  for (i=0; i<BUTTON_MAX_CH; i++)
  {
    button_tbl[i].pressed_cnt    = 0;
    button_tbl[i].pressed        = 0;
    button_tbl[i].released       = 0;
    button_tbl[i].released_event = 0;
  }

  h_button_timer = swtimerGetHandle();
  swtimerSet(h_button_timer, 1, LOOP_TIME, button_isr, NULL );
  swtimerStart(h_button_timer);

#ifdef _USE_HW_CMDIF
  buttonCmdifInit();
#endif


  return true;
}

void buttonResetTime(uint8_t ch)
{
  button_tbl[ch].pressed_start_time    = 0;
  button_tbl[ch].pressed_end_time      = 0;
  button_tbl[ch].released_start_time   = 0;
  button_tbl[ch].released_end_time     = 0;
}

bool buttonGetPin(uint8_t ch)
{
  if (ch >= BUTTON_MAX_CH)
  {
    return false;
  }

  if (button_port_tbl[ch].port != NULL)
  {
    if (HAL_GPIO_ReadPin(button_port_tbl[ch].port, button_port_tbl[ch].pin) == button_port_tbl[ch].on_state)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    bool ret = false;

    if (ch == _DEF_HW_BTN_LEFT  && joypadGetPressedButton(0)) ret = true;
    if (ch == _DEF_HW_BTN_RIGHT && joypadGetPressedButton(1)) ret = true;
    if (ch == _DEF_HW_BTN_UP    && joypadGetPressedButton(2)) ret = true;
    if (ch == _DEF_HW_BTN_DOWN  && joypadGetPressedButton(3)) ret = true;

    return ret;
  }
}

void buttonEnable(bool enable)
{
  is_enable = enable;
}

bool buttonGetPressed(uint8_t ch)
{
  if (ch >= BUTTON_MAX_CH || is_enable == false)
  {
    return false;
  }

  return button_tbl[ch].pressed;
}

bool buttonOsdGetPressed(uint8_t ch)
{
  if (ch >= BUTTON_MAX_CH)
  {
    return false;
  }

  return button_tbl[ch].pressed;
}

bool buttonGetPressedEvent(uint8_t ch)
{
  bool ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return false;

  ret = button_tbl[ch].pressed_event;

  button_tbl[ch].pressed_event = 0;

  return ret;
}

uint32_t buttonGetPressedTime(uint8_t ch)
{
  volatile uint32_t ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return 0;


  ret = button_tbl[ch].pressed_end_time - button_tbl[ch].pressed_start_time;

  return ret;
}


bool buttonGetReleased(uint8_t ch)
{
  bool ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return false;

  ret = button_tbl[ch].released;

  return ret;
}

bool buttonGetReleasedEvent(uint8_t ch)
{
  bool ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return false;

  ret = button_tbl[ch].released_event;

  button_tbl[ch].released_event = 0;

  return ret;
}

uint32_t buttonGetReleasedTime(uint8_t ch)
{
  volatile uint32_t ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return 0;


  ret = button_tbl[ch].released_end_time - button_tbl[ch].released_start_time;

  return ret;
}



#ifdef _USE_HW_CMDIF
void buttonCmdifInit(void)
{
  cmdifAdd("button", buttonCmdif);
}

void buttonCmdif(void)
{
  bool ret = true;
  uint8_t ch;
  uint32_t i;


  if (cmdifGetParamCnt() == 1)
  {
    if(cmdifHasString("show", 0) == true)
    {
      while(cmdifRxAvailable() == 0)
      {
        for (i=0; i<BUTTON_MAX_CH; i++)
        {
          cmdifPrintf("%d", buttonGetPressed(i));
        }
        cmdifPrintf("\r");
        delay(50);
      }
    }
    else
    {
      ret = false;
    }
  }
  else if (cmdifGetParamCnt() == 2)
  {
    ch = (uint8_t)cmdifGetParam(1);

    if (ch > 0)
    {
      ch--;
    }

    if (cmdifHasString("time", 0) == true)
    {
      while(cmdifRxAvailable() == 0)
      {
        if(buttonGetPressed(ch))
        {
          cmdifPrintf("BUTTON%d, Time :  %d ms\n", ch+1, buttonGetPressedTime(ch));
        }
      }
    }
    else
    {
      ret = false;
    }
  }
  else
  {
    ret = false;
  }


  if (ret == false)
  {
    cmdifPrintf( "button [show/time] channel(1~%d) ...\n", BUTTON_MAX_CH);
  }
}
#endif
