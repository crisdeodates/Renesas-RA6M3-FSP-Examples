/***********************************************************************************************************************
 * File Name    : aes_thread_entry.c
 * Description  : Contains macros and functions used in aes_thread_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "aes_thread.h"
#include "common_utils.h"
#include "crypto_ep_definitions.h"
#include "nx_crypto_hmac_sha2.h"
#include "hw_sce_trng_private.h"

/*******************************************************************************************************************//**
 * @addtogroup netx_crypto_ep
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Macro definitions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Exported global variables
 **********************************************************************************************************************/

extern NX_CRYPTO_METHOD crypto_method_aes_cbc_256;
extern NX_CRYPTO_METHOD crypto_method_hmac_sha256;
extern NX_CRYPTO_METHOD crypto_method_aes_cbc_128;
extern NX_CRYPTO_METHOD crypto_method_aes_256_gcm_16;

void *gp_sce_resource;

/*******************************************************************************************************************//**
 Private global variables and functions
 **********************************************************************************************************************/
/* NIST vector */
/* bb4635d766dd0e4a7019d1724c736e1f2c016af9e29e7d3aa2c0de23e780af26 */
static const uint8_t key_gcm_256[] =
{
    0xbb, 0x46, 0x35, 0xd7, 0x66, 0xdd, 0x0e, 0x4a, 0x70, 0x19, 0xd1, 0x72, 0x4c, 0x73, 0x6e, 0x1f,
    0x2c, 0x01, 0x6a, 0xf9, 0xe2, 0x9e, 0x7d, 0x3a, 0xa2, 0xc0, 0xde, 0x23, 0xe7, 0x80, 0xaf, 0x26
};
/* NIST vector */
/* In case of IV the IV length must be the first byte followed by the actual IV.
 * In this example the IV length is 0x01 and the actual IV is 0xab */
/* 01ab */
static const uint8_t iv_gcm_256[] =
{
    0x01, 0xab
};
/* NIST vector */
/* d05ce878d94662d1520b184b4bef3c45 */
static const uint8_t plain_gcm_128[] =
{
    0xd0, 0x5c, 0xe8, 0x78, 0xd9, 0x46, 0x62, 0xd1, 0x52, 0x0b, 0x18, 0x4b, 0x4b, 0xef, 0x3c, 0x45
};
/* NIST vector */
/* 0f85c7dbeb674b7a70c35125d3619350 */
static const uint8_t aad_gcm_128[] =
{
    0x0f, 0x85, 0xc7, 0xdb, 0xeb, 0x67, 0x4b, 0x7a, 0x70, 0xc3, 0x51, 0x25, 0xd3, 0x61, 0x93, 0x50
};
/* NIST vector */
/* 51baa26a6a719c1600645ff3bfdfa53b6bd54e5184eb300934b392c32b7c1a6e */
static const uint8_t secret_gcm_128[] =
{
    /* Ciphertext */
    0x51, 0xba, 0xa2, 0x6a, 0x6a, 0x71, 0x9c, 0x16, 0x00, 0x64, 0x5f, 0xf3, 0xbf, 0xdf, 0xa5, 0x3b,
    /* Tag */
    0x6b, 0xd5, 0x4e, 0x51, 0x84, 0xeb, 0x30, 0x09, 0x34, 0xb3, 0x92, 0xc3, 0x2b, 0x7c, 0x1a, 0x6e
};
/* NIST vector */
/* fe8901fecd3ccd2ec5fdc7c7a0b50519c245b42d611a5ef9e90268d59f3edf33 */
static const uint8_t NIST_AES256_CBC_key[] =
{
    0xfe, 0x89, 0x01, 0xfe, 0xcd, 0x3c, 0xcd, 0x2e, 0xc5, 0xfd, 0xc7, 0xc7, 0xa0, 0xb5, 0x05, 0x19,
    0xc2, 0x45, 0xb4, 0x2d, 0x61, 0x1a, 0x5e, 0xf9, 0xe9, 0x02, 0x68, 0xd5, 0x9f, 0x3e, 0xdf, 0x33
};
/* NIST vector */
/* 851e8764776e6796aab722dbb644ace8 */
static const uint8_t NIST_AES256_CBC_iv[] =
{
    0xbd, 0x41, 0x6c, 0xb3, 0xb9, 0x89, 0x22, 0x28, 0xd8, 0xf1, 0xdf, 0x57, 0x56, 0x92, 0xe4, 0xd0
};
/* NIST vector */
/* 6282b8c05c5c1530b97d4816ca434762 */
static const uint8_t NIST_AES256_CBC_plaintext[] =
{
    0x8d, 0x3a, 0xa1, 0x96, 0xec, 0x3d, 0x7c, 0x9b, 0x5b, 0xb1, 0x22, 0xe7, 0xfe, 0x77, 0xfb, 0x12,
    0x95, 0xa6, 0xda, 0x75, 0xab, 0xe5, 0xd3, 0xa5, 0x10, 0x19, 0x4d, 0x3a, 0x8a, 0x41, 0x57, 0xd5,
    0xc8, 0x9d, 0x40, 0x61, 0x97, 0x16, 0x61, 0x98, 0x59, 0xda, 0x3e, 0xc9, 0xb2, 0x47, 0xce, 0xd9
};
/* NIST vector */
/* 6acc04142e100a65f51b97adf5172c41 */
static const uint8_t NIST_AES256_CBC_ciphertext[] =
{
    0x60, 0x8e, 0x82, 0xc7, 0xab, 0x04, 0x00, 0x7a, 0xdb, 0x22, 0xe3, 0x89, 0xa4, 0x47, 0x97, 0xfe,
    0xd7, 0xde, 0x09, 0x0c, 0x8c, 0x03, 0xca, 0x8a, 0x2c, 0x5a, 0xcd, 0x9e, 0x84, 0xdf, 0x37, 0xfb,
    0xc5, 0x8c, 0xe8, 0xed, 0xb2, 0x93, 0xe9, 0x8f, 0x02, 0xb6, 0x40, 0xd6, 0xd1, 0xd7, 0x24, 0x64
};

/* NIST vector */
/* Note: In CTR mode, key_ctr is the concatenation of the key and nonce */
/* D0E78C4D0B30D33F5BF4A132B2F94A4A38963511A3904B117E35A37B5AAC8A193BF0D158 */
static const uint8_t key_ctr_256[] =
{
    /* AES Key */
    0xD0, 0xE7, 0x8C, 0x4D, 0x0B, 0x30, 0xD3, 0x3F, 0x5B, 0xF4, 0xA1, 0x32, 0xB2, 0xF9, 0x4A, 0x4A,
    0x38, 0x96, 0x35, 0x11, 0xA3, 0x90, 0x4B, 0x11, 0x7E, 0x35, 0xA3, 0x7B, 0x5A, 0xAC, 0x8A, 0x19,

    /* Nonce */
    0x3B, 0xF0, 0xD1, 0x58,
};
/* NIST vector */
/* A1A31704C8B7E16C */
static const uint8_t iv_ctr_256[] =
{
    0xA1, 0xA3, 0x17, 0x04, 0xC8, 0xB7, 0xE1, 0x6C,
};
/* NIST vector */
/* 981FA33222C5451017530155A4BF7F29 */
static const uint8_t plain_ctr_256[] =
{
    0x98, 0x1F, 0xA3, 0x32, 0x22, 0xC5, 0x45, 0x10, 0x17, 0x53, 0x01, 0x55, 0xA4, 0xBF, 0x7F, 0x29,
};
/* NIST vector */
/* 643B91B4E541B20AAAEAB77F2D328566 */
static const uint8_t secret_ctr_256[] =
{
    0x64, 0x3B, 0x91, 0xB4, 0xE5, 0x41, 0xB2, 0x0A, 0xAA, 0xEA, 0xB7, 0x7F, 0x2D, 0x32, 0x85, 0x66,
};

NX_CRYPTO_METHOD crypto_method_aes_ctr_256 =
{
    NX_CRYPTO_ENCRYPTION_AES_CTR,            /* AES crypto algorithm */
    NX_CRYPTO_AES_256_KEY_LEN_IN_BITS,       /* Key size in bits */
    NX_CRYPTO_AES_IV_LEN_IN_BITS,            /* IV size in bits */
    0,                                       /* ICV size in bits, not used */
    (NX_CRYPTO_AES_BLOCK_SIZE_IN_BITS >> 3), /* Block size in bytes */
    sizeof(NX_CRYPTO_AES),                   /* Size of metadata in bytes */
    _nx_crypto_method_aes_init,              /* AES-CTR initialization routine */
    _nx_crypto_method_aes_cleanup,           /* AES-CTR cleanup routine */
    _nx_crypto_method_aes_ctr_operation      /* AES-CTR operation */
};

/* C4DA057B81EA740B697FFE1B6EB8591356BA6D5EA7F1B96E4F048030449ACD64E4BB271CB4DCF94937E6 */
static const uint8_t key_256[] =
{
    0xC4, 0xDA, 0x05, 0x7B, 0x81, 0xEA, 0x74, 0x0B, 0x69, 0x7F, 0xFE, 0x1B, 0x6E, 0xB8, 0x59, 0x13,
    0x56, 0xBA, 0x6D, 0x5E, 0xA7, 0xF1, 0xB9, 0x6E, 0x4F, 0x04, 0x80, 0x30, 0x44, 0x9A, 0xCD, 0x64,
    0xE4, 0xBB, 0x27, 0x1C, 0xB4, 0xDC, 0xF9, 0x49, 0x37, 0xE6,
};

/* BDACB6555D294D3AFFC245520116062D98F88D64276BDA593492AE71CFE16E46CABC287CB00DF21D96066D5856C2224E
 * EF609D4896302540078F3A0EE325F5337E */
static const uint8_t plain_256[] =
{
    0xBD, 0xAC, 0xB6, 0x55, 0x5D, 0x29, 0x4D, 0x3A, 0xFF, 0xC2, 0x45, 0x52, 0x01, 0x16, 0x06, 0x2D,
    0x98, 0xF8, 0x8D, 0x64, 0x27, 0x6B, 0xDA, 0x59, 0x34, 0x92, 0xAE, 0x71, 0xCF, 0xE1, 0x6E, 0x46,
    0xCA, 0xBC, 0x28, 0x7C, 0xB0, 0x0D, 0xF2, 0x1D, 0x96, 0x06, 0x6D, 0x58, 0x56, 0xC2, 0x22, 0x4E,
    0xEF, 0x60, 0x9D, 0x48, 0x96, 0x30, 0x25, 0x40, 0x07, 0x8F, 0x3A, 0x0E, 0xE3, 0x25, 0xF5, 0x33,
    0x7E,
};

/* 940F986AC891C9000B72EF0CEC69AB66AF002E3A34EB8A3A5F94484E45C0396C */
static const uint8_t secret_256[] =
{
    0x94, 0x0F, 0x98, 0x6A, 0xC8, 0x91, 0xC9, 0x00, 0x0B, 0x72, 0xEF, 0x0C, 0xEC, 0x69, 0xAB, 0x66,
    0xAF, 0x00, 0x2E, 0x3A, 0x34, 0xEB, 0x8A, 0x3A, 0x5F, 0x94, 0x48, 0x4E, 0x45, 0xC0, 0x39, 0x6C,
};

static uint32_t netx_secure_crypto_aes256ctr_multipart_example(void);
static uint32_t netx_secure_crypto_aes256cbc_multipart_example(void);
static uint32_t netx_secure_crypto_hmac_sha256_example(void);
static uint32_t netx_secure_crypto_aes128cbc_with_trng_example(void);
static uint32_t netx_secure_crypto_aes256gcm_multipart_example(void);

/* AES Thread entry function */
void aes_thread_entry(void)
{
    uint8_t read_data = RESET_VALUE;
    UINT status = TX_SUCCESS;
    /* Initialize the RTT Thread */
    rtt_thread_init_check();

    /* Print the banner and EP info */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);

    /* Wait for user input */
    do
    {
        status = check_for_rtt_user_ip(&read_data);
    }
    while (TX_SUCCESS != status);

    gp_sce_resource = &g_sce_mutex;
    /* Set AES event flag */
    status = tx_event_flags_set(&g_user_input_event_flags, ECC_EVENT_FLAG, TX_OR);
    tx_thread_sleep (THREAD_SLEEP_TIME);

    while (true)
    {
        tx_mutex_get((TX_MUTEX *)gp_sce_resource, TX_WAIT_FOREVER);
        /* Call AES-256-CBC multipart example to perform the cryptographic operation */
        status = netx_secure_crypto_aes256cbc_multipart_example();
        if (NX_CRYPTO_SUCCESS != status)
        {
            PRINT_ERR_STR("netx_secure_crypto_aes256cbc_multipart_example failed");
            ERROR_TRAP(status);
        }
        tx_mutex_put((TX_MUTEX *)gp_sce_resource);
        tx_thread_sleep (THREAD_SLEEP_TIME);
        tx_mutex_get((TX_MUTEX *)gp_sce_resource, TX_WAIT_FOREVER);
        /* Call AES-256-CTR multipart example to perform the cryptographic operation */
        status = netx_secure_crypto_aes256ctr_multipart_example();
        if (NX_CRYPTO_SUCCESS != status)
        {
            PRINT_ERR_STR("netx_secure_crypto_aes256ctr_multipart_example failed");
            ERROR_TRAP(status);
        }
        tx_mutex_put((TX_MUTEX *)gp_sce_resource);
        tx_thread_sleep (THREAD_SLEEP_TIME);
        tx_mutex_get((TX_MUTEX *)gp_sce_resource, TX_WAIT_FOREVER);
        /* Call the HMAC-SHA256 example to perform the cryptographic operation */
        status = netx_secure_crypto_hmac_sha256_example();
        if (NX_CRYPTO_SUCCESS != status)
        {
            PRINT_ERR_STR("netx_secure_crypto_hmac_sha256_example failed");
            ERROR_TRAP(status);
        }
        tx_mutex_put((TX_MUTEX *)gp_sce_resource);
        tx_thread_sleep (THREAD_SLEEP_TIME);
        tx_mutex_get((TX_MUTEX *)gp_sce_resource, TX_WAIT_FOREVER);
        status = netx_secure_crypto_aes128cbc_with_trng_example();
        if (NX_CRYPTO_SUCCESS != status)
        {
            PRINT_ERR_STR("netx_secure_crypto_aes128cbc_with_trng_example failed");
            ERROR_TRAP(status);
        }
        tx_mutex_put((TX_MUTEX *)gp_sce_resource);
        tx_thread_sleep (THREAD_SLEEP_TIME);
        tx_mutex_get((TX_MUTEX *)gp_sce_resource, TX_WAIT_FOREVER);
        /* Call AES-256-GCM multipart example to execute the cryptographic operation */
        status = netx_secure_crypto_aes256gcm_multipart_example();
        if (NX_CRYPTO_SUCCESS != status)
        {
            PRINT_ERR_STR("netx_secure_crypto_aes256gcm_multipart_example failed");
            ERROR_TRAP(status);
        }
        tx_mutex_put((TX_MUTEX *)gp_sce_resource);
        tx_thread_sleep (THREAD_SLEEP_TIME);
    }
}

/*******************************************************************************************************************//**
 * @brief     Demonstrates AES-256-CBC mode by performing encryption and decryption on multi-block data.
 * @param     None
 * @retval    NX_CRYPTO_SUCCESS       Operation completed successfully.
 * @retval    Others                  Error code indicating failure.
 **********************************************************************************************************************/
static uint32_t netx_secure_crypto_aes256cbc_multipart_example(void)
{
    uint8_t  metadata[sizeof(NX_CRYPTO_AES)] = {RESET_VALUE};
    uint32_t metadata_size = sizeof(NX_CRYPTO_AES);
    void   * handler       = NX_CRYPTO_NULL;
    uint32_t status        = NX_CRYPTO_SUCCESS;

    /* 3 AES blocks */
    uint8_t generated_ciphertext[3U * NX_CRYPTO_AES_BLOCK_SIZE] = {RESET_VALUE};
    uint8_t generated_plaintext[3U * NX_CRYPTO_AES_BLOCK_SIZE]  = {RESET_VALUE};

    /* Initialize the crypto platform, including SCE and TRNG */
    status = _nx_crypto_initialize();
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-CBC: _nx_crypto_initialize failed");
        return status;
    }

    PRINT_INFO_STR("AES-256-CBC multipart algorithm initialized and started successfully");
    status = _nx_crypto_method_aes_init(&crypto_method_aes_cbc_256, (uint8_t *) NIST_AES256_CBC_key,
                                        sizeof(NIST_AES256_CBC_key) << 3U, &handler, metadata, metadata_size);
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-CBC: _nx_crypto_method_aes_init failed");
        return status;
    }
    /* Encryption */
    status = _nx_crypto_method_aes_operation(NX_CRYPTO_ENCRYPT, handler, &crypto_method_aes_cbc_256,
                                             NULL,
                                             NX_CRYPTO_NULL, (uint8_t*) NIST_AES256_CBC_plaintext,
                                             sizeof(NIST_AES256_CBC_plaintext), (uint8_t*) NIST_AES256_CBC_iv,
                                             generated_ciphertext, sizeof(generated_ciphertext), metadata,
                                             metadata_size,
                                             NX_CRYPTO_NULL,
                                             NX_CRYPTO_NULL);
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-CBC: Encryption operation failed");
        return status;
    }

    /* Verify that the generated ciphertext matches the expected ciphertext */
    status = (uint32_t) memcmp(generated_ciphertext, NIST_AES256_CBC_ciphertext, sizeof(generated_ciphertext));
    if(RESET_VALUE != status)
    {
        PRINT_ERR_STR("AES-256-CBC: Generated ciphertext does not match the expected ciphertext");
        return status;
    }

    /* Decryption */
    status = _nx_crypto_method_aes_operation(NX_CRYPTO_DECRYPT, handler, &crypto_method_aes_cbc_256,
                                             NULL,
                                             NX_CRYPTO_NULL, (uint8_t*) NIST_AES256_CBC_ciphertext,
                                             sizeof(NIST_AES256_CBC_ciphertext), (uint8_t*) NIST_AES256_CBC_iv,
                                             generated_plaintext, sizeof(generated_plaintext),
                                             metadata, metadata_size,
                                             NX_CRYPTO_NULL,
                                             NX_CRYPTO_NULL);
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-CBC: Decryption operation failed");
        return status;
    }

    /* Verify that the decrypted plaintext matches the original plaintext */
    status = (uint32_t) memcmp(generated_plaintext, NIST_AES256_CBC_plaintext, sizeof(generated_ciphertext));
    if(RESET_VALUE != status)
    {
        PRINT_ERR_STR("AES-256-CBC: Decrypted plaintext does not match the original plaintext");
        return status;
    }
    PRINT_INFO_STR("AES-256-CBC multipart algorithm executed successfully. Decrypted plaintext"\
                   " matches original plaintext.");
    /* Clean up the AES metadata */
    status = _nx_crypto_method_aes_cleanup(metadata);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-CBC: _nx_crypto_method_aes_cleanup failed");
        return status;
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief     Demonstrates AES-256-CTR encryption and decryption on multi-block data.
 * @param[IN] None
 * @retval    NX_CRYPTO_SUCCESS on success.
 *            Returns an error code other than NX_CRYPTO_SUCCESS upon failure.
 **********************************************************************************************************************/
static uint32_t netx_secure_crypto_aes256ctr_multipart_example (void)
{
    uint8_t  metadata[sizeof(NX_CRYPTO_AES)] = {RESET_VALUE};
    uint32_t metadata_size = sizeof(NX_CRYPTO_AES);
    void   * handler       = NX_CRYPTO_NULL;
    uint32_t status        = NX_CRYPTO_SUCCESS;

    /* 3 AES blocks */
    uint8_t generated_ciphertext[3U * NX_CRYPTO_AES_BLOCK_SIZE] = {RESET_VALUE};
    uint8_t generated_plaintext[3U * NX_CRYPTO_AES_BLOCK_SIZE]  = {RESET_VALUE};

    /* Initialize the crypto platform, including SCE and TRNG */
    status = _nx_crypto_initialize();
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-CTR: _nx_crypto_initialize failed");
        return status;
    }

    PRINT_INFO_STR("AES-256-CTR multipart algorithm initialized and started successfully");
    status = _nx_crypto_method_aes_init(&crypto_method_aes_ctr_256, (uint8_t*) key_ctr_256,
                                        crypto_method_aes_ctr_256.nx_crypto_key_size_in_bits, &handler, metadata,
                                        metadata_size);
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-CTR: _nx_crypto_method_aes_init failed");
        return status;
    }

    /* Encryption */
    status = _nx_crypto_method_aes_operation(NX_CRYPTO_ENCRYPT, handler, &crypto_method_aes_ctr_256,
                                             (uint8_t*) key_ctr_256,
                                             crypto_method_aes_ctr_256.nx_crypto_key_size_in_bits,
                                             (uint8_t*) plain_ctr_256, sizeof(plain_ctr_256), (uint8_t*) iv_ctr_256,
                                             generated_ciphertext, sizeof(generated_ciphertext), metadata,
                                             metadata_size,
                                             NX_CRYPTO_NULL,
                                             NX_CRYPTO_NULL);
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-CTR: Encryption operation failed");
        return status;
    }

    /* Verify that the generated ciphertext matches the expected ciphertext */
    status = (uint32_t) memcmp(generated_ciphertext, secret_ctr_256, sizeof(secret_ctr_256));
    if(RESET_VALUE != status)
    {
        PRINT_ERR_STR("AES-256-CTR: Generated ciphertext does not match the expected ciphertext");
        return status;
    }

    /* Decryption */
    status = _nx_crypto_method_aes_operation(NX_CRYPTO_DECRYPT, handler, &crypto_method_aes_ctr_256,
                                             (uint8_t*) key_ctr_256,
                                             crypto_method_aes_ctr_256.nx_crypto_key_size_in_bits,
                                             (uint8_t*) secret_ctr_256, sizeof(secret_ctr_256), (uint8_t*) iv_ctr_256,
                                             generated_plaintext, sizeof(generated_plaintext),
                                             metadata, metadata_size,
                                             NX_CRYPTO_NULL,
                                             NX_CRYPTO_NULL);
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-CTR: Decryption operation failed");
        return status;
    }

    /* Verify that the decrypted plaintext matches the original plaintext */
    status = (uint32_t) memcmp(generated_plaintext, plain_ctr_256, sizeof(plain_ctr_256));
    if(RESET_VALUE != status)
    {
        PRINT_ERR_STR("AES-256-CTR: Decrypted plaintext does not match the original plaintext");
        return status;
    }
    PRINT_INFO_STR("AES-256-CTR multipart algorithm executed successfully. Decrypted plaintext"\
                   " matches original plaintext.");

    /* Clean up the AES metadata */
    status = _nx_crypto_method_aes_cleanup(metadata);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-CTR: _nx_crypto_method_aes_cleanup failed");
        return status;
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief     Example function demonstrating HMAC-SHA256 generation and verification on input data.
 * @param     None
 * @retval    NX_CRYPTO_SUCCESS on successful execution.
 * @retval    Error code other than NX_CRYPTO_SUCCESS if the operation fails.
 **********************************************************************************************************************/
static uint32_t netx_secure_crypto_hmac_sha256_example(void)
{
    uint8_t  output[RM_NETX_SECURE_CRYPTO_EXAMPLE_SHA256_HASH_SIZE_BYTES] = {RESET_VALUE};
    uint8_t  metadata[sizeof(NX_CRYPTO_SHA256_HMAC)] = {RESET_VALUE};
    uint32_t metadata_size = sizeof(NX_CRYPTO_SHA256_HMAC);
    void   * handler       = NX_CRYPTO_NULL;
    uint32_t status        = NX_CRYPTO_SUCCESS;
    /* Initialize the crypto platform, including SCE and TRNG */
    status = _nx_crypto_initialize();
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("HMAC-SHA256: _nx_crypto_initialize failed");
        return status;
    }
    PRINT_INFO_STR("HMAC-SHA256 algorithm initialized and started successfully");

    /* Initialize HMAC-SHA256 using NetX Crypto */
    status = _nx_crypto_method_hmac_sha256_init(&crypto_method_hmac_sha256, (UCHAR*) key_256,
                                                RM_NETX_SECURE_CRYPTO_BYTES_TO_BITS(sizeof(key_256)), &handler,
                                                metadata, metadata_size);
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("HMAC-SHA256: _nx_crypto_method_hmac_sha256_init failed");
        return status;
    }
    /* Perform HMAC-SHA256 operation: NX_CRYPTO_HASH_INITIALIZE using NetX Crypto */
    status = _nx_crypto_method_hmac_sha256_operation(NX_CRYPTO_HASH_INITIALIZE, handler, &crypto_method_hmac_sha256,
                                                     (UCHAR*) key_256,
                                                     RM_NETX_SECURE_CRYPTO_BYTES_TO_BITS(sizeof(key_256)),
                                                     NX_CRYPTO_NULL,
                                                     NX_CRYPTO_NULL,
                                                     NX_CRYPTO_NULL,
                                                     NX_CRYPTO_NULL, NX_CRYPTO_NULL, metadata, metadata_size,
                                                     NX_CRYPTO_NULL,
                                                     NX_CRYPTO_NULL);
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("HMAC-SHA256: Hash initialization failed");
        return status;
    }

    /* Perform HMAC-SHA256 operation: NX_CRYPTO_HASH_UPDATE using NetX Crypto */
    status = _nx_crypto_method_hmac_sha256_operation(NX_CRYPTO_HASH_UPDATE, handler, &crypto_method_hmac_sha256,
                                                     NX_CRYPTO_NULL,
                                                     NX_CRYPTO_NULL, (UCHAR*) plain_256, sizeof(plain_256),
                                                     NX_CRYPTO_NULL,
                                                     NX_CRYPTO_NULL, NX_CRYPTO_NULL, metadata, metadata_size,
                                                     NX_CRYPTO_NULL,
                                                     NX_CRYPTO_NULL);
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("HMAC-SHA256: Hash update failed");
        return status;
    }
    /* Perform HMAC-SHA256 operation: NX_CRYPTO_HASH_CALCULATE using NetX Crypto */
    status = _nx_crypto_method_hmac_sha256_operation(NX_CRYPTO_HASH_CALCULATE, handler, &crypto_method_hmac_sha256,
                                                     NX_CRYPTO_NULL,
                                                     NX_CRYPTO_NULL,
                                                     NX_CRYPTO_NULL,
                                                     NX_CRYPTO_NULL,
                                                     NX_CRYPTO_NULL,
                                                     (UCHAR*) output, sizeof(output), metadata, metadata_size,
                                                     NX_CRYPTO_NULL,
                                                     NX_CRYPTO_NULL);
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("HMAC-SHA256: Hash calculation failed");
        return status;
    }
    /* Verify that the generated HMAC-SHA256 MAC matches the expected MAC */
    status = (uint32_t) memcmp(output, secret_256, sizeof(secret_256));
    if(RESET_VALUE != status)
    {
        PRINT_ERR_STR("HMAC-SHA256: Generated MAC does not match the expected MAC");
        return status;
    }
    /* Clean up the HMAC metadata */
    status = _nx_crypto_method_hmac_cleanup(metadata);
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_hmac_cleanup failed");
        return status;
    }
    PRINT_INFO_STR("HMAC-SHA256 algorithm executed successfully");
    return status;
}

/*******************************************************************************************************************//**
 * @brief     Example function demonstrating AES-128-CBC encryption and decryption using a random key generated by TRNG.
 * @param[IN] None
 * @retval    NX_CRYPTO_SUCCESS on success.
 * @retval    Error code other than NX_CRYPTO_SUCCESS on failure.
 **********************************************************************************************************************/
static uint32_t netx_secure_crypto_aes128cbc_with_trng_example (void)
{
   UINT status          = NX_CRYPTO_SUCCESS;
   uint32_t random_data[FOUR] = {RESET_VALUE};
   uint8_t  metadata[sizeof(NX_CRYPTO_AES)] = {RESET_VALUE};
   uint32_t metadata_size = sizeof(NX_CRYPTO_AES);
   void   * handler       = NX_CRYPTO_NULL;
   fsp_err_t err = FSP_SUCCESS;
    /* 3 AES blocks */
    uint8_t generated_ciphertext[3U * NX_CRYPTO_AES_BLOCK_SIZE] = {RESET_VALUE};
    uint8_t generated_plaintext[3U * NX_CRYPTO_AES_BLOCK_SIZE]  = {RESET_VALUE};

    /* Initialize the crypto platform, including SCE and TRNG */
    status = _nx_crypto_initialize();
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-128-CBC: _nx_crypto_initialize failed");
        return status;
    }

    PRINT_INFO_STR("AES-128-CBC with TRNG algorithm initialized and started successfully");
    /* Generate 4 bytes of random data using the hardware SCE */
    err = HW_SCE_RNG_Read(random_data);
    if(FSP_SUCCESS != status)
    {
        PRINT_ERR_STR("HW_SCE_RNG_Read API failed");
        return err;
    }
    /* Initialize nx_crypto with AES-128-CBC method */
    status = _nx_crypto_method_aes_init(&crypto_method_aes_cbc_128, (uint8_t*) random_data, AES_TRNG_DATA_SIZE,
                                        &handler, metadata, metadata_size);
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-128-CBC: _nx_crypto_method_aes_init failed");
        return status;
    }
    /* Encryption */
    status = _nx_crypto_method_aes_operation(NX_CRYPTO_ENCRYPT, handler, &crypto_method_aes_cbc_128,
                                             NULL,
                                             NX_CRYPTO_NULL, (uint8_t*) random_data, sizeof(random_data),
                                             (uint8_t*) random_data, generated_ciphertext,
                                             sizeof(generated_ciphertext), metadata, metadata_size,
                                             NX_CRYPTO_NULL,
                                             NX_CRYPTO_NULL);
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-128-CBC:  Encryption operation failed");
        return status;
    }

    /* Decryption */
    status = _nx_crypto_method_aes_operation (NX_CRYPTO_DECRYPT, handler, &crypto_method_aes_cbc_128,
                                              NULL,
                                              NX_CRYPTO_NULL, (uint8_t*) generated_ciphertext,
                                              sizeof(generated_ciphertext), (uint8_t*) random_data,
                                              generated_plaintext,
                                              sizeof(generated_plaintext), metadata, metadata_size,
                                              NX_CRYPTO_NULL,
                                              NX_CRYPTO_NULL);
    if(NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-128-CBC: Decryption operation failed");
        return status;
    }
    /* Verify that the decrypted plaintext matches the original plaintext */
    status = (uint32_t) memcmp(generated_plaintext, random_data, sizeof(random_data));
    if(RESET_VALUE != status)
    {
        PRINT_ERR_STR("AES-128-CBC: Decrypted plaintext does not match the original plaintext");
        return status;
    }
    PRINT_INFO_STR("AES-128-CBC algorithm with TRNG executed successfully");
    /* Clean up the AES metadata */
    status = _nx_crypto_method_aes_cleanup(metadata);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-128-CBC: _nx_crypto_method_aes_cleanup failed");
        return status;
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief     Example function demonstrating AES-256-GCM encryption and decryption using the crypto algorithm.
 * @param[in] None
 * @retval    NX_CRYPTO_SUCCESS on success.
 *            Returns an error code other than NX_CRYPTO_SUCCESS on failure.
 **********************************************************************************************************************/
static uint32_t netx_secure_crypto_aes256gcm_multipart_example(void)
{
    uint8_t  metadata[sizeof(NX_CRYPTO_AES)];
    uint32_t metadata_size = sizeof(NX_CRYPTO_AES);
    void   * handler       = NX_CRYPTO_NULL;
    uint32_t status           = NX_CRYPTO_SUCCESS;
    /* 3 AES Blocks */
    uint8_t generated_ciphertext[3U * NX_CRYPTO_AES_BLOCK_SIZE] = {0};
    uint8_t generated_plaintext[3U * NX_CRYPTO_AES_BLOCK_SIZE]  = {0};
    /* Initialize the crypto platform, including SCE and TRNG */
    status = _nx_crypto_initialize();
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-GCM: _nx_crypto_initialize failed");
        return status;
    }
    PRINT_INFO_STR("AES-256-GCM multipart algorithm initialized and started successfully");
    status = _nx_crypto_method_aes_init(&crypto_method_aes_256_gcm_16,
                                        (uint8_t *) key_gcm_256,
                                        sizeof(key_gcm_256) << 3U,
                                        &handler,
                                        metadata,
                                        metadata_size);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-GCM: _nx_crypto_method_aes_init failed");
        return status;
    }
    /* Set up Additional Authentication Data (AAD) for AES-256-GCM authentication */
    status = _nx_crypto_method_aes_operation(NX_CRYPTO_SET_ADDITIONAL_DATA,
                                             handler,
                                             &crypto_method_aes_256_gcm_16,
                                             NULL,
                                             0,
                                             (uint8_t *) aad_gcm_128,
                                             sizeof(aad_gcm_128),
                                             NULL,
                                             NULL,
                                             0,
                                             metadata,
                                             metadata_size,
                                             NX_CRYPTO_NULL,
                                             NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-GCM: Failed to set Additional Authentication Data (AAD)");
        return status;
    }
    /* Encryption */
    status = _nx_crypto_method_aes_operation(NX_CRYPTO_ENCRYPT,
                                             handler,
                                             &crypto_method_aes_256_gcm_16,
                                             NULL,
                                             0,
                                             (uint8_t *) plain_gcm_128,
                                             sizeof(plain_gcm_128),
                                             (uint8_t *) iv_gcm_256,
                                             generated_ciphertext,
                                             sizeof(generated_ciphertext),
                                             metadata,
                                             metadata_size,
                                             NX_CRYPTO_NULL,
                                             NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-GCM: Encryption operation failed");
        return status;
    }
    /* The 16 byte tag is appended to the generated ciphertext */
    /* Verify that the generated tag matches the expected tag */
    status = (uint32_t) memcmp(&generated_ciphertext[sizeof(plain_gcm_128)],
                               &secret_gcm_128[sizeof(plain_gcm_128)], 16U);
    if (ZERO != status)
    {
        PRINT_ERR_STR("AES-256-GCM: Generated tag does not match expected tag");
        return status;
    }
    /* Verify that the generated ciphertext matches the expected ciphertext */
    status = (uint32_t) memcmp(generated_ciphertext, secret_gcm_128, sizeof(secret_gcm_128));
    if (ZERO != status)
    {
        PRINT_ERR_STR("AES-256-GCM: Generated ciphertext does not match the expected ciphertext");
        return status;
    }
    /* Set Additional Authentication Data again for decryption */
    status = _nx_crypto_method_aes_operation(NX_CRYPTO_SET_ADDITIONAL_DATA,
                                             handler,
                                             &crypto_method_aes_256_gcm_16,
                                             NULL,
                                             0,
                                             (uint8_t *) aad_gcm_128,
                                             sizeof(aad_gcm_128),
                                             NULL,
                                             NULL,
                                             0,
                                             metadata,
                                             metadata_size,
                                             NX_CRYPTO_NULL,
                                             NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-GCM: Failed to set Additional Authentication Data (AAD) before decryption");
        return status;
    }
    /* Decryption */
    status = _nx_crypto_method_aes_operation(NX_CRYPTO_DECRYPT,
                                             handler,
                                             &crypto_method_aes_256_gcm_16,
                                             NULL,
                                             0,
                                             (uint8_t *) generated_ciphertext,
                                             sizeof(secret_gcm_128), /* ciphertext size + tag size */
                                             (uint8_t *) iv_gcm_256,
                                             generated_plaintext,
                                             sizeof(generated_plaintext),
                                             metadata,
                                             metadata_size,
                                             NX_CRYPTO_NULL,
                                             NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-GCM: Decryption operation failed");
        return status;
    }
    /* Verify that the decrypted plaintext matches the input plaintext */
    status = (uint32_t) memcmp(generated_plaintext, plain_gcm_128, sizeof(plain_gcm_128));
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-GCM: Decrypted plaintext does not match the original plaintext");
        return status;
    }
    PRINT_INFO_STR("AES-256-GCM algorithm executed successfully");
    status = _nx_crypto_method_aes_cleanup(metadata);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("AES-256-GCM: _nx_crypto_method_aes_cleanup failed");
        return status;
    }
    return status;
}

/*******************************************************************************************************************//**
 * @} (end defgroup netx_crypto_ep)
 **********************************************************************************************************************/
