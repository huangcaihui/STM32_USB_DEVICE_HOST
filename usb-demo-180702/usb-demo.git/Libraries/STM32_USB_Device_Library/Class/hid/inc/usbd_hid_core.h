/**
  ******************************************************************************
  * @file    usbd_hid_core.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   header file for the usbd_hid_core.c file.
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

#ifndef __USB_HID_CORE_H_
#define __USB_HID_CORE_H_

#include  "usbd_ioreq.h"
//#include "irt10_config.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_HID
  * @brief This file is the Header file for USBD_msc.c
  * @{
  */


/** @defgroup USBD_HID_Exported_Defines
  * @{
  */
#if defined(USB_ONE_INTERFACE)
    #define USB_HID_CONFIG_DESC_SIZ         (41)
#elif defined(USB_TWO_INTERFACE)
    #define USB_HID_CONFIG_DESC_SIZ         (41+32)
#else
    #define USB_HID_CONFIG_DESC_SIZ         (41+32+32)
#endif

#define USB_HID_DESC_SIZ                    (9)

#define USB_HID_CONFIG_DESC_SIZ_MAC         (41)

//------------------------------------------ HID报告描述符大小 -----------------------------------
#define SIZE_DESC_USER_050607     35
#define SIZE_DESC_USER_0506       38
#define SIZE_DESC_USER_A1         29
#define SIZE_DESC_USER_A2         29
#define SIZE_DESC_USER_A3         29
#define SIZE_DESC_USER_FB         29
#define SIZE_DESC_USER_FC         29
#define SIZE_DESC_USER_FB_SW         27
#define SIZE_DESC_USER_FC_SW         27
#define SIZE_DESC_USER_FDFE_SW         60
#define SIZE_DESC_USER_CH         38
#define SIZE_DESC_USER_HH         35
#define SIZE_DESC_USER_PM         53
#define SIZE_DESC_KEY             65
#define SIZE_DESC_KEY_SW          67
#define SIZE_DESC_MOUSE           60
#define SIZE_DESC_MOUSE_SW        94
#define SIZE_DESC_TOUCH_TIME      20
#define SIZE_DESC_TOUCH           480
#define SIZE_DESC_TOUCH_EREA_IST1 442
#define SIZE_DESC_TOUCH_EREA_IST2 (442+11*6)
#ifndef TOUCH_SIZE
  #define SIZE_DESC_TOUCH_EREA    SIZE_DESC_TOUCH_EREA_IST1
#else
  #define SIZE_DESC_TOUCH_EREA    SIZE_DESC_TOUCH_EREA_IST2
#endif
#ifndef POINT_OS_EREA
  #define SIZE_DESC_MULTITOUCH    SIZE_DESC_TOUCH
#else
  #define SIZE_DESC_MULTITOUCH    SIZE_DESC_TOUCH_EREA
#endif
#define SIZE_DESC_TOUCH_EREA_HH2  (SIZE_DESC_TOUCH_EREA_IST2+SIZE_DESC_TOUCH_TIME)
#define SIZE_DESC_TOUCH_EREA_SW   516
#define SIZE_DESC_TOUCH_EREA_TST  533
#define SIZE_DESC_TOUCH_EREA_CK   690
#define SIZE_DESC_PEN             86
#define SIZE_DESC_PEN_NEW         133
#define SIZE_DESC_PEN_PM          70


#define TOUCH_SIZE_W        1920
#define TOUCH_SIZE_H        1080
// 基本描述符长度:HID/MOUSE/TOUCH
	
  #if defined(CK)
    #define HID_MAC_REPORT_DESC_SIZE          (SIZE_DESC_USER_050607+SIZE_DESC_MOUSE)
  #elif defined(CHUANGWEI)
    #define HID_MAC_REPORT_DESC_SIZE          (SIZE_DESC_USER_0506+SIZE_DESC_MOUSE)
  #elif defined(SEEWO_HID)
    #define HID_MAC_REPORT_DESC_SIZE          (SIZE_DESC_USER_FDFE_SW+SIZE_DESC_MOUSE_SW)	
  #elif defined(SEEWO)
    #define HID_MAC_REPORT_DESC_SIZE          (SIZE_DESC_USER_050607+SIZE_DESC_USER_FB+SIZE_DESC_USER_FC+SIZE_DESC_USER_A3+SIZE_DESC_USER_CH+SIZE_DESC_KEY+SIZE_DESC_MOUSE)	
  #elif defined(HONGHE)
    #define HID_MAC_REPORT_DESC_SIZE          (SIZE_DESC_USER_050607+SIZE_DESC_USER_A1+SIZE_DESC_USER_A2+SIZE_DESC_USER_A3+SIZE_DESC_KEY+SIZE_DESC_MOUSE)	  
  #else
    #define HID_MAC_REPORT_DESC_SIZE          (SIZE_DESC_USER_050607+SIZE_DESC_USER_A1+SIZE_DESC_USER_A2+SIZE_DESC_USER_A3+SIZE_DESC_USER_CH+SIZE_DESC_KEY+SIZE_DESC_MOUSE)	
  #endif
		

#if defined(CHUANGWEI)
    #define HID_MOUSE_REPORT_DESC_SIZE      (SIZE_DESC_USER_0506+SIZE_DESC_MOUSE+SIZE_DESC_MULTITOUCH)
    #define HID_WIN7_REPORT_DESC_SIZE       (0)
    #define HID_INTF3_REPORT_DESC_SIZE      (0)    

#elif defined(TST)
    #define HID_MOUSE_REPORT_DESC_SIZE      (SIZE_DESC_TOUCH_EREA_TST)
    #define HID_WIN7_REPORT_DESC_SIZE       (SIZE_DESC_USER_0506+SIZE_DESC_MOUSE+SIZE_DESC_USER_A1+SIZE_DESC_USER_A2+SIZE_DESC_USER_A3+SIZE_DESC_KEY)
    #define HID_INTF3_REPORT_DESC_SIZE      (0)

#elif defined(CK)
    #define HID_MOUSE_REPORT_DESC_SIZE      (SIZE_DESC_USER_050607+SIZE_DESC_MOUSE+SIZE_DESC_TOUCH_EREA_CK)
    #define HID_WIN7_REPORT_DESC_SIZE       (0)
    #define HID_INTF3_REPORT_DESC_SIZE      (0)

#elif defined(CHANGHONG)
    #define HID_MOUSE_REPORT_DESC_SIZE      (SIZE_DESC_USER_050607+SIZE_DESC_USER_A1+SIZE_DESC_USER_A2+SIZE_DESC_USER_A3+SIZE_DESC_USER_CH+SIZE_DESC_KEY+SIZE_DESC_MOUSE)
    #define HID_WIN7_REPORT_DESC_SIZE       (SIZE_DESC_MULTITOUCH)
    #define HID_INTF3_REPORT_DESC_SIZE      (0)
        
#elif defined(PM)
    #define HID_MOUSE_REPORT_DESC_SIZE      (SIZE_DESC_USER_PM)
    #define HID_WIN7_REPORT_DESC_SIZE       (SIZE_DESC_USER_A3+SIZE_DESC_MULTITOUCH+SIZE_DESC_PEN_PM)
    #define HID_INTF3_REPORT_DESC_SIZE      (SIZE_DESC_MOUSE)

#elif defined(HONGHE)
    #define HID_MOUSE_REPORT_DESC_SIZE      (SIZE_DESC_USER_050607+SIZE_DESC_USER_A1+SIZE_DESC_USER_A2+SIZE_DESC_USER_A3+SIZE_DESC_KEY+SIZE_DESC_MOUSE)
  #ifndef POINT_OS_EREA
    #define HID_WIN7_REPORT_DESC_SIZE       (SIZE_DESC_USER_HH+SIZE_DESC_MULTITOUCH)	
  #else	
    #define HID_WIN7_REPORT_DESC_SIZE       (SIZE_DESC_USER_HH+SIZE_DESC_TOUCH_EREA_HH2+SIZE_DESC_PEN_NEW)
  #endif		
    #define HID_INTF3_REPORT_DESC_SIZE      (0)

#elif defined(SEEWO)
  #ifdef SEEWO_HID
    #define HID_MOUSE_REPORT_DESC_SIZE      (SIZE_DESC_USER_FDFE_SW+SIZE_DESC_MOUSE_SW+SIZE_DESC_USER_FB_SW+SIZE_DESC_USER_FC_SW+SIZE_DESC_TOUCH_EREA_SW+SIZE_DESC_USER_A2+SIZE_DESC_USER_A3)//(SIZE_DESC_USER_050607+SIZE_DESC_MOUSE_SW+SIZE_DESC_USER_FB+SIZE_DESC_USER_FC+SIZE_DESC_TOUCH_EREA_SW+SIZE_DESC_USER_A3)
    #define HID_WIN7_REPORT_DESC_SIZE       (SIZE_DESC_KEY_SW)
    #define HID_INTF3_REPORT_DESC_SIZE      (0)//(SIZE_DESC_USER_A2+SIZE_DESC_USER_A3)
  #else
    #define HID_MOUSE_REPORT_DESC_SIZE      (SIZE_DESC_USER_050607+SIZE_DESC_USER_FB+SIZE_DESC_USER_FC+SIZE_DESC_USER_A3+SIZE_DESC_USER_CH+SIZE_DESC_KEY+SIZE_DESC_MOUSE)
    #define HID_WIN7_REPORT_DESC_SIZE       (SIZE_DESC_MULTITOUCH+SIZE_DESC_PEN_NEW)
    #define HID_INTF3_REPORT_DESC_SIZE      (0)
  #endif

#elif defined(VESTEL)
    #define HID_MOUSE_REPORT_DESC_SIZE      (SIZE_DESC_USER_050607+SIZE_DESC_USER_A1+SIZE_DESC_USER_A2+SIZE_DESC_USER_A3+SIZE_DESC_USER_CH+/*SIZE_DESC_KEY+*/SIZE_DESC_MOUSE)
    #define HID_WIN7_REPORT_DESC_SIZE       (SIZE_DESC_MULTITOUCH/*+SIZE_DESC_PEN_NEW*/)
    #define HID_INTF3_REPORT_DESC_SIZE      (0)     

#else  
  #if defined(USB_ONE_INTERFACE)
    #define HID_MOUSE_REPORT_DESC_SIZE      (SIZE_DESC_USER_050607+SIZE_DESC_USER_A1+SIZE_DESC_USER_A2+SIZE_DESC_USER_A3+SIZE_DESC_USER_CH+SIZE_DESC_KEY+SIZE_DESC_MOUSE+SIZE_DESC_MULTITOUCH)
    #define HID_WIN7_REPORT_DESC_SIZE       (0)
    #define HID_INTF3_REPORT_DESC_SIZE      (0)
  #elif defined(USB_TWO_INTERFACE)
    #define HID_MOUSE_REPORT_DESC_SIZE      (SIZE_DESC_USER_050607+SIZE_DESC_USER_A1+SIZE_DESC_USER_A2+SIZE_DESC_USER_A3+SIZE_DESC_USER_CH+SIZE_DESC_KEY+SIZE_DESC_MOUSE)
    #define HID_WIN7_REPORT_DESC_SIZE       (SIZE_DESC_MULTITOUCH+SIZE_DESC_PEN_NEW)
    #define HID_INTF3_REPORT_DESC_SIZE      (0)     
  #else  
    #define HID_MOUSE_REPORT_DESC_SIZE      (SIZE_DESC_USER_050607+SIZE_DESC_USER_A1+SIZE_DESC_USER_A2+SIZE_DESC_USER_A3+SIZE_DESC_USER_CH+SIZE_DESC_KEY)             
    #define HID_WIN7_REPORT_DESC_SIZE       (SIZE_DESC_MULTITOUCH+SIZE_DESC_PEN_NEW)
    #define HID_INTF3_REPORT_DESC_SIZE      (SIZE_DESC_MOUSE)
  #endif
#endif

#define HID_REPORT_DESC_SIZE            (HID_MOUSE_REPORT_DESC_SIZE+HID_WIN7_REPORT_DESC_SIZE+HID_INTF3_REPORT_DESC_SIZE)


#define HID_MOUSE_REPORT_VALUE_SIZE   64//2

#define HID_DESCRIPTOR_TYPE           0x21
#define HID_REPORT_DESC               0x22


#define HID_REQ_SET_PROTOCOL          0x0B
#define HID_REQ_GET_PROTOCOL          0x03

#define HID_REQ_SET_IDLE              0x0A
#define HID_REQ_GET_IDLE              0x02

#define HID_REQ_SET_REPORT            0x09
#define HID_REQ_GET_REPORT            0x01
/**
  * @}
  */


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */


/**
  * @}
  */



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */

extern USBD_Class_cb_TypeDef  USBD_HID_cb;
extern USBD_Class_cb_TypeDef  USBD_HID_cb1;

extern __IO u8 USBD_HID_TxValid_EP1;
extern __IO u8 USBD_HID_TxValid_EP2;
extern __IO u8 USBD_HID_TxValid_EP3;
extern u8 USBD_HID_RcvBuf[64];


extern u8 USB_Buffer_PM[64];
extern u8 Host_OS;


/**
  * @}
  */

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
uint8_t USBD_HID_SendReport (USB_OTG_CORE_HANDLE  *pdev,
                                 uint8_t *report,
                                 uint16_t len);

uint8_t USBD_HID_SendReport_EP2 (USB_OTG_CORE_HANDLE  *pdev,
                                 uint8_t *report,
                                 uint16_t len);
uint8_t USBD_HID_SendReport_EP3 (USB_OTG_CORE_HANDLE  *pdev,
                                 uint8_t *report,
                                 uint16_t len);
void WaitForUSBDFIFO(void);
void USBD_FIFO_FlushAll(void);

/**
  * @}
  */

#endif  // __USB_HID_CORE_H_
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
