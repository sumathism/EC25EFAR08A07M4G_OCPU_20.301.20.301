/**
 * @file ql_customer_at.h
 * @brief Quectel customer AT command lib public function declarations.
 * 
 * @note
 *
 * @copyright Copyright (c) 2009-2017 @ Quectel Wireless Solutions Co., Ltd.
 */

#ifndef __QL_CUSTOMER_AT_H__
#define __QL_CUSTOMER_AT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <sys/un.h>

/**
 * Get socket fd to receive at request from atfwd_daemon process
 *
 * @param
 *   None
 *
 * @return 
 *   On success, 0 is returned, otherwise -1 is returned.
 */
extern int ql_customer_at_get_socket(void);

/**
 * Get request at command from atfwd_daemon process
 *
 * @param [out]request           the request buffer
 * @param [in]len                the maximun length of request buffer
 *
 * @return 
 *   On success, 0 is returned, otherwise -1 is returned.
 */
extern int ql_customer_at_get_request(int sockfd, struct sockaddr_un *src_un, char *request, int size);

/**
 * send at command response to atfwd_daemon process
 *
 * @param [in]response_satus     the at response status, 0 is ERROR, 1 is OK
 * @param [in]response           the response buffer
 * @param [in]len                the maximun length of request buffer
 *
 * @return 
 *   On success, 0 is returned, otherwise -1 is returned.
 */
extern int ql_customer_at_send_response(int sockfd, struct sockaddr_un *src_un, bool response_status, char *response, int size);

#ifdef __cplusplus
}
#endif

#endif /* end of __QL_CUSTOMER_AT_H__ */
