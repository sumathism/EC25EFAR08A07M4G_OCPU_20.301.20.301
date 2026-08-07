#ifndef _QL_SOFTSIM_QMI_H__
#define _QL_SOFTSIM_QMI_H__


#ifdef __cplusplus
extern "C" {
#endif

#define QL_SOFTSIM_MAX_COMMAND_APDU_LEN 261

typedef enum
{
    QL_SOFTSIM_APDU_IND = 0,
	QL_SOFTSIM_CONNECT_IND,
	QL_SOFTSIM_DISCONECT_IND,
	QL_SOFTSIM_POWER_UP_IND,   
	QL_SOFTSIM_POWER_DOWN_IND,
	QL_SOFTSIM_CARD_RESET_IND
}ql_softsim_event_e;

typedef enum
{
    QL_SOFTSIM_NO_APPLICABLE = 0,
	QL_SOFTSIM_SLOT_1,
	QL_SOFTSIM_SLOT_2,
	QL_SOFTSIM_SLOT_3
}ql_softsim_slot_e;

typedef struct
{
    ql_softsim_slot_e slot;
}ql_softsim_connect_t;

typedef struct
{
    ql_softsim_slot_e slot;
}ql_softsim_disconnect_t;

typedef struct {
  ql_softsim_slot_e slot;
}uim_remote_card_reset_t;

typedef enum
{
  QL_SOFTSIM_VOLTAGE_CLASS_C_LOW = 0x0, /**<  VOLTAGE_CLASS_C_LOW \n  */
  QL_SOFTSIM_VOLTAGE_CLASS_C = 0x1, /**<  VOLTAGE_CLASS_C \n  */
  QL_SOFTSIM_VOLTAGE_CLASS_C_HIGH = 0x2, /**<  VOLTAGE_CLASS_C_HIGH  \n  */
  QL_SOFTSIM_VOLTAGE_CLASS_B_LOW = 0x3, /**<  VOLTAGE_CLASS_B_LOW \n  */
  QL_SOFTSIM_VOLTAGE_CLASS_B = 0x4, /**<  VOLTAGE_CLASS_B \n  */
  QL_SOFTSIM_VOLTAGE_CLASS_B_HIGH = 0x5, /**<  VOLTAGE_CLASS_B_HIGH  */
}ql_uim_remote_voltage_class_e;

typedef enum {
  QL_SOFTSIM_POWER_DOWN_TELECOM_INTERFACE = 0x0, /**<  Power down telecom only \n  */
  QL_SOFTSIM_POWER_DOWN_CARD = 0x1, /**<  Power down card \n  */
}ql_uim_remote_power_down_mode_enum;

typedef struct {
  ql_softsim_slot_e slot;
  unsigned char mode_valid;
  ql_uim_remote_power_down_mode_enum mode;
}ql_softsim_card_power_down_t;

typedef struct
{
    ql_softsim_slot_e slot;
	unsigned char response_timeout_valid;
	unsigned int response_timeout;
	unsigned char voltage_class_valid;
	ql_uim_remote_voltage_class_e voltage_class;
}ql_softsim_card_powerup_t;


typedef struct {
  ql_softsim_slot_e slot;
  unsigned int apdu_id;
  unsigned int command_apdu_len;
  unsigned char command_apdu[QL_SOFTSIM_MAX_COMMAND_APDU_LEN];
}ql_softsim_apdu_msg_t;



/**
 * init qmi client call-back function 
 */
typedef void (*ql_qmi_ind_cb)
(
  unsigned int  msg_id,
  void          *ind_buf,
  unsigned int  ind_len
);

/**
 * apdu rsp function.
 *
 * @param [in] RSP rsp data
 * @param [in] len rep data len
 *
 * @return 
 *   On success, TRUE(1) is returned, otherwise FALSE(0) is returned.
 */
int quectel_apdu_rsp_handler(char * RSP, int len);

/**
 * atr reset function.
 *
 * @param [in] ATR  atr data
 * @param [in] len atr len
 *
 * @return 
 *   On success, TRUE(1) is returned, otherwise FALSE(0) is returned.
 */
int quectel_atr_reset_handler(char * ATR, int len);

/**
 * sotfsim connect event report function.
 *
 * @param [NULL] 
 * @param [NULL] 
 *
 * @return 
 *   On success, TRUE(1) is returned, otherwise FALSE(0) is returned.
 */
int quectel_softsim_connect_handler(void);

/**
 * init uim qmi client init api.
 *
 * @param[in]ind_data_cb some ind message call-back 
 *
 * @return 
 *    On success, 0 is returned, otherwise not zero is failed.
 */
int quectel_softsim_client_init(void * ind_data_cb);

/**
 * release uim qmi client api.
 *
 * @param
 *   None
 *
 * @return 
 *    0 success 
 */
int quectel_softsim_client_release(void);

#ifdef __cplusplus
}
#endif

#endif
