/*-----------------------------------------------------------------------------------------------*/
/**
  @file ql_ble_types.h
  @brief Common API
*/
/*-----------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------
  Copyright (c) 2019 Quectel Wireless Solution, Co., Ltd. All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
-------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------
  EDIT HISTORY
  This section contains comments describing changes made to the file.
  Notice that changes are listed in reverse chronological order.
  $Header: $
  when       who          what, where, why
  --------   ---          ----------------------------------------------------------
  20190829   baron.qian   Created .
-------------------------------------------------------------------------------------------------*/

#ifndef QL_BLE_TYPES_H_
#define QL_BLE_TYPES_H_
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#undef  FALSE
#define FALSE (0)

#undef  TRUE
#define TRUE (1)

/* Unsigned fixed width types */
typedef uint8_t                 QuecBtUint8;
typedef uint16_t                QuecBtUint16;
typedef uint32_t                QuecBtUint32;

/* Signed fixed width types */
typedef int8_t                  QuecBtInt8;
typedef int16_t                 QuecBtInt16;
typedef int32_t                 QuecBtInt32;

/* Boolean */
typedef QuecBtUint8             QuecBtBool;

/* String types */
typedef char                    QuecBtCharString;
typedef QuecBtUint8             QuecBtUtf8String;
typedef QuecBtUint16            QuecBtUtf16String;   /* 16-bit UTF16 strings */
typedef QuecBtUint32            QuecBtUint24;

/*
 * 64-bit integers
 *
 * Note: If a given compiler does not support 64-bit types, it is
 * OK to omit these definitions;  32-bit versions of the code using
 * these types may be available.  Consult the relevant documentation
 * or the customer support group for information on this.
 */
#define QUEC_BT_HAVE_64_BIT_INTEGERS
typedef uint64_t                QuecBtUint64;
typedef int64_t                 QuecBtInt64;

/*
 * Floating point
 *
 * Note: If a given compiler does not support floating point, it is
 * OK to omit these definitions;  alternative versions of the code using
 * these types may be available.  Consult the relevant documentation
 * or the customer support group for information on this.
 */
#define QUEC_BT_HAVE_FLOATING_POINT
typedef float                   QuecBtFloat;
typedef double                  QuecBtDouble;




/* UUID types - 16 bit UUID is defined in bluetooth.h */
typedef QuecBtUint16            uuid16_t;
typedef uuid16_t                QuecBtUuid16;
typedef QuecBtUint32            QuecBtUuid32;
typedef QuecBtUint8             QuecBtUuid128[16];

typedef enum
{
    QUEC_UUID_TYPE_16 = 1,
    QUEC_UUID_TYPE_128,
}E_QUEC_UUID_TYPE;


typedef QuecBtUint32            QuecBtPrim;
typedef QuecBtUint32            QuecBtGattId;
typedef QuecBtUint32            QuecBtConnId;
typedef QuecBtUint16            QuecResultCode;
typedef QuecBtUint16            QuecReasonCode;
typedef QuecBtUint32            QuecBtGattConnFlags;
typedef QuecBtUint32            QuecBtGattConnInfo;
typedef QuecBtUint8             QuecBtGattLeRole;
typedef QuecBtUint16            QuecResultCode; 

typedef struct
{
    QuecBtUint24        lap;   /*!< Lower Address Part 00..23 */
    QuecBtUint8         uap;   /*!< upper Address Part 24..31 */
    QuecBtUint16        nap;   /*!< Non-significant    32..47 */
} QuecBtDeviceAddr;

typedef struct
{
    QuecBtUint8             type;
    QuecBtDeviceAddr        addr;
}QuecBtTypedDeviceAddr;

#ifdef __cplusplus
}
#endif
#endif /* !QL_BLE_TYPES_H_ */
