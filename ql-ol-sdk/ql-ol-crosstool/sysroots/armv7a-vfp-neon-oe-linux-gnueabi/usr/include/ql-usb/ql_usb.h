/**
 * @file ql_usb.h
 * @brief Quectel USB configuration.
 * 
 * @note Provide Linux usb functions
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
  ------------     -------     -----------------------------------------------
  11/08/2018       Mike        Initial creation.
  11/28/2018       Mike        Adds load usb for recovery mode.
  12/03/2018       Mike        Re-modify the usb structure, compatible with the previous configuration
==============================================================================*/
#ifndef __QL_USB_H__
#define __QL_USB_H__

#include <linux/quectel/quec_raw_data_item.h>
#include "usb_product_config.h"

#define USB_NUM 0
#define USB_FUNC_ENABLE_DIAG            (1)
#define USB_FUNC_ENABLE_NMEA            (1<<1)
#define USB_FUNC_ENABLE_AT              (1<<2)
#define USB_FUNC_ENABLE_MODEM           (1<<3)
#define USB_FUNC_ENABLE_NET             (1<<4)
#define USB_FUNC_ENABLE_ADB             (1<<5)
#define USB_FUNC_ENABLE_AUDIO_UAC       (1<<6)

struct quectel_usb_cfg {
	unsigned short pid;
	unsigned short vid;
	char product_info[32];
	char manufac_info[32];
	unsigned char bootup;
	unsigned char usbnet;
	unsigned int mask;
	unsigned int usb_fixed_interface;
};

/*========================================================================
  FUNCTION: quectel_usb_config_load
=========================================================================*/
/*! @brief
     

    @param[in] usb                      The usb configuration.
    @param[in] num                      The usb id

    @return
       On Success, 0 is return. On error, -1 is return.
*/
/*=======================================================================*/
extern int quectel_usb_config_load(struct quectel_usb_cfg *usb, unsigned int num);

/*========================================================================
  FUNCTION: quectel_usb_recovery_load
=========================================================================*/
/*! @brief
     

    @param[in] usb                      The usb configuration.
    @param[in] num                      The usb id

    @return
       On Success, 0 is return. On error, -1 is return.
*/
/*=======================================================================*/
extern int quectel_usb_recovery_load(struct quectel_usb_cfg *usb, unsigned int num);

/*========================================================================
  FUNCTION: quectel_usb_config_dump
=========================================================================*/
/*! @brief
     Show usb configuation in log

    @param[out] usb                      The usb configuration.

    @return
       None
*/
/*=======================================================================*/
extern void quectel_usb_config_dump(struct quectel_usb_cfg *usb);

/*========================================================================
  FUNCTION: quectel_usb_get_default_config
=========================================================================*/
/*! @brief
     Gets default usb configuation.

    @param[out] usb                      The usb configuration.

    @return
       None
*/
/*=======================================================================*/
extern void quectel_usb_get_default_config(struct quectel_usb_cfg *usb);

/*========================================================================
  FUNCTION: quectel_usb_config_read
=========================================================================*/
/*! @brief
     Reads usb configuation from raw data flash.

    @param[out] usb                      The usb configuration.

    @return
       On Success, 0 is return. On error, -1 is return.
*/
/*=======================================================================*/
extern int quectel_usb_config_read(struct quectel_usb_cfg *usb);

/*========================================================================
  FUNCTION: quectel_usb_config_read
=========================================================================*/
/*! @brief
     Writes usb configuation to raw data flash.

    @param[out] usb                      The usb configuration.

    @return
       On Success, 0 is return. On error, -1 is return.
*/
/*=======================================================================*/
extern int quectel_usb_config_write(struct quectel_usb_cfg *usb);

/*========================================================================
  FUNCTION: quectel_usb_enable
=========================================================================*/
/*! @brief
     Enable or Disable usb devices.

    @param[out] usb                      The usb configuration.

    @return
       None
*/
/*=======================================================================*/
extern void quectel_usb_enable(int num, bool enable);

/*========================================================================
  FUNCTION: quectel_usb_interface_write
=========================================================================*/
/*! @brief
     Writes usb interface to raw data flash.

    @param[out] usb                      The usb configuration.

    @return
       On Success, 0 is return. On error, -1 is return.
*/
/*=======================================================================*/
extern int quectel_usb_interface_write(struct quectel_usb_cfg *usb);

/*========================================================================
  FUNCTION: quectel_usb_interface_read
=========================================================================*/
/*! @brief
     Reads usb interface from raw data flash.

    @param[out] usb                      The usb configuration.

    @return
       On Success, 0 is return. On error, -1 is return.
*/
/*=======================================================================*/
extern int quectel_usb_interface_read(struct quectel_usb_cfg *usb);

#define QUECTEL_AT_PTIMZE_USB_ENUMFAIL
#ifdef QUECTEL_AT_PTIMZE_USB_ENUMFAIL
//<2020/11/21> //add colin at command contorl ptimize usb enum fail switch --start
/*========================================================================
  FUNCTION:  contorl ptimize usb enum fail switch
=========================================================================*/
/*! @brief
     used at command contorl 

    @return
       None
*/
/*=======================================================================*/
extern int set_optimze_usb_enumfaile_flag();
#endif

#endif /* end of __QL_USB_H__ */
