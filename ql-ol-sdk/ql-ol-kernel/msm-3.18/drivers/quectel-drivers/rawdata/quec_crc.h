/*==========================================================================
 |                    QUECTEL - Build a smart world.
 |
 |               Copyright(c) 2017 QUECTEL Incorporated.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 | quectel crc 
 |
 |--------------------------------------------------------------------------
 |
 | Designed by: will.shao
 |--------------------------------------------------------------------------
 | Revision History
 | ----------------
 | 2018/05/04               will.shao               Initialization
 | 2018/11/18               Mike                    Modify data type   
 | ------------------------------------------------------------------------
 \=========================================================================*/

#ifndef _QUEC_CRC_H_
#define _QUEC_CRC_H_

#include <linux/types.h>

uint32_t crc_32_calc
(
	/* Pointer to data over which to compute CRC */
	uint8_t  *buf_ptr,

	/* Number of bits over which to compute CRC */
	uint16_t	len,

	/* Seed for CRC computation */
	uint32_t	seed
);

#endif //_QUEC_CRC_H_
