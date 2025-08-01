/***********************************************************************************************************************
 * File Name    : r_usb_phid_descriptor.c
 * Description  : Contains macros, data structure of USB PHID
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#include "r_usb_basic_api.h"
#include "r_usb_phid_api.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_phid_ep
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 Macro definitions
 **********************************************************************************************************************/

#define USB_BCDNUM               (0x0200U)                               /* bcdUSB */
#define USB_RELEASE              (0x0200U)                               /* Release Number */
#define USB_CONFIGNUM            (1U)                                    /* Configuration number */
#define USB_DCPMAXP              (64U)                                   /* DCP max packet size */

#define USB_IFPROTOCOL           (USB_IFPRO_KBD)
#define ITEM_LEN                 (76U)
#define ITEM_LEN2                (50U)
#define MXPS                     (8U)
#define NUM_EP                   (1U)

/* [Vendor ID & Product ID setting] */
#define USB_VENDORID             (0x0000U)
#define USB_PRODUCTID            (0x0013U)

/* Descriptor length */
#define NUM_STRING_DESCRIPTOR    (6U)
/* Configuration Descriptor Length */
#define CD_LEN                   (59U)

/* Descriptor data Mask */
#define USB_UCHAR_MAX            (0xffU)
#define USB_W_TOTAL_LENGTH_MASK  (256U)

/*******************************************************************************************************************//**
 Device Descriptor
 **********************************************************************************************************************/
uint8_t g_apl_device[] =
{
    USB_DD_BLENGTH,                                                      /*  0:bLength */
    USB_DT_DEVICE,                                                       /*  1:bDescriptorType */
    (uint8_t) ( USB_BCDNUM & (uint8_t) USB_UCHAR_MAX),                   /*  2:bcdUSB_lo */
    (uint8_t) ((uint8_t) (USB_BCDNUM >> 8) & (uint8_t) USB_UCHAR_MAX),   /*  3:bcdUSB_hi */
    0x00,                                                                /*  4:bDeviceClass */
    0x00,                                                                /*  5:bDeviceSubClass */
    0x00,                                                                /*  6:bDeviceProtocol */
    (uint8_t) USB_DCPMAXP,                                               /*  7:bMAXPacketSize(for DCP) */
    (uint8_t) (USB_VENDORID & (uint8_t) USB_UCHAR_MAX),                  /*  8:idVendor_lo */
    (uint8_t) ((uint8_t) (USB_VENDORID >> 8) & (uint8_t) USB_UCHAR_MAX), /*  9:idVendor_hi */
    (uint8_t) (USB_PRODUCTID & (uint8_t) USB_UCHAR_MAX),                 /* 10:idProduct_lo */
    (uint8_t) ((uint8_t) (USB_PRODUCTID >> 8) & (uint8_t) USB_UCHAR_MAX),/* 11:idProduct_hi */
    (uint8_t) (USB_RELEASE & (uint8_t) USB_UCHAR_MAX),                   /* 12:bcdDevice_lo */
    (uint8_t) ((uint8_t) (USB_RELEASE >> 8) & (uint8_t) USB_UCHAR_MAX),  /* 13:bcdDevice_hi */
    1,                                                                   /* 14:iManufacturer */
    2,                                                                   /* 15:iProduct */
    3,                                                                   /* 16:iSerialNumber */
    USB_CONFIGNUM,                                                       /* 17:bNumConfigurations */
};

/*******************************************************************************************************************//**
 Configuration Descriptor
 **********************************************************************************************************************/
uint8_t g_apl_configuration[CD_LEN + (CD_LEN % 2)] =
{
    USB_CD_BLENGTH,                                             /*  0:bLength */
    USB_DT_CONFIGURATION,                                       /*  1:bDescriptorType */
    (uint8_t) (CD_LEN % USB_W_TOTAL_LENGTH_MASK),               /*  2:wTotalLength(L) */
    (uint8_t) (CD_LEN / USB_W_TOTAL_LENGTH_MASK),               /*  3:wTotalLength(H) */
    2,                                                          /*  4:bNumInterfaces */
    1,                                                          /*  5:bConfigurationValue */
    4,                                                          /*  6:iConfiguration */
    (uint8_t) (USB_CF_RESERVED | USB_CF_SELFP | USB_CF_RWUPON), /*  7:bmAttributes */
    (uint8_t) (100 / 2),                                        /*  8:bMaxPower (2mA unit) */

    /* Interface Descriptor */
    USB_ID_BLENGTH,                                             /*  0:bLength */
    USB_DT_INTERFACE,                                           /*  1:bDescriptor */
    0,                                                          /*  2:bInterfaceNumber */
    0,                                                          /*  3:bAlternateSetting */
    NUM_EP,                                                     /*  4:bNumEndpoints */
    USB_IFCLS_HID,                                              /*  5:bInterfaceClass(HID) */
    USB_IFSUB_NOBOOT,                                           /*  6:bInterfaceSubClass(NonBOOT) */
    USB_IFPROTOCOL,                                             /*  7:bInterfaceProtocol */
    0,                                                          /*  8:iInterface */

    /* HID Descriptor */
    9,                                                          /*  0:bLength */
    USB_DT_TYPE_HIDDESCRIPTOR,                                  /*  1:bDescriptor */
    0x00,                                                       /*  2:HID Ver */
    0x01,                                                       /*  3:HID Ver */
    0x00,                                                       /*  4:bCountryCode */
    0x01,                                                       /*  5:bNumDescriptors */
    0x22,                                                       /*  6:bDescriptorType */
    ITEM_LEN,                                                   /*  7:wItemLength(L) */
    0x00,                                                       /*  8:wItemLength(H) */

    /* Endpoint Descriptor 0 */
    USB_ED_BLENGTH,                                             /*  0:bLength */
    USB_DT_ENDPOINT,                                            /*  1:bDescriptorType */
    (uint8_t) (USB_EP_IN | USB_EP1),                            /*  2:bEndpointAddress */
    USB_EP_INT,                                                 /*  3:bmAttribute */
    MXPS,                                                       /*  4:wMaxPacketSize_lo */
    0,                                                          /*  5:wMaxPacketSize_hi */
    0x0A,                                                       /*  6:bInterval */

/* Human Interface Device Class2 */

    /* Interface Descriptor */
    USB_ID_BLENGTH,                                             /*  0:bLength */
    USB_DT_INTERFACE,                                           /*  1:bDescriptor */
    1,                                                          /*  2:bInterfaceNumber */
    0,                                                          /*  3:bAlternateSetting */
    NUM_EP,                                                     /*  4:bNumEndpoints */
    USB_IFCLS_HID,                                              /*  5:bInterfaceClass(HID) */
    USB_IFSUB_NOBOOT,                                           /*  6:bInterfaceSubClass(NonBOOT) */
    USB_IFPRO_MSE,                                              /*  7:bInterfaceProtocol */
    0,                                                          /*  8:iInterface */

    /* HID Descriptor */
    9,                                                          /*  0:bLength */
    USB_DT_TYPE_HIDDESCRIPTOR,                                  /*  1:bDescriptor */
    0x00,                                                       /*  2:HID Ver */
    0x01,                                                       /*  3:HID Ver */
    0x00,                                                       /*  4:bCountryCode */
    0x01,                                                       /*  5:bNumDescriptors */
    0x22,                                                       /*  6:bDescriptorType */
    ITEM_LEN2,                                                  /*  7:wItemLength(L) */
    0x00,                                                       /*  8:wItemLength(H) */

    /* Endpoint Descriptor 1 */
    USB_ED_BLENGTH,                                             /*  0:bLength */
    USB_DT_ENDPOINT,                                            /*  1:bDescriptorType */
    (uint8_t) (USB_EP_IN | USB_EP2),                            /*  2:bEndpointAddress */
    USB_EP_INT,                                                 /*  3:bmAttribute */
    MXPS,                                                       /*  4:wMaxPacketSize_lo */
    0,                                                          /*  5:wMaxPacketSize_hi */
    0x0A,
} ;

/*******************************************************************************************************************//**
 String Descriptor 0
 **********************************************************************************************************************/
/* UNICODE 0x0409 English (United States) */
uint8_t g_apl_string0[] =
{
    4,                                                          /*  0:bLength */
    USB_DT_STRING,                                              /*  1:bDescriptorType */
    0x09, 0x04                                                  /*  2:wLANGID[0] */
};

/*******************************************************************************************************************//**
 String Descriptor 1
 **********************************************************************************************************************/
/* 14:iManufacturer */
uint8_t g_apl_string1[] =
{
    16,                                                         /*  0:bLength */
    USB_DT_STRING,                                              /*  1:bDescriptorType */
    'R', 0x00,
    'e', 0x00,
    'n', 0x00,
    'e', 0x00,
    's', 0x00,
    'a', 0x00,
    's', 0x00,
};

/*******************************************************************************************************************//**
 String Descriptor 2
 **********************************************************************************************************************/
/* 15:iProduct */

uint8_t g_apl_string2[] =
{
    32,                 /*  0:bLength */
    USB_DT_STRING,      /*  1:bDescriptorType */
    'U', 0x00,
    'S', 0x00,
    'B', 0x00,
    ' ', 0x00,
    'P', 0x00,
    'e', 0x00,
    'r', 0x00,
    'i', 0x00,
    ' ', 0x00,
    'H', 0x00,
    'I', 0x00,
    'D', 0x00,
    ' ', 0x00,
    'F', 0x00,
    'W', 0x00,
};

/*******************************************************************************************************************//**
 String Descriptor 3
 **********************************************************************************************************************/
/* 16:iSerialNumber */
uint8_t g_apl_string3[] =
{
    10,                 /*  0:bLength */
    USB_DT_STRING,      /*  1:bDescriptorType */
    'P', 0x00,
    '0', 0x00,
    '0', 0x00,
    '1', 0x00,
};

/*******************************************************************************************************************//**
 String Descriptor 4(iConfiguration)
 **********************************************************************************************************************/
uint8_t g_apl_string4[] =
{
    18,                 /*  0:bLength */
    USB_DT_STRING,      /*  1:bDescriptorType */

     /*  2:bString */
    'F', 0x00,
    'S', 0x00,
    ' ', 0x00,
    'K', 0x00,
    'e', 0x00,
    'y', 0x00,
    'B', 0x00,
    'o', 0x00,
    'a', 0x00,
    'r', 0x00,
    'd', 0x00
};

/*******************************************************************************************************************//**
 String Descriptor 5(iConfiguration)
 **********************************************************************************************************************/
uint8_t g_apl_string5[] =
{
    12,                 /*  0:bLength */
    USB_DT_STRING,      /*  1:bDescriptorType */

     /*  2:bString */
    'M', 0x00,
    'o', 0x00,
    'u', 0x00,
    's', 0x00,
    'e', 0x00,
};

/*******************************************************************************************************************//**
 String Descriptor Table address
 **********************************************************************************************************************/
uint8_t * g_apl_string_table[] =
{
    g_apl_string0,                                  /* UNICODE 0x0409 English (United States) */
    g_apl_string1,                                  /* iManufacturer */
    g_apl_string2,                                  /* iProduct */
    g_apl_string3,                                  /* iSerialNumber */
    g_apl_string4,                                  /* iConfiguration for Keyboard */
    g_apl_string5,                                  /* iConfiguration for Mouse */
};

/*******************************************************************************************************************//**
 HID Report Descriptor for Keyboard
 **********************************************************************************************************************/
const uint8_t g_apl_report_keyboard[] =
{
    0x05, 0x01,                                     /* Usage Page (Generic Desktop) */
    0x09, 0x06,                                     /* Usage (Keyboard) */
    0xA1, 0x01,                                     /* Collection (Application) */
    0x05, 0x07,                                     /* Usage Page (Key Codes); */
    0x19, 0xE0,                                     /* Usage Minimum (224) */
    0x29, 0xE7,                                     /* Usage Maximum (231) */
    0x15, 0x00,                                     /* Logical Minimum (0) */
    0x25, 0x01,                                     /* Logical Maximum (1) */
    0x75, 0x01,                                     /* Report Size (1) */
    0x95, 0x08,                                     /* Report Count (8) */
    0x81, 0x02,                                     /* Input (Data , Variable , Absolute)  ;Modifier byte */
    0x95, 0x01,                                     /* Report Count (1) */
    0x75, 0x08,                                     /* Report Size (8) */
    0x81, 0x01,                                     /* Input (Constant) ;Reserved byte */
    0x95, 0x05,                                     /* Report Count (5) */
    0x75, 0x01,                                     /* Report Size (1) */
    0x05, 0x08,                                     /* Usage Page (Page# for LEDs) */
    0x19, 0x01,                                     /* Usage Minimum (1) */
    0x29, 0x05,                                     /* Usage Maximum (5) */
    0x91, 0x02,                                     /* Output (Data , Variable , Absolute)  ;LED report */
    0x95, 0x01,                                     /* Report Count (1) */
    0x75, 0x03,                                     /* Report Size (3) */
    0x91, 0x01,                                     /* Output (Constant) ;LED report padding */
    0x95, 0x06,                                     /* Report Count (6) */
    0x75, 0x08,                                     /* Report Size (8) */
    0x15, 0x00,                                     /* Logical Minimum (0) */
    0x25, 0x65,                                     /* Logical Maximum(101) */
    0x05, 0x07,                                     /* Usage Page (Key Codes) */
    0x19, 0x00,                                     /* Usage Minimum (0) */
    0x29, 0x65,                                     /* Usage Maximum (101) */
    0x81, 0x00,                                     /* Input (Data , Array)  ;Key arrays (6 bytes) */

    /* *** The OUTPUR REPORT *** */
    0x09, 0x00,                                     /* Usage ID within this page (Vendor defined)*/
    0x15, 0x00,                                     /* Logical Min 0 */
    0x26, USB_UCHAR_MAX, 0x00,                      /* Logical Max 255 */
    0x75, 0x08,                                     /* Size 8 Bits (Each Field will be 8bits) */
    0x95, 0x01,                                     /* Count (Number of fields(bytes) in OUTPUT report) */
    0x91, 0x02,                                     /* Output Report - type variable data */
    0xC0,                                           /* End Collection */
};

/*******************************************************************************************************************//**
 HID Report Descriptor for Mouse
 **********************************************************************************************************************/
const uint8_t g_apl_report_mouse[] =
{
    0x05, 0x01,                                     /* Usage Page (Generic Desktop) */
    0x09, 0x02,                                     /* Usage (Mouse) */
    0xA1, 0x01,                                     /* Collection (Application) */
    0x09, 0x01,                                     /* Usage (Pointer) */
    0xA1, 0x00,                                     /* Collection (Physical) */
    0x05, 0x09,                                     /* Usage Page (Buttons) */
    0x19, 0x01,                                     /* Usage Minimum (01) */
    0x29, 0x03,                                     /* Usage Maximun (03) */
    0x15, 0x00,                                     /* Logical Minimum (0) */
    0x25, 0x01,                                     /* Logical Maximum (1) */
    0x95, 0x03,                                     /* Report Count (3) */
    0x75, 0x01,                                     /* Report Size (1) */
    0x81, 0x02,                                     /* Input (Data, Variable, Absolute); 3 button bits */
    0x95, 0x01,                                     /* Report Count (1) */
    0x75, 0x05,                                     /* Report Size (5) */
    0x81, 0x01,                                     /* Input (Constant), ;5 bit padding */
    0x05, 0x01,                                     /* Usage Page (Generic Desktop) */
    0x09, 0x30,                                     /* Usage (X) */
    0x09, 0x31,                                     /* Usage (Y) */
    0x15, 0x81,                                     /* Logical Minimum (-127) */
    0x25, 0x7F,                                     /* Logical Maximum (127) */
    0x75, 0x08,                                     /* Report Size (8) */
    0x95, 0x02,                                     /* Report Count (2) */
    0x81, 0x06,                                     /* Input (Data,Variable,Relative): 2position bytes(X & Y) */
    0xC0,                                           /* End Collection */
    0xC0,                                           /* End Collection */
};

const usb_descriptor_t g_usb_descriptor =
{
    g_apl_device,                   /* Pointer to the device descriptor */
    g_apl_configuration,            /* Pointer to the configuration descriptor for Full-speed */
    NULL,                           /* Pointer to the configuration descriptor for Hi-speed */
    NULL,                           /* Pointer to the qualifier descriptor */
    g_apl_string_table,             /* Pointer to the string descriptor table */
    NUM_STRING_DESCRIPTOR
};

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_phid_phid)
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 End  Of File
 **********************************************************************************************************************/
