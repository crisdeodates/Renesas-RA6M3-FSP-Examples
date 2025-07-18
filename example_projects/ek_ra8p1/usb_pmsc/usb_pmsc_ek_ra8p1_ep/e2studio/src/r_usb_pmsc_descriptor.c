/***********************************************************************************************************************
 * File Name    : r_usb_pmsc_descriptor.c
 * Description  : USB PMSC Descriptor definition.
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
#include "hal_data.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_pmsc_descrriptor
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Macro definitions
 **********************************************************************************************************************/

/* bcdUSB */
#define USB_BCDNUM                (0x0200U)

/* Release number */
#define USB_RELEASE               (0x0200U)

/* DCP max packet size */
#define USB_DCPMAXP               (0x40U)

/* Configuration number */
#define USB_CONFIGNUM             (0x01U)

/* Vendor ID */
#define USB_VENDORID              (0x0000U)

/* Product ID */
#define USB_PRODUCTID             (0x0008U)

#define USB_PMSC_QD_BLENGTH       (0x0aU)
#define USB_PMSC_CD1_BLENGTH      (0x20U)
#define USB_PMSC_SD0_BLENGTH      (0x04U)
#define USB_PMSC_SD1_BLENGTH      (0x10U)
#define USB_PMSC_SD2_BLENGTH      (0x22U)
#define USB_PMSC_SD3_BLENGTH      (0x20U)
#define USB_PMSC_SD4_BLENGTH      (0x16U)
#define USB_PMSC_SD5_BLENGTH      (0x12U)
#define USB_PMSC_SD6_BLENGTH      (0x1cU)

/* Sub_class code */
#define USB_ATAPI                 ((uint8_t) 0x05U)
#define USB_SCSI                  ((uint8_t) 0x06U)

/* Protocol code */
#define USB_BOTP                  ((uint8_t) 0x50U)
#define USB_TOTALEP               ((uint8_t) 0x02U)

/* Select SubClass */

// #define USB_INTERFACE_SUBCLASS  (USB_SCSI)
#define USB_INTERFACE_SUBCLASS    (USB_ATAPI)

#define USB_VALUE_FFUH            (0xffU)
#define USB_VALUE_256             (256)
#define USB_VALUE_64              (64)

/*******************************************************************************************************************//**
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

/* Standard Device Descriptor */
uint8_t g_apl_device[USB_DD_BLENGTH + (USB_DD_BLENGTH % 2)] =
{
    USB_DD_BLENGTH,                                              /*  0:bLength */
    USB_DT_DEVICE,                                               /*  1:bDescriptorType */
    (USB_BCDNUM & (uint8_t) USB_VALUE_FFUH),                     /*  2:bcdUSB_lo */
    ((uint8_t) (USB_BCDNUM >> 8) & (uint8_t) USB_VALUE_FFUH),    /*  3:bcdUSB_hi */
    0,                                                           /*  4:bDeviceClass */
    0,                                                           /*  5:bDeviceSubClass */
    0,                                                           /*  6:bDeviceProtocol */
    (uint8_t) USB_DCPMAXP,                                       /*  7:bMAXPacketSize(for DCP) */
    (USB_VENDORID & (uint8_t) USB_VALUE_FFUH),                   /*  8:idVendor_lo */
    ((uint8_t) (USB_VENDORID >> 8) & (uint8_t) USB_VALUE_FFUH),  /*  9:idVendor_hi */
    ((uint16_t) USB_PRODUCTID & (uint8_t) USB_VALUE_FFUH),       /* 10:idProduct_lo */
    ((uint8_t) (USB_PRODUCTID >> 8) & (uint8_t) USB_VALUE_FFUH), /* 11:idProduct_hi */
    (USB_RELEASE & (uint8_t) USB_VALUE_FFUH),                    /* 12:bcdDevice_lo */
    ((uint8_t) (USB_RELEASE >> 8) & (uint8_t) USB_VALUE_FFUH),   /* 13:bcdDevice_hi */
    1,                                                           /* 14:iManufacturer */
    2,                                                           /* 15:iProduct */
    6,                                                           /* 16:iSerialNumber */
    USB_CONFIGNUM                                                /* 17:bNumConfigurations */
};

/* Device Qualifier Descriptor */
uint8_t g_apl_qualifier_descriptor[USB_PMSC_QD_BLENGTH + (USB_PMSC_QD_BLENGTH % 2)] =
{
    USB_PMSC_QD_BLENGTH,                                      /*  0:bLength */
    USB_DT_DEVICE_QUALIFIER,                                  /*  1:bDescriptorType */
    (USB_BCDNUM & (uint8_t) USB_VALUE_FFUH),                  /*  2:bcdUSB_lo */
    ((uint8_t) (USB_BCDNUM >> 8) & (uint8_t) USB_VALUE_FFUH), /*  3:bcdUSB_hi */
    0,                                                        /*  4:bDeviceClass */
    0,                                                        /*  5:bDeviceSubClass */
    0,                                                        /*  6:bDeviceProtocol */
    (uint8_t) USB_DCPMAXP,                                    /*  7:bMAXPacketSize(for DCP) */
    USB_CONFIGNUM,                                            /*  8:bNumConfigurations */
    0                                                         /*  9:bReserved */
};

/*******************************************************************************************************************//**
 *  Configuration Or Other_Speed_Configuration Descriptor
 **********************************************************************************************************************/

/* For Full-Speed */
uint8_t g_apl_configuration[USB_PMSC_CD1_BLENGTH + (USB_PMSC_CD1_BLENGTH % 2)] =
{
    USB_CD_BLENGTH,                       /*  0:bLength */
    USB_SOFT_CHANGE,                      /*  1:bDescriptorType */
    USB_PMSC_CD1_BLENGTH % USB_VALUE_256, /*  2:wTotalLength(L) */
    USB_PMSC_CD1_BLENGTH / USB_VALUE_256, /*  3:wTotalLength(H) */
    1,                                    /*  4:bNumInterfaces */
    1,                                    /*  5:bConfigurationValue */
    4,                                    /*  6:iConfiguration */
    USB_CF_RESERVED | USB_CF_SELFP,       /*  7:bmAttributes */
    (10 / 2),                             /*  8:MAXPower (2mA unit) */

    /* Interface Descriptor */
    USB_ID_BLENGTH,                       /*  0:bLength */
    USB_DT_INTERFACE,                     /*  1:bDescriptor */
    0,                                    /*  2:bInterfaceNumber */
    0,                                    /*  3:bAlternateSetting */
    2,                                    /*  4:bNumEndpoints */
    USB_IFCLS_MAS,                        /*  5:bInterfaceClass */
    USB_INTERFACE_SUBCLASS,               /*  6:bInterfaceSubClass */
    USB_BOTP,                             /*  7:bInterfaceProtocol */
    3,                                    /*  8:iInterface */

    /* Endpoint Descriptor 0 */
    USB_ED_BLENGTH,                       /*  0:bLength */
    USB_DT_ENDPOINT,                      /*  1:bDescriptorType */
    USB_EP_IN | USB_EP1,                  /*  2:bEndpointAddress */
    USB_EP_BULK,                          /*  3:bmAttribute */
    USB_VALUE_64,                         /*  4:wMAXPacketSize_lo */
    0,                                    /*  5:wMAXPacketSize_hi */
    0,                                    /*  6:bInterval */

    /* Endpoint Descriptor 1 */
    USB_ED_BLENGTH,                       /*  0:bLength */
    USB_DT_ENDPOINT,                      /*  1:bDescriptorType */
    USB_EP_OUT | USB_EP2,                 /*  2:bEndpointAddress */
    USB_EP_BULK,                          /*  3:bmAttribute */
    USB_VALUE_64,                         /*  4:wMAXPacketSize_lo */
    0,                                    /*  5:wMAXPacketSize_hi */
    1,                                    /*  6:bInterval */
};

uint8_t g_apl_hs_configuration[USB_PMSC_CD1_BLENGTH + (USB_PMSC_CD1_BLENGTH % 2)] =
{
    9,                                    /*  0:bLength */
    USB_SOFT_CHANGE,                      /*  1:bDescriptorType */
    USB_PMSC_CD1_BLENGTH % USB_VALUE_256, /*  2:wTotalLength(L) */
    USB_PMSC_CD1_BLENGTH / USB_VALUE_256, /*  3:wTotalLength(H) */
    1,                                    /*  4:bNumInterfaces */
    1,                                    /*  5:bConfigurationValue */
    5,                                    /*  6:iConfiguration */
    USB_CF_RESERVED | USB_CF_SELFP,       /*  7:bmAttributes */
    (10 / 2),                             /*  8:MAXPower (2mA unit) */

    /* Interface Descriptor */
    9,                                    /*  0:bLength */
    USB_DT_INTERFACE,                     /*  1:bDescriptor */
    0,                                    /*  2:bInterfaceNumber */
    0,                                    /*  3:bAlternateSetting */
    2,                                    /*  4:bNumEndpoints */
    USB_IFCLS_MAS,                        /*  5:bInterfaceClass */
    USB_INTERFACE_SUBCLASS,               /*  6:bInterfaceSubClass */
    USB_BOTP,                             /*  7:bInterfaceProtocol */
    3,                                    /*  8:iInterface */

    /* Endpoint Descriptor 0 */
    7,                                    /*  0:bLength */
    USB_DT_ENDPOINT,                      /*  1:bDescriptorType */
    USB_EP_IN | USB_EP1,                  /*  2:bEndpointAddress */
    USB_EP_BULK,                          /*  3:bmAttribute */
    0,                                    /*  4:wMAXPacketSize_lo */
    2,                                    /*  5:wMAXPacketSize_hi */
    0,                                    /*  6:bInterval */

    /* Endpoint Descriptor 1 */
    7,                                    /*  0:bLength */
    USB_DT_ENDPOINT,                      /*  1:bDescriptorType */
    USB_EP_OUT | USB_EP2,                 /*  2:bEndpointAddress */
    USB_EP_BULK,                          /*  3:bmAttribute */
    0,                                    /*  4:wMAXPacketSize_lo */
    2,                                    /*  5:wMAXPacketSize_hi */
    1,                                    /*  6:bInterval */
};

/*******************************************************************************************************************//**
 *    String Descriptor
 **********************************************************************************************************************/
uint8_t g_usb_pmsc_string_descriptor0[USB_PMSC_SD0_BLENGTH + (USB_PMSC_SD0_BLENGTH % 2)] =
{
    /* UNICODE 0x0409 English (United States) */
    USB_PMSC_SD0_BLENGTH,              /*  0:bLength */
    USB_DT_STRING,                     /*  1:bDescriptorType */
    0x09, 0x04                         /*  2:wLANGID[0] */
};

uint8_t g_usb_pmsc_string_descriptor1[USB_PMSC_SD1_BLENGTH + (USB_PMSC_SD1_BLENGTH % 2)] =
{
    /* iManufacturer */
    USB_PMSC_SD1_BLENGTH,              /*  0:bLength */
    USB_DT_STRING,                     /*  1:bDescriptorType */
    'R', 0x00,                         /*  2:wLANGID[0] */
    'E', 0x00,
    'N', 0x00,
    'E', 0x00,
    'S', 0x00,
    'A', 0x00,
    'S', 0x00,
};

uint8_t g_usb_pmsc_string_descriptor2[USB_PMSC_SD2_BLENGTH + (USB_PMSC_SD2_BLENGTH % 2)] =
{
    /* iProduct */
    USB_PMSC_SD2_BLENGTH,              /*  0:bLength */
    USB_DT_STRING,                     /*  1:bDescriptorType */
    'U', 0x00,
    'S', 0x00,
    'B', 0x00,
    ' ', 0x00,
    'M', 0x00,
    'a', 0x00,
    's', 0x00,
    's', 0x00,
    ' ', 0x00,
    'S', 0x00,
    't', 0x00,
    'o', 0x00,
    'r', 0x00,
    'a', 0x00,
    'g', 0x00,
    'e', 0x00
};

uint8_t g_usb_pmsc_string_descriptor3[USB_PMSC_SD3_BLENGTH + (USB_PMSC_SD3_BLENGTH % 2)] =
{
    /* iInterface */
    USB_PMSC_SD3_BLENGTH,              /*  0:bLength */
    USB_DT_STRING,                     /*  1:bDescriptorType */
    'R', 0x00,
    'e', 0x00,
    'm', 0x00,
    'o', 0x00,
    'v', 0x00,
    'a', 0x00,
    'b', 0x00,
    'l', 0x00,
    'e', 0x00,
    ' ', 0x00,
    'D', 0x00,
    'r', 0x00,
    'i', 0x00,
    'v', 0x00,
    'e', 0x00
};

uint8_t g_usb_pmsc_string_descriptor4[USB_PMSC_SD4_BLENGTH + (USB_PMSC_SD4_BLENGTH % 2)] =
{
    /* iConfiguration */
    USB_PMSC_SD4_BLENGTH,              /*  0:bLength */
    USB_DT_STRING,                     /*  1:bDescriptorType */
    'F', 0x00,                         /*  2:wLANGID[0] */
    'u', 0x00,
    'l', 0x00,
    'l', 0x00,
    '-', 0x00,
    'S', 0x00,
    'p', 0x00,
    'e', 0x00,
    'e', 0x00,
    'd', 0x00
};

uint8_t g_usb_pmsc_string_descriptor5[USB_PMSC_SD5_BLENGTH + (USB_PMSC_SD5_BLENGTH % 2)] =
{
    /* iConfiguration */
    USB_PMSC_SD5_BLENGTH,              /*  0:bLength */
    USB_DT_STRING,                     /*  1:bDescriptorType */
    'H', 0x00,                         /*  2:wLANGID[0] */
    'i', 0x00,
    '-', 0x00,
    'S', 0x00,
    'p', 0x00,
    'e', 0x00,
    'e', 0x00,
    'd', 0x00
};

uint8_t g_usb_pmsc_string_descriptor6[USB_PMSC_SD6_BLENGTH + (USB_PMSC_SD6_BLENGTH % 2)] =
{
    /* iSerialNumber */
    USB_PMSC_SD6_BLENGTH,              /*  0:bLength */
    USB_DT_STRING,                     /*  1:bDescriptorType */
    '0', 0x00,                         /*  2:wLANGID[0] */
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '0', 0x00,
    '1', 0x00,
};

uint8_t * gp_apl_string_table[] =
{
    g_usb_pmsc_string_descriptor0,
    g_usb_pmsc_string_descriptor1,
    g_usb_pmsc_string_descriptor2,
    g_usb_pmsc_string_descriptor3,
    g_usb_pmsc_string_descriptor4,
    g_usb_pmsc_string_descriptor5,
    g_usb_pmsc_string_descriptor6
};

/*******************************************************************************************************************//**
 * Renesas Abstracted Peripheral Mass Storage Class Driver API functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_pmsc_descriptor)
 **********************************************************************************************************************/
