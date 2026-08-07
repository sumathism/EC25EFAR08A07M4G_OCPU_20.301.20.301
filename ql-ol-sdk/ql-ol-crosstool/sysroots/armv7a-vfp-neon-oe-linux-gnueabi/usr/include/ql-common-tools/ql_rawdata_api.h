/**
 * @file ql_rawdata_api.h
 * @brief Quectel raw data flash public function declarations.
 * 
 * @note 
 *
 * @copyright Copyright (c) 2010-2017 @ Quectel Wireless Solutions Co., Ltd.
 */
/*============================================================================

                          EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  WHEN             WHO         WHAT, WHERE, WHY
  ------------     -------     ----------------------------------------
  09/22/2018       Asa        Initial creation.
============================================================================*/
#ifndef _QL_RAWDATA_API_H_
#define _QL_RAWDATA_API_H_

#include <linux/quectel/quec_raw_data_item.h>

/**
 * Writes data to raw data flash.
 *
 * @return 
 *   On success, 0 is returned.  On error, -1 is returned.
 * 
 */
extern int ql_rawdata_write_item(int id, void* data, int len);

/**
 * Reads data from raw data flash.
 *
 * @return 
 *   On success, 0 is returned.  On error, -1 is returned.
 * 
 */
extern int ql_rawdata_read_item(int id, void* data, int len);

#endif //_QL_RAWDATA_API_H_

