#include <usbd_core.h>
#include <usbd_cdc.h>
#include <log.h>

#include "usbd_dfu.h"
//#include "usb_uart.h"

/*!< endpoint address */
/* v-uart1 */
#define CDC_IN_EP1  0x81
#define CDC_OUT_EP1 0x02
#define CDC_INT_EP1 0x83

/* v-uart2 */
#define CDC_IN_EP2  0x82
#define CDC_OUT_EP2 0x03
#define CDC_INT_EP2 0x84

#if 0
/* v-uart 3*/
#define CDC_IN_EP3  0x83
#define CDC_OUT_EP3 0x04
#define CDC_INT_EP3 0x86

/* v-uart 4 */
#define CDC_IN_EP4  0x84
#define CDC_OUT_EP4 0x04
#define CDC_INT_EP4 0x88
#endif

#define USBD_VID           0x000F
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[3][256];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[3][256];


typedef void (*vuart_readcb)(uint8_t uart, char *data, uint32_t len);

typedef struct cdc_endpoints {
    uint8_t uart;
    uint8_t inep;
    uint8_t outep;
    uint8_t intep;
    uint8_t *rdbuf;
    uint8_t *wrbuf;
    vuart_readcb readcb;
    bool tx_busy;
} cdc_endpoints;

cdc_endpoints *get_vuart(uint8_t ep);

void vuart_write(uint8_t uart, char *buf, uint8_t len) {
    volatile cdc_endpoints *cdcep = get_vuart(uart);
    if (cdcep == NULL) {
        LOG_W("Invalid vUART %d\r\n", uart);
        return;
    }
    LOG_I("Sending (%d) %d\r\n", uart, len);
    memcpy(cdcep->wrbuf, buf, len);
    cdcep->tx_busy = true;
    usbd_ep_start_write(cdcep->inep, cdcep->wrbuf, len);
}


void vuart_read(uint8_t uart, char *data, uint32_t len) {
    char buf[256];
    strlcpy(buf, data, MIN(len+1, 256));
    LOG_I("Read (%d): %s - %d %d\r\n", uart, buf, MIN(len, 2047), len);
}


cdc_endpoints vuart[] = {
    {1, CDC_IN_EP1, CDC_OUT_EP1, CDC_INT_EP1, read_buffer[0], write_buffer[0], &vuart_read, false},
    {2, CDC_IN_EP2, CDC_OUT_EP2, CDC_INT_EP2, read_buffer[1], write_buffer[1], &vuart_read, false},
//    {3, CDC_IN_EP3, CDC_OUT_EP3, CDC_INT_EP3, read_buffer[2], write_buffer[2], &vuart_read, false},        
};

cdc_endpoints *get_vuart_outep(uint8_t ep) {
    for (uint8_t i = 0; i < (sizeof(vuart)/sizeof(vuart[0])); i++) {
        if (vuart[i].outep == ep)
            return &vuart[i];
    }
    return NULL;
};

cdc_endpoints *get_vuart_inep(uint8_t ep) {
    for (uint8_t i = 0; i < (sizeof(vuart)/sizeof(vuart[0])); i++) {
        if (vuart[i].inep == ep)
            return &vuart[i];
    }
    return NULL;
};

cdc_endpoints *get_vuart(uint8_t uart) {
    for (uint8_t i = 0; i < (sizeof(vuart)/sizeof(vuart[0])); i++) {
        if (vuart[i].uart == uart)
            return &vuart[i];
    }
    return NULL;
};


/*!< config descriptor size */
/* 3 devices */
#define USB_CONFIG_SIZE (9 + 9 + 9 + (CDC_ACM_DESCRIPTOR_LEN * 1))

/*!< global descriptor */
static const uint8_t cdc_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, USB_DEVICE_CLASS_MISC, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT((0x09 + 0x12 + (CDC_ACM_DESCRIPTOR_LEN * 2)), 0x05, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    DFU_DESCRIPTOR_INIT(),
    CDC_ACM_DESCRIPTOR_INIT(0x01, CDC_INT_EP1, CDC_OUT_EP1, CDC_IN_EP1, 0x02),
    CDC_ACM_DESCRIPTOR_INIT(0x03, CDC_INT_EP2, CDC_OUT_EP2, CDC_IN_EP2, 0x02),

    
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x0E,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'P', 0x00,                  /* wcChar0 */
    'I', 0x00,                  /* wcChar1 */
    'N', 0x00,                  /* wcChar2 */
    'E', 0x00,                  /* wcChar3 */
    '6', 0x00,                  /* wcChar4 */
    '4', 0x00,                  /* wcChar5 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x0A,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'O', 0x00,                  /* wcChar0 */
    'x', 0x00,                  /* wcChar1 */
    '6', 0x00,                  /* wcChar2 */
    '4', 0x00,                  /* wcChar3 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '1', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
    ///////////////////////////////////////
    /// string4 descriptor - 0x58 000 000
    ///////////////////////////////////////
    0x56,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '@', 0x00,                  /* wcChar0 */
    'M', 0x00,                  /* wcChar1 */
    '0', 0x00,                  /* wcChar2 */
    ' ', 0x00,                  /* wcChar3 */
    'F', 0x00,                  /* wcChar4 */
    'l', 0x00,                  /* wcChar5 */
    'a', 0x00,                  /* wcChar6 */
    's', 0x00,                  /* wcChar7 */
    'h', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar17 */
    ' ', 0x00,                  /* wcChar17 */
    ' ', 0x00,                  /* wcChar17 */
    ' ', 0x00,                  /* wcChar17 */
    '/', 0x00,                  /* wcChar18 */
    '0', 0x00,                  /* wcChar19 */
    'x', 0x00,                  /* wcChar20 */
    '5', 0x00,                  /* wcChar21*/
    '8', 0x00,                  /* wcChar22 */
    '0', 0x00,                  /* wcChar23 */
    '0', 0x00,                  /* wcChar24 */
    '0', 0x00,                  /* wcChar25 */
    '0', 0x00,                  /* wcChar26 */
    '0', 0x00,                  /* wcChar27 */
    '0', 0x00,                  /* wcChar28 */
    '/', 0x00,                  /* wcChar29 */
    '9', 0x00,                  /* wcChar30 */
    '9', 0x00,                  /* wcChar31*/
    '*', 0x00,                  /* wcChar32 */
    '0', 0x00,                  /* wcChar33 */
    '0', 0x00,                  /* wcChar34 */
    '1', 0x00,                  /* wcChar35 */
    'K', 0x00,                  /* wcChar36 */
    'g', 0x00,                  /* wcChar37 */
    ',', 0x00,                  /* wcChar38 */
    '1', 0x00,                  /* wcChar39 */
    '1', 0x00,                  /* wcChar40 */
    '2', 0x00,                  /* wcChar41*/
    '*', 0x00,                  /* wcChar42 */
    '0', 0x00,                  /* wcChar43 */
    '1', 0x00,                  /* wcChar44 */
    'K', 0x00,                  /* wcChar45 */
    'g', 0x00,                  /* wcChar46 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    // 0x0a,
    // USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    // 0x00,
    // 0x02,
    // 0x02,
    // 0x02,
    // 0x01,
    // 0x40,
    // 0x01,
    // 0x00,
    0x09, // len
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER, // bDescriptorType
    0x00, // bcdUSB
    0xEF, // bDeviceClass
    0x02, // bDeviceSubClass 
    0x01, // bDeviceProtocol
    0x40, // bMaxPacketSize0
    0x01, // bNumConfigurations
    0x00,
#endif
    0x00
};


#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

void usbd_configure_done_callback(void)
{
    /* setup first out ep read transfer */
    for (uint8_t i = 0; i < (sizeof(vuart)/sizeof(vuart[0])); i++) {
        LOG_I("Configure CDCEP: UART:%d IN:%d OUT:%d INT:%d RDptr:%p Wptr:%p\r\n", vuart[i].uart, vuart[i].inep, vuart[i].outep, vuart[i].intep, vuart[i].rdbuf, vuart[i].wrbuf);
        usbd_ep_start_read(vuart[i].outep, vuart[i].rdbuf, 256);
    }
}

void usbd_cdc_acm_bulk_out(uint8_t ep, uint32_t nbytes)
{
    volatile cdc_endpoints *cdcep = get_vuart_outep(ep);
    if (cdcep == NULL) {
        LOG_W("Invalid CDC Endpoint %d\r\n", ep);
        return;
    }
    LOG_I("actual out len:%d ep: %d\r\n", nbytes, ep);
    cdcep->readcb(cdcep->uart, (char *)cdcep->rdbuf, nbytes);

    /* setup next out ep read transfer */
    usbd_ep_start_read(cdcep->outep, cdcep->rdbuf, 256);
}

void usbd_cdc_acm_bulk_in(uint8_t ep, uint32_t nbytes)
{
    volatile cdc_endpoints *cdcep = get_vuart_inep(ep);
    if (cdcep == NULL) {
        LOG_W("Invalid CDC Endpoint %d\r\n", ep);
        return;
    }

    LOG_I("actual in len:%d ep:%d\r\n", nbytes, ep);

    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(cdcep->inep, NULL, 0);
    } else {
        cdcep->tx_busy = false;
    }
}

/*!< endpoint call back */
struct usbd_endpoint cdc_out_ep1 = {
    .ep_addr = CDC_OUT_EP1,
    .ep_cb = usbd_cdc_acm_bulk_out
};

struct usbd_endpoint cdc_in_ep1 = {
    .ep_addr = CDC_IN_EP1,
    .ep_cb = usbd_cdc_acm_bulk_in
};

struct usbd_endpoint cdc_out_ep2 = {
    .ep_addr = CDC_OUT_EP2,
    .ep_cb = usbd_cdc_acm_bulk_out
};

struct usbd_endpoint cdc_in_ep2 = {
    .ep_addr = CDC_IN_EP2,
    .ep_cb = usbd_cdc_acm_bulk_in
};

#if 0

struct usbd_endpoint cdc_out_ep3 = {
    .ep_addr = CDC_OUT_EP3,
    .ep_cb = usbd_cdc_acm_bulk_out
};

struct usbd_endpoint cdc_in_ep3 = {
    .ep_addr = CDC_IN_EP3,
    .ep_cb = usbd_cdc_acm_bulk_in
};
#endif

struct usbd_interface intf0;
struct usbd_interface intf1;
struct usbd_interface intf2;
struct usbd_interface intf3;
struct usbd_interface intf4;
#if 0
struct usbd_interface intf5;
#endif

/* function ------------------------------------------------------------------*/
void cdc_acm_multi_init(void)
{
    usbd_desc_register(cdc_descriptor);
    /* DFU First */
    usbd_add_interface(usbd_dfu_init_intf(&intf0));

    usbd_add_interface(usbd_cdc_acm_init_intf(&intf1));
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf2));
    usbd_add_endpoint(&cdc_out_ep1);
    usbd_add_endpoint(&cdc_in_ep1);
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf3));
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf4));
    usbd_add_endpoint(&cdc_out_ep2);
    usbd_add_endpoint(&cdc_in_ep2);
#if 0
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf4));
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf5));
    usbd_add_endpoint(&cdc_out_ep3);
    usbd_add_endpoint(&cdc_in_ep3);
#endif
    usbd_initialize();
}

volatile uint8_t dtr_enable = 0;

void usbd_cdc_acm_set_dtr(uint8_t intf, bool dtr)
{
    LOG_I("DTR Change %d %d\r\n", intf, dtr);
    if (dtr) {
        dtr_enable = 1;
    } else {
        dtr_enable = 0;
    }
}

void usbd_cdc_acm_set_rts(uint8_t intf, bool rts) {
    LOG_I("RTS Change %d %d\r\n", intf, rts);
}

void usbd_cdc_acm_set_line_coding(uint8_t intf, struct cdc_line_coding *line_coding) {
    LOG_I("Line Coding Change %d %d %d %d\r\n", intf, line_coding->dwDTERate, line_coding->bCharFormat, line_coding->bParityType, line_coding->bDataBits);
}


uint8_t *dfu_read_flash(uint8_t *src, uint8_t *dest, uint32_t len)
{
    LOG_I("DFU Read %lx %lx %ld\r\n", src, dest, len);
    uint32_t i = 0;
    uint8_t *psrc = src;

    for (i = 0; i < len; i++) {
        dest[i] = *psrc++;
    }
    /* Return a valid address to avoid HardFault */
    return (uint8_t *)(dest);
}

uint16_t dfu_write_flash(uint8_t *src, uint8_t *dest, uint32_t len)
{
    LOG_I("DFU Write %lx %lx %ld\r\n", src, dest, len);
    return 0;
}

uint16_t dfu_erase_flash(uint32_t add)
{
    LOG_I("DFU Erase %lx\r\n", add);
    return 0;
}

void dfu_leave(void)
{
    LOG_I("DFU Leave\r\n");
    dfu_reset();
}
