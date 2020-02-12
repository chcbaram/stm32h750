/*
 * usb.h
 *
 *  Created on: 2018. 3. 16.
 *      Author: HanCheol Cho
 */

#ifndef SRC_HW_USB_CDC_USB_H_
#define SRC_HW_USB_CDC_USB_H_




#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_USB


enum UsbMode
{
  USB_NON_MODE,
  USB_CDC_MODE,
  USB_MSC_MODE
} ;


#include "usbd_core.h"
#include "usbd_desc.h"


#include "usbd_cdc.h"
#include "usbd_cdc_interface.h"
#include "vcp.h"

#include "usbd_msc.h"


bool usbInit(void);
bool usbBegin(enum UsbMode usb_mode);
void usbDeInit(void);

enum UsbMode usbGetMode(void);


#endif


#ifdef __cplusplus
}
#endif




#endif /* SRC_HW_USB_CDC_USB_H_ */
