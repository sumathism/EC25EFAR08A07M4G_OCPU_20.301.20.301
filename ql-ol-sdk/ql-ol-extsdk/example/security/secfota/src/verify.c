#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <openssl/sha.h>
#include <openssl/ec.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ossl_typ.h>
#include <openssl/rsa.h> 

#include "fota_log.h"
#include "ql_fota_verify.h"

#define ZIP_LOCAL_FILE_HEADER_SIGNATURE   (0x04034b50)
#define ZIP_END_OF_CENTRAL_DIR_SIGNATURE  (0x06054b50)
#define ZIP_COMMENT_SIZE_MAX          (0xffff)
#define ZIP_COMMENT_HASH_MAX          128

#define MAX_DATA_LEN                  1024
#define SIGNATURE_HEAD_LEN            16

typedef enum {
    QL_VERIFY_TYPE_NONE        = 0,
    QL_VERIFY_TYPE_CERT        = 1,
    QL_VERIFY_TYPE_PUBKEY      = 2 ,
    QL_VERIFY_TYPE_CERT_MEM    = 3,
    QL_VERIFY_TYPE_PUBKEY_MEM  = 4 ,
} ql_verify_type_e;


#pragma pack(1) // 1 byte alignment
typedef struct {
    int8_t signature[4];
    uint16_t disk_num;
    uint16_t start_disk_num;
    uint16_t eocd_total_entries_disk;
    uint16_t eocd_total;
    uint32_t eocd_size;
    uint32_t eocd_start_offset;
    uint16_t eocd_commet_len;
    int8_t eocd_commet[0];
} zip_eocd_t;
#pragma pack()

typedef struct zip_handle_s {
    char zip_path[QL_ZIP_PATH_LEN_MAX+1];
    char verify_path[QL_ZIP_PATH_LEN_MAX+1];
    char verify_mem[QL_ZIP_CERT_LEN_MAX+1];
    ql_verify_type_e verify_mode;

    int eocd_comment_offset;
    char hash_with_comment_str[ZIP_COMMENT_HASH_MAX];

    zip_eocd_t *eocd;
} zip_handle_t;

static void zip_handle_deinit(zip_handle_t *zip)
{
    if (zip->eocd) {
        free(zip->eocd); zip->eocd = NULL;
    } 
}

static int zip_handle_init(zip_handle_t *zip)
{
    int ret = E_SUCCESS;

    memset(zip, 0, sizeof(zip_handle_t));

    zip->eocd = (zip_eocd_t *) malloc(sizeof (zip_eocd_t) + ZIP_COMMENT_SIZE_MAX);
    ZIPVERIFY_CHK_MSG((zip->eocd == NULL), -E_NMEM, end, "eocd is NULL\n");
end:
    return ret;
}

/**
 * quectel zip comment struct: 16 Bytes head + n Bytes signature
 * magic: 8 Bytes
 * reserve: 6 Bytes
 * signature length: 2 Bytes
 * signature: n Bytes
 */
static int signature_segment_check(zip_handle_t *zip)
{
    int ret = E_SUCCESS;
    int signature_len = 0;

    ZIPVERIFY_CHK_MSG((strncmp((const char *)zip->eocd->eocd_commet, "secfota!", 8) != 0), -E_INVAL, end, "comment magic match failed.\n");

    signature_len = (int)(zip->eocd->eocd_commet[14] | zip->eocd->eocd_commet[15] << 8 );
    ZIPVERIFY_CHK_MSG((signature_len > ZIP_COMMENT_SIZE_MAX - SIGNATURE_HEAD_LEN || signature_len <= SIGNATURE_HEAD_LEN), 
                        -E_INVAL, end, "signature length is invalid.\n");

end:
    return ret;
}

static int zip_get_eocd_comment(zip_handle_t *zip)
{
    int ret = E_SUCCESS;
    FILE *fp = NULL;
    char *buffer = NULL;
    bool found = false;
    unsigned int zip_length = 0;
    unsigned int buffer_size = 0;
    unsigned int offset = 0;
    unsigned int act_size = 0;
    unsigned int pos = 0;

    fp = fopen(zip->zip_path, "rb");
    ZIPVERIFY_CHK_MSG((fp == NULL), -E_FOPEN, end, "Open zip file failed.\n");

    fseek(fp, 0, SEEK_END);
    zip_length = ftell(fp);
    buffer_size = zip_length < (sizeof (zip_eocd_t) + ZIP_COMMENT_SIZE_MAX) ? \
                    zip_length : (sizeof (zip_eocd_t) + ZIP_COMMENT_SIZE_MAX);
    offset = zip_length - buffer_size;
    fseek(fp, offset, SEEK_SET);

    buffer = malloc(buffer_size);
    ZIPVERIFY_CHK_MSG((buffer == NULL), -E_NMEM, end, "malloc failed.\n");

    act_size = fread(buffer, 1, buffer_size, fp);
    pos = act_size - sizeof (zip_eocd_t);
    while ((!found) && (pos > 0)) {
        if (   (buffer[pos + 3] == ((ZIP_END_OF_CENTRAL_DIR_SIGNATURE>>24) & 0xff))
            && (buffer[pos + 2] == ((ZIP_END_OF_CENTRAL_DIR_SIGNATURE>>16) & 0xff))
            && (buffer[pos + 1] == ((ZIP_END_OF_CENTRAL_DIR_SIGNATURE>>8) & 0xff))
            && (buffer[pos + 0] == (ZIP_END_OF_CENTRAL_DIR_SIGNATURE & 0xff)) ) {
            found = true;
        } else {
            pos--;
        }
    }
    ZIPVERIFY_CHK_MSG((found != true), -E_EOCDR, end, "not fount eocd signature.\n");

    fseek(fp, offset + pos, SEEK_SET);
    act_size = fread(buffer, 1, buffer_size, fp);
    memcpy(zip->eocd, buffer, act_size);
    zip->eocd_comment_offset = offset + pos + sizeof (zip_eocd_t);

    if (act_size - sizeof (zip_eocd_t) > SIGNATURE_HEAD_LEN ) {
        ret = signature_segment_check(zip);
        ZIPVERIFY_CHK_MSG((ret != E_SUCCESS), ret, end, "signature segment check failed.\n");
    } else {
        ret = -E_INVAL;
        goto end;
    }
    
end:
    if (buffer) { free(buffer); buffer = NULL; }
    if (fp) { fclose(fp); fp = NULL; }
    return ret;
}

static int zip_calcu_dgst_without_ecod_comment(zip_handle_t *zip)
{
    int ret = 0;
    FILE *fp = NULL;
    SHA512_CTX sha384_ctx = {0};
    int t = 0;
    unsigned int act_size = 0;
    int len = 0;
    int idx = 0;
    int i;

    char data[MAX_DATA_LEN] = {0};
    unsigned char SHA384result[SHA384_DIGEST_LENGTH] = {0};

    fp = fopen(zip->zip_path, "rb");
    ZIPVERIFY_CHK_MSG((fp == NULL), -E_FOPEN, end, "open zip file failed.\n");

    SHA384_Init(&sha384_ctx);

    int multiplier = zip->eocd_comment_offset/MAX_DATA_LEN;
    int remainder = zip->eocd_comment_offset%MAX_DATA_LEN;

    while (multiplier > 0)
    {
        memset(data, 0x00, sizeof(data));
        act_size = fread(data, 1, MAX_DATA_LEN, fp);
        if(act_size) {
            t += act_size;
            SHA384_Update(&sha384_ctx, data, act_size);
        }
        multiplier --;
    }
    memset(data, 0x00, sizeof(data));
    act_size = fread(data, 1, remainder, fp);
    data[remainder-1] = 0x00;
    data[remainder-2] = 0x00;
    if (act_size) {
        t += act_size;
        SHA384_Update(&sha384_ctx, data, act_size);
    }
    SHA384_Final(SHA384result, &sha384_ctx);

    for (i = 0; i < SHA384_DIGEST_LENGTH; i++) {
        len = snprintf(zip->hash_with_comment_str + idx, sizeof(zip->hash_with_comment_str) - idx, "%02x", SHA384result[i]);
        idx += len;
    }
    QLOG_DEBUG("comment hash str %s\n", zip->hash_with_comment_str);

end:
    if (fp) {
        fclose(fp); fp = NULL;
    }

    return ret;
}

static int zip_verify_signature_with_eocd_comment(zip_handle_t *zip)
{
    int ret = E_SUCCESS;
    FILE *fp = NULL;
    BIO *bio = NULL;
    EC_KEY *eckey = NULL;
    RSA *rsakey = NULL;
    EVP_PKEY *pkey = NULL;
    X509 *cert = NULL;
    int key_id = 0;
    SHA512_CTX sha384_ctx = {0};
    unsigned char digest[SHA512_DIGEST_LENGTH] = {0};
    int signature_len = 0;
    unsigned char *signature = NULL;

    signature_len = (int)(zip->eocd->eocd_commet[14] | zip->eocd->eocd_commet[15] << 8 );
    signature = (unsigned char*)malloc(signature_len);
    ZIPVERIFY_CHK_MSG((signature == NULL), -E_NMEM, end, "malloc failed.\n");
    memcpy(signature, zip->eocd->eocd_commet+SIGNATURE_HEAD_LEN, signature_len);

    if(zip->verify_mode ==  QL_VERIFY_TYPE_PUBKEY) {
        bio = BIO_new(BIO_s_file());
        ZIPVERIFY_CHK_MSG((bio == NULL), -E_NMEM, end, "BIO new failed.\n");

        ret = BIO_read_filename(bio, zip->verify_path);
        ZIPVERIFY_CHK_MSG((ret != 1), -E_CERTFMT, end, "BIO read public key file failed.\n");

        pkey = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
        ZIPVERIFY_CHK_MSG((pkey == NULL), -E_CERTFMT, end, "PEM read public key failed.\n");

    } else if(zip->verify_mode ==  QL_VERIFY_TYPE_CERT) {
        fp = fopen(zip->verify_path, "r");
        ZIPVERIFY_CHK_MSG((fp == NULL), -E_FOPEN, end, "open cert file failed.\n");

        cert = PEM_read_X509(fp, &cert, NULL, NULL);
        ZIPVERIFY_CHK_MSG((cert == NULL), -E_CERTFMT, end, "read x509 pem failed.\n");

        pkey = X509_get_pubkey(cert);
        ZIPVERIFY_CHK_MSG((pkey == NULL), -E_CERTFMT, end, "x509 get public key failed.\n");

    } else if(zip->verify_mode ==  QL_VERIFY_TYPE_PUBKEY_MEM) {
        bio = BIO_new_mem_buf(zip->verify_mem, -1);
        ZIPVERIFY_CHK_MSG((bio == NULL), -E_CERTFMT, end, "BIO_new_mem_buf failed.\n");

        pkey = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
        ZIPVERIFY_CHK_MSG((pkey == NULL), -E_CERTFMT, end, "read public key failed.\n");

    } else if(zip->verify_mode ==  QL_VERIFY_TYPE_CERT_MEM) {
        bio = BIO_new_mem_buf(zip->verify_mem, strlen(zip->verify_mem));
        ZIPVERIFY_CHK_MSG((bio == NULL), -E_CERTFMT, end, "BIO_new_mem_buf failed.\n");

        cert = PEM_read_bio_X509(bio, NULL, 0, NULL);
        ZIPVERIFY_CHK_MSG((cert == NULL), -E_CERTFMT, end, "read x509 pem failed.\n");

        pkey = X509_get_pubkey(cert);
        ZIPVERIFY_CHK_MSG((pkey == NULL), -E_CERTFMT, end, "x509 get pubkey failed.\n");
    } 

    key_id = EVP_PKEY_id(pkey);

    if (key_id == NID_X9_62_id_ecPublicKey) {
        eckey = EVP_PKEY_get1_EC_KEY(pkey);
    } else if (key_id == NID_rsaEncryption) {
        rsakey = EVP_PKEY_get1_RSA(pkey);
    } else {
        ret = -E_CERTFMT;
        goto end;
    }

    SHA384_Init(&sha384_ctx);
    SHA384_Update(&sha384_ctx, zip->hash_with_comment_str, strlen(zip->hash_with_comment_str));
    SHA384_Final(digest, &sha384_ctx);

    /*do verify*/
    if (eckey) {
        ret = ECDSA_verify(0, (const unsigned char*)digest, SHA384_DIGEST_LENGTH, 
                        (const unsigned char *)signature, signature_len, eckey);
    } else {
        ret = RSA_verify(NID_sha384, (const unsigned char*)digest, SHA384_DIGEST_LENGTH,
                (const unsigned char *)signature, signature_len, rsakey);
    }

    if (ret != 1) {
        QLOG_DEBUG("Verified failed.\n");
        ret = -E_VERIFY;
        goto end;
    }
    QLOG_DEBUG("Verified OK\n");
    ret = E_SUCCESS;
    
end:
    if (pkey) { EVP_PKEY_free(pkey); pkey = NULL; }
    if (bio) { BIO_free(bio); bio = NULL; }
    if (eckey) { EC_KEY_free(eckey); eckey = NULL; }
    if (rsakey) { RSA_free(rsakey); rsakey = NULL; }
    if (cert) { X509_free(cert); cert = NULL; }
    if (fp) { fclose(fp); fp = NULL; }
    if (signature) {free(signature); signature = NULL;}

    return ret;
}

static int zip_verify_signature(zip_handle_t *zip)
{
    int ret = E_SUCCESS;

    ret = zip_get_eocd_comment(zip);
    ZIPVERIFY_CHK_MSG((ret != E_SUCCESS), ret, end, "get zip eocd comment failed.\n");

    ret = zip_calcu_dgst_without_ecod_comment(zip);
    ZIPVERIFY_CHK_MSG((ret != E_SUCCESS), ret, end, "calcu zip digest without ecod comment failed.\n");

    ret = zip_verify_signature_with_eocd_comment(zip);
    ZIPVERIFY_CHK_MSG((ret != E_SUCCESS), ret, end, "verify zip comment signature failed.\n");

end:
    return ret;
}

static int zip_check_format(const char *zip_path)
{
    int ret = E_SUCCESS;
    FILE *fp = NULL;
    int ziphead_size = 0;
    unsigned char ziphead[16] = {0};

    fp = fopen(zip_path, "rb");
    ZIPVERIFY_CHK_MSG((fp == NULL), -E_FOPEN, end, "open zip file failed.\n");

    ziphead_size = fread(ziphead, 1, 4, fp);
    ZIPVERIFY_CHK_MSG((ziphead_size != 4), -E_ZIPFMT, end, "read zip file head failed.\n");

    if (   (ziphead[3] == ((ZIP_LOCAL_FILE_HEADER_SIGNATURE>>24) & 0xff))
        && (ziphead[2] == ((ZIP_LOCAL_FILE_HEADER_SIGNATURE>>16) & 0xff))
        && (ziphead[1] == ((ZIP_LOCAL_FILE_HEADER_SIGNATURE>>8) & 0xff))
        && (ziphead[0] == (ZIP_LOCAL_FILE_HEADER_SIGNATURE & 0xff)) ) {
        ret = E_SUCCESS;
    } else {
        ret = -E_ZIPFMT;
    }
end:
    if (fp) {
        fclose(fp); fp = NULL;
    }

    return ret;
}


static int zip_path_init(zip_handle_t *zip, const char *zip_path, int path_len)
{
    int ret = E_SUCCESS;

    ret = access(zip_path, F_OK);
    ZIPVERIFY_CHK_MSG((ret != E_SUCCESS), -E_ZIPNE, end, "zip file don't exist.\n");

    ret = zip_check_format(zip_path);
    ZIPVERIFY_CHK_MSG((ret != E_SUCCESS), ret, end, "zip file not in zip format.\n");

    snprintf(zip->zip_path, path_len+1, "%s", zip_path);

end:
    return ret;
}

static int zip_verify_switch_cert(zip_handle_t *zip, const char *cert, int cert_len)
{
    int ret = E_SUCCESS;
    char *head = NULL;
    size_t head_len = 0;

    if (strstr(cert, "-----BEGIN CERTIFICATE-----") != NULL) {
        zip->verify_mode = QL_VERIFY_TYPE_CERT_MEM;
    } else {
        ret = -E_CERTFMT;
        goto end;
    }

    snprintf(zip->verify_mem, cert_len+1, "%s", cert);  
end:
    return ret;
}

int ql_zip_verify(const char *zip_path, int zip_path_len, \
                    const char *cert, int cert_len)
{
    int ret = E_SUCCESS;
    zip_handle_t zip_handle = {0};

    ZIPVERIFY_CHK_MSG((zip_path_len > QL_ZIP_PATH_LEN_MAX), -E_INVAL, end, "the len value out of length.\n");
    ZIPVERIFY_CHK_MSG((zip_path == NULL), -E_INVAL, end, "the zip path value is NULL.\n");
    ZIPVERIFY_CHK_MSG((strlen(zip_path) != zip_path_len), -E_INVAL, end, "the zip path dosen't match the length.\n");

    ZIPVERIFY_CHK_MSG((cert_len > QL_ZIP_CERT_LEN_MAX), -E_INVAL, end, "the cert len value out of length.\n");
    ZIPVERIFY_CHK_MSG((cert == NULL), -E_INVAL, end, "the cert path value is NULL.\n");
    ZIPVERIFY_CHK_MSG((strlen(cert) != cert_len), -E_INVAL, end, "the cert path dosen't match the length.\n");

    ret = zip_handle_init(&zip_handle);
    ZIPVERIFY_CHK_MSG((ret != E_SUCCESS), ret, end, "zip handle init failed.\n");

    ret = zip_path_init(&zip_handle, zip_path, zip_path_len);
    ZIPVERIFY_CHK_MSG((ret != E_SUCCESS), ret, end, "zip path init failed.\n");

    ret = zip_verify_switch_cert(&zip_handle, cert, cert_len);
    ZIPVERIFY_CHK_MSG((ret != E_SUCCESS), ret, end, "zip verify switch cert failed.\n");

    ret = zip_verify_signature(&zip_handle);
    ZIPVERIFY_CHK_MSG((ret != E_SUCCESS), ret, end, "verify zip signature failed.\n");

    zip_handle_deinit(&zip_handle);
end:
    return ret;
}
