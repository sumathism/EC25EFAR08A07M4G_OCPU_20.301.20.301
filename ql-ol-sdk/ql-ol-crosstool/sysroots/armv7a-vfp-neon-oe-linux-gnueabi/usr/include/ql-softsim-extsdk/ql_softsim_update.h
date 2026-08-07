/**
 * @file ql_softsim_update.h
 * @brief Update software sim partition lib public function declarations.
 * 
 * @note
 *
 * @copyright Copyright (c) 2009-2017 @ Quectel Wireless Solutions Co., Ltd.
 */

#ifndef __QL_SOFTSIM_UPDATE__
#define	__QL_SOFTSIM_UPDATE__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TRUE
#define	TRUE	1
#endif

#ifndef FALSE
#define	FALSE	0
#endif

#define	QL_SOFTSIM_FILENAME_PATH	"/etc/ql_softsim_filename"


typedef enum
{
	QL_SOFTSIM_UPDATE_OK = 0,
	QL_SOFTSIM_UPDATE_FAILED,

	QL_SOFTSIM_UPDATE_INVALID
}_QL_SOFTSIM_RETURN_TYPE_;


/**
 * Upgrade oemapp partition
 *
 * @param [in] file             the source oemapp ubi file
 *
 * @return 
 *   On success, 0 is returned.  On error, -1 is returned.
 * 
 */
extern _QL_SOFTSIM_RETURN_TYPE_ ql_file_update(char *file);
extern _QL_SOFTSIM_RETURN_TYPE_ ql_back_oemapp(char *file);
extern _QL_SOFTSIM_RETURN_TYPE_ ql_restore_oemapp();
extern unsigned char ql_del_mark(void);

#ifdef __cplusplus
}
#endif

#endif
