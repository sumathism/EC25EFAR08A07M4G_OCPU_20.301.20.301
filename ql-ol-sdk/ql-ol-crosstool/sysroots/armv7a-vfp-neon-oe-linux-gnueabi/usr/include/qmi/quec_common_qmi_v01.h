#ifndef QUEC_COMMON_QMI_SERVICE_01_H
#define QUEC_COMMON_QMI_SERVICE_01_H
/**
  @file quec_common_qmi_v01.h

  @brief This is the public header file which defines the quec_common_qmi service Data structures.

  This header file defines the types and structures that were defined in
  quec_common_qmi. It contains the constant values defined, enums, structures,
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
   It was generated on: Mon Dec  8 2025 (Spin 0)
   From IDL File: quec_common_qmi_v01.idl */

/** @defgroup quec_common_qmi_qmi_consts Constant values defined in the IDL */
/** @defgroup quec_common_qmi_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup quec_common_qmi_qmi_enums Enumerated types used in QMI messages */
/** @defgroup quec_common_qmi_qmi_messages Structures sent as QMI messages */
/** @defgroup quec_common_qmi_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup quec_common_qmi_qmi_accessor Accessor for QMI service object */
/** @defgroup quec_common_qmi_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup quec_common_qmi_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define QUEC_COMMON_QMI_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define QUEC_COMMON_QMI_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define QUEC_COMMON_QMI_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define QUEC_COMMON_QMI_V01_MAX_MESSAGE_ID 0x003A
/**
    @}
  */


/** @addtogroup quec_common_qmi_qmi_consts
    @{
  */

/**     */
#define QUEC_COMMON_QMI_SERVICE_V01 227

/**  Maximum size of the ecall urc event */
#define QUEC_COMMMON_MAX_ECALL_URC_EVENT_LENGTH_V01 128
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_enums
    @{
  */
typedef enum {
  QUEC_COMMON_QMI_RESULT_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QUEC_COMM_QMI_SUCCESS_V01 = 0, 
  QUEC_COMM_QMI_FAILUTE_V01 = 1, 
  QUEC_COMMON_QMI_RESULT_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}quec_common_qmi_result_type_v01;
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_enums
    @{
  */
typedef enum {
  QUEC_COMMON_QMI_ERROR_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QUEC_COMM_QMI_ERR_NONE_V01 = 0x0000, 
  QUEC_COMMON_QMI_ERROR_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}quec_common_qmi_error_type_v01;
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_aggregates
    @{
  */
typedef struct {

  quec_common_qmi_result_type_v01 result_t;

  quec_common_qmi_error_type_v01 err_t;
}quec_qcommon_qmi_response_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_enums
    @{
  */
typedef enum {
  QUEC_RESULT_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QUEC_RESULT_SUCCESS_V01 = 0, /**<  Success. */
  QUEC_RESULT_FAILURE_V01 = 1, /**<  Failure. */
  QUEC_RESULT_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}quec_result_t_v01;
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_enums
    @{
  */
typedef enum {
  QUEC_ERROR_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QUEC_SUCCESS_V01 = 0, /**<  Success. */
  QUEC_SUCCESS_CONDITIONAL_SUCCESS_V01 = 1, /**<  Conditional success. */
  QUEC_ERROR_QUEC_SERVICES_NOT_AVAILABLE_V01 = 2, /**<  QUEC services not available. */
  QUEC_ERROR_GENERIC_V01 = 3, /**<  Generic error. */
  QUEC_ERROR_BADPARM_V01 = 4, /**<  Bad parameter. */
  QUEC_ERROR_MEMORY_V01 = 5, /**<  Memory error. */
  QUEC_ERROR_INVALID_STATE_V01 = 6, /**<  Invalid state. */
  QUEC_ERROR_MALFORMED_MSG_V01 = 7, /**<  Malformed message. */
  QUEC_ERROR_NO_MEMORY_V01 = 8, /**<  No memory. */
  QUEC_ERROR_INTERNAL_V01 = 9, /**<  Internal error. */
  QUEC_ERROR_ABORTED_V01 = 10, /**<  Action was aborted. */
  QUEC_ERROR_CLIENT_IDS_EXHAUSTED_V01 = 11, /**<  Client IDs have been exhausted. */
  QUEC_ERROR_UNABORTABLE_TRANSACTION_V01 = 12, /**<  Unabortable transaction. */
  QUEC_ERROR_INVALID_CLIENT_ID_V01 = 13, /**<  Invalid client ID. */
  QUEC_ERROR_NO_THRESHOLDS_V01 = 14, /**<  No thresholds. */
  QUEC_ERROR_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}quec_error_t_v01;
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_enums
    @{
  */
typedef enum {
  QUEC_VOICE_ECALL_STATE_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QUEC_VOICE_ECALL_INACTIVE_V01 = 0, 
  QUEC_VOICE_ECALL_ORIGINATING_CALL_V01 = 1, 
  QUEC_VOICE_ECALL_IN_CALL_TRANSMITTING_V01 = 2, /**<  ECALL APP TRANSMITTING */
  QUEC_VOICE_ECALL_WATING_FOR_AL_ACK_V01 = 3, 
  QUEC_VOICE_ECALL_IN_CALL_V01 = 4, 
  QUEC_VOICE_ECALL_IDLE_ALLOW_MT_ECALL_V01 = 5, 
  QUEC_VOICE_ECALL_STATE_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}quec_voice_ecall_state_t_v01;
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_enums
    @{
  */
typedef enum {
  VTONE_COUNTRYENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  VTONE_COUNTRY_AUSTRALIA_V01 = 0, 
  VTONE_COUNTRY_OTHER_V01 = 1, 
  VTONE_COUNTRY_SPARK_V01 = 2, 
  VTONE_COUNTRY_MAX_V01 = 3, 
  VTONE_COUNTRYENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}VTONE_CountryEnum_v01;
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_enums
    @{
  */
typedef enum {
  VTONE_TONEENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  VTONE_TONE_BUSY_V01 = 0, 
  VTONE_TONE_CONGESTION_V01 = 1, 
  VTONE_TONE_NUMBER_UNOBTAINABLE_V01 = 2, 
  VTONE_TONE_RINGING_V01 = 3, 
  VTONE_TONE_CALL_WAITING_V01 = 4, 
  VTONE_TONE_MAX_V01 = 5, 
  VTONE_TONEENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}VTONE_ToneEnum_v01;
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_aggregates
    @{
  */
typedef struct {

  quec_result_t_v01 result;
  /**<   Result code:
                            - QUEC_RESULT_SUCCESS
                            - QUEC_RESULT_FAILURE
                          */

  quec_error_t_v01 error;
  /**<   Error code. Possible error code values are
                            described in the error codes section of each
                            message definition.
                          */
}quec_response_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}quec_common_qmi_get_audio_mod_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t value;
}quec_common_qmi_get_audio_mod_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t value;
}quec_common_qmi_get_audio_mod_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t gpio;

  uint32_t status;

  uint32_t on_time;

  uint32_t off_time;
}gpio_on_off_status_v01;  /* Type */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_enums
    @{
  */
typedef enum {
  QUEC_VOICE_ECALL_MODE_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QUEC_VOICE_ECALL_MODE_NOMAL_V01 = 0, 
  QUEC_VOICE_ECALL_MODE_ONLY_V01 = 1, 
  QUEC_VOICE_ECALL_MODE_DEFAULT_V01 = 2, /**<  ECALL APP TRANSMITTING */
  QUEC_VOICE_ECALL_MODE_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}quec_voice_ecall_mode_t_v01;
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t enable;

  /*  Enable or disable ecall 
	0 -- Disable; 1 -- Enable */
  uint8_t voiceconfig;

  /*  Enable or disable to mute IVS speaker in MSD
	0 -- Disable to mute IVS speaker automatical in MSD transmission
	1 -- Enable to mute IVS speaker automatical in MSD transmission */
  quec_voice_ecall_mode_t_v01 ecallmode;

  /*  The Ecall mode. */
  uint8_t processinfo;

  /*  Enable or disable to report ecall event info.
	0 -- Disable; 1 -- Enable */
  uint16_t T5;

  /*  The timer of IVS waiting for "START". */
  uint16_t T6;

  /*  The timer of IVS waiting for "HACK". */
  uint16_t T7;

  /*  The timer of MSD transmission. */
  uint8_t mofailredial;

  /*  The dial fail times. */
  uint8_t dropredial;
}quec_voice_ecall_config_info_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_enums
    @{
  */
typedef enum {
  QUEC_VOICE_ECALL_EVENT_FAIL_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QUEC_VOICE_ECALL_EVENT_FAIL_START_TIMEOUT_V01 = 1, /**<  Wait for START timeout. */
  QUEC_VOICE_ECALL_EVENT_FAIL_HACK_TIMEOUT_V01 = 2, /**<  Wait for HACK timeout.	 */
  QUEC_VOICE_ECALL_EVENT_FAIL_MSD_TRANSMISSION_TIMEOUT_V01 = 3, /**<  MSD transmission timeout.	 */
  QUEC_VOICE_ECALL_EVENT_FAIL_IVS_RESET_TIMEOUT_V01 = 4, /**<  IVS reset. */
  QUEC_VOICE_ECALL_EVENT_FAIL_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}quec_voice_ecall_event_fail_t_v01;
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_enums
    @{
  */
typedef enum {
  QUEC_VOICE_ECALL_EVENT_PROCESS_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QUEC_VOICE_ECALL_EVENT_PROCESS_IVS_START_RECEIVED_MSD_V01 = 1, /**<  IVS Link Layer receives START message and starts to send MSD. */
  QUEC_VOICE_ECALL_EVENT_PROCESS_IVS_NACK_RECEIVED_V01 = 2, /**<  IVS Link Layer receives NACK message. */
  QUEC_VOICE_ECALL_EVENT_PROCESS_IVS_ACK_RECEIVED_V01 = 3, /**<  IVS Link Layer receives the first LACK message. */
  QUEC_VOICE_ECALL_EVENT_PROCESS_IVS_TX_COMPLETED_V01 = 4, /**<  IVS MSD transmission is complete. */
  QUEC_VOICE_ECALL_EVENT_PROCESS_IVS_HLACK_RECEIVED_V01 = 5, /**<  IVS Link Layer receives first HACK message. */
  QUEC_VOICE_ECALL_EVENT_PROCESS_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}quec_voice_ecall_event_process_t_v01;
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_enums
    @{
  */
typedef enum {
  QUEC_VOICE_ECALL_EVENT_MSDUPADTE_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QUEC_VOICE_ECALL_EVENT_MSDUPDATE_IVS_UPDATING_MSD_V01 = 0, /**<  Indicates MSD has been updated. */
  QUEC_VOICE_ECALL_EVENT_MSDUPDATE_PSAP_REQURE_UPDATE_MSD_V01 = 1, /**<  Indicate to update MSD in 5 seconds. */
  QUEC_VOICE_ECALL_EVENT_MSDUPDATE_IVS_UPDATE_MSD_TIMEOUT_V01 = 2, /**<  Indicate timeout of updating MSD and module starts to transfer the old MSD. */
  QUEC_VOICE_ECALL_EVENT_MSDUPADTE_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}quec_voice_ecall_event_msdupadte_t_v01;
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_enums
    @{
  */
typedef enum {
  QUEC_VOICE_ECALL_EVENT_ESTABLISH_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QUEC_VOICE_ECALL_EVENT_ECALL_ESTABLISHNG_SUCCESS_V01 = 0, /**<  Establish eCall successfully. */
  QUEC_VOICE_ECALL_EVENT_ECALL_ESTABLISHNG_FAIL_V01 = 1, /**<  Establish eCall fails. */
  QUEC_VOICE_ECALL_EVENT_ESTABLISH_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}quec_voice_ecall_event_establish_t_v01;
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t hack_code;
  /**<  psap hack code*/
}quec_voice_ecall_event_hackcode_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t ori_remainder_times;
  /**<   originate fail remainder times*/

  uint16_t time;
  /**<   the minimum time duration between the previous call attempt*/
}quec_voice_ecall_event_originate_fail_and_redial_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t drop_remainder_times;
  /**<   dorp remainder times*/

  uint16_t time;
  /**<   the minimum time duration between the previous call attempt*/
}quec_voice_ecall_event_drop_and_redial_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}quec_common_qmi_get_gpio_status_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  gpio_on_off_status_v01 gpio_status;
}quec_common_qmi_get_gpio_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  gpio_on_off_status_v01 gpio_status;
}quec_common_qmi_get_gpio_status_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}quec_common_qmi_get_sclk_value_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t sclk_value;
}quec_common_qmi_get_sclk_value_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t sclk_value;
}quec_common_qmi_get_sclk_value_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t timer_id;
}quec_common_qmi_register_timer_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t result;
}quec_common_qmi_register_timer_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t timer_id;

  /* Mandatory */
  uint32_t interval;

  /* Mandatory */
  uint32_t auto_repeat;
}quec_common_qmi_start_timer_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t result;
}quec_common_qmi_start_timer_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t timer_id;
}quec_common_qmi_stop_timer_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t result;
}quec_common_qmi_stop_timer_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t timer_id;
}quec_common_qmi_timer_expire_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t forbid_ind;
}quec_common_qmi_forbid_ind_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t result;
}quec_common_qmi_forbid_ind_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}quec_common_qmi_get_mob_model_id_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  char mob_model_id[256];
}quec_common_qmi_get_mob_model_id_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t profile_num;
}quec_common_qmi_get_3gpp_pdn_state_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t profile_num;

  /* Mandatory */
  int32_t pdn_state;
}quec_common_qmi_get_3gpp_pdn_state_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t profile_num;
}quec_common_qmi_deact_3gpp_pdn_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t result;
}quec_common_qmi_deact_3gpp_pdn_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t update_state;
}quec_common_qmi_sbfota_update_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t resp_code;
}quec_common_qmi_sbfota_update_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t profile_id;
}quec_common_qmi_sbfota_get_apn_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t profile_id;

  /* Mandatory */
  char apn[128];

  /* Mandatory */
  char user[128];

  /* Mandatory */
  char password[128];

  /* Mandatory */
  int32_t auth;
}quec_common_qmi_sbfota_get_apn_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t profile_id;

  /* Mandatory */
  char apn[128];

  /* Mandatory */
  char user[128];

  /* Mandatory */
  char password[128];

  /* Mandatory */
  int32_t auth;
}quec_common_qmi_sbfota_set_apn_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t result;
}quec_common_qmi_sbfota_set_apn_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t call_type;

  /* Mandatory */
  uint32_t call_event;
}quec_common_qmi_call_event_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}quec_common_qmi_get_fw_version_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  char fw_version[256];
}quec_common_qmi_get_fw_version_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t country;

  /* Mandatory */
  uint32_t tone;
}quec_common_qmi_tone_event_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}quec_common_qmi_wificfg_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t mac[6];
}quec_common_qmi_wificfg_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t mac[6];
}quec_common_qmi_wificfg_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t thermal_limit_rates_flag;
}quec_common_qmi_get_thermal_limit_rates_flag_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t thermal_limit_rates_flag;
}quec_common_qmi_get_thermal_limit_rates_flag_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t thermal_limit_rates_flag;
}quec_common_qmi_get_thermal_limit_rates_flag_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  char file_name[256];

  /* Mandatory */
  int32_t open_flag;
}quec_common_qmi_efs_file_open_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t result;

  /* Mandatory */
  int32_t file_handle;
}quec_common_qmi_efs_file_open_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  char file_name[256];
}quec_common_qmi_efs_file_size_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t result;

  /* Mandatory */
  uint32_t file_size;
}quec_common_qmi_efs_file_size_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t file_handle;

  /* Mandatory */
  uint32_t read_len;
}quec_common_qmi_efs_file_read_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t result;

  /* Mandatory */
  uint8_t data[1024];

  /* Mandatory */
  uint32_t actual_read_len;
}quec_common_qmi_efs_file_read_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t file_handle;

  /* Mandatory */
  uint8_t data[1024];

  /* Mandatory */
  uint32_t write_len;
}quec_common_qmi_efs_file_write_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t result;

  /* Mandatory */
  uint32_t actual_write_len;
}quec_common_qmi_efs_file_write_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t file_handle;
}quec_common_qmi_efs_file_close_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t result;
}quec_common_qmi_efs_file_close_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  char file_name[256];
}quec_common_qmi_efs_file_delete_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t result;
}quec_common_qmi_efs_file_delete_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t file_handle;

  /* Mandatory */
  int32_t offset;

  /* Mandatory */
  int32_t whence;
}quec_common_qmi_efs_file_seek_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t result;
}quec_common_qmi_efs_file_seek_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t sms_data_len;

  /* Mandatory */
  uint8_t sms_data[256];
}quec_common_qmi_ktfota_sms_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t status;
}quec_common_qmi_ktfota_dload_status_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  quec_qcommon_qmi_response_type_v01 result_code;
}quec_common_qmi_ktfota_dload_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}quec_common_qmi_ktfota_get_mob_model_id_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  char mob_model_id[256];
}quec_common_qmi_ktfota_get_mob_model_id_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}quec_common_qmi_ktfota_get_sw_version_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  char sw_version[256];
}quec_common_qmi_ktfota_get_sw_version_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}quec_common_qmi_ktfota_get_hw_version_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  char hw_version[256];
}quec_common_qmi_ktfota_get_hw_version_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}quec_common_qmi_ktfota_get_fw_version_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  char fw_version[256];
}quec_common_qmi_ktfota_get_fw_version_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t profile_num;
}quec_common_qmi_ktfota_get_pdn_state_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t pdn_state;
}quec_common_qmi_ktfota_get_pdn_state_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t profile_num;
}quec_common_qmi_ktfota_deactive_pdn_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t result;
}quec_common_qmi_ktfota_deactive_pdn_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t profile_num;

  /* Mandatory */
  uint8_t result;

  /* Mandatory */
  uint32_t sm_cause;
}quec_common_qmi_ktfota_deactive_pdn_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t profile_num;
}quec_common_qmi_ktfota_active_pdn_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t result;
}quec_common_qmi_ktfota_active_pdn_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t profile_num;

  /* Mandatory */
  uint8_t result;

  /* Mandatory */
  uint32_t sm_cause;
}quec_common_qmi_ktfota_active_pdn_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}quec_common_qmi_ktfota_get_device_quality_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t srv_status;

  /* Optional */
  uint8_t rat_type_valid;  /**< Must be set to true if rat_type is being passed */
  uint8_t rat_type;

  /* Optional */
  uint8_t lte_pci_valid;  /**< Must be set to true if lte_pci is being passed */
  int32_t lte_pci;

  /* Optional */
  uint8_t wcdma_psc_valid;  /**< Must be set to true if wcdma_psc is being passed */
  int32_t wcdma_psc;

  /* Optional */
  uint8_t lte_earfcn_valid;  /**< Must be set to true if lte_earfcn is being passed */
  int32_t lte_earfcn;

  /* Optional */
  uint8_t uarfcn_val_valid;  /**< Must be set to true if uarfcn_val is being passed */
  int32_t uarfcn_val;

  /* Optional */
  uint8_t rssi_val_valid;  /**< Must be set to true if rssi_val is being passed */
  int32_t rssi_val;

  /* Optional */
  uint8_t wcdma_rscp_valid;  /**< Must be set to true if wcdma_rscp is being passed */
  int32_t wcdma_rscp;

  /* Optional */
  uint8_t wcdma_ecio_valid;  /**< Must be set to true if wcdma_ecio is being passed */
  int32_t wcdma_ecio;

  /* Optional */
  uint8_t lte_rsrp_valid;  /**< Must be set to true if lte_rsrp is being passed */
  int32_t lte_rsrp;

  /* Optional */
  uint8_t lte_rsrq_valid;  /**< Must be set to true if lte_rsrq is being passed */
  int32_t lte_rsrq;

  /* Optional */
  uint8_t lte_sinr_valid;  /**< Must be set to true if lte_sinr is being passed */
  int32_t lte_sinr;

  /* Optional */
  uint8_t tx_power_valid;  /**< Must be set to true if tx_power is being passed */
  int32_t tx_power;

  /* Optional */
  uint8_t call_state_valid;  /**< Must be set to true if call_state is being passed */
  int32_t call_state;
}quec_common_qmi_ktfota_get_device_quality_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t reject_cause;
}quec_common_qmi_ktfota_ps_attach_reject_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t softsim_value;
}quec_common_qmi_get_softsim_value_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t softsim_value;
}quec_common_qmi_get_softsim_value_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t softsim_value;
}quec_common_qmi_get_softsim_value_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t quec_sleep_ind_flag;
}quec_common_qmi_quec_sleep_ind_flag_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t quec_sleep_ind_flag;
}quec_common_qmi_quec_sleep_ind_flag_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t quec_sleep_ind_flag;
}quec_common_qmi_quec_sleep_ind_flag_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t call_id;
}quec_common_qmi_ecall_hangup_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  quec_response_t_v01 resp;
}quec_common_qmi_ecall_hangup_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Optional */
  uint8_t enable_valid;  /**< Must be set to true if enable is being passed */
  uint8_t enable;

  /* Optional */
  /*  Enable or disable ecall 
	0 -- Disable; 1 -- Enable */
  uint8_t voiceconfig_valid;  /**< Must be set to true if voiceconfig is being passed */
  uint8_t voiceconfig;

  /* Optional */
  /*  Enable or disable to mute IVS speaker in MSD
	0 -- Disable to mute IVS speaker automatical in MSD transmission
	1 -- Enable to mute IVS speaker automatical in MSD transmission */
  uint8_t ecallmode_valid;  /**< Must be set to true if ecallmode is being passed */
  quec_voice_ecall_mode_t_v01 ecallmode;

  /* Optional */
  /*  The Ecall mode. */
  uint8_t processinfo_valid;  /**< Must be set to true if processinfo is being passed */
  uint8_t processinfo;

  /* Optional */
  /*  Enable or disable to report ecall event info.
	0 -- Disable; 1 -- Enable */
  uint8_t T5_valid;  /**< Must be set to true if T5 is being passed */
  uint16_t T5;

  /* Optional */
  /*  The timer of IVS waiting for "START". */
  uint8_t T6_valid;  /**< Must be set to true if T6 is being passed */
  uint16_t T6;

  /* Optional */
  /*  The timer of IVS waiting for "HACK". */
  uint8_t T7_valid;  /**< Must be set to true if T7 is being passed */
  uint16_t T7;

  /* Optional */
  /*  The timer of MSD transmission. */
  uint8_t mofailredial_valid;  /**< Must be set to true if mofailredial is being passed */
  uint8_t mofailredial;

  /* Optional */
  /*  The dial fail times. */
  uint8_t dropredial_valid;  /**< Must be set to true if dropredial is being passed */
  uint8_t dropredial;
}quec_common_qmi_set_ecall_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  quec_response_t_v01 resp;
}quec_common_qmi_set_ecall_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}quec_common_qmi_get_ecall_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  quec_response_t_v01 resp;

  /* Mandatory */
  quec_voice_ecall_config_info_t_v01 ecall_config;
}quec_common_qmi_get_ecall_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}quec_common_qmi_ecall_command_push_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  quec_response_t_v01 resp;

  /* Optional */
  uint8_t ecall_state_valid;  /**< Must be set to true if ecall_state is being passed */
  quec_voice_ecall_state_t_v01 ecall_state;
}quec_common_qmi_ecall_command_push_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t call_id;
  /**<  call_id.*/

  /* Mandatory */
  uint32_t ecall_urc_event_len;  /**< Must be set to # of elements in ecall_urc_event */
  char ecall_urc_event[QUEC_COMMMON_MAX_ECALL_URC_EVENT_LENGTH_V01];
}quec_common_qmi_ecall_urc_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Optional */
  uint8_t ecall_event_fails_valid;  /**< Must be set to true if ecall_event_fails is being passed */
  quec_voice_ecall_event_fail_t_v01 ecall_event_fails;

  /* Optional */
  /* 	event of eCall Failed. */
  uint8_t ecall_event_process_valid;  /**< Must be set to true if ecall_event_process is being passed */
  quec_voice_ecall_event_process_t_v01 ecall_event_process;

  /* Optional */
  /*  event of eCall process */
  uint8_t ecall_event_msdupdate_valid;  /**< Must be set to true if ecall_event_msdupdate is being passed */
  quec_voice_ecall_event_msdupadte_t_v01 ecall_event_msdupdate;
  /**<   event of ecall msd update.*/

  /* Optional */
  uint8_t ecall_event_establish_valid;  /**< Must be set to true if ecall_event_establish is being passed */
  quec_voice_ecall_event_establish_t_v01 ecall_event_establish;

  /* Optional */
  /*  event of eCall establish */
  uint8_t ecall_event_hackcode_valid;  /**< Must be set to true if ecall_event_hackcode is being passed */
  quec_voice_ecall_event_hackcode_t_v01 ecall_event_hackcode;

  /* Optional */
  /*  event of eCall hack code */
  uint8_t ecall_event_ori_redial_valid;  /**< Must be set to true if ecall_event_ori_redial is being passed */
  quec_voice_ecall_event_originate_fail_and_redial_t_v01 ecall_event_ori_redial;
  /**<   event of originate fail and redial.*/

  /* Optional */
  uint8_t ecall_event_drop_redial_valid;  /**< Must be set to true if ecall_event_drop_redial is being passed */
  quec_voice_ecall_event_drop_and_redial_t_v01 ecall_event_drop_redial;
  /**<   event of drop and redial.*/
}quec_common_qmi_ecall_event_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Optional */
  uint8_t ecall_urc_valid;  /**< Must be set to true if ecall_urc is being passed */
  uint8_t ecall_urc;

  /* Optional */
  /*  Enable or disable ecall_urc  */
  uint8_t ecall_event_valid;  /**< Must be set to true if ecall_event is being passed */
  uint8_t ecall_event;
}quec_common_qmi_event_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  quec_response_t_v01 resp;
}quec_common_qmi_event_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  char ap_version[128];
}quec_common_qmi_sync_ap_version_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t result_flag;
}quec_common_qmi_sync_ap_version_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t profile_id;
}quec_common_qmi_get_pdp_profile_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t profile_id;

  /* Mandatory */
  int32_t result;

  /* Mandatory */
  char apn_val[256];
}quec_common_qmi_get_pdp_profile_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t profile_id;

  /* Mandatory */
  uint8_t pdp_type;

  /* Mandatory */
  char apn_val[256];
}quec_common_qmi_set_pdp_profile_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t profile_id;

  /* Mandatory */
  int32_t result;
}quec_common_qmi_set_pdp_profile_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  char singed_data[1024];
}quec_common_qmi_alipay_iot_signed_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t result;
}quec_common_qmi_alipay_iot_signed_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t modem_rdy;
}quec_common_qmi_get_modem_rdy_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t modem_rdy;
}quec_common_qmi_get_modem_rdy_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t modem_rdy;
}quec_common_qmi_get_modem_rdy_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t usbmode_value;
}quec_common_qmi_get_usbmode_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t usbmode_value;
}quec_common_qmi_get_usbmode_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t usbmode_value;
}quec_common_qmi_get_usbmode_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  char common_val[1024];
}quec_common_qmi_common_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t result;

  /* Mandatory */
  char common_val[1024];
}quec_common_qmi_common_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  char common_val[1024];
}quec_common_qmi_common_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t ap_wakeupvalue;
}quec_common_qmi_sync_ap_wakeup_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t result_flag;
}quec_common_qmi_sync_ap_wakeup_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t ats12_value;
}quec_common_qmi_get_ats12_value_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t ats12_value;
}quec_common_qmi_get_ats12_value_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t ats12_value;
}quec_common_qmi_get_ats12_value_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}quec_common_qmi_read_nv_bt_addr_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t mac[6];
}quec_common_qmi_read_nv_bt_addr_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t mac[6];
}quec_common_qmi_write_nv_bt_addr_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  int32_t result_flag;
}quec_common_qmi_write_nv_bt_addr_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t dm_connect_state;
}quec_common_qmi_get_dm_connect_state_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup quec_common_qmi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t dm_connect_state;
}quec_common_qmi_get_dm_connect_state_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 

/*Service Message Definition*/
/** @addtogroup quec_common_qmi_qmi_msg_ids
    @{
  */
#define QMI_COMM_QMI_GET_AUDIO_MOD_REQ_V01 0x0001
#define QMI_COMM_QMI_GET_AUDIO_MOD_RESP_V01 0x0001
#define QMI_COMM_QMI_GET_AUDIO_MOD_IND_V01 0x0001
#define QMI_COMM_QMI_GET_GPIO_STATUS_REQ_V01 0x0002
#define QMI_COMM_QMI_GET_GPIO_STATUS_RESP_V01 0x0002
#define QMI_COMM_QMI_GET_GPIO_STATUS_IND_V01 0x0002
#define QMI_COMM_QMI_GET_SCLK_VALUE_REQ_V01 0x0003
#define QMI_COMM_QMI_GET_SCLK_VALUE_RESP_V01 0x0003
#define QMI_COMM_QMI_GET_SCLK_VALUE_IND_V01 0x0003
#define QMI_COMM_QMI_GET_MOB_MODEL_ID_REQ_V01 0x0004
#define QMI_COMM_QMI_GET_MOB_MODEL_ID_RESP_V01 0x0004
#define QMI_COMM_QMI_SBFOTA_UPDATE_REQ_V01 0x0005
#define QMI_COMM_QMI_SBFOTA_UPDATE_RESP_V01 0x0005
#define QMI_COMM_QMI_GET_3GPP_PDN_STATE_REQ_V01 0x0006
#define QMI_COMM_QMI_GET_3GPP_PDN_STATE_RESP_V01 0x0006
#define QMI_COMM_QMI_DEACT_3GPP_PDN_REQ_V01 0x0007
#define QMI_COMM_QMI_DEACT_3GPP_PDN_RESP_V01 0x0007
#define QMI_COMM_QMI_SBFOTA_GET_APN_REQ_V01 0x0008
#define QMI_COMM_QMI_SBFOTA_GET_APN_RESP_V01 0x0008
#define QMI_COMM_QMI_SBFOTA_SET_APN_REQ_V01 0x0009
#define QMI_COMM_QMI_SBFOTA_SET_APN_RESP_V01 0x0009
#define QMI_COMM_QMI_GET_FW_VER_REQ_V01 0x000A
#define QMI_COMM_QMI_GET_FW_VER_RESP_V01 0x000A
#define QMI_COMM_QMI_CALL_EVENT_IND_V01 0x000B
#define QMI_COMM_QMI_TONE_EVENT_IND_V01 0x000C
#define QMI_COMM_QMI_WIFICFG_REQ_V01 0x000D
#define QMI_COMM_QMI_WIFICFG_RESP_V01 0x000D
#define QMI_COMM_QMI_WIFICFG_IND_V01 0x000D
#define QMI_COMM_QMI_GET_THERMAL_LIMIT_RATES_FLAG_REQ_V01 0x000E
#define QMI_COMM_QMI_GET_THERMAL_LIMIT_RATES_FLAG_RESP_V01 0x000E
#define QMI_COMM_QMI_GET_THERMAL_LIMIT_RATES_FLAG_IND_V01 0x000E
#define QMI_COMM_QMI_EFS_FILE_OPEN_REQ_V01 0x000F
#define QMI_COMM_QMI_EFS_FILE_OPEN_RESP_V01 0x000F
#define QMI_COMM_QMI_EFS_FILE_SIZE_REQ_V01 0x0010
#define QMI_COMM_QMI_EFS_FILE_SIZE_RESP_V01 0x0010
#define QMI_COMM_QMI_EFS_FILE_READ_REQ_V01 0x0011
#define QMI_COMM_QMI_EFS_FILE_READ_RESP_V01 0x0011
#define QMI_COMM_QMI_EFS_FILE_WRITE_REQ_V01 0x0012
#define QMI_COMM_QMI_EFS_FILE_WRITE_RESP_V01 0x0012
#define QMI_COMM_QMI_EFS_FILE_CLOSE_REQ_V01 0x0013
#define QMI_COMM_QMI_EFS_FILE_CLOSE_RESP_V01 0x0013
#define QMI_COMM_QMI_EFS_FILE_DELETE_REQ_V01 0x0014
#define QMI_COMM_QMI_EFS_FILE_DELETE_RESP_V01 0x0014
#define QMI_COMM_QMI_EFS_FILE_SEEK_REQ_V01 0x0015
#define QMI_COMM_QMI_EFS_FILE_SEEK_RESP_V01 0x0015
#define QUEC_KTFOTA_SMS_IND_V01 0x0016
#define QUEC_KTFOTA_DLOAD_STATUS_REQ_V01 0x0017
#define QUEC_KTFOTA_DLOAD_STATUS_RESP_V01 0x0017
#define QUEC_KTFOTA_GET_MODEL_ID_REQ_V01 0x0018
#define QUEC_KTFOTA_GET_MODEL_ID_RESP_V01 0x0018
#define QUEC_KTFOTA_GET_SW_VER_REQ_V01 0x0019
#define QUEC_KTFOTA_GET_SW_VER_RESP_V01 0x0019
#define QUEC_KTFOTA_GET_HW_VER_REQ_V01 0x001A
#define QUEC_KTFOTA_GET_HW_VER_RESP_V01 0x001A
#define QUEC_KTFOTA_GET_FW_VER_REQ_V01 0x001B
#define QUEC_KTFOTA_GET_FW_VER_RESP_V01 0x001B
#define QUEC_KTFOTA_GET_PDN_STATE_REQ_V01 0x001C
#define QUEC_KTFOTA_GET_PDN_STATE_RESP_V01 0x001C
#define QUEC_KTFOTA_DEACTIVE_PDN_REQ_V01 0x001D
#define QUEC_KTFOTA_DEACTIVE_PDN_RESP_V01 0x001D
#define QUEC_KTFOTA_DEACTIVE_PDN_IND_V01 0x001D
#define QUEC_KTFOTA_ACTIVE_PDN_REQ_V01 0x001E
#define QUEC_KTFOTA_ACTIVE_PDN_RESP_V01 0x001E
#define QUEC_KTFOTA_ACTIVE_PDN_IND_V01 0x001E
#define QUEC_KTFOTA_GET_DEV_QUALITY_REQ_V01 0x001F
#define QUEC_KTFOTA_GET_DEV_QUALITY_RESP_V01 0x001F
#define QUEC_KTFOTA_PS_ATTACH_REJ_IND_V01 0x0020
#define QMI_COMM_QMI_GET_SOFTSIM_VALUE_REQ_V01 0x0021
#define QMI_COMM_QMI_GET_SOFTSIM_VALUE_RESP_V01 0x0021
#define QMI_COMM_QMI_GET_SOFTSIM_VALUE_IND_V01 0x0021
#define QMI_COMM_QMI_QUEC_SLEEP_IND_FLAG_REQ_V01 0x0022
#define QMI_COMM_QMI_QUEC_SLEEP_IND_FLAG_RESP_V01 0x0022
#define QMI_COMM_QMI_QUEC_SLEEP_IND_FLAG_IND_V01 0x0022
#define QMI_COMM_QMI_REG_TIMER_REQ_V01 0x0023
#define QMI_COMM_QMI_REG_TIMER_RSP_V01 0x0023
#define QMI_COMM_QMI_START_TIMER_REQ_V01 0x0024
#define QMI_COMM_QMI_START_TIMER_RSP_V01 0x0024
#define QMI_COMM_QMI_STOP_TIMER_REQ_V01 0x0025
#define QMI_COMM_QMI_STOP_TIMER_RSP_V01 0x0025
#define QMI_COMM_QMI_TIMER_EXP_IND_V01 0x0026
#define QMI_COMM_QMI_FORBID_IND_REQ_V01 0x0027
#define QMI_COMM_QMI_FORBID_IND_RESP_V01 0x0027
#define QUEC_COMMON_QMI_ECALL_HANGUP_REQ_V01 0x0028
#define QUEC_COMMON_QMI_ECALL_HANGUP_RESP_V01 0x0028
#define QUEC_COMMON_QMI_SET_ECALL_CONFIG_REQ_V01 0x0029
#define QUEC_COMMON_QMI_SET_ECALL_CONFIG_RESP_V01 0x0029
#define QUEC_COMMON_QMI_GET_ECALL_CONFIG_REQ_V01 0x002A
#define QUEC_COMMON_QMI_GET_ECALL_CONFIG_RESP_V01 0x002A
#define QUEC_COMMON_QMI_ECALL_COMMAND_PUSH_REQ_V01 0x002B
#define QUEC_COMMON_QMI_ECALL_COMMAND_PUSH_RESP_V01 0x002B
#define QUEC_COMMON_QMI_ECALL_URC_IND_V01 0x002C
#define QUEC_COMMON_QMI_ECALL_EVENT_IND_V01 0x002D
#define QUEC_COMMON_QMI_EVENT_REGISTER_REQ_V01 0x002E
#define QUEC_COMMON_QMI_EVENT_REGISTER_RESP_V01 0x002E
#define QMI_COMM_QMI_SYNC_AP_VERSION_REQ_V01 0x002F
#define QMI_COMM_QMI_SYNC_AP_VERSION_RESP_V01 0x002F
#define QUEC_COMM_QMI_GET_PDP_PROFILE_REQ_V01 0x0030
#define QUEC_COMM_QMI_GET_PDP_PROFILE_RESP_V01 0x0030
#define QUEC_COMM_QMI_SET_PDP_PROFILE_REQ_V01 0x0031
#define QUEC_COMM_QMI_SET_PDP_PROFILE_RESP_V01 0x0031
#define QMI_COMM_QMI_ALIPAY_SIGNED_REQ_V01 0x0032
#define QMI_COMM_QMI_ALIPAY_SIGNED_RESP_V01 0x0032
#define QMI_COMM_QMI_GET_MODEM_RDY_REQ_V01 0x0033
#define QMI_COMM_QMI_GET_MODEM_RDY_RESP_V01 0x0033
#define QMI_COMM_QMI_GET_MODEM_RDY_IND_V01 0x0033
#define QMI_COMM_QMI_GET_USBMODE_REQ_V01 0x0034
#define QMI_COMM_QMI_GET_USBMODE_RESP_V01 0x0034
#define QMI_COMM_QMI_GET_USBMODE_IND_V01 0x0034
#define QMI_COMM_QMI_COMMON_REQ_V01 0x0035
#define QMI_COMM_QMI_COMMON_RESP_V01 0x0035
#define QMI_COMM_QMI_COMMON_IND_V01 0x0035
#define QMI_COMM_QMI_SYNC_AP_WAKEUP_REQ_V01 0x0036
#define QMI_COMM_QMI_SYNC_AP_WAKEUP_RESP_V01 0x0036
#define QMI_COMM_QMI_GET_ATS12_VALUE_REQ_V01 0x0037
#define QMI_COMM_QMI_GET_ATS12_VALUE_RESP_V01 0x0037
#define QMI_COMM_QMI_GET_ATS12_VALUE_IND_V01 0x0037
#define QMI_COMM_QMI_READ_NV_BT_ADDR_REQ_V01 0x0038
#define QMI_COMM_QMI_READ_NV_BT_ADDR_RESP_V01 0x0038
#define QMI_COMM_QMI_WRITE_NV_BT_ADDR_REQ_V01 0x0039
#define QMI_COMM_QMI_WRITE_NV_BT_ADDR_RESP_V01 0x0039
#define QMI_COMM_QMI_GET_DM_CONN_STATE_REQ_V01 0x003A
#define QMI_COMM_QMI_GET_DM_CONN_STATE_RESP_V01 0x003A
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro quec_common_qmi_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type quec_common_qmi_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define quec_common_qmi_get_service_object_v01( ) \
          quec_common_qmi_get_service_object_internal_v01( \
            QUEC_COMMON_QMI_V01_IDL_MAJOR_VERS, QUEC_COMMON_QMI_V01_IDL_MINOR_VERS, \
            QUEC_COMMON_QMI_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

