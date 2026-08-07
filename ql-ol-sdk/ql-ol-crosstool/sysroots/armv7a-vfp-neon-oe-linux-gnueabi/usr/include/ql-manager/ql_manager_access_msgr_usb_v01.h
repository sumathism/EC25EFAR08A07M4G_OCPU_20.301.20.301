#ifndef QL_MANAGER_ACCESS_MSGR_USB_SERVICE_01_H
#define QL_MANAGER_ACCESS_MSGR_USB_SERVICE_01_H
/**
  @file ql_manager_access_msgr_usb_v01.h

  @brief This is the public header file which defines the ql_manager_access_msgr_usb service Data structures.

  This header file defines the types and structures that were defined in
  ql_manager_access_msgr_usb. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  

  
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Mon Dec 15 2025 (Spin 0)
   From IDL File: ql_manager_access_msgr_usb_v01.idl */

/** @defgroup ql_manager_access_msgr_usb_qmi_consts Constant values defined in the IDL */
/** @defgroup ql_manager_access_msgr_usb_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup ql_manager_access_msgr_usb_qmi_enums Enumerated types used in QMI messages */
/** @defgroup ql_manager_access_msgr_usb_qmi_messages Structures sent as QMI messages */
/** @defgroup ql_manager_access_msgr_usb_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup ql_manager_access_msgr_usb_qmi_accessor Accessor for QMI service object */
/** @defgroup ql_manager_access_msgr_usb_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "ql_manager_access_msgr_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ql_manager_access_msgr_usb_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define QL_MANAGER_ACCESS_MSGR_USB_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define QL_MANAGER_ACCESS_MSGR_USB_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define QL_MANAGER_ACCESS_MSGR_USB_V01_IDL_TOOL_VERS 0x06

/**
    @}
  */


/** @addtogroup ql_manager_access_msgr_usb_qmi_consts
    @{
  */
#define USB_PRODUCT_INFO_MAX_LEN_V01 33
#define USB_MANUFAC_INFO_MAX_LEN_V01 33
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_enums
    @{
  */
typedef enum {
  USB_NUM_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  USB_NUM_0_V01 = 0, 
  USB_NUM_MAX_V01 = 1, 
  USB_NUM_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}usb_num_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_enums
    @{
  */
typedef enum {
  USB_BOOTUP_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  USB_BOOTUP_FS_UP_V01 = 0, /**<  The usb enable when the sysfs mounts done. */
  USB_BOOTUP_MODEM_UP_V01 = 1, /**<  The usb enable when the modem starts up done. */
  USB_BOOTUP_LINUX_UP_V01 = 2, /**<  The usb enable when the linux starts up done. */
  USB_BOOTUP_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}usb_bootup_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_enums
    @{
  */
typedef enum {
  USB_ACTION_TYPE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  USB_ACTION_TYPE_ENABLE_V01 = 0, 
  USB_ACTION_TYPE_DISABLE_V01 = 1, 
  USB_ACTION_TYPE_RELOAD_V01 = 2, 
  USB_ACTION_TYPE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}usb_action_type_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_enums
    @{
  */
typedef enum {
  USB_NET_TYPE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  USB_NET_TYPE_RMENT_V01 = 0, 
  USB_NET_TYPE_ECM_V01 = 1, 
  USB_NET_TYPE_MBIM_V01 = 2, 
  USB_NET_TYPE_RNDIS_V01 = 3, 
  USB_NET_TYPE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}usb_net_type_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t pid;

  uint16_t vid;
}__usb_id_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ql_manager_msgr_set_usb_config_write_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_usb_config_write_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  usb_num_e_v01 num;

  /* Mandatory */
  usb_action_type_e_v01 action_type;
}ql_manager_msgr_set_usb_action_type_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_usb_action_type_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  usb_num_e_v01 num;

  /* Mandatory */
  usb_bootup_e_v01 up;
}ql_manager_msgr_set_usb_bootup_type_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_usb_bootup_type_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  usb_num_e_v01 num;

  /* Mandatory */
  __usb_id_v01 usb_id;
}ql_manager_msgr_set_usb_id_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_usb_id_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  usb_num_e_v01 num;

  /* Mandatory */
  uint8_t product_info[USB_PRODUCT_INFO_MAX_LEN_V01];
}ql_manager_msgr_set_usb_product_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_usb_product_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  usb_num_e_v01 num;

  /* Mandatory */
  uint8_t manufac_info[USB_MANUFAC_INFO_MAX_LEN_V01];
}ql_manager_msgr_set_usb_manufac_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_usb_manufac_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  usb_num_e_v01 num;

  /* Mandatory */
  usb_net_type_e_v01 net;
}ql_manager_msgr_set_usb_net_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_usb_net_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  usb_num_e_v01 num;

  /* Mandatory */
  uint8_t diag_enable;

  /* Mandatory */
  uint8_t nmea_enable;

  /* Mandatory */
  uint8_t at_enable;

  /* Mandatory */
  uint8_t modem_enable;

  /* Mandatory */
  uint8_t net_enable;

  /* Mandatory */
  uint8_t adb_enable;

  /* Mandatory */
  uint8_t audio_uac_enable;
}ql_manager_msgr_set_usb_func_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_usb_func_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  usb_num_e_v01 num;
}ql_manager_msgr_get_usb_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  usb_bootup_e_v01 bootup;

  /* Mandatory */
  __usb_id_v01 usb_id;

  /* Mandatory */
  uint8_t product_info[USB_PRODUCT_INFO_MAX_LEN_V01];

  /* Mandatory */
  uint8_t manufac_info[USB_MANUFAC_INFO_MAX_LEN_V01];

  /* Mandatory */
  usb_net_type_e_v01 net;

  /* Mandatory */
  uint8_t diag_enable;

  /* Mandatory */
  uint8_t nmea_enable;

  /* Mandatory */
  uint8_t at_enable;

  /* Mandatory */
  uint8_t modem_enable;

  /* Mandatory */
  uint8_t net_enable;

  /* Mandatory */
  uint8_t adb_enable;

  /* Mandatory */
  uint8_t audio_uac_enable;
}ql_manager_msgr_get_usb_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  usb_num_e_v01 num;

  /* Mandatory */
  uint8_t interface;
}ql_manager_msgr_set_usb_interface_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_usb_interface_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  usb_num_e_v01 num;
}ql_manager_msgr_get_usb_interface_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_usb_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t interface;
}ql_manager_msgr_get_usb_interface_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 

/*Extern Definition of Type Table Object*/
/*THIS IS AN INTERNAL OBJECT AND SHOULD ONLY*/
/*BE ACCESSED BY AUTOGENERATED FILES*/
extern const qmi_idl_type_table_object ql_manager_access_msgr_usb_qmi_idl_type_table_object_v01;


#ifdef __cplusplus
}
#endif
#endif

