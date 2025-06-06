/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

/* This file contains certs needed to communicate with Azure (IoT) */

#include "nx_azure_iot_cert.h"

/* DigiCert Global Root G2 --Used Globally--  */

const uint8_t g_nx_azure_iot_root_cert[] =
{
0x30, 0x82, 0x03, 0x8e, 0x30, 0x82, 0x02, 0x76, 0xa0, 0x03, 0x02, 0x01,
  0x02, 0x02, 0x10, 0x03, 0x3a, 0xf1, 0xe6, 0xa7, 0x11, 0xa9, 0xa0, 0xbb,
  0x28, 0x64, 0xb1, 0x1d, 0x09, 0xfa, 0xe5, 0x30, 0x0d, 0x06, 0x09, 0x2a,
  0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x30, 0x61,
  0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55,
  0x53, 0x31, 0x15, 0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x0c,
  0x44, 0x69, 0x67, 0x69, 0x43, 0x65, 0x72, 0x74, 0x20, 0x49, 0x6e, 0x63,
  0x31, 0x19, 0x30, 0x17, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x13, 0x10, 0x77,
  0x77, 0x77, 0x2e, 0x64, 0x69, 0x67, 0x69, 0x63, 0x65, 0x72, 0x74, 0x2e,
  0x63, 0x6f, 0x6d, 0x31, 0x20, 0x30, 0x1e, 0x06, 0x03, 0x55, 0x04, 0x03,
  0x13, 0x17, 0x44, 0x69, 0x67, 0x69, 0x43, 0x65, 0x72, 0x74, 0x20, 0x47,
  0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x20, 0x52, 0x6f, 0x6f, 0x74, 0x20, 0x47,
  0x32, 0x30, 0x1e, 0x17, 0x0d, 0x31, 0x33, 0x30, 0x38, 0x30, 0x31, 0x31,
  0x32, 0x30, 0x30, 0x30, 0x30, 0x5a, 0x17, 0x0d, 0x33, 0x38, 0x30, 0x31,
  0x31, 0x35, 0x31, 0x32, 0x30, 0x30, 0x30, 0x30, 0x5a, 0x30, 0x61, 0x31,
  0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53,
  0x31, 0x15, 0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x0c, 0x44,
  0x69, 0x67, 0x69, 0x43, 0x65, 0x72, 0x74, 0x20, 0x49, 0x6e, 0x63, 0x31,
  0x19, 0x30, 0x17, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x13, 0x10, 0x77, 0x77,
  0x77, 0x2e, 0x64, 0x69, 0x67, 0x69, 0x63, 0x65, 0x72, 0x74, 0x2e, 0x63,
  0x6f, 0x6d, 0x31, 0x20, 0x30, 0x1e, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13,
  0x17, 0x44, 0x69, 0x67, 0x69, 0x43, 0x65, 0x72, 0x74, 0x20, 0x47, 0x6c,
  0x6f, 0x62, 0x61, 0x6c, 0x20, 0x52, 0x6f, 0x6f, 0x74, 0x20, 0x47, 0x32,
  0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86,
  0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00,
  0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01, 0x00, 0xbb, 0x37, 0xcd,
  0x34, 0xdc, 0x7b, 0x6b, 0xc9, 0xb2, 0x68, 0x90, 0xad, 0x4a, 0x75, 0xff,
  0x46, 0xba, 0x21, 0x0a, 0x08, 0x8d, 0xf5, 0x19, 0x54, 0xc9, 0xfb, 0x88,
  0xdb, 0xf3, 0xae, 0xf2, 0x3a, 0x89, 0x91, 0x3c, 0x7a, 0xe6, 0xab, 0x06,
  0x1a, 0x6b, 0xcf, 0xac, 0x2d, 0xe8, 0x5e, 0x09, 0x24, 0x44, 0xba, 0x62,
  0x9a, 0x7e, 0xd6, 0xa3, 0xa8, 0x7e, 0xe0, 0x54, 0x75, 0x20, 0x05, 0xac,
  0x50, 0xb7, 0x9c, 0x63, 0x1a, 0x6c, 0x30, 0xdc, 0xda, 0x1f, 0x19, 0xb1,
  0xd7, 0x1e, 0xde, 0xfd, 0xd7, 0xe0, 0xcb, 0x94, 0x83, 0x37, 0xae, 0xec,
  0x1f, 0x43, 0x4e, 0xdd, 0x7b, 0x2c, 0xd2, 0xbd, 0x2e, 0xa5, 0x2f, 0xe4,
  0xa9, 0xb8, 0xad, 0x3a, 0xd4, 0x99, 0xa4, 0xb6, 0x25, 0xe9, 0x9b, 0x6b,
  0x00, 0x60, 0x92, 0x60, 0xff, 0x4f, 0x21, 0x49, 0x18, 0xf7, 0x67, 0x90,
  0xab, 0x61, 0x06, 0x9c, 0x8f, 0xf2, 0xba, 0xe9, 0xb4, 0xe9, 0x92, 0x32,
  0x6b, 0xb5, 0xf3, 0x57, 0xe8, 0x5d, 0x1b, 0xcd, 0x8c, 0x1d, 0xab, 0x95,
  0x04, 0x95, 0x49, 0xf3, 0x35, 0x2d, 0x96, 0xe3, 0x49, 0x6d, 0xdd, 0x77,
  0xe3, 0xfb, 0x49, 0x4b, 0xb4, 0xac, 0x55, 0x07, 0xa9, 0x8f, 0x95, 0xb3,
  0xb4, 0x23, 0xbb, 0x4c, 0x6d, 0x45, 0xf0, 0xf6, 0xa9, 0xb2, 0x95, 0x30,
  0xb4, 0xfd, 0x4c, 0x55, 0x8c, 0x27, 0x4a, 0x57, 0x14, 0x7c, 0x82, 0x9d,
  0xcd, 0x73, 0x92, 0xd3, 0x16, 0x4a, 0x06, 0x0c, 0x8c, 0x50, 0xd1, 0x8f,
  0x1e, 0x09, 0xbe, 0x17, 0xa1, 0xe6, 0x21, 0xca, 0xfd, 0x83, 0xe5, 0x10,
  0xbc, 0x83, 0xa5, 0x0a, 0xc4, 0x67, 0x28, 0xf6, 0x73, 0x14, 0x14, 0x3d,
  0x46, 0x76, 0xc3, 0x87, 0x14, 0x89, 0x21, 0x34, 0x4d, 0xaf, 0x0f, 0x45,
  0x0c, 0xa6, 0x49, 0xa1, 0xba, 0xbb, 0x9c, 0xc5, 0xb1, 0x33, 0x83, 0x29,
  0x85, 0x02, 0x03, 0x01, 0x00, 0x01, 0xa3, 0x42, 0x30, 0x40, 0x30, 0x0f,
  0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x05, 0x30, 0x03,
  0x01, 0x01, 0xff, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01,
  0xff, 0x04, 0x04, 0x03, 0x02, 0x01, 0x86, 0x30, 0x1d, 0x06, 0x03, 0x55,
  0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x4e, 0x22, 0x54, 0x20, 0x18, 0x95,
  0xe6, 0xe3, 0x6e, 0xe6, 0x0f, 0xfa, 0xfa, 0xb9, 0x12, 0xed, 0x06, 0x17,
  0x8f, 0x39, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d,
  0x01, 0x01, 0x0b, 0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0x60, 0x67,
  0x28, 0x94, 0x6f, 0x0e, 0x48, 0x63, 0xeb, 0x31, 0xdd, 0xea, 0x67, 0x18,
  0xd5, 0x89, 0x7d, 0x3c, 0xc5, 0x8b, 0x4a, 0x7f, 0xe9, 0xbe, 0xdb, 0x2b,
  0x17, 0xdf, 0xb0, 0x5f, 0x73, 0x77, 0x2a, 0x32, 0x13, 0x39, 0x81, 0x67,
  0x42, 0x84, 0x23, 0xf2, 0x45, 0x67, 0x35, 0xec, 0x88, 0xbf, 0xf8, 0x8f,
  0xb0, 0x61, 0x0c, 0x34, 0xa4, 0xae, 0x20, 0x4c, 0x84, 0xc6, 0xdb, 0xf8,
  0x35, 0xe1, 0x76, 0xd9, 0xdf, 0xa6, 0x42, 0xbb, 0xc7, 0x44, 0x08, 0x86,
  0x7f, 0x36, 0x74, 0x24, 0x5a, 0xda, 0x6c, 0x0d, 0x14, 0x59, 0x35, 0xbd,
  0xf2, 0x49, 0xdd, 0xb6, 0x1f, 0xc9, 0xb3, 0x0d, 0x47, 0x2a, 0x3d, 0x99,
  0x2f, 0xbb, 0x5c, 0xbb, 0xb5, 0xd4, 0x20, 0xe1, 0x99, 0x5f, 0x53, 0x46,
  0x15, 0xdb, 0x68, 0x9b, 0xf0, 0xf3, 0x30, 0xd5, 0x3e, 0x31, 0xe2, 0x8d,
  0x84, 0x9e, 0xe3, 0x8a, 0xda, 0xda, 0x96, 0x3e, 0x35, 0x13, 0xa5, 0x5f,
  0xf0, 0xf9, 0x70, 0x50, 0x70, 0x47, 0x41, 0x11, 0x57, 0x19, 0x4e, 0xc0,
  0x8f, 0xae, 0x06, 0xc4, 0x95, 0x13, 0x17, 0x2f, 0x1b, 0x25, 0x9f, 0x75,
  0xf2, 0xb1, 0x8e, 0x99, 0xa1, 0x6f, 0x13, 0xb1, 0x41, 0x71, 0xfe, 0x88,
  0x2a, 0xc8, 0x4f, 0x10, 0x20, 0x55, 0xd7, 0xf3, 0x14, 0x45, 0xe5, 0xe0,
  0x44, 0xf4, 0xea, 0x87, 0x95, 0x32, 0x93, 0x0e, 0xfe, 0x53, 0x46, 0xfa,
  0x2c, 0x9d, 0xff, 0x8b, 0x22, 0xb9, 0x4b, 0xd9, 0x09, 0x45, 0xa4, 0xde,
  0xa4, 0xb8, 0x9a, 0x58, 0xdd, 0x1b, 0x7d, 0x52, 0x9f, 0x8e, 0x59, 0x43,
  0x88, 0x81, 0xa4, 0x9e, 0x26, 0xd5, 0x6f, 0xad, 0xdd, 0x0d, 0xc6, 0x37,
  0x7d, 0xed, 0x03, 0x92, 0x1b, 0xe5, 0x77, 0x5f, 0x76, 0xee, 0x3c, 0x8d,
  0xc4, 0x5d, 0x56, 0x5b, 0xa2, 0xd9, 0x66, 0x6e, 0xb3, 0x35, 0x37, 0xe5,
  0x32, 0xb6
};

const uint32_t g_nx_azure_iot_root_cert_size = sizeof(g_nx_azure_iot_root_cert);
