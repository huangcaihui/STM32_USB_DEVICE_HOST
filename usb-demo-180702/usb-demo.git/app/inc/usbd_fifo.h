#ifndef __USBD_FIFO_H
#define __USBD_FIFO_H
#include "usb_conf.h"
#define USBD_FIFO_SIZE 20

typedef struct {
    int nr;
    int wr;
    int rd;
    u8 dat[USBD_FIFO_SIZE][65];
} USBD_FIFO_type;

extern USBD_FIFO_type USBD_HID_TxFIFO_EP1;
extern USBD_FIFO_type USBD_HID_TxFIFO_EP2;
extern USBD_FIFO_type USBD_HID_TxFIFO_EP3;

extern void USBD_FIFO_Push(USBD_FIFO_type *p, u8 *report, u16 *len);
extern void USBD_FIFO_Pop(USBD_FIFO_type *p, u8 *report, u16 *len);
extern void USBD_FIFO_Flush(USBD_FIFO_type *p);

__inline int USBD_FIFO_Peek(USBD_FIFO_type *p)
{
    return p->nr;
}

__inline int USBD_FIFO_Capacity(USBD_FIFO_type *p)
{
    return USBD_FIFO_SIZE;
}

__inline int USBD_FIFO_Used(USBD_FIFO_type *p)
{
    return p->nr;
}

__inline int USBD_FIFO_Unused(USBD_FIFO_type *p)
{
    return USBD_FIFO_SIZE - p->nr;
}

#endif
