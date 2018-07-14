/**
  ******************************************************************************
  * @file    usbd_hid_core.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    09-November-2015
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
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
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
#include <stdio.h>


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
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN uint8_t hid_report_buffer[HID_TXRX_DATA_SIZE] __ALIGN_END ;

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

#define USB_HID_IDLE         0
#define USB_HID_BUSY         1
#define USB_HID_ZLP          2

uint8_t HID_Tx_State = USB_HID_IDLE;
uint8_t HID_report_id = 0;

uint32_t HID_report_size = 0;

uint32_t HID_Rx_ptr_in  = 0;
uint32_t HID_Rx_ptr_out = 0;
uint32_t HID_Rx_length  = 0;

uint8_t IsReportAvailable = 0;
uint8_t USBD_HID_Report_ID=0;
uint8_t USBD_HID_Report_LENGTH=0;

/** @defgroup USBD_HID_Private_FunctionPrototypes
  * @{
  */


uint8_t  USBD_HID_Init (void  *pdev, 
                               uint8_t cfgidx);

uint8_t  USBD_HID_DeInit (void  *pdev, 
                                 uint8_t cfgidx);

uint8_t  USBD_HID_Setup (void  *pdev, 
                                USB_SETUP_REQ *req);

static uint8_t  *USBD_HID_GetCfgDesc (uint8_t speed, uint16_t *length);

uint8_t  USBD_HID_DataIn (void  *pdev, uint8_t epnum);

uint8_t  USBD_HID_DataOut (void *pdev, uint8_t epnum);
uint8_t  USBD_HID_EP0_RxReady (void  *pdev);

/**
  * @}
  */ 

/** @defgroup USBD_HID_Private_Variables
  * @{
  */ 

USBD_Class_cb_TypeDef  USBD_HID_cb = 
{
  USBD_HID_Init,
  USBD_HID_DeInit,
  USBD_HID_Setup,
  NULL, /*EP0_TxSent*/  
  USBD_HID_EP0_RxReady, /*EP0_RxReady*/
  USBD_HID_DataIn, /*DataIn*/
  USBD_HID_DataOut, /*DataOut*/
  NULL, /*SOF */
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
__ALIGN_BEGIN uint8_t USB_Hid_Rx_Buffer   [HID_IN_PACKET] __ALIGN_END ;
//__ALIGN_BEGIN uint8_t USB_Hid_GetReport_Buffer   [8] __ALIGN_END = {0x03,0x06};
//__ALIGN_BEGIN uint8_t USB_Hid_SetReport_Buffer   [8] __ALIGN_END = {0,0};

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
/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_CfgDesc[USB_HID_CONFIG_DESC_SIZ] __ALIGN_END =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
  USB_HID_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing
  the configuration*/
  0xA0,         /*bmAttributes: bus powered and Support Remote Wake-up */
  0xFA,         /*MaxPower 100 mA: this current is used for detecting Vbus*/
  
  /************** Descriptor of Joystick Mouse interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x02,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: HID*/
  0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of Joystick Mouse HID ********************/
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x00,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  HID_MOUSE_REPORT_DESC_SIZE & 0xff,/*wItemLength: Total length of Report descriptor*/
  HID_MOUSE_REPORT_DESC_SIZE >> 8,
  /******************** Descriptor of Mouse endpoint ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/
  
  HID_IN_EP,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_IN_PACKET, /*wMaxPacketSize: 4 Byte max */
  0x00,
  HID_FS_BINTERVAL,          /*bInterval: Polling Interval (10 ms)*/
  /* 34 */

  // OUT
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

  HID_OUT_EP,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_OUT_PACKET, /*wMaxPacketSize: 4 Byte max */
  0x00,
  0x01,          /*bInterval: Polling Interval (10 ms)*/
  /* 41 */
} ;

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
  0x00,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  HID_MOUSE_REPORT_DESC_SIZE & 0xff,/*wItemLength: Total length of Report descriptor*/
  HID_MOUSE_REPORT_DESC_SIZE >> 8,
};
#endif 


#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */  
__ALIGN_BEGIN static uint8_t HID_MOUSE_ReportDesc[HID_MOUSE_REPORT_DESC_SIZE] __ALIGN_END = {
        0x06,0x00,0xFF,     /* Usage Page (Vendor-Defined 1)                            */ \
        0x09,0x00,          /*   Usage (Undefined)                                      */ \
        0xA1,0x01,          /*   Collection (Application)                               */ \
        0x85,0xFD,          /*     Report ID (253)                                      */ \
        0x06,0x00,0xFF,     /*     Usage Page (Vendor-Defined 1)                        */ \
        0x09,0x01,          /*     Usage (Vendor-Defined 1)                             */ \
        0x09,0x02,          /*     Usage (Vendor-Defined 2)                             */ \
        0x09,0x03,          /*     Usage (Vendor-Defined 3)                             */ \
        0x09,0x04,          /*     Usage (Vendor-Defined 4)                             */ \
        0x09,0x05,          /*     Usage (Vendor-Defined 5)                             */ \
        0x09,0x06,          /*     Usage (Vendor-Defined 6)                             */ \
        0x15,0x00,          /*     Logical Minimum (0)                                  */ \
        0x26,0xFF,0x00,     /*     Logical Maximum (255)                                */ \
        0x75,0x08,          /*     Report Size (8)                                      */ \
        0x95,0x06,          /*     Report Count (6)                                     */ \
        0x81,0x02,          /*     Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)          */ \
        0x85,0xFE,          /*     Report ID (254)                                      */ \
        0x06,0x00,0xFF,     /*     Usage Page (Vendor-Defined 1)                        */ \
        0x09,0x01,          /*     Usage (Vendor-Defined 1)                             */ \
        0x09,0x02,          /*     Usage (Vendor-Defined 2)                             */ \
        0x09,0x03,          /*     Usage (Vendor-Defined 3)                             */ \
        0x09,0x04,          /*     Usage (Vendor-Defined 4)                             */ \
        0x15,0x00,          /*     Logical Minimum (0)                                  */ \
        0x26,0xFF,0x00,     /*     Logical Maximum (255)                                */ \
        0x75,0x08,          /*     Report Size (8)                                      */ \
        0x95,0x04,          /*     Report Count (4)                                     */ \
        0xB1,0x02,          /*     Feature (Data,Var,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)   */ \
        0xC0,               /*   End Collection                                         */ \
        \
        0x05,0x01,          /* Usage Page (Generic Desktop)                             */ \
        0x09,0x02,          /*   Usage (Mouse)                                          */ \
        0xA1,0x01,          /*   Collection (Application)                               */ \
        0x09,0x01,          /*     Usage (Pointer)                                      */ \
        0xA1,0x00,          /*     Collection (Physical)                                */ \
        0x85,0x01,          /*       Report ID (1)                                      */ \
        0x05,0x09,          /*       Usage Page (Button)                                */ \
        0x19,0x01,          /*       Usage Minimum (Button 1)                           */ \
        0x29,0x03,          /*       Usage Maximum (Button 3)                           */ \
        0x15,0x00,          /*       Logical Minimum (0)                                */ \
        0x25,0x01,          /*       Logical Maximum (1)                                */ \
        0x95,0x03,          /*       Report Count (3)                                   */ \
        0x75,0x01,          /*       Report Size (1)                                    */ \
        0x81,0x02,          /*       Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)        */ \
        0x95,0x01,          /*       Report Count (1)                                   */ \
        0x75,0x05,          /*       Report Size (5)                                    */ \
        0x81,0x03,          /*       Input (Cnst,Var,Abs,NWrp,Lin,Pref,NNul,Bit)        */ \
        0x05,0x01,          /*       Usage Page (Generic Desktop)                       */ \
        0x09,0x30,          /*       Usage (X)                                          */ \
        0x09,0x31,          /*       Usage (Y)                                          */ \
        0x15,0x00,          /*       Logical Minimum (0)                                */ \
        0x26,0xFF,0x7F,     /*       Logical Maximum (32767)                            */ \
        0x35,0x00,          /*       Physical Minimum (0)                               */ \
        0x46,0xFF,0x7F,     /*       Physical Maximum (32767)                           */ \
        0x75,0x10,          /*       Report Size (16)                                   */ \
        0x95,0x02,          /*       Report Count (2)                                   */ \
        0x81,0x02,          /*       Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)        */ \
        0x05,0x0D,          /*       Usage Page (Digitizer)                             */ \
        0x09,0x33,          /*       Usage (Touch)                                      */ \
        0x15,0x00,          /*       Logical Minimum (0)                                */ \
        0x26,0xFF,0x00,     /*       Logical Maximum (255)                              */ \
        0x35,0x00,          /*       Physical Minimum (0)                               */ \
        0x46,0xFF,0x00,     /*       Physical Maximum (255)                             */ \
        0x75,0x08,          /*       Report Size (8)                                    */ \
        0x95,0x01,          /*       Report Count (1)                                   */ \
        0x81,0x02,          /*       Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)        */ \
        0x05,0x01,          /*       Usage Page (Generic Desktop)                       */ \
        0x09,0x38,          /*       Usage (Wheel)                                      */ \
        0x15,0x81,          /*       Logical Minimum (-127)                             */ \
        0x25,0x7F,          /*       Logical Maximum (127)                              */ \
        0x35,0x81,          /*       Physical Minimum (-127)                            */ \
        0x45,0x7F,          /*       Physical Maximum (127)                             */ \
        0x95,0x01,          /*       Report Count (1)                                   */ \
        0x81,0x06,          /*       Input (Data,Var,Rel,NWrp,Lin,Pref,NNul,Bit)        */ \
        0xC0,               /*     End Collection                                       */ \
        0xC0,               /* End Collection                                           */ \
        \
        0x06,0x00,0xFF,     /* Usage Page (Vendor-Defined 1)                            */ \
        0x09,0x00,          /*   Usage (Undefined)                                      */ \
        0xA1,0x01,          /*   Collection (Application)                               */ \
        0x85,0xFC,          /*      Report ID (252)                                     */ \
        0x15,0x00,          /*      Logical Minimum (0)                                 */ \
        0x26,0xFF,0x00,     /*      Logical Maximum (255)                               */ \
        0x19,0x01,          /*      Usage Minimum (Vendor-Defined 1)                    */ \
        0x29,0x3F,          /*      Usage Maximum (Vendor-Defined 63)                   */ \
        0x75,0x08,          /*      Report Size (8)                                     */ \
        0x95,0x3F,          /*      Report Count (63)                                   */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x19,0x01,          /*      Usage Minimum (Vendor-Defined 1)                    */ \
        0x29,0x3F,          /*      Usage Maximum (Vendor-Defined 63)                   */ \
        0x91,0x02,          /*      Output (Data,Var,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)   */ \
        0xC0,               /*  End Collection                                          */ \
        \
        0x06,0x00,0xFF,     /* Usage Page (Vendor-Defined 1)                            */ \
        0x09,0x00,          /*   Usage (Undefined)                                      */ \
        0xA1,0x01,          /*   Collection (Application)                               */ \
        0x85,0xFB,          /*      Report ID (251)                                     */ \
        0x15,0x00,          /*      Logical Minimum (0)                                 */ \
        0x26,0xFF,0x00,     /*      Logical Maximum (255)                               */ \
        0x19,0x01,          /*      Usage Minimum (Vendor-Defined 1)                    */ \
        0x29,0x3F,          /*      Usage Maximum (Vendor-Defined 63)                   */ \
        0x75,0x08,          /*      Report Size (8)                                     */ \
        0x95,0x3F,          /*      Report Count (63)                                   */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x19,0x01,          /*      Usage Minimum (Vendor-Defined 1)                    */ \
        0x29,0x3F,          /*      Usage Maximum (Vendor-Defined 63)                   */ \
        0x91,0x02,          /*      Output (Data,Var,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)   */ \
        0xC0,               /* End Collection                                           */ \
        \
        0x05,0x0D,          /* Usage Page (Digitizer)                                   */ \
        0x09,0x04,          /*   Usage (Touch Screen)                                   */ \
        0xA1,0x01,          /*   Collection (Application)                               */ \
        0x85,0x02,          /*   Report ID (2)                                          */ \
        0x09,0x22,          /*      Usage (Finger)                                      */ \
        0xA1,0x02,          /*      Collection (Logical)                                */ \
        0x09,0x42,          /*      Usage (Tip Switch)                                  */ \
        0x15,0x00,          /*      Logical Minimum (0)                                 */ \
        0x25,0x01,          /*      Logical Maximum (1)                                 */ \
        0x75,0x01,          /*      Report Size (1)                                     */ \
        0x95,0x01,          /*      Report Count (1)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x32,          /*      Usage (In Range)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x47,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x95,0x05,          /*      Report Count (5)                                    */ \
        0x81,0x03,          /*      Input (Cnst,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x75,0x08,          /*      Report Size (8)                                     */ \
        0x09,0x51,          /*      Usage                                               */ \
        0x95,0x01,          /*      Report Count (1)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x05,0x01,          /*      Usage Page (Generic Desktop)                        */ \
        0x15,0x00,          /*      Logical Minimum (0)                                 */ \
        0x26,0xFF,0x7F,     /*      Logical Maximum (32767)                             */ \
        0x75,0x10,          /*      Report Size (16)                                    */ \
        0x55,0x0E,          /*      Unit Exponent (-2)                                  */ \
        0x65,0x11,          /*      Unit (SI Lin: Length (cm))                          */ \
        0x09,0x30,          /*      Usage (X)                                           */ \
        0x35,0x00,          /*      Physical Minimum (0)                                */ \
        0x46,0xF5,0x37,     /*      Physical Maximum (14325)                            */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x46,0x8B,0x1F,     /*      Physical Maximum (8075)                             */ \
        0x09,0x31,          /*      Usage (Y)                                           */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x05,0x0D,          /*      Usage Page (Digitizer)                              */ \
        0x09,0x48,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x49,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0xC0,               /*   End Collection                                         */ \
        0xA1,0x02,          /*   Collection (Logical)                                   */ \
        0x05,0x0D,          /*      Usage Page (Digitizer)                              */ \
        0x09,0x42,          /*      Usage (Tip Switch)                                  */ \
        0x15,0x00,          /*      Logical Minimum (0)                                 */ \
        0x25,0x01,          /*      Logical Maximum (1)                                 */ \
        0x75,0x01,          /*      Report Size (1)                                     */ \
        0x95,0x01,          /*      Report Count (1)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x32,          /*      Usage (In Range)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x47,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x95,0x05,          /*      Report Count (5)                                    */ \
        0x81,0x03,          /*      Input (Cnst,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x75,0x08,          /*      Report Size (8)                                     */ \
        0x09,0x51,          /*      Usage                                               */ \
        0x95,0x01,          /*      Report Count (1)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x05,0x01,          /*      Usage Page (Generic Desktop)                        */ \
        0x15,0x00,          /*      Logical Minimum (0)                                 */ \
        0x26,0xFF,0x7F,     /*      Logical Maximum (32767)                             */ \
        0x75,0x10,          /*      Report Size (16)                                    */ \
        0x55,0x0E,          /*      Unit Exponent (-2)                                  */ \
        0x65,0x11,          /*      Unit (SI Lin: Length (cm))                          */ \
        0x09,0x30,          /*      Usage (X)                                           */ \
        0x35,0x00,          /*      Physical Minimum (0)                                */ \
        0x46,0xF5,0x37,     /*      Physical Maximum (14325)                            */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x46,0x8B,0x1F,     /*      Physical Maximum (8075)                             */ \
        0x09,0x31,          /*      Usage (Y)                                           */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x05,0x0D,          /*      Usage Page (Digitizer)                              */ \
        0x09,0x48,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x49,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0xC0,               /*    End Collection                                        */ \
        0xA1,0x02,          /*    Collection (Logical)                                  */ \
        0x05,0x0D,          /*      Usage Page (Digitizer)                              */ \
        0x09,0x42,          /*      Usage (Tip Switch)                                  */ \
        0x15,0x00,          /*      Logical Minimum (0)                                 */ \
        0x25,0x01,          /*      Logical Maximum (1)                                 */ \
        0x75,0x01,          /*      Report Size (1)                                     */ \
        0x95,0x01,          /*      Report Count (1)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x32,          /*      Usage (In Range)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x47,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x95,0x05,          /*      Report Count (5)                                    */ \
        0x81,0x03,          /*      Input (Cnst,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x75,0x08,          /*      Report Size (8)                                     */ \
        0x09,0x51,          /*      Usage                                               */ \
        0x95,0x01,          /*      Report Count (1)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x05,0x01,          /*      Usage Page (Generic Desktop)                        */ \
        0x15,0x00,          /*      Logical Minimum (0)                                 */ \
        0x26,0xFF,0x7F,     /*      Logical Maximum (32767)                             */ \
        0x75,0x10,          /*      Report Size (16)                                    */ \
        0x55,0x0E,          /*      Unit Exponent (-2)                                  */ \
        0x65,0x11,          /*      Unit (SI Lin: Length (cm))                          */ \
        0x09,0x30,          /*      Usage (X)                                           */ \
        0x35,0x00,          /*      Physical Minimum (0)                                */ \
        0x46,0xF5,0x37,     /*      Physical Maximum (14325)                            */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x46,0x8B,0x1F,     /*      Physical Maximum (8075)                             */ \
        0x09,0x31,          /*      Usage (Y)                                           */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x05,0x0D,          /*      Usage Page (Digitizer)                              */ \
        0x09,0x48,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x49,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0xC0,               /*    End Collection                                        */ \
        0xA1,0x02,          /*    Collection (Logical)                                  */ \
        0x05,0x0D,          /*      Usage Page (Digitizer)                              */ \
        0x09,0x42,          /*      Usage (Tip Switch)                                  */ \
        0x15,0x00,          /*      Logical Minimum (0)                                 */ \
        0x25,0x01,          /*      Logical Maximum (1)                                 */ \
        0x75,0x01,          /*      Report Size (1)                                     */ \
        0x95,0x01,          /*      Report Count (1)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x32,          /*      Usage (In Range)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x47,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x95,0x05,          /*      Report Count (5)                                    */ \
        0x81,0x03,          /*      Input (Cnst,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x75,0x08,          /*      Report Size (8)                                     */ \
        0x09,0x51,          /*      Usage                                               */ \
        0x95,0x01,          /*      Report Count (1)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x05,0x01,          /*      Usage Page (Generic Desktop)                        */ \
        0x15,0x00,          /*      Logical Minimum (0)                                 */ \
        0x26,0xFF,0x7F,     /*      Logical Maximum (32767)                             */ \
        0x75,0x10,          /*      Report Size (16)                                    */ \
        0x55,0x0E,          /*      Unit Exponent (-2)                                  */ \
        0x65,0x11,          /*      Unit (SI Lin: Length (cm))                          */ \
        0x09,0x30,          /*      Usage (X)                                           */ \
        0x35,0x00,          /*      Physical Minimum (0)                                */ \
        0x46,0xF5,0x37,     /*      Physical Maximum (14325)                            */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x46,0x8B,0x1F,     /*      Physical Maximum (8075)                             */ \
        0x09,0x31,          /*      Usage (Y)                                           */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x05,0x0D,          /*      Usage Page (Digitizer)                              */ \
        0x09,0x48,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x49,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0xC0,               /*   End Collection                                         */ \
        0xA1,0x02,          /*   Collection (Logical)                                   */ \
        0x05,0x0D,          /*      Usage Page (Digitizer)                              */ \
        0x09,0x42,          /*      Usage (Tip Switch)                                  */ \
        0x15,0x00,          /*      Logical Minimum (0)                                 */ \
        0x25,0x01,          /*      Logical Maximum (1)                                 */ \
        0x75,0x01,          /*      Report Size (1)                                     */ \
        0x95,0x01,          /*      Report Count (1)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x32,          /*      Usage (In Range)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x47,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x95,0x05,          /*      Report Count (5)                                    */ \
        0x81,0x03,          /*      Input (Cnst,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x75,0x08,          /*      Report Size (8)                                     */ \
        0x09,0x51,          /*      Usage                                               */ \
        0x95,0x01,          /*      Report Count (1)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x05,0x01,          /*      Usage Page (Generic Desktop)                        */ \
        0x15,0x00,          /*      Logical Minimum (0)                                 */ \
        0x26,0xFF,0x7F,     /*      Logical Maximum (32767)                             */ \
        0x75,0x10,          /*      Report Size (16)                                    */ \
        0x55,0x0E,          /*      Unit Exponent (-2)                                  */ \
        0x65,0x11,          /*      Unit (SI Lin: Length (cm))                          */ \
        0x09,0x30,          /*      Usage (X)                                           */ \
        0x35,0x00,          /*      Physical Minimum (0)                                */ \
        0x46,0xF5,0x37,     /*      Physical Maximum (14325)                            */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x46,0x8B,0x1F,     /*      Physical Maximum (8075)                             */ \
        0x09,0x31,          /*      Usage (Y)                                           */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x05,0x0D,          /*      Usage Page (Digitizer)                              */ \
        0x09,0x48,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x49,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0xC0,               /*   End Collection                                         */ \
        0xA1,0x02,          /*   Collection (Logical)                                   */ \
        0x05,0x0D,          /*      Usage Page (Digitizer)                              */ \
        0x09,0x42,          /*      Usage (Tip Switch)                                  */ \
        0x15,0x00,          /*      Logical Minimum (0)                                 */ \
        0x25,0x01,          /*      Logical Maximum (1)                                 */ \
        0x75,0x01,          /*      Report Size (1)                                     */ \
        0x95,0x01,          /*      Report Count (1)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x32,          /*      Usage (In Range)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x47,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x95,0x05,          /*      Report Count (5)                                    */ \
        0x81,0x03,          /*      Input (Cnst,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x75,0x08,          /*      Report Size (8)                                     */ \
        0x09,0x51,          /*      Usage                                               */ \
        0x95,0x01,          /*      Report Count (1)                                    */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x05,0x01,          /*      Usage Page (Generic Desktop)                        */ \
        0x15,0x00,          /*      Logical Minimum (0)                                 */ \
        0x26,0xFF,0x7F,     /*      Logical Maximum (32767)                             */ \
        0x75,0x10,          /*      Report Size (16)                                    */ \
        0x55,0x0E,          /*      Unit Exponent (-2)                                  */ \
        0x65,0x11,          /*      Unit (SI Lin: Length (cm))                          */ \
        0x09,0x30,          /*      Usage (X)                                           */ \
        0x35,0x00,          /*      Physical Minimum (0)                                */ \
        0x46,0xF5,0x37,     /*      Physical Maximum (14325)                            */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x46,0x8B,0x1F,     /*      Physical Maximum (8075)                             */ \
        0x09,0x31,          /*      Usage (Y)                                           */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x05,0x0D,          /*      Usage Page (Digitizer)                              */ \
        0x09,0x48,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x09,0x49,          /*      Usage                                               */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0xC0,               /*   End Collection                                         */ \
        \
        0x05,0x0D,          /*   Usage Page (Digitizer)                                 */ \
        0x09,0x54,          /*      Usage                                               */ \
        0x15,0x00,          /*      Logical Minimum (0)                                 */ \
        0x26,0xFF,0x00,     /*      Logical Maximum (255)                               */ \
        0x95,0x01,          /*      Report Count (1)                                    */ \
        0x75,0x08,          /*      Report Size (8)                                     */ \
        0x81,0x02,          /*      Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)         */ \
        0x85,0x03,          /*      Report ID (3)                                       */ \
        0x09,0x55,          /*      Usage                                               */ \
        0x15,0x00,          /*      Logical Minimum (0)                                 */ \
        0x25,0x80,          /*      Logical Maximum (-128)                              */ \
        0xB1,0x02,          /*      Feature (Data,Var,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)  */ \
        0xC0,               /*   End Collection                                         */ \
        \
        0x09,0x0E,          /*  Usage (Undefined)                                       */ \
        0xA1,0x01,          /*  Collection (Application)                                */ \
        0x85,0x04,          /*    Report ID (4)                                         */ \
        0x09,0x23,          /*    Usage (Undefined)                                     */ \
        0xA1,0x02,          /*    Collection (Logical)                                  */ \
        0x09,0x52,          /*        Usage                                             */ \
        0x09,0x53,          /*        Usage                                             */ \
        0x15,0x00,          /*        Logical Minimum (0)                               */ \
        0x25,0x0A,          /*        Logical Maximum (10)                              */ \
        0x75,0x08,          /*        Report Size (8)                                   */ \
        0x95,0x02,          /*        Report Count (2)                                  */ \
        0xB1,0x02,          /*        Feature (Data,Var,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)*/ \
        0xC0,               /*    End Collection                                        */ \
        0xC0,               /*  End Collection                                          */

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
uint8_t  USBD_HID_Init (void  *pdev, 
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

  /* Prepare Out endpoint to receive next packet */
  DCD_EP_PrepareRx(pdev,
                   HID_OUT_EP,
                   (uint8_t*)(USB_Hid_Rx_Buffer),
                   HID_OUT_PACKET);

  return USBD_OK;
}

/**
  * @brief  USBD_HID_Init
  *         DeInitialize the HID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
uint8_t  USBD_HID_DeInit (void  *pdev, 
                                 uint8_t cfgidx)
{
  /* Close HID EPs */
  DCD_EP_Close (pdev , HID_IN_EP);  
  DCD_EP_Close (pdev , HID_OUT_EP);
  return USBD_OK;
}

/**
  * @brief  USBD_HID_Setup
  *         Handle the HID specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
uint8_t  USBD_HID_Setup (void  *pdev, 
                         USB_SETUP_REQ *req)
{
  uint16_t len = 0;
  uint8_t  *pbuf = NULL;
  
  debugout(":Hid_Set");
  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :  
      debugout(":CLS=%x",req->bRequest);
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
    case HID_REQ_SET_REPORT:
        IsReportAvailable = 1;
        USBD_HID_Report_ID = (uint8_t)(req->wValue);
        USBD_HID_Report_LENGTH = (uint8_t)(req->wLength);
        USBD_CtlPrepareRx(pdev,
                (uint8_t *)&USB_Hid_Rx_Buffer,
                USBD_HID_Report_LENGTH);
        debugout(":SREP=%x,%x,%x",req->wValue,req->wIndex,req->wLength);
        break;

    case HID_REQ_GET_REPORT:
        USB_Hid_Rx_Buffer[0] = 0x03;
        USB_Hid_Rx_Buffer[1] = 0x0a;
        USBD_CtlSendData (pdev,
                          (uint8_t *)&USB_Hid_Rx_Buffer,
                          req->wLength);
        debugout(":GREP=%x,%x,%x",req->wValue,req->wIndex,req->wLength);
        break;

    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL; 
    }
    break;
    
  case USB_REQ_TYPE_STANDARD:
      debugout(":STD=%x",req->bRequest);
    switch (req->bRequest)
    {
    case USB_REQ_GET_DESCRIPTOR: 
        debugout(":%x",req->wValue >> 8);
      if( req->wValue >> 8 == HID_REPORT_DESC)
      {
        len = MIN(HID_MOUSE_REPORT_DESC_SIZE , req->wLength);
        pbuf = HID_MOUSE_ReportDesc;
      }
      else if( req->wValue >> 8 == HID_DESCRIPTOR_TYPE)
      {
        
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
        pbuf = USBD_HID_Desc;   
#else
        pbuf = USBD_HID_CfgDesc + 0x12;
#endif 
        len = MIN(USB_HID_DESC_SIZ , req->wLength);
      }
      else
      {
        /* Do Nothing */
      }
      
      USBD_CtlSendData (pdev, 
                        pbuf,
                        len);
      
      break;
      
    case USB_REQ_GET_INTERFACE :
        debugout(":GIF=%x",req->bRequest);
      USBD_CtlSendData (pdev,
                        (uint8_t *)&USBD_HID_AltSet,
                        1);
      break;
      
    case USB_REQ_SET_INTERFACE :
        debugout(":SIF=%x",req->bRequest);
      USBD_HID_AltSet = (uint8_t)(req->wValue);
      break;
      
    default:
      USBD_HID_AltSet = (uint8_t)(req->wValue);
      break; 
    }
    break;
    
  default:
    USBD_CtlSendData (pdev,
                      (uint8_t *)&USBD_HID_AltSet,
                      1);
    break; 
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
  if (pdev->dev.device_status == USB_OTG_CONFIGURED )
  {
    DCD_EP_Tx (pdev, HID_IN_EP, report, len);
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
  *length = sizeof (USBD_HID_CfgDesc);
  return USBD_HID_CfgDesc;
}

/**
  * @brief  USBD_HID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
uint8_t  USBD_HID_DataIn (void  *pdev, 
                              uint8_t epnum)
{
    printf("USBD_HID_DataIn 000 \n");
    uint16_t USB_Tx_ptr;
    uint16_t USB_Tx_length;

    if (HID_Tx_State == USB_HID_BUSY)
    {
      if(HID_report_size == 0)
      {
        HID_Tx_State = USB_HID_ZLP;
      }
      else if (HID_Rx_length == 0)
      {
          HID_Tx_State = USB_HID_IDLE;
          HID_report_size = 0;
      }
      else
      {
        if (HID_Rx_length > HID_report_size){
          USB_Tx_ptr = HID_Rx_ptr_out;
          USB_Tx_length = HID_report_size;

          HID_Rx_ptr_out += HID_report_size;
          HID_Rx_length -= HID_report_size;
        }
        else
        {
          USB_Tx_ptr = HID_Rx_ptr_out;
          USB_Tx_length = HID_Rx_length;

          HID_Rx_ptr_out += HID_Rx_length;
          HID_Rx_length = 0;
          if ( (USB_Tx_length == HID_report_size)
             &&(USB_Tx_length == HID_IN_PACKET) )
          {
              HID_Tx_State = USB_HID_ZLP;
          }
        }

        if(HID_Rx_ptr_out >= HID_TXRX_DATA_SIZE) HID_Rx_ptr_out -= HID_TXRX_DATA_SIZE;
        /* Prepare the available data buffer to be sent on IN endpoint */
        DCD_EP_Tx (pdev,
                   HID_IN_EP,
                   (uint8_t*)&hid_report_buffer[USB_Tx_ptr],
                   USB_Tx_length);

        /* other function deal with */
        //APP_FOPS.pIf_DataTx();

        return USBD_OK;
      }
    }

    /* Avoid any asynchronous transfer during ZLP */
    if (HID_Tx_State == USB_HID_ZLP)
    {
      /*Send ZLP to indicate the end of the current transfer */
      DCD_EP_Tx (pdev,
                 HID_IN_EP,
                 NULL,
                 0);

      HID_Tx_State = USB_HID_IDLE;
      HID_report_size = 0;
    }

    /* Ensure that the FIFO is empty before a new transfer, this condition could
    be caused by  a new transfer before the end of the previous transfer */
//    DCD_EP_Flush(pdev, HID_IN_EP);
    return USBD_OK;
}

uint8_t  USBD_HID_DataOut (void *pdev, uint8_t epnum)
{
    uint16_t USB_Rx_Cnt;
    printf("USBD_HID_DataOut 111 \n");
    /* Get the received data buffer and update the counter */
    USB_Rx_Cnt = ((USB_OTG_CORE_HANDLE*)pdev)->dev.out_ep[epnum].xfer_count;


    // receive data && deal with other function
    dbg_dump(USB_Hid_Rx_Buffer,USB_Rx_Cnt);

    /* Prepare Out endpoint to receive next packet */
    DCD_EP_PrepareRx(pdev,
                   HID_OUT_EP,
                   (uint8_t*)(USB_Hid_Rx_Buffer),
                   HID_OUT_PACKET);
  return USBD_OK;
}

/**
  * @brief  USBD_HID_EP0_RxReady
  *         Handles control request data.
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */

uint8_t USBD_HID_EP0_RxReady(void *pdev)
{
    printf("USBD_HID_DataOut 4444 \n");
  if (IsReportAvailable == 1)
  {
    IsReportAvailable = 0;
    debugout("HID_EP0_RX,");
    dbg_dump(USB_Hid_Rx_Buffer,USBD_HID_Report_LENGTH);
  }
  return USBD_OK;
}

/**
  * @brief  USBD_HID_GetPollingInterval 
  *         return polling interval from endpoint descriptor
  * @param  pdev: device instance
  * @retval polling interval
  */
uint32_t USBD_HID_GetPollingInterval (USB_OTG_CORE_HANDLE *pdev)
{
  uint32_t polling_interval = 0;
  printf("USBD_HID_DataOut 555 \n");

  /* HIGH-speed endpoints */
  if(pdev->cfg.speed == USB_OTG_SPEED_HIGH)
  {
   /* Sets the data transfer polling interval for high speed transfers. 
    Values between 1..16 are allowed. Values correspond to interval 
    of 2 ^ (bInterval-1). This option (8 ms, corresponds to HID_HS_BINTERVAL */
    polling_interval = (((1 <<(HID_HS_BINTERVAL - 1)))/8);
  }
  else   /* LOW and FULL-speed endpoints */
  {
    /* Sets the data transfer polling interval for low and full 
    speed transfers */
    polling_interval =  HID_FS_BINTERVAL;
  }
  
  return ((uint32_t)(polling_interval));
}

/**
  * @}
  */ 
int usbd_hid_trigger(void *pdev,uint32_t len)
{
    printf("USBD_HID_DataOut 666 \n");
    uint16_t USB_Tx_ptr;
    uint16_t USB_Tx_length;

    if(HID_Tx_State == USB_HID_IDLE)
    {
      HID_Rx_ptr_out = 0;
      HID_Rx_ptr_in = len;
      HID_Rx_length = len;

      HID_report_id = hid_report_buffer[0];
      HID_report_size = 0;
      switch(HID_report_id)
      {
      case 1:   // mouse id+btn+x+y+touch+wheel
          HID_report_size = 8;
          break;
      case 2:   // touch
          HID_report_size = 62;
          break;
      default:
          return 0;
      }

      if (HID_Rx_length > HID_report_size)
      {
        USB_Tx_ptr = HID_Rx_ptr_out;
        USB_Tx_length = HID_report_size;

        HID_Rx_ptr_out += HID_report_size;
        HID_Rx_length  -= HID_report_size;
        HID_Tx_State = USB_HID_BUSY;
      }
      else
      {
        USB_Tx_ptr = HID_Rx_ptr_out;
        USB_Tx_length = HID_Rx_length;

        HID_Rx_ptr_out += HID_Rx_length;
        HID_Rx_length = 0;
        if ( (USB_Tx_length == HID_report_size)
           &&(USB_Tx_length == HID_IN_PACKET) )
        {
            HID_Tx_State = USB_HID_ZLP;
        }
        else
        {
            HID_Tx_State = USB_HID_BUSY;
        }
      }
      // will be send
      DCD_EP_Tx (pdev,
                 HID_IN_EP,
                 (uint8_t*)&hid_report_buffer[USB_Tx_ptr],
                 USB_Tx_length);
      return 1;
    }
    return 0;
}

/**
  * @}
  */ 


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
