/**
  ******************************************************************************
  * @file    usbd_hid_core.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides the HID core functions.
  *
  * @verbatim
  *
  *          ===================================================================
  *                                HID Class  Description
  *          ===================================================================
  *           This module manages the HID class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (HID) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - The Mouse protocol
  *             - Usage Page : Generic Desktop
  *             - Usage : Joystick)
  *             - Collection : Application
  *
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *
  *
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbd_hid_core.h"
#include "usbd_desc.h"
#include "usbd_req.h"
//#include "includes.h"
//#include "irt10_config.h"
#include "usbd_fifo.h"

#define INT_BULK 3
//#define INT_BULK 2


#define DESCRIPTOR_USER050607 \
        0x06, 0x00, 0xff, \
        0x09, 0x00,       \
        0xa1, 0x01,       \
        0x85, 0x05,       \
        0x09, 0x01,       \
        0x15, 0x00,       \
        0x26, 0xff, 0x00, \
        0x75, 0x08,       \
        0x95, 63,         \
        0x81, 0x02,       \
        0x85, 0x06,       \
        0x09, 0x02,       \
        0x91, 0x02,       \
        0x85, 0x07,       \
        0x09, 0x03,       \
        0xb1, 0x02,       \
        0xc0,

#define DESCRIPTOR_USER0506\
        0x06, 0x00, 0xff, \
        0x09, 0x00,       \
        0xa1, 0x01,       \
        0x85, 5,          \
        0x09, 0x01,       \
        0x15, 0x00,       \
        0x26, 0xff, 0x00, \
        0x75, 0x08,       \
        0x95, 63,         \
        0x81, 0x02,       \
                          \
        0x85, 6,          \
        0x09, 0x02,       \
        0x15, 0x00,       \
        0x26, 0xff, 0x00, \
        0x75, 0x08,       \
        0x95, 63,         \
        0x91, 0x02,       \
        0xc0,
        
#define DESCRIPTOR_USERA1 \
        0x06, 0x00, 0xff, \
        0x09, 0xa1,       \
        0xa1, 0x01,       \
        0x85, 0xa1,       \
        0x09, 0x01,       \
        0x15, 0x00,       \
        0x26, 0xff, 0x00, \
        0x75, 0x08,       \
        0x95, 63,         \
        0x81, 0x02,       \
        0x85, 0xa1,       \
        0x09, 0x02,       \
        0x91, 0x02,       \
        0xc0,

#define DESCRIPTOR_USERA2 \
        0x06, 0x00, 0xff, \
        0x09, 0xa2,       \
        0xa1, 0x01,       \
        0x85, 0xa2,       \
        0x09, 0x01,       \
        0x15, 0x00,       \
        0x26, 0xff, 0x00, \
        0x75, 0x08,       \
        0x95, 63,         \
        0x81, 0x02,       \
        0x85, 0xa2,       \
        0x09, 0x02,       \
        0x91, 0x02,       \
        0xc0,

#define DESCRIPTOR_USERA3 \
        0x06, 0x00, 0xff, \
        0x09, 0xa3,       \
        0xa1, 0x01,       \
        0x85, 0xa3,       \
        0x09, 0x01,       \
        0x15, 0x00,       \
        0x26, 0xff, 0x00, \
        0x75, 0x08,       \
        0x95, 63,         \
        0x81, 0x02,       \
        0x85, 0xa3,       \
        0x09, 0x02,       \
        0x91, 0x02,       \
        0xc0,

#define DESCRIPTOR_USERFB \
        0x06, 0x00, 0xff, \
        0x09, 0xfb,       \
        0xa1, 0x01,       \
        0x85, 0xfb,       \
        0x09, 0x01,       \
        0x15, 0x00,       \
        0x26, 0xff, 0x00, \
        0x75, 0x08,       \
        0x95, 63,         \
        0x81, 0x02,       \
        0x85, 0xfb,       \
        0x09, 0x02,       \
        0x91, 0x02,       \
        0xc0,

#define DESCRIPTOR_USERFC \
        0x06, 0x00, 0xff, \
        0x09, 0xfc,       \
        0xa1, 0x01,       \
        0x85, 0xfc,       \
        0x09, 0x01,       \
        0x15, 0x00,       \
        0x26, 0xff, 0x00, \
        0x75, 0x08,       \
        0x95, 63,         \
        0x81, 0x02,       \
        0x85, 0xfc,       \
        0x09, 0x02,       \
        0x91, 0x02,       \
        0xc0,
        
#define DESCRIPTOR_USER_CH\
        0x06, 0xf0, 0xff, \
        0x09, 0x01,       \
        0xa1, 0x01,       \
        0x85, 0xf0,       \
        0x09, 0xf0,       \
        0x15, 0x00,       \
        0x26, 0xff, 0x00, \
        0x75, 0x08,       \
        0x95, 63,         \
        0x81, 0x02,       \
                          \
        0x85, 0xf1,       \
        0x09, 0xf1,       \
        0x15, 0x00,       \
        0x26, 0xff, 0x00, \
        0x75, 0x08,       \
        0x95, 63,         \
        0x91, 0x02,       \
        0xc0,

#define DESCRIPTOR_USER_HH\
        0x06, 0x00, 0xff, \
        0x09, 0x01,       \
        0xa1, 0x01,       \
        0x85, 0xfe,       \
        0x09, 0x01,       \
        0x15, 0x00,       \
        0x26, 0xff, 0x00, \
        0x75, 0x08,       \
        0x95, 63,         \
        0x81, 0x02,       \
        0x85, 0xfe,       \
        0x09, 0x02,       \
        0x91, 0x02,       \
        0x85, 0x01,       \
        0x09, 0x03,       \
        0x81, 0x02,       \
        0xc0,

#define DESCRIPTOR_USER_PM\
        0x06, 0x00, 0xff, \
        0x09, 0x00,       \
        0xa1, 0x01,       \
        0x85, 5,          \
        0x09, 0x01,       \
        0x15, 0x00,       \
        0x26, 0xff, 0x00, \
        0x75, 0x08,       \
        0x95, 63,         \
        0x81, 0x02,       \
        0x85, 6,          \
        0x09, 0x02,       \
        0x15, 0x00,       \
        0x26, 0xff, 0x00, \
        0x75, 0x08,       \
        0x95, 63,         \
        0x91, 0x02,       \
        0x85, 7,          \
        0x09, 0x03,       \
        0x15, 0x00,       \
        0x26, 0xff, 0x00, \
        0x75, 0x08,       \
        0x95, 63,         \
        0xb1, 0x02,       \
        0xc0,
    
        
#define DESCRIPTOR_KEY    \
        0x05, 0x01,       \
        0x09, 0x06,       \
        0xa1, 0x01,       \
        0x85, 0x0f,       \
        0x05, 0x07,       \
                          \
        0x19, 0xe0,       \
        0x29, 0xe7,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x75, 0x01,       \
        0x95, 0x08,       \
        0x81, 0x02,       \
        0x95, 0x01,       \
        0x75, 0x08,       \
        0x81, 0x03,       \
                          \
        0x95, 0x05,       \
        0x75, 0x01,       \
        0x05, 0x08,       \
        0x19, 0x01,       \
        0x29, 0x05,       \
        0x91, 0x02,       \
                          \
        0x95, 0x01,       \
        0x75, 0x03,       \
        0x91, 0x03,       \
                          \
        0x95, 0x06,       \
        0x75, 0x08,       \
        0x15, 0x00,       \
        0x25, 0xFF,       \
        0x05, 0x07,       \
        0x19, 0x00,       \
        0x29, 0x65,       \
        0x81, 0x00,       \
        0xc0,

#define DESCRIPTOR_MOUSE  \
        0x05, 0x01,       \
        0x09, 0x02,       \
        0xa1, 0x01,       \
        0x85, 0x01,       \
        0x09, 0x01,       \
        0xa1, 0x00,       \
        0x05, 0x09,       \
        0x19, 0x01,       \
        0x29, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x95, 0x03,       \
        0x75, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x01,       \
        0x75, 0x05,       \
        0x81, 0x01,       \
        0x05, 0x01,       \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x16, 0x00, 0x00, \
        0x26, 0xff, 0x7f, \
        0x36, 0x00, 0x00, \
        0x46, 0xff, 0x7f, \
        0x75, 0x10,       \
        0x95, 0x02,       \
        0x81, 0x62,       \
        0xc0,             \
        0xc0,

#define DESCRIPTOR_MOUSE_HH  \
        0x05, 0x01,       \
        0x09, 0x02,       \
        0xa1, 0x01,       \
        0x85, 0x10,       \
        0x09, 0x01,       \
        0xa1, 0x00,       \
        0x05, 0x09,       \
        0x19, 0x01,       \
        0x29, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x95, 0x03,       \
        0x75, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x01,       \
        0x75, 0x05,       \
        0x81, 0x01,       \
        0x05, 0x01,       \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x16, 0x00, 0x00, \
        0x26, 0xff, 0x7f, \
        0x36, 0x00, 0x00, \
        0x46, 0xff, 0x7f, \
        0x75, 0x10,       \
        0x95, 0x02,       \
        0x81, 0x62,       \
        0xc0,             \
        0xc0,
				
#define DESCRIPTOR_TOUCH  \
        0x05, 0x0d,       \
        0x09, 0x04,       \
        0xa1, 0x01,       \
        0x85, 0x02,       \
        0x09, 0x22,       \
                          \
        0xa1, 0x02,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0x05, 0x01,       \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x75, 0x10,       \
        0x95, 0x02,       \
        0x16, 0x00, 0x00, \
        0x26, 0xff, 0x7f, \
        0x46, 0x00, 0x00, \
        0xa4,             \
        0x81, 0x02,       \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0xa4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0xa4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0xa4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0xa4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0xa4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0xa4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0xa4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0xa4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
        0xc0,             \
                          \
        0x05, 0x0d,       \
        0x09, 0x54,       \
        0x09, 0x01,       \
        0x95, 0x01,       \
        0x75, 0x08,       \
        0x15, 0x00,       \
        0x25, 0x80,       \
        0x81, 0x02,       \
        0x85, 0x03,       \
        0x09, 0x55,       \
        0xb1, 0x02,       \
                          \
        0xc0,             \
                          \
        0x09, 0x0e,       \
        0xa1, 0x01,       \
        0x85, 0x04,       \
        0x09, 0x23,       \
        0xa1, 0x02,       \
        0x09, 0x52,       \
        0x09, 0x53,       \
        0x15, 0x00,       \
        0x25, 0x80,       \
        0x75, 0x08,       \
        0x95, 0x02,       \
        0xb1, 0x02,       \
        0xc0,             \
        0xc0,

#define DESCRIPTOR_TOUCH_EREA1 \
        0x05, 0x0d,       \
        0x09, 0x04,       \
        0xa1, 0x01,       \
        0x85, 0x02,       \
        0x09, 0x22,       \
                          \
        0xa1, 0x02,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0x05, 0x01,       \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x75, 0x10,       \
        0x95, 0x02,       \
        0x16, 0x00, 0x00, \
        0x26, 0xff, 0x7f, \
        0x46, 0x00, 0x00, \
        0xa4,             \
        0x81, 0x02,       \
                          \
        0x05,0x0d,        \
        0x09,0x48,        \
        0x09,0x49,        \
        0x55,0x0e,0x65,0x33,0x35,0x00,0x46,0x1d,0x08,\
        0x75,0x10,        \
        0x95,0x02,        \
        0x81,0x02,        \
                          \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0xa4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
                          \
        0x05,0x0d,        \
        0x09,0x48,        \
        0x09,0x49,        \
        0x55,0x0e,0x65,0x33,0x35,0x00,0x46,0x1d,0x08,\
        0x75,0x10,        \
        0x95,0x02,        \
        0x81, 0x02,       \
                          \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0xa4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
                          \
        0x05,0x0d,        \
        0x09,0x48,        \
        0x09,0x49,        \
        0x55,0x0e,0x65,0x33,0x35,0x00,0x46,0x1d,0x08,\
        0x75,0x10,        \
        0x95,0x02,        \
        0x81, 0x02,       \
                          \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0xa4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
                          \
        0x05,0x0d,        \
        0x09,0x48,        \
        0x09,0x49,        \
        0x55,0x0e,0x65,0x33,0x35,0x00,0x46,0x1d,0x08,\
        0x75,0x10,        \
        0x95,0x02,        \
        0x81, 0x02,       \
                          \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0xa4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
                          \
        0x05,0x0d,        \
        0x09,0x48,        \
        0x09,0x49,        \
        0x55,0x0e,0x65,0x33,0x35,0x00,0x46,0x1d,0x08,\
        0x75,0x10,        \
        0x95,0x02,        \
        0x81, 0x02,       \
                          \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
                          \
        0x05,0x0d,        \
        0x09,0x48,        \
        0x09,0x49,        \
        0x55,0x0e,0x65,0x33,0x35,0x00,0x46,0x1d,0x08,\
        0x75,0x10,        \
        0x95,0x02,        \
        0x81,0x02,        \
        0xc0,

#define DESCRIPTOR_TOUCH_EREA2 \
        0x05, 0x0d,       \
        0x09, 0x04,       \
        0xa1, 0x01,       \
        0x85, 0x02,       \
        0x09, 0x22,       \
                          \
        0xa1, 0x02,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0x05, 0x01,       \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x75, 0x10,       \
        0x95, 0x02,       \
        0x16, 0x00, 0x00, \
        0x26, 0xff, 0x7f, \
        0x46, 0x00, 0x00, \
        0xa4,             \
        0x81, 0x02,       \
                          \
        0x05, 0x0D,       \
        0x65, 0x11,       \
        0x55, 0x0E,       \
        0x75, 0x10,       \
        0x95, 0x01,       \
        0x35, 0x00,       \
        0x26, (TOUCH_SIZE_W%256), (TOUCH_SIZE_W/256),       \
        0x46, (TOUCH_SIZE_W%256), (TOUCH_SIZE_W/256),       \
        0x09, 0x48,       \
        0x81, 0x02,       \
        0x26, (TOUCH_SIZE_H%256), (TOUCH_SIZE_H/256),       \
        0x46, (TOUCH_SIZE_H%256), (TOUCH_SIZE_H/256),       \
        0x09, 0x49,       \
        0x81, 0x02,       \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0xa4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
                          \
        0x05, 0x0D,       \
        0x65, 0x11,       \
        0x55, 0x0E,       \
        0x75, 0x10,       \
        0x95, 0x01,       \
        0x35, 0x00,       \
        0x26, (TOUCH_SIZE_W%256), (TOUCH_SIZE_W/256),       \
        0x46, (TOUCH_SIZE_W%256), (TOUCH_SIZE_W/256),       \
        0x09, 0x48,       \
        0x81, 0x02,       \
        0x26, (TOUCH_SIZE_H%256), (TOUCH_SIZE_H/256),       \
        0x46, (TOUCH_SIZE_H%256), (TOUCH_SIZE_H/256),       \
        0x09, 0x49,       \
        0x81, 0x02,       \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0xa4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
                          \
        0x05, 0x0D,       \
        0x65, 0x11,       \
        0x55, 0x0E,       \
        0x75, 0x10,       \
        0x95, 0x01,       \
        0x35, 0x00,       \
        0x26, (TOUCH_SIZE_W%256), (TOUCH_SIZE_W/256),       \
        0x46, (TOUCH_SIZE_W%256), (TOUCH_SIZE_W/256),       \
        0x09, 0x48,       \
        0x81, 0x02,       \
        0x26, (TOUCH_SIZE_H%256), (TOUCH_SIZE_H/256),       \
        0x46, (TOUCH_SIZE_H%256), (TOUCH_SIZE_H/256),       \
        0x09, 0x49,       \
        0x81, 0x02,       \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0xa4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
                          \
        0x05, 0x0D,       \
        0x65, 0x11,       \
        0x55, 0x0E,       \
        0x75, 0x10,       \
        0x95, 0x01,       \
        0x35, 0x00,       \
        0x26, (TOUCH_SIZE_W%256), (TOUCH_SIZE_W/256),       \
        0x46, (TOUCH_SIZE_W%256), (TOUCH_SIZE_W/256),       \
        0x09, 0x48,       \
        0x81, 0x02,       \
        0x26, (TOUCH_SIZE_H%256), (TOUCH_SIZE_H/256),       \
        0x46, (TOUCH_SIZE_H%256), (TOUCH_SIZE_H/256),       \
        0x09, 0x49,       \
        0x81, 0x02,       \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0xa4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
                          \
        0x05, 0x0D,       \
        0x65, 0x11,       \
        0x55, 0x0E,       \
        0x75, 0x10,       \
        0x95, 0x01,       \
        0x35, 0x00,       \
        0x26, (TOUCH_SIZE_W%256), (TOUCH_SIZE_W/256),       \
        0x46, (TOUCH_SIZE_W%256), (TOUCH_SIZE_W/256),       \
        0x09, 0x48,       \
        0x81, 0x02,       \
        0x26, (TOUCH_SIZE_H%256), (TOUCH_SIZE_H/256),       \
        0x46, (TOUCH_SIZE_H%256), (TOUCH_SIZE_H/256),       \
        0x09, 0x49,       \
        0x81, 0x02,       \
        0xc0,             \
                          \
        0xa1, 0x02,       \
        0x05, 0x0d,       \
        0x09, 0x42,       \
        0x09, 0x32,       \
        0x09, 0x47,       \
        0x75, 0x01,       \
        0x95, 0x03,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x81, 0x02,       \
        0x95, 0x05,       \
        0x81, 0x03,       \
                          \
        0x09, 0x51,       \
        0x75, 0x08,       \
        0x95, 0x01,       \
        0x81, 0x02,       \
                          \
        0xb4,             \
        0x09, 0x30,       \
        0x09, 0x31,       \
        0x81, 0x02,       \
                          \
        0x05, 0x0D,       \
        0x65, 0x11,       \
        0x55, 0x0E,       \
        0x75, 0x10,       \
        0x95, 0x01,       \
        0x35, 0x00,       \
        0x26, (TOUCH_SIZE_W%256), (TOUCH_SIZE_W/256),       \
        0x46, (TOUCH_SIZE_W%256), (TOUCH_SIZE_W/256),       \
        0x09, 0x48,       \
        0x81, 0x02,       \
        0x26, (TOUCH_SIZE_H%256), (TOUCH_SIZE_H/256),       \
        0x46, (TOUCH_SIZE_H%256), (TOUCH_SIZE_H/256),       \
        0x09, 0x49,       \
        0x81, 0x02,       \
        0xc0,
        
#define DESCRIPTOR_TOUCH_EREA3 \
        0x05, 0x0d,       \
        0x09, 0x54,       \
        0x09, 0x01,       \
        0x95, 0x01,       \
        0x75, 0x08,       \
        0x15, 0x00,       \
        0x25, 0x80,       \
        0x81, 0x02,       \
        0x85, 0x03,       \
        0x09, 0x55,       \
        0xb1, 0x02,       \
        0xc0,             \
                          \
        0x09, 0x0e,       \
        0xa1, 0x01,       \
        0x85, 0x04,       \
        0x09, 0x23,       \
        0xa1, 0x02,       \
        0x09, 0x52,       \
        0x09, 0x53,       \
        0x15, 0x00,       \
        0x25, 0x80,       \
        0x75, 0x08,       \
        0x95, 0x02,       \
        0xb1, 0x02,       \
        0xc0,             \
        0xc0,

#define DESCRIPTOR_TOUCH_EREA_IST1  \
        DESCRIPTOR_TOUCH_EREA1     \
        DESCRIPTOR_TOUCH_EREA3

#define DESCRIPTOR_TOUCH_EREA_IST2  \
        DESCRIPTOR_TOUCH_EREA2     \
        DESCRIPTOR_TOUCH_EREA3

#ifndef TOUCH_SIZE
    #define DESCRIPTOR_TOUCH_EREA  DESCRIPTOR_TOUCH_EREA_IST1
#else
    #define DESCRIPTOR_TOUCH_EREA  DESCRIPTOR_TOUCH_EREA_IST2
#endif

#ifndef POINT_OS_EREA
    #define DESCRIPTOR_MULTITOUCH  DESCRIPTOR_TOUCH
#else
    #define DESCRIPTOR_MULTITOUCH  DESCRIPTOR_TOUCH_EREA
#endif    
                
#define DESCRIPTOR_TOUCH_TIME    \
    0x05, 0x0D,       \
    0x55, 0x0C,       \
    0x66, 0x01, 0x10, \
    0x75, 0x10,       \
    0x95, 0x01,       \
    0x26, 0xFF, 0xFF, \
    0x45, 0x00,       \
    0x09, 0x80,       \
    0x81, 0x02,

#define DESCRIPTOR_TOUCH_EREA_HH2 \
        DESCRIPTOR_TOUCH_EREA2    \
        DESCRIPTOR_TOUCH_TIME     \
        DESCRIPTOR_TOUCH_EREA3
                    
#define DESCRIPTOR_TOUCH_EREA_TST    \
    0x06, 0x00, 0xff, 0x09,  0x01, 0xa1, 0x01, 0x85,  0x04, 0x15, 0x00, 0x26,  0xff, 0x00, 0x75, 0x08, \
    0x95, 0x3f, 0x09, 0x02,  0x81, 0x02, 0x95, 0x3f,  0x09, 0x02, 0x91, 0x02,  0xc0, 0x05, 0x01, 0x09, \
    0x02, 0xa1, 0x01, 0x85,  0x03, 0x09, 0x01, 0xa1,  0x00, 0x05, 0x09, 0x19,  0x01, 0x29, 0x02, 0x15, \
    0x00, 0x25, 0x01, 0x75,  0x01, 0x95, 0x02, 0x81,  0x02, 0x95, 0x06, 0x81,  0x03, 0x05, 0x01, 0x75, \
    0x10, 0x95, 0x01, 0x26,  0xff, 0x7f, 0x09, 0x30,  0x81, 0x02, 0x26, 0xff,  0x7f, 0x09, 0x31, 0x81, \
    0x02, 0x05, 0x01, 0x09,  0x38, 0x15, 0x81, 0x25,  0x7f, 0x75, 0x08, 0x95,  0x01, 0x81, 0x06, 0xc0, \
    0xc0, 0x05, 0x0d, 0x09,  0x04, 0xa1, 0x01, 0x85,  0x01, 0x09, 0x22, 0xa1,  0x02, 0x09, 0x42, 0x15, \
    0x00, 0x25, 0x01, 0x75,  0x01, 0x95, 0x01, 0x81,  0x02, 0x09, 0x32, 0x81,  0x02, 0x09, 0x47, 0x81, \
    0x02, 0x95, 0x05, 0x81,  0x03, 0x75, 0x08, 0x09,  0x51, 0x95, 0x01, 0x81,  0x02, 0x05, 0x01, 0x75, \
    0x10, 0x55, 0x0e, 0x35,  0x00, 0x65, 0x11, 0x26,  0x00, 0x1e, 0x46, 0xe4,  0x13, 0x09, 0x30, 0x81, \
    0x02, 0x26, 0xe0, 0x10,  0x46, 0x30, 0x0b, 0x09,  0x31, 0x81, 0x02, 0x05,  0x0d, 0x09, 0x48, 0x26, \
    0x00, 0x1e, 0x46, 0xe4,  0x13, 0x81, 0x02, 0x09,  0x49, 0x26, 0xe0, 0x10,  0x46, 0x30, 0x0b, 0x81, \
    0x02, 0xc0, 0x09, 0x22,  0xa1, 0x02, 0x09, 0x42,  0x15, 0x00, 0x25, 0x01,  0x75, 0x01, 0x95, 0x01, \
    0x81, 0x02, 0x09, 0x32,  0x81, 0x02, 0x09, 0x47,  0x81, 0x02, 0x95, 0x05,  0x81, 0x03, 0x75, 0x08, \
    0x09, 0x51, 0x95, 0x01,  0x81, 0x02, 0x05, 0x01,  0x75, 0x10, 0x55, 0x0e,  0x35, 0x00, 0x65, 0x11, \
    0x26, 0x00, 0x1e, 0x46,  0xe4, 0x13, 0x09, 0x30,  0x81, 0x02, 0x26, 0xe0,  0x10, 0x46, 0x30, 0x0b, \
    0x09, 0x31, 0x81, 0x02,  0x05, 0x0d, 0x09, 0x48,  0x26, 0x00, 0x1e, 0x46,  0xe4, 0x13, 0x81, 0x02, \
    0x09, 0x49, 0x26, 0xe0,  0x10, 0x46, 0x30, 0x0b,  0x81, 0x02, 0xc0, 0x09,  0x22, 0xa1, 0x02, 0x09, \
    0x42, 0x15, 0x00, 0x25,  0x01, 0x75, 0x01, 0x95,  0x01, 0x81, 0x02, 0x09,  0x32, 0x81, 0x02, 0x09, \
    0x47, 0x81, 0x02, 0x95,  0x05, 0x81, 0x03, 0x75,  0x08, 0x09, 0x51, 0x95,  0x01, 0x81, 0x02, 0x05, \
    0x01, 0x75, 0x10, 0x55,  0x0e, 0x35, 0x00, 0x65,  0x11, 0x26, 0x00, 0x1e,  0x46, 0xe4, 0x13, 0x09, \
    0x30, 0x81, 0x02, 0x26,  0xe0, 0x10, 0x46, 0x30,  0x0b, 0x09, 0x31, 0x81,  0x02, 0x05, 0x0d, 0x09, \
    0x48, 0x26, 0x00, 0x1e,  0x46, 0xe4, 0x13, 0x81,  0x02, 0x09, 0x49, 0x26,  0xe0, 0x10, 0x46, 0x30, \
    0x0b, 0x81, 0x02, 0xc0,  0x09, 0x22, 0xa1, 0x02,  0x09, 0x42, 0x15, 0x00,  0x25, 0x01, 0x75, 0x01, \
    0x95, 0x01, 0x81, 0x02,  0x09, 0x32, 0x81, 0x02,  0x09, 0x47, 0x81, 0x02,  0x95, 0x05, 0x81, 0x03, \
    0x75, 0x08, 0x09, 0x51,  0x95, 0x01, 0x81, 0x02,  0x05, 0x01, 0x75, 0x10,  0x55, 0x0e, 0x35, 0x00, \
    0x65, 0x11, 0x26, 0x00,  0x1e, 0x46, 0xe4, 0x13,  0x09, 0x30, 0x81, 0x02,  0x26, 0xe0, 0x10, 0x46, \
    0x30, 0x0b, 0x09, 0x31,  0x81, 0x02, 0x05, 0x0d,  0x09, 0x48, 0x26, 0x00,  0x1e, 0x46, 0xe4, 0x13, \
    0x81, 0x02, 0x09, 0x49,  0x26, 0xe0, 0x10, 0x46,  0x30, 0x0b, 0x81, 0x02,  0xc0, 0x05, 0x0d, 0x55, \
    0x0c, 0x66, 0x01, 0x10,  0x47, 0xff, 0xff, 0xff,  0x7f, 0x27, 0xff, 0xff,  0xff, 0x7f, 0x75, 0x20, \
    0x95, 0x01, 0x09, 0x56,  0x81, 0x02, 0x09, 0x54,  0x95, 0x01, 0x75, 0x08,  0x15, 0x00, 0x25, 0x10, \
    0x81, 0x02, 0x85, 0x07,  0x09, 0x55, 0x25, 0x30,  0xb1, 0x02, 0xc0, 0x09,  0x0e, 0xa1, 0x01, 0x85, \
    0x02, 0x09, 0x23, 0xa1,  0x02, 0x09, 0x52, 0x09,  0x53, 0x15, 0x00, 0x25,  0x0a, 0x75, 0x08, 0x95, \
    0x02, 0xb1, 0x02, 0xc0,  0xc0,

#define DESCRIPTOR_TOUCH_EREA_CK    \
    0x06, 0x01, 0xff, 0x09,  0x00, 0xa1, 0x01, 0x85,  0xfc, 0x06, 0x00, 0xff,  0x09, 0x01, 0x09, 0x02,  0x09, 0x03, 0x09, 0x04,  0x09, 0x05, 0x09, 0x06,  0x15, 0x00, 0x26, 0xff,  0x00, 0x75, 0x08, 0x95, \
    0x06, 0x81, 0x02, 0x85,  0xfe, 0x06, 0x00, 0xff,  0x09, 0x01, 0x09, 0x02,  0x09, 0x03, 0x09, 0x04,  0x15, 0x00, 0x26, 0xff,  0x00, 0x75, 0x08, 0x95,  0x04, 0xb1, 0x02, 0xc0,  0x05, 0x01, 0x09, 0x02, \
    0xa1, 0x01, 0x09, 0x01,  0xa1, 0x00, 0x85, 0x11,  0x05, 0x09, 0x19, 0x01,  0x29, 0x03, 0x15, 0x00,  0x25, 0x01, 0x95, 0x03,  0x75, 0x01, 0x81, 0x02,  0x95, 0x01, 0x75, 0x05,  0x81, 0x03, 0x05, 0x01, \
    0x09, 0x30, 0x09, 0x31,  0x15, 0x00, 0x26, 0xff,  0x7f, 0x35, 0x00, 0x46,  0xff, 0x7f, 0x75, 0x10,  0x95, 0x02, 0x81, 0x02,  0x05, 0x0d, 0x09, 0x33,  0x15, 0x00, 0x26, 0xff,  0x00, 0x35, 0x00, 0x46, \
    0xff, 0x00, 0x75, 0x08,  0x95, 0x01, 0x81, 0x02,  0x05, 0x01, 0x09, 0x38,  0x15, 0x81, 0x25, 0x7f,  0x35, 0x81, 0x45, 0x7f,  0x95, 0x01, 0x81, 0x06,  0xc0, 0xc0, 0x06, 0x00,  0xff, 0x09, 0x00, 0xa1, \
    0x01, 0x85, 0xfd, 0x15,  0x00, 0x26, 0xff, 0x00,  0x19, 0x01, 0x29, 0x3f,  0x75, 0x08, 0x95, 0x3f,  0x81, 0x02, 0x19, 0x01,  0x29, 0x3f, 0x91, 0x02,  0xc0, 0x06, 0x00, 0xff,  0x09, 0x00, 0xa1, 0x01, \
    0x85, 0xfb, 0x15, 0x00,  0x26, 0xff, 0x00, 0x19,  0x01, 0x29, 0x3f, 0x75,  0x08, 0x95, 0x3f, 0x81,  0x02, 0x19, 0x01, 0x29,  0x3f, 0x91, 0x02, 0xc0,  0x05, 0x0d, 0x09, 0x04,  0xa1, 0x01, 0x85, 0x02, \
    0x09, 0x22, 0xa1, 0x02,  0x09, 0x42, 0x15, 0x00,  0x25, 0x01, 0x75, 0x01,  0x95, 0x01, 0x81, 0x02,  0x09, 0x32, 0x81, 0x02,  0x95, 0x06, 0x81, 0x03,  0x75, 0x08, 0x09, 0x51,  0x95, 0x01, 0x81, 0x02, \
    0x05, 0x01, 0x15, 0x00,  0x26, 0xff, 0x7f, 0x75,  0x10, 0x55, 0x00, 0x65,  0x00, 0x09, 0x30, 0x35,  0x00, 0x46, 0x00, 0x00,  0x81, 0x02, 0x09, 0x31,  0x81, 0x02, 0x05, 0x0d,  0x09, 0x60, 0x81, 0x02, \
    0x09, 0x61, 0x81, 0x02,  0xc0, 0xa1, 0x02, 0x05,  0x0d, 0x09, 0x42, 0x15,  0x00, 0x25, 0x01, 0x75,  0x01, 0x95, 0x01, 0x81,  0x02, 0x09, 0x32, 0x81,  0x02, 0x95, 0x06, 0x81,  0x03, 0x75, 0x08, 0x09, \
    0x51, 0x95, 0x01, 0x81,  0x02, 0x05, 0x01, 0x15,  0x00, 0x26, 0xff, 0x7f,  0x75, 0x10, 0x55, 0x00,  0x65, 0x00, 0x09, 0x30,  0x35, 0x00, 0x46, 0x00,  0x00, 0x81, 0x02, 0x09,  0x31, 0x81, 0x02, 0x05, \
    0x0d, 0x09, 0x60, 0x81,  0x02, 0x09, 0x61, 0x81,  0x02, 0xc0, 0xa1, 0x02,  0x05, 0x0d, 0x09, 0x42,  0x15, 0x00, 0x25, 0x01,  0x75, 0x01, 0x95, 0x01,  0x81, 0x02, 0x09, 0x32,  0x81, 0x02, 0x95, 0x06, \
    0x81, 0x03, 0x75, 0x08,  0x09, 0x51, 0x95, 0x01,  0x81, 0x02, 0x05, 0x01,  0x15, 0x00, 0x26, 0xff,  0x7f, 0x75, 0x10, 0x55,  0x00, 0x65, 0x00, 0x09,  0x30, 0x35, 0x00, 0x46,  0x00, 0x00, 0x81, 0x02, \
    0x09, 0x31, 0x81, 0x02,  0x05, 0x0d, 0x09, 0x60,  0x81, 0x02, 0x09, 0x61,  0x81, 0x02, 0xc0, 0xa1,  0x02, 0x05, 0x0d, 0x09,  0x42, 0x15, 0x00, 0x25,  0x01, 0x75, 0x01, 0x95,  0x01, 0x81, 0x02, 0x09, \
    0x32, 0x81, 0x02, 0x95,  0x06, 0x81, 0x03, 0x75,  0x08, 0x09, 0x51, 0x95,  0x01, 0x81, 0x02, 0x05,  0x01, 0x15, 0x00, 0x26,  0xff, 0x7f, 0x75, 0x10,  0x55, 0x00, 0x65, 0x00,  0x09, 0x30, 0x35, 0x00, \
    0x46, 0x00, 0x00, 0x81,  0x02, 0x09, 0x31, 0x81,  0x02, 0x05, 0x0d, 0x09,  0x60, 0x81, 0x02, 0x09,  0x61, 0x81, 0x02, 0xc0,  0xa1, 0x02, 0x05, 0x0d,  0x09, 0x42, 0x15, 0x00,  0x25, 0x01, 0x75, 0x01, \
    0x95, 0x01, 0x81, 0x02,  0x09, 0x32, 0x81, 0x02,  0x95, 0x06, 0x81, 0x03,  0x75, 0x08, 0x09, 0x51,  0x95, 0x01, 0x81, 0x02,  0x05, 0x01, 0x15, 0x00,  0x26, 0xff, 0x7f, 0x75,  0x10, 0x55, 0x00, 0x65, \
    0x00, 0x09, 0x30, 0x35,  0x00, 0x46, 0x00, 0x00,  0x81, 0x02, 0x09, 0x31,  0x81, 0x02, 0x05, 0x0d,  0x09, 0x60, 0x81, 0x02,  0x09, 0x61, 0x81, 0x02,  0xc0, 0xa1, 0x02, 0x05,  0x0d, 0x09, 0x42, 0x15, \
    0x00, 0x25, 0x01, 0x75,  0x01, 0x95, 0x01, 0x81,  0x02, 0x09, 0x32, 0x81,  0x02, 0x95, 0x06, 0x81,  0x03, 0x75, 0x08, 0x09,  0x51, 0x95, 0x01, 0x81,  0x02, 0x05, 0x01, 0x15,  0x00, 0x26, 0xff, 0x7f, \
    0x75, 0x10, 0x55, 0x00,  0x65, 0x00, 0x09, 0x30,  0x35, 0x00, 0x46, 0x00,  0x00, 0x81, 0x02, 0x09,  0x31, 0x81, 0x02, 0x05,  0x0d, 0x09, 0x60, 0x81,  0x02, 0x09, 0x61, 0x81,  0x02, 0xc0, 0x05, 0x0d, \
    0x09, 0x54, 0x15, 0x00,  0x26, 0xff, 0x00, 0x95,  0x01, 0x75, 0x08, 0x81,  0x02, 0x85, 0x03, 0x09,  0x55, 0x15, 0x00, 0x25,  0x02, 0xb1, 0x02, 0xc0,  0x09, 0x0e, 0xa1, 0x01,  0x85, 0x04, 0x09, 0x23, \
    0xa1, 0x02, 0x09, 0x52,  0x09, 0x53, 0x15, 0x00,  0x25, 0x0a, 0x75, 0x08,  0x95, 0x02, 0xb1, 0x02,  0xc0, 0xc0,
        
    
#define DESCRIPTOR_PEN    \
        0x05, 0x0d,       \
        0x09, 0x02,       \
        0xa1, 0x01,       \
        0x85, 0x08,       \
        0x09, 0x20,       \
        0xa1, 0x00,       \
        0x09, 0x42,       \
        0x09, 0x44,       \
        0x09, 0x45,       \
        0x09, 0x3c,       \
        0x09, 0x32,       \
        0x15, 0x00,       \
        0x25, 0x01,       \
        0x75, 0x01,       \
        0x95, 0x05,       \
        0x81, 0x02,       \
        0x95, 0x0b,       \
        0x81, 0x03,       \
        0x05, 0x01,       \
        0x26, 0xff, 0x7f, \
        0x75, 0x10,       \
        0x95, 0x01,       \
        0xa4,             \
        0x55, 0x0d,       \
        0x65, 0x33,       \
        0x09, 0x30,       \
        0x35, 0x00,       \
        0x46, 0x00, 0x00, \
        0x81, 0x02,       \
        0x09, 0x31,       \
        0x46, 0x00, 0x00, \
        0x81, 0x02,       \
        0xb4,             \
        0x05, 0x0d,       \
        0x09, 0x30,       \
        0x81, 0x02,       \
        0x09, 0x3d,       \
        0x09, 0x3e,       \
        0x16, 0x01, 0x80, \
        0x95, 0x02,       \
        0x81, 0x02,       \
        0xc0,             \
        0xc0,

#define DESCRIPTOR_PEN_NEW    \
        0x05, 0x0D,                         \
        0x09, 0x02,                         \
        0xA1, 0x01,                         \
        0x85, 0x08,                         \
        0x09, 0x20,                         \
        0xA1, 0x00,                         \
        0x05, 0x0D,                         \
        0x15, 0x00,                         \
        0x25, 0x01,                         \
        0x75, 0x01,                         \
        0x95, 0x01,                         \
        0x09, 0x42,                         \
        0x81, 0x02,                         \
        0x09, 0x32,                         \
        0x81, 0x02,                         \
        0x09, 0x45,                         \
        0x81, 0x02,                         \
        0x09, 0x3C,                         \
        0x81, 0x02,                         \
        0x95, 0x04,                         \
        0x81, 0x03,                         \
        0x05, 0x0D,                         \
        0x75, 0x08,                         \
        0x95, 0x01,                         \
        0x09, 0x5B,                         \
        0x81, 0x02,                         \
        0x05, 0x01,                         \
        0x65, 0x11,                         \
        0x55, 0x0E,                         \
        0x75, 0x10,                         \
        0x35, 0x00,                         \
        0x26, 0xff, 0x7f,                   \
        0x46, 0x00, 0x00,                   \
        0x09, 0x30,                         \
        0x81, 0x02,                         \
        0x26, 0xff, 0x7f,                   \
        0x46, 0x00, 0x00,                   \
        0x09, 0x31,                         \
        0x81, 0x02,                         \
        0x05, 0x0D,                         \
        0x65, 0x11,                         \
        0x55, 0x0E,                         \
        0x75, 0x10,                         \
        0x35, 0x00,                         \
        0x26, (TOUCH_SIZE_W%256), (TOUCH_SIZE_W/256),                   \
        0x46, (TOUCH_SIZE_W%256), (TOUCH_SIZE_W/256),                   \
        0x09, 0x48,                         \
        0x81, 0x02,                         \
        0x26, (TOUCH_SIZE_H%256), (TOUCH_SIZE_H/256),                   \
        0x46, (TOUCH_SIZE_H%256), (TOUCH_SIZE_H/256),                   \
        0x09, 0x49,                         \
        0x81, 0x02,                         \
        0x05, 0x0D,                         \
        0x55, 0x0E,                         \
        0x75, 0x08,                         \
        0x15, 0x00,                         \
        0x25, 0x7E,                         \
        0x35, 0x00,                         \
        0x46, 0x50, 0x46,                   \
        0x09, 0x3F,                         \
        0x81, 0x02,                         \
        0xC0,                               \
        0xC0,
        
#define DESCRIPTOR_PEN_PM \
        0x05,0x0D,0x09,0x02,0xA1,0x01,0x85,0x08,0x09,0x20,0xA1,0x00,0x15,0x00,\
        0x25,0x01,0x09,0x42,0x75,0x01,0x95,0x01,0x81,0x02,0x09,0x32,0x81,0x02,\
        0x09,0x44,0x81,0x02,0x95,0x05,0x81,0x01,0x05,0x01,0x26,0xFF,0x7F,0x75,\
        0x10,0x95,0x01,0x55,0x0D,0x65,0x11,0x35,0x00,0x46,0x00,0x00,0x09,0x30,\
        0x81,0x02,0x46,0x00,0x00,0x26,0xFF,0x7F,0x09,0x31,0x81,0x02,0xC0,0xC0,
        
        
/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_HID
  * @brief usbd core module
  * @{
  */

/** @defgroup USBD_HID_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_HID_Private_Defines
  * @{
  */

/**
  * @}
  */


/** @defgroup USBD_HID_Private_Macros
  * @{
  */
/**
  * @}
  */




/** @defgroup USBD_HID_Private_FunctionPrototypes
  * @{
  */


static uint8_t  USBD_HID_Init (void  *pdev,
                               uint8_t cfgidx);

static uint8_t  USBD_HID_DeInit (void  *pdev,
                                 uint8_t cfgidx);

static uint8_t  USBD_HID_Setup (void  *pdev,
                                USB_SETUP_REQ *req);

static uint8_t  *USBD_HID_GetCfgDesc (uint8_t speed, uint16_t *length);

static uint8_t  USBD_HID_DataIn (void  *pdev, uint8_t epnum);
static uint8_t  USBD_HID_DataOut (void  *pdev, uint8_t epnum);
static uint8_t  USBD_HID_DataOut_HS (void  *pdev, uint8_t epnum);

static uint8_t  USBD_HID_SOF (void  *pdev);

extern void EP1_OUT_Callback(u8 *);

int usb_check = 0;
#if 0
typedef enum
{
    USB_CHECK_FS = 0x00,
    USB_CHECK_HS = 0x01,
    USB_CHECK_NUM,
}USB_CHECK;

USB_CHECK usb_check;
#endif

/**
  * @}
  */

/** @defgroup USBD_HID_Private_Variables
  * @{
  */
__ALIGN_BEGIN u8 USBD_HID_SndBuf[64] __ALIGN_END;
__ALIGN_BEGIN u8 USBD_HID_SndBuf2[64] __ALIGN_END;
__ALIGN_BEGIN u8 USBD_HID_SndBuf3[64] __ALIGN_END;
__ALIGN_BEGIN u8 USBD_HID_RcvBuf[64] __ALIGN_END;
__ALIGN_BEGIN u8 USBD_HID_RcvBuf2[64] __ALIGN_END;
__ALIGN_BEGIN u8 USBD_HID_RcvBuf3[64] __ALIGN_END;
__ALIGN_BEGIN u8 USB_Buffer_PM[64] __ALIGN_END;

// USB HID 发送冲突控制
__IO u8 USBD_HID_TxValid_EP1 = 1;
__IO u8 USBD_HID_TxValid_EP2 = 1;
__IO u8 USBD_HID_TxValid_EP3 = 1;
__IO u8 USBD_HID_TxCnt = 0;    


USBD_Class_cb_TypeDef  USBD_HID_cb =
{
  USBD_HID_Init,
  USBD_HID_DeInit,
  USBD_HID_Setup,
  NULL, /*EP0_TxSent*/
  NULL, /*EP0_RxReady*/
  USBD_HID_DataIn, /*DataIn*/
  USBD_HID_DataOut,
  USBD_HID_SOF, /*SOF */
  NULL,
  NULL,
  USBD_HID_GetCfgDesc,
#ifdef USB_OTG_HS_CORE
  USBD_HID_GetCfgDesc, /* use same config as per FS */
#endif
};

USBD_Class_cb_TypeDef  USBD_HID_cb1 =
{
  USBD_HID_Init,
  USBD_HID_DeInit,
  USBD_HID_Setup,
  NULL, /*EP0_TxSent*/
  NULL, /*EP0_RxReady*/
  USBD_HID_DataIn, /*DataIn*/
  USBD_HID_DataOut_HS,
  USBD_HID_SOF, /*SOF */
  NULL,
  NULL,
  USBD_HID_GetCfgDesc,
#ifdef USB_OTG_HS_CORE
  USBD_HID_GetCfgDesc, /* use same config as per FS */
#endif
};

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN static uint32_t  USBD_HID_AltSet  __ALIGN_END = 0;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN static uint32_t  USBD_HID_Protocol  __ALIGN_END = 0;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN static uint32_t  USBD_HID_IdleState __ALIGN_END = 0;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

//=========================================== 配置描述符 =======================================
#if defined(USB_ONE_INTERFACE)
    #define NR_INTF     1
#elif defined(USB_TWO_INTERFACE)
    #define NR_INTF     2    
#else
    #define NR_INTF     3
#endif
/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_CfgDesc[USB_HID_CONFIG_DESC_SIZ] __ALIGN_END =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
  USB_HID_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  NR_INTF,      /*bNumInterfaces: 2 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing
  the configuration*/
  0xa0,         /*bmAttributes: bus powered and Support Remote Wake-up */
  0xfa,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

  /************** Descriptor of hid interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x02,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: HID*/
  0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  2,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of Joystick Mouse HID ********************/
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  (u8)HID_MOUSE_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  HID_MOUSE_REPORT_DESC_SIZE/256,
  /******************** Descriptor of Mouse endpoint ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

  HID_IN_EP,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_IN_PACKET, /*wMaxPacketSize: 4 Byte max */
  0x00,
  1,          /*bInterval: Polling Interval (10 ms)*/
  /* 34 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

  HID_OUT_EP,          /*bEndpointAddress: Endpoint Address (OUT)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_OUT_PACKET,          /*wMaxPacketSize: 4 Byte max */
  0x00,
  1,          /*bInterval: Polling Interval (1 ms)*/
  /* 41 */

#ifndef USB_ONE_INTERFACE
    /************** Descriptor of Touch interface ****************/
    /* 09 */
    0x09,         /*bLength: Interface Descriptor size*/
    USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor type*/
    0x01,         /*bInterfaceNumber: Number of Interface*/
    0x00,         /*bAlternateSetting: Alternate setting*/
    0x02,         /*bNumEndpoints*/
    0x03,         /*bInterfaceClass: HID*/
    0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
    0x01,//0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
    0,            /*iInterface: Index of string descriptor*/
    /******************** Descriptor of Touch HID ********************/
    /* 18 */
    0x09,         /*bLength: HID Descriptor size*/
    HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
    0x11,         /*bcdHID: HID Class Spec release number*/
    0x01,
    0x00,         /*bCountryCode: Hardware target country*/
    0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
    0x22,         /*bDescriptorType*/
    (u8)HID_WIN7_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
    HID_WIN7_REPORT_DESC_SIZE/256,
    /******************** Descriptor of Touch endpoint ********************/
    /* 27 */ //IN
    0x07,          /*bLength: Endpoint Descriptor size*/
    USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

    HID_IN_EP2,          /*bEndpointAddress: Endpoint Address (IN)*/
    0x03,          /*bmAttributes: Interrupt endpoint*/
    HID_IN_PACKET,          /*wMaxPacketSize: 64 Byte max */
    0x00,
    1,          /*bInterval: Polling Interval (1 ms)*/
    /******************** Descriptor of Touch endpoint ********************/
    /* 34 */  //OUT
    0x07,          /*bLength: Endpoint Descriptor size*/
    USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

    HID_OUT_EP2,          /*bEndpointAddress: Endpoint Address (OUT)*/
    0x03,          /*bmAttributes: Interrupt endpoint*/
    HID_OUT_PACKET,          /*wMaxPacketSize: 4 Byte max */
    0x00,
    1,          /*bInterval: Polling Interval (1 ms)*/
    /* 41 */

#ifndef USB_TWO_INTERFACE
/************** Descriptor of interface 3****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor type*/
  0x02,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x02,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: HID*/
  0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of Joystick Mouse HID ********************/
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  (u8)HID_INTF3_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  HID_INTF3_REPORT_DESC_SIZE/256,
  /******************** Descriptor of Mouse endpoint ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

  HID_IN_EP3,     /*bEndpointAddress: Endpoint Address (IN)*/
  
  INT_BULK,    //0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_IN_PACKET, /*wMaxPacketSize: 4 Byte max */
  0x00,
  1,          /*bInterval: Polling Interval (10 ms)*/
  /* 34 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

  HID_OUT_EP3,          /*bEndpointAddress: Endpoint Address (OUT)*/
  INT_BULK,    //0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_OUT_PACKET,          /*wMaxPacketSize: 4 Byte max */
  0x00,
  1,          /*bInterval: Polling Interval (1 ms)*/
  /* 41 */ 
#endif  
#endif
} ;

/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_CfgDesc_MAC[USB_HID_CONFIG_DESC_SIZ_MAC] __ALIGN_END =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
  USB_HID_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,      /*bNumInterfaces: 2 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing
  the configuration*/
  0xa0,         /*bmAttributes: bus powered and Support Remote Wake-up */
  0xfa,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

  /************** Descriptor of hid interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x02,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: HID*/
  0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of Joystick Mouse HID ********************/
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  (u8)HID_MAC_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  HID_MAC_REPORT_DESC_SIZE/256,
  /******************** Descriptor of Mouse endpoint ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

  HID_IN_EP,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_IN_PACKET, /*wMaxPacketSize: 4 Byte max */
  0x00,
  0x01,          /*bInterval: Polling Interval (10 ms)*/
  /* 34 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

  HID_OUT_EP,          /*bEndpointAddress: Endpoint Address (OUT)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_OUT_PACKET,          /*wMaxPacketSize: 4 Byte max */
  0x00,
  0x01,          /*bInterval: Polling Interval (1 ms)*/
  /* 41 */
} ;

//============================================ HID描述符 =====================================
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_Desc[USB_HID_DESC_SIZ] __ALIGN_END=
{
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  (u8)HID_MOUSE_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  HID_MOUSE_REPORT_DESC_SIZE/256,
};

/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_Desc_WIN7[USB_HID_DESC_SIZ] __ALIGN_END=
{
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  (u8)HID_WIN7_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  HID_WIN7_REPORT_DESC_SIZE/256,
};

/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_Desc_MAC[USB_HID_DESC_SIZ] __ALIGN_END=
{
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  (u8)HID_MAC_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  HID_MAC_REPORT_DESC_SIZE/256,
};
#endif

//========================================= HID报告描述符 ======================================
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */



//----------------------------------- CW -------------------------------------
#if defined(CHUANGWEI)
__ALIGN_BEGIN static uint8_t HID_Mouse_Touch_ReportDesc[HID_REPORT_DESC_SIZE] __ALIGN_END =
{
    // interface 1
    DESCRIPTOR_USER0506
    DESCRIPTOR_MOUSE
    DESCRIPTOR_MULTITOUCH
};
//----------------------------------- TST -------------------------------------
#elif defined(TST)
__ALIGN_BEGIN static uint8_t HID_Mouse_Touch_ReportDesc[HID_REPORT_DESC_SIZE] __ALIGN_END =
{
    // interface 1
    DESCRIPTOR_TOUCH_EREA_TST
    // interface 2
    DESCRIPTOR_USER0506
    DESCRIPTOR_MOUSE_HH
};
//----------------------------------- CK -------------------------------------
#elif defined(CK)
__ALIGN_BEGIN static uint8_t HID_Mouse_Touch_ReportDesc[HID_REPORT_DESC_SIZE] __ALIGN_END =
{
    // interface 1
    DESCRIPTOR_USER050607
    DESCRIPTOR_MOUSE
    DESCRIPTOR_TOUCH_EREA_CK
};
//----------------------------------- CH -------------------------------------
#elif defined(CHANGHONG)
__ALIGN_BEGIN static uint8_t HID_Mouse_Touch_ReportDesc[HID_REPORT_DESC_SIZE] __ALIGN_END =
{
    // interface 1
    DESCRIPTOR_USER050607
    DESCRIPTOR_USERA1
    DESCRIPTOR_USERA2
    DESCRIPTOR_USERA3
    DESCRIPTOR_USER_CH
    DESCRIPTOR_MOUSE
    DESCRIPTOR_KEY
    // interface 2
    DESCRIPTOR_MULTITOUCH
};
//----------------------------------- PM -------------------------------------
#elif defined(PM)
__ALIGN_BEGIN static uint8_t HID_Mouse_Touch_ReportDesc[HID_REPORT_DESC_SIZE] __ALIGN_END =
{
    // interface 1
    DESCRIPTOR_USER_PM
    DESCRIPTOR_MOUSE
    // interface 2
    DESCRIPTOR_USERA3
    DESCRIPTOR_MULTITOUCH
    DESCRIPTOR_PEN_PM
};
//----------------------------------- HH -------------------------------------
#elif defined(HONGHE)
__ALIGN_BEGIN static uint8_t HID_Mouse_Touch_ReportDesc[HID_REPORT_DESC_SIZE] __ALIGN_END =
{
    // interface 1
    DESCRIPTOR_USER050607
    DESCRIPTOR_USERA1
    DESCRIPTOR_USERA2
    DESCRIPTOR_USERA3
//#if defined(I2C_IST2)
//    DESCRIPTOR_USER_CH
//#endif
    DESCRIPTOR_KEY
    DESCRIPTOR_MOUSE_HH
    // interface 2
    DESCRIPTOR_USER_HH
#ifndef POINT_OS_EREA
    DESCRIPTOR_TOUCH
#else
    DESCRIPTOR_TOUCH_EREA_HH2
    DESCRIPTOR_PEN_NEW
#endif
};
//----------------------------------- SEEWO -------------------------------------
#elif defined(SEEWO)
__ALIGN_BEGIN static uint8_t HID_Mouse_Touch_ReportDesc[HID_REPORT_DESC_SIZE] __ALIGN_END =
{
    // interface 1
    DESCRIPTOR_USER050607
    DESCRIPTOR_USERFB
    DESCRIPTOR_USERFC
    DESCRIPTOR_USERA3
    DESCRIPTOR_USER_CH
    DESCRIPTOR_KEY
    DESCRIPTOR_MOUSE

    // interface 2
    DESCRIPTOR_MULTITOUCH
    DESCRIPTOR_PEN_NEW
};

//----------------------------------- IST -------------------------------------
#else
__ALIGN_BEGIN static uint8_t HID_Mouse_Touch_ReportDesc[HID_REPORT_DESC_SIZE] __ALIGN_END =
{
    // interface 1
    DESCRIPTOR_USER050607
    DESCRIPTOR_USERA1
    DESCRIPTOR_USERA2
    DESCRIPTOR_USERA3
    DESCRIPTOR_USER_CH
    DESCRIPTOR_KEY
    DESCRIPTOR_MOUSE

    // interface 2
    DESCRIPTOR_MULTITOUCH
#ifndef USB_ONE_INTERFACE
    DESCRIPTOR_PEN_NEW
#endif
};
#endif

u8* HID_MOUSE_ReportDesc = HID_Mouse_Touch_ReportDesc;
u8* HID_Intf3_ReportDesc = HID_Mouse_Touch_ReportDesc +HID_MOUSE_REPORT_DESC_SIZE;
u8* HID_WIN7_ReportDesc  = HID_Mouse_Touch_ReportDesc +HID_MOUSE_REPORT_DESC_SIZE+HID_INTF3_REPORT_DESC_SIZE;


__ALIGN_BEGIN static uint8_t HID_MOUSE_ReportValue[HID_MOUSE_REPORT_VALUE_SIZE] __ALIGN_END =
{
#ifdef TST
    0x07,
#else
    0x03,
#endif
  10,
};

/**
  * @}
  */

/** @defgroup USBD_HID_Private_Functions
  * @{
  */

/**
  * @brief  USBD_HID_Init
  *         Initialize the HID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_HID_Init (void  *pdev,
                               uint8_t cfgidx)
{

  /* Open EP IN */
  DCD_EP_Open(pdev,
              HID_IN_EP,
              HID_IN_PACKET,
              USB_OTG_EP_INT);

  /* Open EP OUT */
  DCD_EP_Open(pdev,
              HID_OUT_EP,
              HID_OUT_PACKET,
              USB_OTG_EP_INT);

    /* Open EP IN */
  DCD_EP_Open(pdev,
              HID_IN_EP2,
              HID_IN_PACKET,
              USB_OTG_EP_INT);

  /* Open EP OUT */
  DCD_EP_Open(pdev,
              HID_OUT_EP2,
              HID_OUT_PACKET,
              USB_OTG_EP_INT);

  	/* Open EP IN */
  DCD_EP_Open(pdev,
              HID_IN_EP3,
              HID_IN_PACKET,
              USB_OTG_EP_INT);

  /* Open EP OUT */
  DCD_EP_Open(pdev,
              HID_OUT_EP3,
              HID_OUT_PACKET,
              USB_OTG_EP_INT);

  DCD_EP_PrepareRx(pdev, HID_OUT_EP, USBD_HID_RcvBuf, 64);
  DCD_EP_PrepareRx(pdev, HID_OUT_EP2, USBD_HID_RcvBuf2, 64);
  DCD_EP_PrepareRx(pdev, HID_OUT_EP3, USBD_HID_RcvBuf3, 64);

  return USBD_OK;
}

/**
  * @brief  USBD_HID_Init
  *         DeInitialize the HID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_HID_DeInit (void  *pdev,
                                 uint8_t cfgidx)
{
  /* Close HID EPs */
  DCD_EP_Close (pdev , HID_IN_EP);
  DCD_EP_Close (pdev , HID_OUT_EP);

  DCD_EP_Close (pdev , HID_IN_EP2);
  DCD_EP_Close (pdev , HID_OUT_EP2);

  DCD_EP_Close (pdev , HID_IN_EP3);
  DCD_EP_Close (pdev , HID_OUT_EP3);

  return USBD_OK;
}

/**
  * @brief  USBD_HID_Setup
  *         Handle the HID specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_HID_Setup (void  *pdev,
                                USB_SETUP_REQ *req)
{
  uint16_t len = 0;
  uint8_t  *pbuf = NULL;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :
    switch (req->bRequest)
    {

    case HID_REQ_SET_PROTOCOL:
      USBD_HID_Protocol = (uint8_t)(req->wValue);
      break;

    case HID_REQ_GET_PROTOCOL:
      USBD_CtlSendData (pdev,
                        (uint8_t *)&USBD_HID_Protocol,
                        1);
      break;

    case HID_REQ_SET_IDLE:
      USBD_HID_IdleState = (uint8_t)(req->wValue >> 8);
      break;

    case HID_REQ_GET_IDLE:
      USBD_CtlSendData (pdev,
                        (uint8_t *)&USBD_HID_IdleState,
                        1);
      break;

    case HID_REQ_GET_REPORT:
    {
//#if defined(WIDI) || defined(CHUANGWEI)
    //  Host_OS = Host_OS_Win7;
//#endif
      len = MIN(HID_MOUSE_REPORT_VALUE_SIZE , req->wLength);
      pbuf = HID_MOUSE_ReportValue;
      USBD_CtlSendData (pdev,
                        pbuf,
                        len);
    }
      break;
    case HID_REQ_SET_REPORT:
    {
#ifdef TST
      if ((req->wValue & 0xff) == 0x02) {        
#else
      if ((req->wValue & 0xff) == 0x04) {
#endif
      //  Host_OS = Host_OS_Win7;
      }
      USBD_CtlPrepareRx (pdev, USB_Buffer_PM, 64);
    }
      break;
    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL;
    }
    break;

  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
      case USB_REQ_GET_DESCRIPTOR:
      if( req->wValue >> 8 == HID_REPORT_DESC)
      {
          if((req->wIndex & 0xFF) == 0) {
#ifdef MAC_RECONNECT						
            if(macUsbReconnectFlag)
              len = MIN(HID_MAC_REPORT_DESC_SIZE , req->wLength);
            else
#endif							
              len = MIN(HID_MOUSE_REPORT_DESC_SIZE , req->wLength);
            pbuf = HID_MOUSE_ReportDesc;
          }
#ifndef USB_ONE_INTERFACE
          else if((req->wIndex & 0xFF) == 1) {
            len = MIN(HID_WIN7_REPORT_DESC_SIZE , req->wLength);
            pbuf = HID_WIN7_ReportDesc;
          }
#ifndef USB_TWO_INTERFACE          
          else if((req->wIndex & 0xFF) == 2) {					
            len = MIN(HID_INTF3_REPORT_DESC_SIZE , req->wLength);
            pbuf = HID_Intf3_ReportDesc;
          }
#endif          
#endif
        }
      else if( req->wValue >> 8 == HID_DESCRIPTOR_TYPE)
      {
        if((req->wIndex & 0xFF) == 0) {
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
    #ifdef MAC_RECONNECT					
            if(macUsbReconnectFlag)
                pbuf = USBD_HID_Desc_MAC;	
            else
    #endif							
                pbuf = USBD_HID_Desc;
#else
    #ifdef MAC_RECONNECT
            if(macUsbReconnectFlag)
                pbuf = USBD_HID_CfgDesc_MAC + 0x12;	
            else
    #endif							
                pbuf = USBD_HID_CfgDesc + 0x12;						            
#endif
            len = MIN(USB_HID_DESC_SIZ , req->wLength);
        }
        else if((req->wIndex & 0xFF) == 1) {
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
            pbuf = USBD_HID_Desc_WIN7;
#else
            pbuf = USBD_HID_CfgDesc + 0x12;
#endif
            len = MIN(USB_HID_DESC_SIZ , req->wLength);
        }
      }

      USBD_CtlSendData (pdev,
                        pbuf,
                        len);

      break;

    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev,
                        (uint8_t *)&USBD_HID_AltSet,
                        1);
      break;

    case USB_REQ_SET_INTERFACE :
      USBD_HID_AltSet = (uint8_t)(req->wValue);
      break;
    }
  }
  return USBD_OK;
}

/**
  * @brief  USBD_HID_SendReport
  *         Send HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t USBD_HID_SendReport     (USB_OTG_CORE_HANDLE  *pdev,
                                 uint8_t *report,
                                 uint16_t len)
{

#ifdef TST
    if (report[0] == 0x05)
        return USBD_HID_SendReport_EP2 (pdev, report, len);
#endif
    
#ifndef USB_ONE_INTERFACE
#ifndef USB_TWO_INTERFACE
    #ifdef PM       //  为了支持PM mac手势,触摸接口中拉增加A3, 所以A3需要发到EP2
    if (report[0] == 0xa3)
        return USBD_HID_SendReport_EP2 (pdev, report, len);
    #endif
#endif
#endif
    //if (!(Mode_Point&Mode_Point_OS) && report[0]==0x01) // block report from picasso subsystem
        //return USBD_OK;

    if (pdev->dev.device_status == USB_OTG_CONFIGURED )
    {
        if (USBD_HID_TxValid_EP1)
        {
            USBD_HID_TxValid_EP1 = 0;
            memcpy(USBD_HID_SndBuf, report, len);
            DCD_EP_Tx (pdev, HID_IN_EP, USBD_HID_SndBuf, len);
        }
        else {
            USBD_FIFO_Push(&USBD_HID_TxFIFO_EP1, report, &len);
        }
    }
    return USBD_OK;
}

/**
  * @brief  USBD_HID_SendReport
  *         Send HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t USBD_HID_SendReport_EP2 (USB_OTG_CORE_HANDLE  *pdev,
                                 uint8_t *report,
                                 uint16_t len)
{
#ifdef USB_ONE_INTERFACE
    return USBD_HID_SendReport (pdev, report, len);
#endif
    //if (!(Mode_Point&Mode_Point_OS) && report[0]==0x02) // block report from picasso subsystem
        //return USBD_OK;
    
    if (pdev->dev.device_status == USB_OTG_CONFIGURED )
    {
        if (USBD_HID_TxValid_EP2)
        {
            USBD_HID_TxValid_EP2 = 0;
            memcpy(USBD_HID_SndBuf2, report, len);
            DCD_EP_Tx (pdev, HID_IN_EP2, USBD_HID_SndBuf2, len);
        }
        else {
            USBD_FIFO_Push(&USBD_HID_TxFIFO_EP2, report, &len);
        }
    }
    return USBD_OK;
}


uint8_t USBD_HID_SendReport_EP3 (USB_OTG_CORE_HANDLE  *pdev,
                                 uint8_t *report,
                                 uint16_t len)
{
#if defined(USB_ONE_INTERFACE)||defined(USB_TWO_INTERFACE)
    return USBD_HID_SendReport (pdev, report, len);
#endif    
    //if ((Mode_Point&Mode_Point_OS)==0 && report[0]==0x01) // block report from picasso subsystem
        //return USBD_OK;
    
    if (pdev->dev.device_status == USB_OTG_CONFIGURED )
    {
        if (USBD_HID_TxValid_EP3)
        {
            USBD_HID_TxValid_EP3 = 0;
            memcpy(USBD_HID_SndBuf3, report, len);
            DCD_EP_Tx (pdev, HID_IN_EP3, USBD_HID_SndBuf3, len);
        }
        else {
            USBD_FIFO_Push(&USBD_HID_TxFIFO_EP3, report, &len);
        }
    }
    return USBD_OK;
}

/**
  * @brief  USBD_HID_GetCfgDesc
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_HID_GetCfgDesc (uint8_t speed, uint16_t *length)
{
#ifdef MAC_RECONNECT
    if(macUsbReconnectFlag) {			
        *length = sizeof (USBD_HID_CfgDesc_MAC);
        return USBD_HID_CfgDesc_MAC;
    }
    else {
        *length = sizeof (USBD_HID_CfgDesc);
        return USBD_HID_CfgDesc;
    }
#else
    *length = sizeof (USBD_HID_CfgDesc);
    return USBD_HID_CfgDesc;
#endif		
}

/**
  * @brief  USBD_HID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_HID_DataIn (void  *pdev,
                              uint8_t epnum)
{
    u16 len;
    //printf(" USBD_HID_DataIn epnum=%d 000 \n",epnum);//接收
    /* Ensure that the FIFO is empty before a new transfer, this condition could
    be caused by  a new transfer before the end of the previous transfer */
#if INT_BULK==2
    if (epnum == 3) {
        memcpy(USBD_HID_SndBuf3, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", 50);
        DCD_EP_Flush(pdev, HID_IN_EP3);
        DCD_EP_Tx (pdev, HID_IN_EP3, USBD_HID_SndBuf3, 64);
        return USBD_OK;
    }else
#endif
    if (epnum == 1) {
        DCD_EP_Flush(pdev, HID_IN_EP);
        if (USBD_FIFO_Peek(&USBD_HID_TxFIFO_EP1) > 0) {
            
            USBD_FIFO_Pop(&USBD_HID_TxFIFO_EP1, USBD_HID_SndBuf, &len);
            DCD_EP_Tx (pdev, HID_IN_EP, USBD_HID_SndBuf, len);
            //printf("DataIn len=%d buf[0]=%x,buf[1]=%x,buf[2]=%x,buf[3]=%x \n",len,USBD_HID_SndBuf[0],USBD_HID_SndBuf[1],USBD_HID_SndBuf[2],USBD_HID_SndBuf[3]);
        } else {            
        //printf(" USBD_HID_DataIn 33333333 \n");
            USBD_HID_TxValid_EP1 = 1;
        }
    }
    else if (epnum == 2) {
        DCD_EP_Flush(pdev, HID_IN_EP2);    
        if (USBD_FIFO_Peek(&USBD_HID_TxFIFO_EP2) > 0) {
            USBD_FIFO_Pop(&USBD_HID_TxFIFO_EP2, USBD_HID_SndBuf2, &len);
            DCD_EP_Tx (pdev, HID_IN_EP2, USBD_HID_SndBuf2, len);
        } else {
            USBD_HID_TxValid_EP2 = 1;
        }
    }        
    else if (epnum == 3) {
        DCD_EP_Flush(pdev, HID_IN_EP3);
        if (USBD_FIFO_Peek(&USBD_HID_TxFIFO_EP3) > 0) {
            USBD_FIFO_Pop(&USBD_HID_TxFIFO_EP3, USBD_HID_SndBuf3, &len);
            DCD_EP_Tx (pdev, HID_IN_EP3, USBD_HID_SndBuf3, len);
        } else {
            USBD_HID_TxValid_EP3 = 1;
        }
    }        
    return USBD_OK;
}

static uint8_t  USBD_HID_DataOut (void  *pdev,
                              uint8_t epnum)
{
    
    u8 buffer[64];
    printf(" USBD_HID_DataOut_FS epnum=%d 111 \n",epnum);//接收
    if (epnum == 1) {
        memcpy(buffer, USBD_HID_RcvBuf, 64);
        DCD_EP_PrepareRx(pdev, HID_OUT_EP, USBD_HID_RcvBuf, 64);
        USBD_HID_RcvBuf[3] = 0;
        usb_check = 1;
        printf("DataOUT  buf[0]=%x,buf[1]=%x,buf[2]=%x,buf[3]=%x \n",USBD_HID_RcvBuf[0],USBD_HID_RcvBuf[1],USBD_HID_RcvBuf[2],USBD_HID_RcvBuf[3]);
        } else {            
        //EP1_OUT_Callback(buffer);
    }
    if (epnum == 2) {
        memcpy(buffer, USBD_HID_RcvBuf2, 64);
        DCD_EP_PrepareRx(pdev, HID_OUT_EP2, USBD_HID_RcvBuf2, 64);
        //EP1_OUT_Callback(buffer);
    }        
    if (epnum == 3) {
        memcpy(buffer, USBD_HID_RcvBuf3, 64);
        DCD_EP_PrepareRx(pdev, HID_OUT_EP3, USBD_HID_RcvBuf3, 64);
        //EP1_OUT_Callback(buffer);
    }        
    return USBD_OK;
}

static uint8_t  USBD_HID_DataOut_HS(void * pdev,uint8_t epnum)
{
    
    u8 buffer[64];
    printf(" USBD_HID_DataOut_HS epnum=%d 222 \n",epnum);//接收
    if (epnum == 1) {
        memcpy(buffer, USBD_HID_RcvBuf2, 64);
        DCD_EP_PrepareRx(pdev, HID_OUT_EP, USBD_HID_RcvBuf2, 64);
        USBD_HID_RcvBuf[3] = 1;
        usb_check = 1;
        printf("DataOUT  buf[0]=%x,buf[1]=%x,buf[2]=%x,buf[3]=%x \n",USBD_HID_RcvBuf[0],USBD_HID_RcvBuf[1],USBD_HID_RcvBuf[2],USBD_HID_RcvBuf[3]);
        } else {            
        //EP1_OUT_Callback(buffer);
    }
    if (epnum == 2) {
        memcpy(buffer, USBD_HID_RcvBuf2, 64);
        DCD_EP_PrepareRx(pdev, HID_OUT_EP2, USBD_HID_RcvBuf2, 64);
        //EP1_OUT_Callback(buffer);
    }        
    if (epnum == 3) {
        memcpy(buffer, USBD_HID_RcvBuf3, 64);
        DCD_EP_PrepareRx(pdev, HID_OUT_EP3, USBD_HID_RcvBuf3, 64);
        //EP1_OUT_Callback(buffer);
    }        
    return USBD_OK;
}

static uint8_t  USBD_HID_SOF (void  *pdev)
{
//	extern __IO u32 jiffies_SOF;
    //extern __IO u32 jiffies;
    //jiffies_SOF = jiffies;
}

void WaitForUSBDFIFO(void)
{
    while (USBD_FIFO_Used(&USBD_HID_TxFIFO_EP1) > USBD_FIFO_Unused(&USBD_HID_TxFIFO_EP1));
    while (USBD_FIFO_Used(&USBD_HID_TxFIFO_EP2) > USBD_FIFO_Unused(&USBD_HID_TxFIFO_EP2));
    while (USBD_FIFO_Used(&USBD_HID_TxFIFO_EP3) > USBD_FIFO_Unused(&USBD_HID_TxFIFO_EP3));
}

void USBD_FIFO_FlushAll()
{
    USBD_FIFO_Flush(&USBD_HID_TxFIFO_EP1);
    USBD_FIFO_Flush(&USBD_HID_TxFIFO_EP2);
    USBD_FIFO_Flush(&USBD_HID_TxFIFO_EP3);
}


/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
