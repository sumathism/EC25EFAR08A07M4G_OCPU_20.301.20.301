/*-----------------------------------------------------------------------------------------------*/
/**
  @file ql_ble.h
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

#ifndef QL_BLE_V2_H_
#define QL_BLE_V2_H_
#include "ql_ble_types.h"
#include "ql_bt.h"  //<2023/03/30>add Harry Obtaining or setting BT MAC is supported when bt is disenabled

#ifdef __cplusplus
extern "C" {
#endif
#define QUEC_BLE_SERVER_UNIX_PATH   "/tmp/.ble_socket_server"
#define QUEC_BLE_SERVICE_NAME       "quectel_ble_service"


/*******************************************************************************
 * the primitive define about indications, user should be process 
 * these messages in the client_cb callback function.
 *******************************************************************************/
#define QUEC_BT_INDICATION_BASE                     ((QuecBtUint32) (0x2000))
#define QUEC_BT_GATT_CONNECT_IND                    ((QuecBtUint32) (0x0001 + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_GATT_DISCONNECT_IND                 ((QuecBtUint32) (0x0002 + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_GATT_DB_READ_ACCESS_IND             ((QuecBtUint32) (0x0003 + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_GATT_DB_WRITE_ACCESS_IND            ((QuecBtUint32) (0x0004 + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_SD_SEARCH_IND                       ((QuecBtUint32) (0x0008 + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_AV_CONNECT_IND                      ((QuecBtUint32) (0x0009 + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_AV_DISCONNECT_IND                   ((QuecBtUint32) (0x000a + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_HFG_CONNECT_IND                     ((QuecBtUint32) (0x000b + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_HFG_DISCONNECT_IND                  ((QuecBtUint32) (0x000c + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_HFG_ANSWER_IND                      ((QuecBtUint32) (0x000d + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_HFG_REJECT_IND                      ((QuecBtUint32) (0x000e + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_AVRCP_CONNECT_IND                   ((QuecBtUint32) (0x000f + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_AVRCP_DISCONNECT_IND                ((QuecBtUint32) (0x0010 + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_AVRCP_PASS_THROUGH_IND              ((QuecBtUint32) (0x0011 + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_GATT_MTU_CHANGED_IND                ((QuecBtUint32) (0x0013 + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_INDICATION_MAX                      ((QuecBtUint32) (0x00FF + QUEC_BT_INDICATION_BASE))


/*******************************************************************************
 * ble address type
 *******************************************************************************/
#define QUEC_TBDADDR_PUBLIC                         ((QuecBtUint8)0x00)
#define QUEC_TBDADDR_RANDOM                         ((QuecBtUint8)0x01)
#define QUEC_TBDADDR_INVALID                        ((QuecBtUint8)0xFF)


/*******************************************************************************
 * Connect flags. Comments show what what flags are valid where.
 *******************************************************************************/
#define QUEC_BT_GATT_FLAGS_NONE                                  ((QuecBtGattConnFlags)0x00000000)    /* no special options, use LE radio (all) */
#define QUEC_BT_GATT_FLAGS_WHITELIST                             ((QuecBtGattConnFlags)0x00000001)    /* allow connections from whitelist only (peripheral/central) */
#define QUEC_BT_GATT_FLAGS_UNDIRECTED                            ((QuecBtGattConnFlags)0x00000002)    /* use undirected connection (peripheral) */
#define QUEC_BT_GATT_FLAGS_ADVERTISE_TIMEOUT                     ((QuecBtGattConnFlags)0x00000004)    /* undirected advertising times out (peripheral) */
#define QUEC_BT_GATT_FLAGS_NONDISCOVERABLE                       ((QuecBtGattConnFlags)0x00000008)    /* AD flags are non-discoverable (advertise/peripheral) */
#define QUEC_BT_GATT_FLAGS_LIMITED_DISCOVERABLE                  ((QuecBtGattConnFlags)0x00000010)    /* AD flags are limited discoverable (advertise/peripheral) */
#define QUEC_BT_GATT_FLAGS_DISABLE_SCAN_RESPONSE                 ((QuecBtGattConnFlags)0x00000020)    /* disable scan response (advertise) */
#define QUEC_BT_GATT_FLAGS_APPEND_DATA                           ((QuecBtGattConnFlags)0x00000040)    /* append advertise/scan-rsp data (advertise/peripheral) */
#define QUEC_BT_GATT_FLAGS_NO_AUTO_SECURITY                      ((QuecBtGattConnFlags)0x00000080)    /* do not attempt to highten security (central/peripheral) */
#define QUEC_BT_GATT_FLAGS_NO_AUTO_SIGN_UPGRADE                  ((QuecBtGattConnFlags)0x00000100)    /* do not allow sign-to-normal write commands while encrypted */
#define QUEC_BT_GATT_FLAGS_WHITELIST_SCANRSP                     ((QuecBtGattConnFlags)0x00000200)    /* allow scan response to whitelist only (advertise/peripheral)*/
#define QUEC_BT_GATT_FLAGS_ATTEMPTED_SECURITY                    ((QuecBtGattConnFlags)0x40000000)    /* for internal use only */
#define QUEC_BT_GATT_FLAGS_CENTRAL_TIMEOUT                       QUEC_BT_GATT_FLAGS_ADVERTISE_TIMEOUT /* Central connection times out */


/*******************************************************************************
 * add colin support sd av and hfg 
 *******************************************************************************/
/*Rami:SD Event*/
#define QUEC_BT_SD_COMMAND_BASE                     ((QuecBtUint32)(0x4000))
#define QUEC_BT_SD_SCAN_REQ                         ((QuecBtUint32) (0x0001 + QUEC_BT_SD_COMMAND_BASE))
#define QUEC_BT_SD_SCAN_CFM                         ((QuecBtUint32) (0x0002 + QUEC_BT_SD_COMMAND_BASE))
#define QUEC_BT_SD_CANCEL_SCAN_REQ                  ((QuecBtUint32) (0x0003 + QUEC_BT_SD_COMMAND_BASE))
#define QUEC_BT_SD_CANCEL_SCAN_CFM                  ((QuecBtUint32) (0x0004 + QUEC_BT_SD_COMMAND_BASE))

/*Rami:AV Event*/
#define QUEC_BT_AV_COMMAND_BASE                     ((QuecBtUint32)(0x5000))
#define QUEC_BT_AV_ACTIVATE_REQ                     ((QuecBtUint32) (0x0001 + QUEC_BT_AV_COMMAND_BASE))
#define QUEC_BT_AV_ACTIVATE_CFM                     ((QuecBtUint32) (0x0002 + QUEC_BT_AV_COMMAND_BASE))
#define QUEC_BT_AV_DEACTIVATE_REQ                   ((QuecBtUint32) (0x0003 + QUEC_BT_AV_COMMAND_BASE))
#define QUEC_BT_AV_DEACTIVATE_CFM                   ((QuecBtUint32) (0x0004 + QUEC_BT_AV_COMMAND_BASE))
#define QUEC_BT_AV_CONNECT_REQ                      ((QuecBtUint32) (0x0005 + QUEC_BT_AV_COMMAND_BASE))
#define QUEC_BT_AV_CONNECT_CFM                      ((QuecBtUint32) (0x0006 + QUEC_BT_AV_COMMAND_BASE))
#define QUEC_BT_AV_CANCEL_CONNECT_REQ               ((QuecBtUint32) (0x0007 + QUEC_BT_AV_COMMAND_BASE))
#define QUEC_BT_AV_CANCEL_CONNECT_CFM               ((QuecBtUint32) (0x0008 + QUEC_BT_AV_COMMAND_BASE))
#define QUEC_BT_AV_DISCONNECT_REQ                   ((QuecBtUint32) (0x0009 + QUEC_BT_AV_COMMAND_BASE))
#define QUEC_BT_AV_DISCONNECT_CFM                   ((QuecBtUint32) (0x000a + QUEC_BT_AV_COMMAND_BASE))
#define QUEC_BT_AV_REGISTER_STREAM_HANDLE_REQ       ((QuecBtUint32) (0x000b + QUEC_BT_AV_COMMAND_BASE))
#define QUEC_BT_AV_REGISTER_STREAM_HANDLE_CFM       ((QuecBtUint32) (0x000c + QUEC_BT_AV_COMMAND_BASE))


/*Rami:HFG Event*/
#define QUEC_BT_HFG_COMMAND_BASE                     ((QuecBtUint32)(0x7000))
#define QUEC_BT_HFG_CONNECT_REQ                      ((QuecBtUint32) (0x0001 + QUEC_BT_HFG_COMMAND_BASE))
#define QUEC_BT_HFG_CONNECT_CFM                      ((QuecBtUint32) (0x0002 + QUEC_BT_HFG_COMMAND_BASE))
#define QUEC_BT_HFG_CANCEL_CONNECT_REQ               ((QuecBtUint32) (0x0003 + QUEC_BT_HFG_COMMAND_BASE))
#define QUEC_BT_HFG_CANCEL_CONNECT_CFM               ((QuecBtUint32) (0x0004 + QUEC_BT_HFG_COMMAND_BASE))
#define QUEC_BT_HFG_SPEAKER_GAIN_REQ                 ((QuecBtUint32) (0x0005 + QUEC_BT_HFG_COMMAND_BASE))
#define QUEC_BT_HFG_SPEAKER_GAIN_CFM                 ((QuecBtUint32) (0x0006 + QUEC_BT_HFG_COMMAND_BASE))
#define QUEC_BT_HFG_CREATE_SCO_REQ                    ((QuecBtUint32) (0x0007 + QUEC_BT_HFG_COMMAND_BASE))
#define QUEC_BT_HFG_CREATE_SCO_CFM                    ((QuecBtUint32) (0x0008 + QUEC_BT_HFG_COMMAND_BASE))
#define QUEC_BT_HFG_DESTROY_SCO_REQ                 ((QuecBtUint32) (0x0009 + QUEC_BT_HFG_COMMAND_BASE))
#define QUEC_BT_HFG_DESTROY_SCO_CFM                 ((QuecBtUint32) (0x000a + QUEC_BT_HFG_COMMAND_BASE))


#define QUEC_BT_AVRCP_COMMAND_BASE                  ((QuecBtUint32)(0x8000))
#define QUEC_BT_AVRCP_CONNECT_REQ                   ((QuecBtUint32) (0x0001 + QUEC_BT_AVRCP_COMMAND_BASE))
#define QUEC_BT_AVRCP_CONNECT_CFM                   ((QuecBtUint32) (0x0002 + QUEC_BT_AVRCP_COMMAND_BASE))
#define QUEC_BT_AVRCP_CONFIG_REQ                    ((QuecBtUint32) (0x0003 + QUEC_BT_AVRCP_COMMAND_BASE))
#define QUEC_BT_AVRCP_CONFIG_CFM                    ((QuecBtUint32) (0x0004 + QUEC_BT_AVRCP_COMMAND_BASE))
#define QUEC_BT_AVRCP_CANCEL_CONNECT_REQ            ((QuecBtUint32) (0x0005 + QUEC_BT_AVRCP_COMMAND_BASE))
#define QUEC_BT_AVRCP_CANCEL_CONNECT_CFM            ((QuecBtUint32) (0x0006 + QUEC_BT_AVRCP_COMMAND_BASE))


/*******************************************************************************
 * ble db operate error code
 *******************************************************************************/

enum
{
    QUEC_BLE_DB_ERR_OK = 0,
    QUEC_BLE_DB_ERR_INTERNAL,
    QUEC_BLE_DB_ERR_SERVICE_EXIST,
    QUEC_BLE_DB_ERR_SERVICE_NOT_EXIST,
    QUEC_BLE_DB_ERR_CHARACT_EXIST,
    QUEC_BLE_DB_ERR_CHARACT_NOT_EXIST,
    QUEC_BLE_DB_ERR_DESCRIPTOR_EXIST,
    QUEC_BLE_DB_ERR_DESCRIPTOR_NOT_EXIST,
};


/*******************************************************************************
 * Connection info flags
 *******************************************************************************/
#define QUEC_BT_GATT_CONNINFO_LE                                 ((QuecBtGattConnInfo)0x000000000) /* connection runs on the LE radio */
#define QUEC_BT_GATT_CONNINFO_BREDR                              ((QuecBtGattConnInfo)0x000000001) /* connection runs on the BREDR radio */


/*******************************************************************************
 * Le Role define
 *******************************************************************************/
#define QUEC_BT_GATT_LE_ROLE_UNDEFINED                           ((QuecBtGattLeRole)0x00)         /* Using the BREDR radio */
#define QUEC_BT_GATT_LE_ROLE_MASTER                              ((QuecBtGattLeRole)0x01)         /* Using the LE radio as Master/Central   */
#define QUEC_BT_GATT_LE_ROLE_SLAVE                               ((QuecBtGattLeRole)0x02)         /* Using the LE radio as Slave/Peripheral */



/*******************************************************************************
 * permits flags
 *******************************************************************************/
/*! If set, permits broadcasts of the Characteristic Value using
  Characteristic Configuration Descriptor. */
#define QUEC_ATT_PERM_CONFIGURE_BROADCAST            0x01
/*! If set, permits reads of the Characteristic Value. */
#define QUEC_ATT_PERM_READ                           0x02
/*! If set, permit writes of the Characteristic Value without response. */
#define QUEC_ATT_PERM_WRITE_CMD                      0x04
/*! If set, permits writes of the Characteristic Value with response. */
#define QUEC_ATT_PERM_WRITE_REQ                      0x08
/*! If set, permits notifications of a Characteristic Value without
    acknowledgment. */
#define QUEC_ATT_PERM_NOTIFY                         0x10
/*! If set, permits indications of a Characteristic Value with
    acknowledgment. */
#define QUEC_ATT_PERM_INDICATE                       0x20
/*! If set, permits signed writes to the Characteristic Value. */
#define QUEC_ATT_PERM_AUTHENTICATED                  0x40
/*! If set, additional characteristic properties are defined in the
    Characteristic Extended Properties Descriptor. */
#define QUEC_ATT_PERM_EXTENDED                       0x80
/********************************************/

/*******************************************************************************
 * Attribute flags
 *******************************************************************************/
/* No attribute flags*/
#define QUEC_BT_GATT_ATTR_FLAGS_NONE                             ((QuecBtUint16) 0x00)
/* Attribute length can be changed */
#define QUEC_BT_GATT_ATTR_FLAGS_DYNLEN                           ((QuecBtUint16) 0x08)
/* Read access to the attribute sends QUEC_BT_GATT_DB_ACCESS_READ_IND
 * to the application */
#define QUEC_BT_GATT_ATTR_FLAGS_IRQ_READ                         ((QuecBtUint16) 0x10)
/* Write access to the attribute sends QUEC_BT_GATT_DB_ACCESS_WRITE_IND
 * to the application */
#define QUEC_BT_GATT_ATTR_FLAGS_IRQ_WRITE                        ((QuecBtUint16) 0x20)
/* Legacy Encrypted link required for read access. */
#define QUEC_BT_GATT_ATTR_FLAGS_READ_ENCRYPTION                  ((QuecBtUint16) 0x40)
/* Legacy Authenticated (MITM) link required for read access. */
#define QUEC_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION              ((QuecBtUint16) 0x80)
/* Secure Connections Authenticated (SC_MITM) link required for read access. */
#define QUEC_BT_GATT_ATTR_FLAGS_READ_SC_AUTHENTICATION           ((QuecBtUint16) 0xC0)
/* Legacy Encrypted link required for write access. */
#define QUEC_BT_GATT_ATTR_FLAGS_WRITE_ENCRYPTION                 ((QuecBtUint16) 0x01)
/* Legacy Authenticated (MITM) link required for write access. */
#define QUEC_BT_GATT_ATTR_FLAGS_WRITE_AUTHENTICATION             ((QuecBtUint16) 0x02)
/* Secure Connections Authenticated (SC_MITM) link required for write access. */
#define QUEC_BT_GATT_ATTR_FLAGS_WRITE_SC_AUTHENTICATION          ((QuecBtUint16) 0x03)
/* Authorisation (require application access ind/rsp before read/write is allowed).
   I.e. a QUEC_BT_GATT_DB_ACCESS_READ_IND or a QUEC_BT_GATT_DB_ACCESS_WRITE_IND it sent to the application */
#define QUEC_BT_GATT_ATTR_FLAGS_AUTHORISATION                    ((QuecBtUint16) 0x100)
/* Encryption key size checks required. 
   I.e. a QUEC_BT_GATT_DB_ACCESS_READ_IND or a QUEC_BT_GATT_DB_ACCESS_WRITE_IND it sent to the application */
#define QUEC_BT_GATT_ATTR_FLAGS_ENCR_KEY_SIZE                    ((QuecBtUint16) 0x200)
/* Disable access or LE radio */
#define QUEC_BT_GATT_ATTR_FLAGS_DISABLE_LE                       ((QuecBtUint16) 0x400)
/* Disable access on BR/EDR radio */
#define QUEC_BT_GATT_ATTR_FLAGS_DISABLE_BREDR                    ((QuecBtUint16) 0x800)




/*******************************************************************************
 * Client Characteristic Configuration bit definition
 *******************************************************************************/
#define QUEC_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT                ((QuecBtUint16) 0x0000)
#define QUEC_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION           ((QuecBtUint16) 0x0001)
#define QUEC_BT_GATT_CLIENT_CHARAC_CONFIG_INDICATION             ((QuecBtUint16) 0x0002)


/*******************************************************************************
 * Server Characteristic Configuration bit definition
 *******************************************************************************/
#define QUEC_BT_GATT_SERVER_CHARAC_CONFIG_DISABLE                ((QuecBtUint16) 0x0000)
#define QUEC_BT_GATT_SERVER_CHARAC_CONFIG_BROADCAST              ((QuecBtUint16) 0x0001)

/*******************************************************************************
* Characteristic Format types use in the Characteristic 
    Presentation Format declaration
 *******************************************************************************/
#define QUEC_BT_GATT_CHARAC_FORMAT_RFU                           ((QuecBtUint8) 0x00) /* Reserved for future used */
#define QUEC_BT_GATT_CHARAC_FORMAT_BOOLEAN                       ((QuecBtUint8) 0x01) /* Unsigned 1-bit, 0 = FALSE, 1 = TRUE */
#define QUEC_BT_GATT_CHARAC_FORMAT_2BIT                          ((QuecBtUint8) 0x02) /* Unsigned 2-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_NIBBLE                        ((QuecBtUint8) 0x03) /* Unsigned 4-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_UINT8                         ((QuecBtUint8) 0x04) /* Unsigned 8-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_UINT12                        ((QuecBtUint8) 0x05) /* Unsigned 12-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_UINT16                        ((QuecBtUint8) 0x06) /* Unsigned 16-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_UINT24                        ((QuecBtUint8) 0x07) /* Unsigned 24-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_UINT32                        ((QuecBtUint8) 0x08) /* Unsigned 32-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_UINT48                        ((QuecBtUint8) 0x09) /* Unsigned 48-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_UINT64                        ((QuecBtUint8) 0x0A) /* Unsigned 64-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_UINT128                       ((QuecBtUint8) 0x0B) /* Unsigned 128-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_SINT8                         ((QuecBtUint8) 0x0C) /* Signed 8-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_SINT12                        ((QuecBtUint8) 0x0D) /* Signed 12-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_SINT16                        ((QuecBtUint8) 0x0E) /* Signed 16-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_SINT24                        ((QuecBtUint8) 0x0F) /* Signed 24-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_SINT32                        ((QuecBtUint8) 0x10) /* Signed 32-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_SINT48                        ((QuecBtUint8) 0x11) /* Signed 48-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_SINT64                        ((QuecBtUint8) 0x12) /* Signed 64-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_SINT128                       ((QuecBtUint8) 0x13) /* Signed 128-bit integer */
#define QUEC_BT_GATT_CHARAC_FORMAT_FLOAT32                       ((QuecBtUint8) 0x14) /* IEEE-754 32-bit floating point */
#define QUEC_BT_GATT_CHARAC_FORMAT_FLOAT64                       ((QuecBtUint8) 0x15) /* IEEE-754 64-bit floating point */
#define QUEC_BT_GATT_CHARAC_FORMAT_SFLOAT                        ((QuecBtUint8) 0x16) /* IEEE-11073 16-bit SFLOAT */
#define QUEC_BT_GATT_CHARAC_FORMAT_FLOAT                         ((QuecBtUint8) 0x17) /* IEEE-11073 32-bit FLOAT */
#define QUEC_BT_GATT_CHARAC_FORMAT_DUINT16                       ((QuecBtUint8) 0x18) /* IEEE-20601 format */
#define QUEC_BT_GATT_CHARAC_FORMAT_UTF8S                         ((QuecBtUint8) 0x19) /* UTF8-String */
#define QUEC_BT_GATT_CHARAC_FORMAT_UTF16S                        ((QuecBtUint8) 0x1A) /* UTF16-String */
#define QUEC_BT_GATT_CHARAC_FORMAT_STRUCT                        ((QuecBtUint8) 0x1B) /* Opaque structure */


/*******************************************************************************
* GATT error codes 
 *******************************************************************************/
#define QUEC_BT_GATT_RESULT_SUCCESS                              ((QuecBtUint16)0x0000) /* Not an error */
#define QUEC_BT_GATT_RESULT_UNACCEPTABLE_PARAMETER               ((QuecBtUint16)0x0001) /* Invalid/unacceptable parameters */
#define QUEC_BT_GATT_RESULT_UNKNOWN_CONN_ID                      ((QuecBtUint16)0x0002) /* Unknown connection id */
#define QUEC_BT_GATT_RESULT_ALREADY_ACTIVATED                    ((QuecBtUint16)0x0003) /* App have called activate req before */
#define QUEC_BT_GATT_RESULT_INTERNAL_ERROR                       ((QuecBtUint16)0x0004) /* Internal GATT Error */
#define QUEC_BT_GATT_RESULT_INSUFFICIENT_NUM_OF_HANDLES          ((QuecBtUint16)0x0005) /* Insufficient number of free attribute handles */
#define QUEC_BT_GATT_RESULT_ATTR_HANDLES_ALREADY_ALLOCATED       ((QuecBtUint16)0x0006) /* The application have already allocated attribute handles */
#define QUEC_BT_GATT_RESULT_CANCELLED                            ((QuecBtUint16)0x0007) /* Operation cancelled */
#define QUEC_BT_GATT_RESULT_SCATTERNET                           ((QuecBtUint16)0x0008) /* Scatternet not allowed */
#define QUEC_BT_GATT_RESULT_INVALID_LENGTH                       ((QuecBtUint16)0x0009) /* The attribute value length is invalid for the operation */
#define QUEC_BT_GATT_RESULT_RELIABLE_WRITE_VALIDATION_ERROR      ((QuecBtUint16)0x000A) /* Validation of the written attribute value failed */
#define QUEC_BT_GATT_RESULT_INVALID_ATTRIBUTE_VALUE_RECEIVED     ((QuecBtUint16)0x000B) /* An invalied attribute value is received */
#define QUEC_BT_GATT_RESULT_CLIENT_CONFIGURATION_IN_USED         ((QuecBtUint16)0x000C) /* The application is allready using Client Configuration */
#define QUEC_BT_GATT_RESULT_BR_EDR_NO_PRIMARY_SERVICES_FOUND     ((QuecBtUint16)0x000D) /* No Primary Services that support BR/EDR were found  */
#define QUEC_BT_GATT_RESULT_INVALID_HANDLE_RANGE                 ((QuecBtUint16)0x000E) /* The given handle range is invalid */
#define QUEC_BT_GATT_RESULT_PARAM_CONN_UPDATE_LOCAL_REJECT       ((QuecBtUint16)0x000F) /* A local service/application has rejected connection parameter update request */
#define QUEC_BT_GATT_RESULT_WHITE_FILTER_IN_USE                  ((QuecBtUint16)0x0010) /* The application tries to add/clear its whitelist while using it */
#define QUEC_BT_GATT_RESULT_ALREADY_CONNECTED                    ((QuecBtUint16)0x0011) /* The application is already connected to the given address */
#define QUEC_BT_GATT_RESULT_ALREADY_CONNECTING                   ((QuecBtUint16)0x0012) /* The application is already connecting to the given address */
#define QUEC_BT_GATT_RESULT_WHITE_LIST_LIMIT_EXCEEDED            ((QuecBtUint16)0x0013) /* The Procedure fails because the limit of the number of devices on the whitelist is exceeded */    
#define QUEC_BT_GATT_RESULT_ALREADY_ADVERTISING                  ((QuecBtUint16)0x0014) /* The application is already advertising nothing to do */
#define QUEC_BT_GATT_RESULT_WHITE_FILTER_SCANRSP_IN_USE          ((QuecBtUint16)0x0015) /* White filter is already in use with a differend white list */
#define QUEC_BT_GATT_RESULT_WHITE_FILTER_CONTROL_FAILED          ((QuecBtUint16)0x0016) /* GATT should not get control over the whitelist during the Central Procedure */

/*_Harry 20230903*/
#ifndef	QUEC_BT_MAX_ADVPKT
	#define	QUEC_BT_MAX_ADVPKT	31U
#endif

#define	Q_BLE_SCSET_OP_SECMD	0x1 /*OP code for security mode*/
#define	Q_SCSET_SECMD_MD1		0x1
#define	Q_SCSET_SECMD_MD2		0x2 /*Mode2*/
#define Q_SCSET_SECMD_MD3		0x3
#define	Q_SCSET_SECMD_MD4		0x4 /*Mode4*/

#define Q_BLE_SCSET_OP_IOCAP	0x2 /*OP code for IO capability*/
#define	Q_SCSET_OP_IO_MIN	0x0
#define	Q_SCSET_OP_IO_DISPONLY 0x0 /*Display Only;  No Input-Num Output*/
#define	Q_SCSET_OP_IO_DISP_YN	0x01  /*keyboard and Y/N; (Input Y/N)/(Input Keyboard) - Num Output*/
#define	Q_SCSET_OP_IO_KBOARD_ONLY	0x2 /*Key board only£» Input Keyboard - No Output*/
#define	Q_SCSET_OP_IO_NOINNOOUT 0x3 /*No input No output*/
#define	Q_SCSET_OP_IO_KBOARD_DISPLAY  0x4  /* What the hell is this*/
#define	Q_SCSET_OP_IO_MAX 0x04 

/*******************************************************************************
* indication struct define, used in custom callback function
 *******************************************************************************/

typedef struct
{
    QuecBtPrim                type;                   /* Identity */
    QuecBtDeviceAddr            address;
    QuecBtUint8              devicename[0];
}BtSdScanAddress;


/*HFG*/
typedef struct
{
    QuecBtPrim            type;                   /* Identity */
    QuecBtUint32          connId;
    QuecBtDeviceAddr        address;    
}BtHfgConnectInd;

typedef BtHfgConnectInd BtHfgDisconnectInd;

typedef struct
{
    QuecBtPrim      type;
    QuecBtUint32    connid;
}BtHfgAnswer;

typedef BtHfgAnswer BtHfgReject;

/*AV*/
typedef struct
{
    QuecBtPrim              type;                   /* Identity */
    QuecBtUint8             connectId;
    QuecBtDeviceAddr        address; 
}BtAvConnectInd;

typedef struct
{
    QuecBtPrim              type;                   /* Identity */
    QuecBtUint8             connectId;
}BtAvDisconnectInd;

typedef struct
{
    QuecBtPrim              type;                   /* Identity */
    QuecBtUint32            connectId;
    QuecBtDeviceAddr        address; 
}BtAvrcpConnectInd;


typedef struct
{
    QuecBtPrim              type;                   /* Identity */
    QuecBtUint32            connectId;
}BtAvrcpDisconnectInd;

typedef struct
{
    QuecBtPrim              type;                   /* Identity */
    QuecBtUint32            CmdType;
}BtAvrcpPassThroughInd;



/*******************************************************************************
* indication struct define, used in custom callback function
 *******************************************************************************/
typedef struct
{
    QuecBtPrim              type;                   /* Identity */
    QuecBtGattId            gattId;                 /* The application handle */
    QuecResultCode          resultCode;             /* Result code */
    QuecBtConnId            connId;               /* Connection identifier */
    QuecBtGattConnInfo      connInfo;               /* Connection info flags (radio type etc.) */
    QuecBtTypedDeviceAddr   address;                /* Peer address */
    QuecBtUint16            mtu;                    /* Maximum packet size */
    QuecBtGattLeRole        leRole;                 /* Defines which role the connection has on the LE Radio */  
}BtGattConnectInd;

typedef struct
{
    QuecBtPrim              type;                   /* Identity */
    QuecBtGattId            gattId;                 /* The application handle */
    QuecReasonCode          reasonCode;             /* Reason code */
    QuecBtConnId            connId;               /* Connection identifier */
    QuecBtTypedDeviceAddr   address;                /* Peer address */
    QuecBtGattConnInfo      connInfo;               /* Connection info flags (radio type etc.) */
}BtGattDisconnectInd;

typedef struct
{
    QuecBtPrim           type;                   /* Identity */
    QuecBtGattId         gattId;                 /* The application handle */
    QuecBtConnId         connId;                 /* Connection identifier */
    QuecBtUint16         mtu;                    /* MTU for the connection */
}BtGattMtuChangedInd;

typedef struct
{
    QuecBtPrim                  type;               /* Identity */
    QuecBtGattId                gattId;             /* Application handle */
    QuecBtConnId                connId;             /* Connection identifier */
    QuecBtUint16                attrHandle;         /* The handle of the attribute */
    QuecBtUint16                offset;             /* The offset of the first octet to be accessed */
    QuecBtUint16                maxRspValueLength;  /* The maximum length that the value of the attribute must have */
    QuecBtUint16                check;              /* Special conditions that needs to be checked */
    QuecBtGattConnInfo          connInfo;           /* Connection info flags (radio type etc.) */
    QuecBtTypedDeviceAddr       address;            /* Peer address */
}BtGattDBReadAccessInd;

typedef struct
{
    QuecBtUint16                attrHandle;             /* The handle of the attribute to be written */
    QuecBtUint16                offset;                 /* Reserved for future used */
    QuecBtUint16                valueLength;            /* Length of the value */
    QuecBtUint8                 value[0];               /* Value placeholder*/
}BtGattAttrWritePairs;

typedef struct
{
    QuecBtPrim                  type;               /* Identity */
    QuecBtGattId                gattId;             /* Application handle */
    QuecBtConnId                connId;           /* Connection identifier */
    QuecBtUint16                check;              /* Special conditions that needs to be checked */
    QuecBtUint16                attrHandle;         /* The handle of the attribute. Return this handle in the AccessRes. */
    QuecBtGattConnInfo          connInfo;           /* Connection info flags (radio type etc.) */
    QuecBtTypedDeviceAddr       address;            /* Peer address */
    QuecBtUint16                writeUnitCount;     /* Number of sub-write units in list */
    BtGattAttrWritePairs        writeUnit[0];       /* placeholder. Only offset/value/valueLength should be used. */
}BtGattDBWriteAccessInd;

/*MK427U============================*/
typedef struct
{
    QuecBtPrim                type;
	QuecBtUint16                puuid;
	QuecBtUint16				startHandle;
	QuecBtUint16				endHandle;
} BtBleClientFindServiceInd;
typedef struct
{
    QuecBtPrim                type;
	QuecBtUint16              valueLength; 
    QuecBtUint8               value[512];             
} BtBleClientReadServiceCfm;
/*MK427D==========================*/
/*******************************************************************************/
/*By Victor*/
int ql_ble_client_connect(BtSdScanAddress devaddr, QuecBtGattId gattid);

//<2020/03/26> add colin support sd av ag function
int ql_hfg_power_on();
//open scan api
int ql_open_scan_device();

//close scan api
int ql_close_scan_device();

//av register api
int ql_av_register();

//av activate api 
int ql_av_activate();

//av deactivate api
int ql_av_deactivate();

//connect av
int ql_av_connect(QuecBtDeviceAddr address, QuecBtUint8* connId);

//disconnect av
int ql_av_disconnect(QuecBtUint8 conn_id);

//cancel connect av
int ql_av_cancelconnect(QuecBtDeviceAddr address);

//connect ag api
int ql_hfg_connect(QuecBtDeviceAddr address);

//disconnect ag
int ql_hfg_cancelconnect(QuecBtDeviceAddr address);

//ag create sco api
int ql_hfg_create_sco(QuecBtUint32 connect_id);

//ag destroy sco api
int ql_hfg_destroy_sco(QuecBtUint32 connect_id);


/*******************************************************************************
* @brief set BT chip power on. we must call this function first before
        use every api functions.
  @return if success return 0, else return -1
 *******************************************************************************/
int ql_ble_power_on();

/*******************************************************************************
* @brief set BT chip power off. 
  @return if success return 0, else return -1
 *******************************************************************************/
int ql_ble_power_off();

/*******************************************************************************
* @brief initialize client environment, in this function, client initiate an establish
        with ble service, and register with gatt.
  @param client_cb, callback function, used to process indication from ble service
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_client_init(int (*client_cb)(QuecBtPrim type, char *data, int len));

/*******************************************************************************
* @brief deinitialize client environment
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_client_deinit();


/*******************************************************************************
* @brief set local device name, the name usually dispaly on the peer device when 
        scanning the adverting.
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_set_local_name(char *name);


/*******************************************************************************
* @brief Request to Register an application instance to Gatt.

  @param
        *gattId: [out] if register success, return gattId here.
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_gatt_register(QuecBtGattId *gattId);


/*******************************************************************************
* @brief Request to Unregister/Remove an application instance from Gatt
      Note that if the application has:
      1 A Connection or has created one, GATT will ensure that it is released.
      2 Added a local data base, GATT will ensure that it is removed.
      3 Reserved/allocated a range of attribute handles, GATT will ensure
        that these are deallocated.
  @param
        gattId: the gattId which will be unregistered.
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_gatt_unregister(QuecBtGattId gattId);


/*******************************************************************************
* @brief reserving/allocating a range of attribute handles which it can use
    later for creating the local database
  @param
        gattId: the gattId which will be unregistered.
        numOfAttrHandles: numbers of attribute handle we want alloced
        preferredStartHandle: prederred start handle when alloced.
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_gatt_db_alloc(QuecBtGattId gattId, QuecBtUint16 numOfAttrHandles, QuecBtUint16 preferredStartHandle);

/*******************************************************************************
* @brief free/deallocate the range of attribute handles that are previously allocated
  @param
        gattId: the gattId which will be unregistered.
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_gatt_db_dealloc(QuecBtGattId gattId);

/*******************************************************************************
* @brief adding attributes to the local database
  @param
        gattId: gattId which register by ql_ble_gatt_register
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_gatt_db_add(QuecBtGattId gattId);

/*******************************************************************************
* @brief removing attributes from the local database
  @param
        gattId: gattId which register by ql_ble_gatt_register
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_gatt_db_remove(QuecBtGattId gattId);


/*******************************************************************************
* @brief Request to accept the establishment of a LE physical connection 
        as a Peripheral
  @param
        gattId: gattId which register by ql_ble_gatt_register
        addr:  The device address of the peer server
        flags: The Connection flags, which are defined in ql_ble.h, these are bellow:
                        QUEC_BT_GATT_FLAGS_NONE                 
                        QUEC_BT_GATT_FLAGS_WHITELIST            
                        QUEC_BT_GATT_FLAGS_UNDIRECTED           
                        QUEC_BT_GATT_FLAGS_ADVERTISE_TIMEOUT    
                        QUEC_BT_GATT_FLAGS_NONDISCOVERABLE      
                        QUEC_BT_GATT_FLAGS_LIMITED_DISCOVERABLE 
                        QUEC_BT_GATT_FLAGS_DISABLE_SCAN_RESPONSE
                        QUEC_BT_GATT_FLAGS_APPEND_DATA          
                        QUEC_BT_GATT_FLAGS_NO_AUTO_SECURITY     
                        QUEC_BT_GATT_FLAGS_NO_AUTO_SIGN_UPGRADE 
                        QUEC_BT_GATT_FLAGS_WHITELIST_SCANRSP    
                        QUEC_BT_GATT_FLAGS_ATTEMPTED_SECURITY   
                        QUEC_BT_GATT_FLAGS_CENTRAL_TIMEOUT
        mtu:    The preferable packet size. 0 = no preference. Minimum/default is 23 bytes.
        *connId: [out] Connection identifier
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_gatt_peripheral(QuecBtGattId gattId, 
                            QuecBtTypedDeviceAddr addr, 
                            QuecBtGattConnFlags flags, 
                            QuecBtUint16 mtu,
                            QuecBtConnId *connId);

/*******************************************************************************
* @brief Request to release/cancel a LE connection
  @param
      gattId: gattId which register by ql_ble_gatt_register
      connId: Connection identifier
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_gatt_disconnect(QuecBtGattId gattId, QuecBtConnId connId);


/*******************************************************************************
* @brief Request to start transmitting advertising packets
  @param
      gattId: gattId which register by ql_ble_gatt_register
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_adverting_start(QuecBtGattId gattId);

/*******************************************************************************
* @brief Request to stop transmitting advertising packets
  @param
      gattId: gattId which register by ql_ble_gatt_register
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_adverting_stop(QuecBtGattId gattId);

/*******************************************************************************
* @brief The GATT application starts GATT advertising with no advertisement and scan response data.
  @param:
        gattId: gattId which register by ql_ble_gatt_register.
        flags: The Connection flags, which are defined in ql_ble.h, these are bellow:
                        QUEC_BT_GATT_FLAGS_NONE                 
                        QUEC_BT_GATT_FLAGS_WHITELIST            
                        QUEC_BT_GATT_FLAGS_UNDIRECTED           
                        QUEC_BT_GATT_FLAGS_ADVERTISE_TIMEOUT    
                        QUEC_BT_GATT_FLAGS_NONDISCOVERABLE      
                        QUEC_BT_GATT_FLAGS_LIMITED_DISCOVERABLE 
                        QUEC_BT_GATT_FLAGS_DISABLE_SCAN_RESPONSE
                        QUEC_BT_GATT_FLAGS_APPEND_DATA          
                        QUEC_BT_GATT_FLAGS_NO_AUTO_SECURITY     
                        QUEC_BT_GATT_FLAGS_NO_AUTO_SIGN_UPGRADE 
                        QUEC_BT_GATT_FLAGS_WHITELIST_SCANRSP    
                        QUEC_BT_GATT_FLAGS_ATTEMPTED_SECURITY   
                        QUEC_BT_GATT_FLAGS_CENTRAL_TIMEOUT
        adv_data: Advertising data formatted as defined in Bluetooth Core Specification
                  Version 4.2 Volume 3 Part C ADVERTISING AND SCAN RESPONSE DATA FORMAT.
        adv_data_len: Length of the advertising Data in octets.
        scresp_data:  Scan Response Data formatted as defined in Bluetooth Core Specification
                      Version 4.2 Volume 3 Part C ADVERTISING AND SCAN RESPONSE DATA FORMAT. 
        scresp_data_len: Length of the Scan Response Data in octets.

  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_ble_gattadvdata(QuecBtGattId gattId,
                       QuecBtGattConnFlags flags,
                       unsigned char *adv_data,
                       unsigned char adv_data_len,
                       unsigned char *scresp_data,
                       unsigned char scresp_data_len);

/*******************************************************************************
* @brief A GATT application accepts the establishment of an LE connection with devices in the central role, the
         local device acts as a peripheral. Data on the device's advertising and scan response are provided.
  @param:
        gattId: gattId which register by ql_ble_gatt_register.
        flags: The Connection flags, which are defined in ql_ble.h, these are bellow:
                        QUEC_BT_GATT_FLAGS_NONE                 
                        QUEC_BT_GATT_FLAGS_WHITELIST            
                        QUEC_BT_GATT_FLAGS_UNDIRECTED           
                        QUEC_BT_GATT_FLAGS_ADVERTISE_TIMEOUT    
                        QUEC_BT_GATT_FLAGS_NONDISCOVERABLE      
                        QUEC_BT_GATT_FLAGS_LIMITED_DISCOVERABLE 
                        QUEC_BT_GATT_FLAGS_DISABLE_SCAN_RESPONSE
                        QUEC_BT_GATT_FLAGS_APPEND_DATA          
                        QUEC_BT_GATT_FLAGS_NO_AUTO_SECURITY     
                        QUEC_BT_GATT_FLAGS_NO_AUTO_SIGN_UPGRADE 
                        QUEC_BT_GATT_FLAGS_WHITELIST_SCANRSP    
                        QUEC_BT_GATT_FLAGS_ATTEMPTED_SECURITY   
                        QUEC_BT_GATT_FLAGS_CENTRAL_TIMEOUT
        preferredMtu:    The preferable packet size. 0 = no preference. Minimum/default is 23 bytes.
        adv_invterval_min: Min. advertising interval (slots)
        adv_invterval_max: Max. advertising interval (slots)
        pkt_adv: Advertising data formatted as defined in Bluetooth Core Specification
                  Version 4.2 Volume 3 Part C ADVERTISING AND SCAN RESPONSE DATA FORMAT.
        pkt_adv_len: Length of the advertising Data in octets.
        pkt_scan_rsp: Scan Response Data formatted as defined in Bluetooth Core Specification
                      Version 4.2 Volume 3 Part C ADVERTISING AND SCAN RESPONSE DATA FORMAT. 
        pkt_scan_rsp_len: Length of the Scan Response Data in octets.

  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_ble_gattadvdata_connectable(QuecBtGattId gattid,
                                   QuecBtUint32 flags,
                                   QuecBtUint16 preferredMtu,
                                   QuecBtUint16 adv_invterval_min,
                                   QuecBtUint16 adv_invterval_max,
                                   QuecBtUint8 *pkt_adv,
                                   QuecBtUint8 pkt_adv_len,
                                   QuecBtUint8 *pkt_scan_rsp,
                                   QuecBtUint8 pkt_scan_rsp_len);

/*******************************************************************************
* @brief read accress response, this function may called after recv QUEC_BT_GATT_DB_READ_ACCESS_IND
  @param:
        gattId: gattId which register by ql_ble_gatt_register
        connId: Connection identifier
        attrHandle: attribute handle
        result: result of write access indication
        size: response data length
        value: value of the attribute
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_gatt_read_response(QuecBtGattId gattId, 
                                    QuecBtConnId connId, 
                                    QuecBtUint16 attrHandle, 
                                    QuecResultCode result, 
                                    QuecBtUint16 valueLength, 
                                    QuecBtUint8* value);


/*******************************************************************************
* @brief write accress response, this function may called after recv QUEC_BT_GATT_DB_WRITE_ACCESS_IND
  @param:
        gattId: gattId which register by ql_ble_gatt_register
        connId: Connection identifier
        attrHandle: attribute handle
        result: result of write access indication
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_gatt_write_response(QuecBtGattId gattId,
                                        QuecBtConnId connId,
                                        QuecBtUint16 attrHandle, 
                                        QuecResultCode result);

/*******************************************************************************
* @brief send indication
  @param:
        gattId: gattId which register by ql_ble_gatt_register
        connId: Connection identifier
        attrHandle: attribute handle
        valueLength: value length of the attribute
        value:  value of the attribute
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_gatt_send_notification(QuecBtGattId gattId,
                                QuecBtConnId connId,
                                QuecBtUint16 attrHandle, 
                                QuecBtUint16 valueLength, 
                                QuecBtUint8 *value);


/*******************************************************************************
* @brief send notification
  @param:
        gattId: gattId which register by ql_ble_gatt_register
        connId: Connection identifier
        attrHandle: attribute handle
        valueLength: value length of the attribute
        value:  value of the attribute
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define
 *******************************************************************************/
int ql_ble_gatt_send_indication(QuecBtGattId gattId,
                                QuecBtConnId connId,
                                QuecBtUint16 attrHandle, 
                                QuecBtUint16 valueLength, 
                                QuecBtUint8 *value);

/*

*/
/*******************************************************************************
* @brief Change default parameters for advertising. 
        advIntervalMin and advIntervalMax rang : 0x20 ~ 0x4000 and the Time = N * 0.625 msec
  @param:
        gattId: gattId which register by ql_ble_gatt_register
        advIntervalMin: Min. advertising interval (slots)
        advIntervalMax: Max. advertising interval (slots)

  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       ble error, see ble error define 
 *******************************************************************************/
int ql_ble_gatt_set_adverting_param(QuecBtGattId gattId,
                                    QuecBtUint16 advIntervalMin, 
                                    QuecBtUint16 advIntervalMax);




/*ble db operate*/
/*******************************************************************************
* @brief add a service in ble service database struct
  @param:
        gattId: gattId which register by ql_ble_gatt_register
        svrID: sevrice id, start with 1
        uuid:  service uuid, 16-bit
        isPrimary: if service is primary, value is 1, else 0
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_ble_db_service_add(QuecBtGattId gattId,
                            QuecBtUint16 svrID, 
                            QuecBtUuid16 uuid, 
                            QuecBtUint8 isPrimary);
int ql_ble_db_service_add_128bit(QuecBtGattId gattId,
                            QuecBtUint16 svrID, 
                            QuecBtUuid128 uuid, 
                            QuecBtUint8 isPrimary);

/*******************************************************************************
* @brief delete a service from ble service database struct
  @param:
        gattId: gattId which register by ql_ble_gatt_register
        svrID: sevrice id
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_ble_db_service_del(QuecBtGattId gattId, QuecBtUint16 svrID);

/*******************************************************************************
* @brief active/inactive a service
  @param:
        gattId: gattId which register by ql_ble_gatt_register
        svrID: sevrice id
        isActive: if service is active, value is 1, else 0
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_ble_db_service_set_active(QuecBtGattId gattId, QuecBtUint16 svrID, QuecBtUint8 isActive);

/*******************************************************************************
* @brief add a characteristic in ble service database struct
  @param:
        gattId: gattId which register by ql_ble_gatt_register
        svrID: service id which the characteristic will be inserted
        charactID: characteristic id, start with 1
        uuid:   the characteristic uuid
        valueLength: the characteristic value length
        prop:   the characteristic properties, which are defined in ql_ble.h, these are bellow:
                        QUEC_ATT_PERM_CONFIGURE_BROADCAST
                        QUEC_ATT_PERM_READ
                        QUEC_ATT_PERM_WRITE_CMD
                        QUEC_ATT_PERM_WRITE_REQ
                        QUEC_ATT_PERM_NOTIFY
                        QUEC_ATT_PERM_INDICATE
                        QUEC_ATT_PERM_AUTHENTICATED
                        QUEC_ATT_PERM_EXTENDED
        attrValueFlags:the characteristic attribute , which are defined in ql_ble.h, these are bellow:
                        QUEC_BT_GATT_ATTR_FLAGS_NONE
                        QUEC_BT_GATT_ATTR_FLAGS_DYNLEN
                        QUEC_BT_GATT_ATTR_FLAGS_IRQ_READ
                        QUEC_BT_GATT_ATTR_FLAGS_IRQ_WRITE
                        QUEC_BT_GATT_ATTR_FLAGS_READ_ENCRYPTION
                        QUEC_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION
                        QUEC_BT_GATT_ATTR_FLAGS_READ_SC_AUTHENTICAT
                        QUEC_BT_GATT_ATTR_FLAGS_WRITE_ENCRYPTION
                        QUEC_BT_GATT_ATTR_FLAGS_WRITE_AUTHENTICATIO
                        QUEC_BT_GATT_ATTR_FLAGS_WRITE_SC_AUTHENTICA
                        QUEC_BT_GATT_ATTR_FLAGS_AUTHORISATION
                        QUEC_BT_GATT_ATTR_FLAGS_ENCR_KEY_SIZE
                        QUEC_BT_GATT_ATTR_FLAGS_DISABLE_LE
                        QUEC_BT_GATT_ATTR_FLAGS_DISABLE_BREDR
        value: the characteristic value
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_ble_db_charact_add(QuecBtGattId gattId, 
                            QuecBtUint16 svrID, 
                            QuecBtUint16 charactID, 
                            QuecBtUint16 uuid,
                            QuecBtUint16 valueLength, 
                            QuecBtUint8 prop,
                            QuecBtUint16 attrValueFlags, 
                            QuecBtUint8 *value);
int ql_ble_db_charact_add_128bit(QuecBtGattId gattId, 
                            QuecBtUint16 svrID, 
                            QuecBtUint16 charactID, 
                            QuecBtUuid128 uuid,
                            QuecBtUint16 valueLength, 
                            QuecBtUint8 prop,
                            QuecBtUint16 attrValueFlags, 
                            QuecBtUint8 *value);

/*******************************************************************************
* @brief delete a characteristic from ble service database struct
  @param:
        gattId: gattId which register by ql_ble_gatt_register
        svrID: service id which the characteristic will be deleted
        charactID: characteristic id which will be deleted
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_ble_db_charact_del(QuecBtGattId gattId, QuecBtUint16 svrID, QuecBtUint16 charactID);

/*******************************************************************************
* @brief add a descriptor in ble service database struct
  @param:
        gattId: gattId which register by ql_ble_gatt_register
        svrID: service id which the descriptor will be inserted
        charactID: characteristic id which the descriptor will be inserted.
        descID: descriptor id, start with 1
        uuid:   the descriptor uuid
        valueLength: the descriptor value length
        prop:   the descriptor properties, which are defined in ql_ble.h, these are bellow:
                        QUEC_ATT_PERM_CONFIGURE_BROADCAST
                        QUEC_ATT_PERM_READ
                        QUEC_ATT_PERM_WRITE_CMD
                        QUEC_ATT_PERM_WRITE_REQ
                        QUEC_ATT_PERM_NOTIFY
                        QUEC_ATT_PERM_INDICATE
                        QUEC_ATT_PERM_AUTHENTICATED
                        QUEC_ATT_PERM_EXTENDED
        attrValueFlags:the descriptor attribute , which are defined in ql_ble.h, these are bellow:
                        QUEC_BT_GATT_ATTR_FLAGS_NONE
                        QUEC_BT_GATT_ATTR_FLAGS_DYNLEN
                        QUEC_BT_GATT_ATTR_FLAGS_IRQ_READ
                        QUEC_BT_GATT_ATTR_FLAGS_IRQ_WRITE
                        QUEC_BT_GATT_ATTR_FLAGS_READ_ENCRYPTION
                        QUEC_BT_GATT_ATTR_FLAGS_READ_AUTHENTICATION
                        QUEC_BT_GATT_ATTR_FLAGS_READ_SC_AUTHENTICAT
                        QUEC_BT_GATT_ATTR_FLAGS_WRITE_ENCRYPTION
                        QUEC_BT_GATT_ATTR_FLAGS_WRITE_AUTHENTICATIO
                        QUEC_BT_GATT_ATTR_FLAGS_WRITE_SC_AUTHENTICA
                        QUEC_BT_GATT_ATTR_FLAGS_AUTHORISATION
                        QUEC_BT_GATT_ATTR_FLAGS_ENCR_KEY_SIZE
                        QUEC_BT_GATT_ATTR_FLAGS_DISABLE_LE
                        QUEC_BT_GATT_ATTR_FLAGS_DISABLE_BREDR
        value: the descriptor value
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_ble_db_descriptor_add(QuecBtGattId gattId, 
                            QuecBtUint16 svrID, 
                            QuecBtUint16 charactID,
                            QuecBtUint16 descID,
                            QuecBtUuid16 uuid,
                            QuecBtUint16 valueLength,
                            QuecBtUint8 prop,
                            QuecBtUint16 attrValueFlags,
                            QuecBtUint8 *value);
int ql_ble_db_descriptor_add_128bit(QuecBtGattId gattId, 
                            QuecBtUint16 svrID, 
                            QuecBtUint16 charactID,
                            QuecBtUint16 descID,
                            QuecBtUuid128 uuid,
                            QuecBtUint16 valueLength,
                            QuecBtUint8 prop,
                            QuecBtUint16 attrValueFlags,
                            QuecBtUint8 *value);

/*******************************************************************************
* @brief delete a descriptor from ble service database struct
  @param:
        gattId: gattId which register by ql_ble_gatt_register 
      
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_ble_db_descriptor_del(QuecBtGattId gattId, 
                            QuecBtUint16 svrID, 
                            QuecBtUint16 charactID, 
                            QuecBtUint16 descID);

/*******************************************************************************
* @brief help function, show ble service database in the log when be called, ##do not use##.
  @param:
        gattId: gattId which register by ql_ble_gatt_register 
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_ble_db_show(QuecBtGattId gattId);

/*Function: connect to BLE server
*/
int ql_ble_client_connect(BtSdScanAddress devaddr, QuecBtGattId gattid);
int ql_ble_client_find_service(QuecBtUint16 puuid, QuecBtGattId gattid, QuecBtConnId connid, BtBleClientFindServiceInd *serviceind);
int ql_ble_gatt_client_init(QuecBtGattId *gattid);
int ql_ble_client_configure_attr(QuecBtGattId gattid, QuecBtConnId connid, QuecBtUint16 puuid, QuecBtUint16 cuuid, QuecBtUint16 config);
int ql_ble_client_unregister( QuecBtGattId gattid, QuecBtConnId connid);

int ql_ble_set_security_controller(QuecBtUint8 secmode, QuecBtGattId gattid, QuecBtUint8 ioCapability);
#ifdef __cplusplus
}
#endif

#endif /* !QL_BLE_V2_H_ */
