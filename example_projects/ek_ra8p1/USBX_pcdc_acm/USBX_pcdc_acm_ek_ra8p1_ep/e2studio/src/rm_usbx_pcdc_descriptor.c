/***********************************************************************************************************************
 * File Name    : rm_usbx_pcdc_descriptor.c
 * Description  : Contains macros and functions used in rm_usbx_pcdc_descriptor.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "r_usb_basic.h"

/*******************************************************************************************************************//**
 * Macro definitions
 **********************************************************************************************************************/
/* bcdUSB */
#define USB_BCDNUM                      (0x0200U)
/* Release Number */
#define USB_RELEASE                     (0x0200U)
/* DCP max packet size */
#define USB_DCPMAXP                     (64U)
/* Configuration number */
#define USB_CONFIGNUM                   (1U)
/* Vendor ID */
#define USB_VENDORID                    (0x0000U)
/* Product ID */
#define USB_PRODUCTID                   (0x0002U)

/* Interface Association Descriptor Type */
#define USB_IAD_TYPE                    (0x0B)

/* Communications Class Subclass Codes */
#define USB_PCDC_CLASS_SUBCLASS_CODE_ABS_CTR_MDL                (0x02U)

/* Class-Specific Configuration Descriptors */
#define     USB_PCDC_CS_INTERFACE                               (0x24U)

/* bDescriptor SubType in Communications Class Functional Descriptors */
/* Header Functional Descriptor */
#define     USB_PCDC_DT_SUBTYPE_HEADER_FUNC                     (0x00U)

/* Call Management Functional Descriptor */
#define     USB_PCDC_DT_SUBTYPE_CALL_MANAGE_FUNC                (0x01U)

/* Abstract Control Management Functional Descriptor */
#define     USB_PCDC_DT_SUBTYPE_ABSTRACT_CTR_MANAGE_FUNC        (0x02U)

/* Union Functional Descriptor */
#define     USB_PCDC_DT_SUBTYPE_UNION_FUNC                      (0x06U)

/* Communications Class Subclass Codes */
#define     USB_PCDC_CLASS_SUBCLASS_CODE_ABS_CTR_MDL            (0x02U)

/* USB Class Definitions for Communications Devices Specification
 * release number in binary-coded decimal */
#define     USB_PCDC_BCD_CDC                                    (0x0110U)

/* Descriptor length */
#define USB_PCDC_QD_LEN               (10U)
#define USB_PCDC_CD1_LEN              (75U)
#define STRING_DESCRIPTOR0_LEN        (4U)
#define STRING_DESCRIPTOR1_LEN        (16U)
#define STRING_DESCRIPTOR2_LEN        (44U)
#define STRING_DESCRIPTOR3_LEN        (46U)
#define STRING_DESCRIPTOR4_LEN        (22U)
#define STRING_DESCRIPTOR5_LEN        (18U)
#define STRING_DESCRIPTOR6_LEN        (28U)

/* Descriptor data Mask */
#define USB_UCHAR_MAX                 (0xffU)
#define USB_W_TOTAL_LENGTH_MASK       (256U)
#define USB_W_MAX_PACKET_SIZE_MASK    (64U)

#define VALUE_103                     (103U)
#define VALUE_93                      (93U)
#define VALUE_105                     (105U)
#define VALUE_2                       (2U)

/*******************************************************************************************************************//**
 * Private global variables and functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Exported global variables
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

#define DEVICE_FRAME_LENGTH_FULL_SPEED  (VALUE_93)
/* Standard device descriptor */
uint8_t g_device_framework_full_speed[DEVICE_FRAME_LENGTH_FULL_SPEED] =
{
    USB_DD_BLENGTH,                                             /* 0:bLength */
    USB_DT_DEVICE,                                              /* 1:bDescriptorType */
    (USB_BCDNUM & (uint8_t) USB_UCHAR_MAX),                     /* 2:bcdUSB_lo */
    ((uint8_t) (USB_BCDNUM >> 8) & (uint8_t) USB_UCHAR_MAX),    /* 3:bcdUSB_hi */
    USB_IFCLS_CDCC,                                             /* 4:bDeviceClass */
    0,                                                          /* 5:bDeviceSubClass */
    0,                                                          /* 6:bDeviceProtocol */
    (uint8_t) USB_DCPMAXP,                                      /* 7:bMAXPacketSize(for DCP) */
    (USB_VENDORID & (uint8_t) USB_UCHAR_MAX),                   /* 8:idVendor_lo */
    ((uint8_t) (USB_VENDORID >> 8) & (uint8_t) USB_UCHAR_MAX),  /* 9:idVendor_hi */
    ((uint16_t) USB_PRODUCTID & (uint8_t) USB_UCHAR_MAX),       /* 10:idProduct_lo */
    ((uint8_t) (USB_PRODUCTID >> 8) & (uint8_t) USB_UCHAR_MAX), /* 11:idProduct_hi */
    (USB_RELEASE & (uint8_t) USB_UCHAR_MAX),                    /* 12:bcdDevice_lo */
    ((uint8_t) (USB_RELEASE >> 8) & (uint8_t) USB_UCHAR_MAX),   /* 13:bcdDevice_hi */
    1,                                                          /* 14:iManufacturer */
    2,                                                          /* 15:iProduct */
    6,                                                          /* 16:iSerialNumber */
    USB_CONFIGNUM,                                              /* 17:bNumConfigurations */

    USB_CD_BLENGTH,                                 /* 0:bLength */
//    USB_DT_CONFIGURATION,                         /* 1:bDescriptorType */    /* For Full-speed */
    USB_DT_OTHER_SPEED_CONF,                        /* 1:bDescriptorType */    /* For High-speed */
    USB_PCDC_CD1_LEN % USB_W_TOTAL_LENGTH_MASK,     /* 2:wTotalLength(L) */
    USB_PCDC_CD1_LEN / USB_W_TOTAL_LENGTH_MASK,     /* 3:wTotalLength(H) */
    2,                                              /* 4:bNumInterfaces */
    1,                                              /* 5:bConfigurationValue */
    0,                                              /* 6:iConfiguration */
    USB_CF_RESERVED | USB_CF_SELFP,                 /* 7:bmAttributes */
    (10 / 2),                                       /* 8:MAXPower (2mA unit) */

    /* Interface association descriptor (IAD) */
    0x08,                                       /* 0:bLength */
    USB_IAD_TYPE,                               /* 1:bDescriptorType */
    0x00,                                       /* 2:bFirstInterface */
    0x02,                                       /* 3:bInterfaceCount */
    USB_IFCLS_CDCC,                             /* 4:bFunctionClass */
    USB_PCDC_CLASS_SUBCLASS_CODE_ABS_CTR_MDL,   /* 5:bFunctionSubClass */
    0x00,                                       /* 6:bFunctionProtocol */
    0x00,                                       /* 7:iFunction */

    /* Interface descriptor */
    USB_ID_BLENGTH,                             /* 0:bLength */
    USB_DT_INTERFACE,                           /* 1:bDescriptor */
    0,                                          /* 2:bInterfaceNumber */
    0,                                          /* 3:bAlternateSetting */
    1,                                          /* 4:bNumEndpoints */
    USB_IFCLS_CDCC,                             /* 5:bInterfaceClass */
    USB_PCDC_CLASS_SUBCLASS_CODE_ABS_CTR_MDL,   /* 6:bInterfaceSubClass */
    1,                                          /* 7:bInterfaceProtocol */
    0,                                          /* 8:iInterface */

        /* Communications class functional descriptors */
        5,                                              /* 0:bLength */
        USB_PCDC_CS_INTERFACE,                          /* 1:bDescriptorType */
        USB_PCDC_DT_SUBTYPE_HEADER_FUNC,                /* 2:bDescriptorSubtype */
        USB_PCDC_BCD_CDC % USB_W_TOTAL_LENGTH_MASK,     /* 3:bcdCDC_lo */
        USB_PCDC_BCD_CDC / USB_W_TOTAL_LENGTH_MASK,     /* 4:bcdCDC_hi */

        /* Communications class functional descriptors */
        4,                                              /* 0:bLength */
        USB_PCDC_CS_INTERFACE,                          /* 1:bDescriptorType */
        USB_PCDC_DT_SUBTYPE_ABSTRACT_CTR_MANAGE_FUNC,   /* 2:bDescriptorSubtype */
        2,                                              /* 3:bmCapabilities */

        /* Communications class functional descriptors */
        5,                                  /* 0:bLength */
        USB_PCDC_CS_INTERFACE,              /* 1:bDescriptorType */
        USB_PCDC_DT_SUBTYPE_UNION_FUNC,     /* 2:bDescriptorSubtype */
        0,                                  /* 3:bMasterInterface */
        1,                                  /* 4:bSlaveInterface0 */

        /* Communications class functional descriptors */
        5,                                      /* 0:bLength */
        USB_PCDC_CS_INTERFACE,                  /* 1:bDescriptorType */
        USB_PCDC_DT_SUBTYPE_CALL_MANAGE_FUNC,   /* 2:bDescriptorSubtype */
        /* D1:1-Device can send/receive call management information over a Data Class interface. */
        /* D0:1-Device handles call management itself. */
        3,                                      /* 3:bmCapabilities */
        1,                                      /* 4:bDataInterface */

        /* End point descriptor 0 */
        7,                          /* 0:bLength */
        USB_DT_ENDPOINT,            /* 1:bDescriptorType */
        USB_EP_IN | USB_EP3,        /* 2:bEndpointAddress */
        USB_EP_INT,                 /* 3:bmAttribute */
        16,                         /* 4:wMAXPacketSize_lo */
        0,                          /* 5:wMAXPacketSize_hi */
        0x10,                       /* 6:bInterval */

    /* Interface descriptor */
    USB_ID_BLENGTH,                 /* 0:bLength */
    USB_DT_INTERFACE,               /* 1:bDescriptor */
    1,                              /* 2:bInterfaceNumber */
    0,                              /* 3:bAlternateSetting */
    2,                              /* 4:bNumEndpoints */
    USB_IFCLS_CDCD,                 /* 5:bInterfaceClass */
    0,                              /* 6:bInterfaceSubClass */
    0,                              /* 7:bInterfaceProtocol */
    0,                              /* 8:iInterface */

        /* End point descriptor 0 */
        USB_ED_BLENGTH,             /* 0:bLength */
        USB_DT_ENDPOINT,            /* 1:bDescriptorType */
        USB_EP_IN | USB_EP1,        /* 2:bEndpointAddress */
        USB_EP_BULK,                /* 3:bmAttribute */
		USB_W_MAX_PACKET_SIZE_MASK, /* 4:wMAXPacketSize_lo */
        0,                          /* 5:wMAXPacketSize_hi */
        0,                          /* 6:bInterval */

        /* End point descriptor 1 */
        USB_ED_BLENGTH,             /* 0:bLength */
        USB_DT_ENDPOINT,            /* 1:bDescriptorType */
        USB_EP_OUT | USB_EP2,       /* 2:bEndpointAddress */
        USB_EP_BULK,                /* 3:bmAttribute */
		USB_W_MAX_PACKET_SIZE_MASK, /* 4:wMAXPacketSize_lo */
        0,                          /* 5:wMAXPacketSize_hi */
        0,                          /* 6:bInterval */
};

#define DEVICE_FRAME_LENGTH_HI_SPEED    (VALUE_103)
uint8_t g_device_framework_hi_speed[DEVICE_FRAME_LENGTH_HI_SPEED] =
{
    USB_DD_BLENGTH,                                             /* 0:bLength */
    USB_DT_DEVICE,                                              /* 1:bDescriptorType */
    (USB_BCDNUM & (uint8_t) USB_UCHAR_MAX),                     /* 2:bcdUSB_lo */
    ((uint8_t) (USB_BCDNUM >> 8) & (uint8_t) USB_UCHAR_MAX),    /* 3:bcdUSB_hi */
    USB_IFCLS_CDCC,                                             /* 4:bDeviceClass */
    0,                                                          /* 5:bDeviceSubClass */
    0,                                                          /* 6:bDeviceProtocol */
    (uint8_t) USB_DCPMAXP,                                      /* 7:bMAXPacketSize(for DCP) */
    (USB_VENDORID & (uint8_t) USB_UCHAR_MAX),                   /* 8:idVendor_lo */
    ((uint8_t) (USB_VENDORID >> 8) & (uint8_t) USB_UCHAR_MAX),  /* 9:idVendor_hi */
    ((uint16_t) USB_PRODUCTID & (uint8_t) USB_UCHAR_MAX),       /* 10:idProduct_lo */
    ((uint8_t) (USB_PRODUCTID >> 8) & (uint8_t) USB_UCHAR_MAX), /* 11:idProduct_hi */
    (USB_RELEASE & (uint8_t) USB_UCHAR_MAX),                    /* 12:bcdDevice_lo */
    ((uint8_t) (USB_RELEASE >> 8) & (uint8_t) USB_UCHAR_MAX),   /* 13:bcdDevice_hi */
    1,                                                          /* 14:iManufacturer */
    2,                                                          /* 15:iProduct */
    6,                                                          /* 16:iSerialNumber */
    USB_CONFIGNUM,                                              /* 17:bNumConfigurations */

    USB_PCDC_QD_LEN,                                            /* 0:bLength */
    USB_DT_DEVICE_QUALIFIER,                                    /* 1:bDescriptorType */
    (USB_BCDNUM & (uint8_t) USB_UCHAR_MAX),                     /* 2:bcdUSB_lo */
    ((uint8_t) (USB_BCDNUM >> 8) & (uint8_t) USB_UCHAR_MAX),    /* 3:bcdUSB_hi */
    0,                                                          /* 4:bDeviceClass */
    0,                                                          /* 5:bDeviceSubClass */
    0,                                                          /* 6:bDeviceProtocol */
    (uint8_t) USB_DCPMAXP,                                      /* 7:bMAXPacketSize(for DCP) */
    USB_CONFIGNUM,                                              /* 8:bNumConfigurations */
    0,                                                          /* 9:bReserved */

    USB_CD_BLENGTH,                                 /* 0:bLength */
    USB_DT_CONFIGURATION,                           /* 1:bDescriptorType */    /* For High-speed */
//    USB_DT_OTHER_SPEED_CONF,                      /* 1:bDescriptorType */    /* For Full-speed */
    USB_PCDC_CD1_LEN % USB_W_TOTAL_LENGTH_MASK,     /* 2:wTotalLength(L) */
    USB_PCDC_CD1_LEN / USB_W_TOTAL_LENGTH_MASK,     /* 3:wTotalLength(H) */
    2,                                              /* 4:bNumInterfaces */
    1,                                              /* 5:bConfigurationValue */
    0,                                              /* 6:iConfiguration */
    USB_CF_RESERVED | USB_CF_SELFP,                 /* 7:bmAttributes */
    (10 / 2),                                       /* 8:MAXPower (2mA unit) */

    /* Interface association descriptor (IAD) */
    0x08,                                           /* 0:bLength */
    USB_IAD_TYPE,                                   /* 1:bDescriptorType */
    0x00,                                           /* 2:bFirstInterface */
    0x02,                                           /* 3:bInterfaceCount */
    USB_IFCLS_CDCC,                                 /* 4:bFunctionClass */
    USB_PCDC_CLASS_SUBCLASS_CODE_ABS_CTR_MDL,       /* 5:bFunctionSubClass */
    0x00,                                           /* 6:bFunctionProtocol */
    0x00,                                           /* 7:iFunction */

    /* Interface descriptor */
    9,                                              /* 0:bLength */
    USB_DT_INTERFACE,                               /* 1:bDescriptor */
    0,                                              /* 2:bInterfaceNumber */
    0,                                              /* 3:bAlternateSetting */
    1,                                              /* 4:bNumEndpoints */
    USB_IFCLS_CDCC,                                 /* 5:bInterfaceClass */
    USB_PCDC_CLASS_SUBCLASS_CODE_ABS_CTR_MDL,       /* 6:bInterfaceSubClass */
    1,                                              /* 7:bInterfaceProtocol */
    0,                                              /* 8:iInterface */

        /* Communications class functional descriptors */
        5,                                              /* 0:bLength */
        USB_PCDC_CS_INTERFACE,                          /* 1:bDescriptorType */
        USB_PCDC_DT_SUBTYPE_HEADER_FUNC,                /* 2:bDescriptorSubtype */
        USB_PCDC_BCD_CDC % USB_W_TOTAL_LENGTH_MASK,     /* 3:bcdCDC_lo */
        USB_PCDC_BCD_CDC / USB_W_TOTAL_LENGTH_MASK,     /* 4:bcdCDC_hi */

        /* Communications class functional descriptors */
        4,                                              /* 0:bLength */
        USB_PCDC_CS_INTERFACE,                          /* 1:bDescriptorType */
        USB_PCDC_DT_SUBTYPE_ABSTRACT_CTR_MANAGE_FUNC,   /* 2:bDescriptorSubtype */
        2,                                              /* 3:bmCapabilities */

        /* Communications class functional descriptors */
        5,                                              /* 0:bLength */
        USB_PCDC_CS_INTERFACE,                          /* 1:bDescriptorType */
        USB_PCDC_DT_SUBTYPE_UNION_FUNC,                 /* 2:bDescriptorSubtype */
        0,                                              /* 3:bMasterInterface */
        1,                                              /* 4:bSlaveInterface0 */

        /* Communications class functional descriptors */
        5,                                              /* 0:bLength */
        USB_PCDC_CS_INTERFACE,                          /* 1:bDescriptorType */
        USB_PCDC_DT_SUBTYPE_CALL_MANAGE_FUNC,           /* 2:bDescriptorSubtype */
        /* D1:1-Device can send/receive call management information over a Data Class interface. */
        /* D0:1-Device handles call management itself. */
        3,                                              /* 3:bmCapabilities */
        1,                                              /* 4:bDataInterface */

        /* End point descriptor 0 */
        7,                          /* 0:bLength */
        USB_DT_ENDPOINT,            /* 1:bDescriptorType */
        USB_EP_IN | USB_EP3,        /* 2:bEndpointAddress */
        USB_EP_INT,                 /* 3:bmAttribute */
        16,                         /* 4:wMAXPacketSize_lo */
        0,                          /* 5:wMAXPacketSize_hi */
        0x10,                       /* 6:bInterval */

    /* Interface descriptor */
    9,                              /* 0:bLength */
    USB_DT_INTERFACE,               /* 1:bDescriptor */
    1,                              /* 2:bInterfaceNumber */
    0,                              /* 3:bAlternateSetting */
    2,                              /* 4:bNumEndpoints */
    USB_IFCLS_CDCD,                 /* 5:bInterfaceClass */
    0,                              /* 6:bInterfaceSubClass */
    0,                              /* 7:bInterfaceProtocol */
    0,                              /* 8:iInterface */

        /* End point descriptor 0 */
        7,                          /* 0:bLength */
        USB_DT_ENDPOINT,            /* 1:bDescriptorType */
        USB_EP_IN | USB_EP1,        /* 2:bEndpointAddress */
        USB_EP_BULK,                /* 3:bmAttribute */
        0,                          /* 4:wMAXPacketSize_lo */
        2,                          /* 5:wMAXPacketSize_hi */
        0,                          /* 6:bInterval */

        /* End point descriptor 1 */
        7,                          /* 0:bLength */
        USB_DT_ENDPOINT,            /* 1:bDescriptorType */
        USB_EP_OUT | USB_EP2,       /* 2:bEndpointAddress */
        USB_EP_BULK,                /* 3:bmAttribute */
        0,                          /* 4:wMAXPacketSize_lo */
        2,                          /* 5:wMAXPacketSize_hi */
        0,                          /* 6:bInterval */
};

#define STRING_FRAMEWORK_LENGTH (VALUE_105)
uint8_t g_string_framework[] =
{
    /* iManufacturer */
    0x09, 0x04, 0x1, 7,
    'R',
    'E',
    'N',
    'E',
    'S',
    'A',
    'S',

    /* iProduct */
    0x09, 0x04, 0x2, 21,
    'C',
    'D',
    'C',
    ' ',
    'U',
    'S',
    'B',
    ' ',
    'D',
    'e',
    'm',
    'o',
    'n',
    's',
    't',
    'r',
    'a',
    't',
    'i',
    'o',
    'n',

    /* iInterface */
    0x09, 0x04, 0x3, 22,
    'C',
    'o',
    'm',
    'm',
    'u',
    'n',
    'i',
    'c',
    'a',
    't',
    'i',
    'o',
    'n',
    's',
    ' ',
    'D',
    'e',
    'v',
    'i',
    'c',
    'e',
    's',

    /* iConfiguration */
    0x09, 0x04, 0x4, 10,
    'F',
    'u',
    'l',
    'l',
    '-',
    'S',
    'p',
    'e',
    'e',
    'd',

    /* iConfiguration */
    0x09, 0x04, 0x5, 8,
    'H',
    'i',
    '-',
    'S',
    'p',
    'e',
    'e',
    'd',

    /* iConfiguration */
    0x09, 0x04, 0x6, 13,
    '0',
    '0',
    '0',
    '0',
    '0',
    '0',
    '0',
    '0',
    '0',
    '0',
    '0',
    '0',
    '1',
};

#define LANGUAGE_ID_FRAME_WORK_LENGTH  (VALUE_2)
uint8_t g_language_id_framework[] =
{
    0x09,
    0x04,
};

/*******************************************************************************************************************//**
 * End Of File
 **********************************************************************************************************************/
