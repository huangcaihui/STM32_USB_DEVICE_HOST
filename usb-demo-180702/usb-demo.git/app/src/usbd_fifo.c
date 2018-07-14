//#include "includes.h"
#include "usbd_fifo.h"

USBD_FIFO_type USBD_HID_TxFIFO_EP1 = {0, 0, 0,};
USBD_FIFO_type USBD_HID_TxFIFO_EP2 = {0, 0, 0,};
USBD_FIFO_type USBD_HID_TxFIFO_EP3 = {0, 0, 0,};

void USBD_FIFO_Push(USBD_FIFO_type *p, u8 *report, u16 *len)
{
    int sr;
    if (p->nr < USBD_FIFO_SIZE) {
        p->dat[p->wr][0] = *len;
        memcpy(p->dat[p->wr]+1, report, *len);
        //ENTER_CRITICAL();
        p->nr++, p->wr++;
        if (p->wr > USBD_FIFO_SIZE-1)
            p->wr = 0;
        //EXIT_CRITICAL();
    }    
}

void USBD_FIFO_Pop(USBD_FIFO_type *p, u8 *report, u16 *len)
{
    int sr;
    if (p->nr > 0) {
        *len = p->dat[p->rd][0];
        memcpy(report, p->dat[p->rd]+1, *len);
        //ENTER_CRITICAL();
        p->nr--, p->rd++;
        if (p->rd > USBD_FIFO_SIZE-1)
            p->rd = 0;
        //EXIT_CRITICAL();
    }
}

void USBD_FIFO_Flush(USBD_FIFO_type *p)
{
    p->nr = p->rd = p->wr = 0;
}
