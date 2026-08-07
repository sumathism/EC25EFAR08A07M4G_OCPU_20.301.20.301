/**  
  @file
    fota.h

  @brief
    Downloading Quectel FOTA software packs.
 */

/*=============================================================================
  Copyright (c) 2018 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=============================================================================*/

/*============================================================================

                          EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  WHEN             WHO         WHAT, WHERE, WHY
  ------------     -------     ----------------------------------------
  03/11/2019       Mike        Initial creation.
============================================================================*/
#ifndef __FOTA_H__
#define __FOTA_H__

int urc_sendto_modem(char *format, ...);
extern int dfota_update(char *url, char *temp, char *file, char *usrname, char *password, int log_level);

#endif /* end of __FOTA_H__ */
