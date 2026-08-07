#ifndef __QL_ZIP_VERIFY_H_
#define __QL_ZIP_VERIFY_H_

#include <stdint.h>

#define QL_ZIP_PATH_LEN_MAX              128
#define QL_ZIP_CERT_LEN_MAX              2048

/**
 * @brief This function will be used to verify the zip file signature
 *
 * @param[in] zip_path       zip file path
 * @param[in] zip_path_len   zip file path length
 * @param[in] cert           The certificate includes: certificate file, public key file, certificate buffer, public key buffer
 * @param[in] cert_len       certificate file name length or buffer length
 * 
 * @return 0 on success, negative on failure
 */
int ql_zip_verify(const char *zip_path, int zip_path_len, const char *cert, int cert_len);

typedef enum {
    E_SUCCESS    = 0,
    E_INVAL      = 1, //invalid argument
    E_ZIPNE      = 2, //zip file does not exist
    E_CERTNE     = 3, //certificate file does not exist
    E_ZIPFMT     = 4, //not in zip format
    E_CERTFMT    = 5, //certificate format fault
    E_VERIFY     = 6, //verify error
    E_NMEM       = 7, //malloc error
    E_FOPEN      = 8, //fopen failed
    E_EOCDR      = 9, //invalid zip archive: EOCDR not found
}APP_ERROR_TYPE;

#endif /* __QL_ZIP_VERIFY_H_ */
