/*
 *  Quectel rawdata flag manage 
 *
 *  file name: ql_rawdata_flag_manage.h
 *
 *  Design by: colin.cui
 *
 *  Date: 2020.11.19
 */

#ifndef __QUECTEL_RAWDATA_FLAG_MANAGE_H__
#define __QUECTEL_RAWDATA_FLAG_MANAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define quec_pr(...) (fprintf(stderr, "func: %s, line: %d, ", __FUNCTION__, __LINE__), fprintf(stderr, " " __VA_ARGS__))

//if add rawdata flag,you can add this
//it min value 1 optimize usb enum fail
//it max value 32
typedef enum
{
    QUECTEL_RAW_MIN = 0,
    QUECTEL_USB_ENUM_OPTIMIZATION = 1,
    QUECTEL_QCFG_QCAUTOCONNECT = 2,
    QUECTEL_QUICK_SHUTDOWN,
    QUECTEL_CLK_OUT_CFG,  //config clk_out pin 
    QUECTEL_SWITCH_PIN,
    QUECTEL_ALTERNATIVE_SLEEP,
    QUECTEL_SDIO_CLK_CHOSEN,/*_Harry 20230328 AT+QCFG="SDIO_CLK" command to set the sdio clk. Change the throughput of wifi.*/
	QUECTEL_LED_STATUS_CFG,
    QUECTEL_LED_NETSTATUS_CFG,
    QUECTEL_LED_NETMODE_CFG,
    QUECTEL_USB_SPEED,/* Zeke 20250424 Add use usb full speed mode */
    QUECTEL_XTWIFI_CFG, // lucus.lin 20250924 Add xtwifi control to solve the timer period wake up module for the "Loc" thread during sleep.
    QUECTEL_RAW_MAX,
} quec_rawdata_flag_type;

//add data use struct
#define QUEC_FLAG_DATA_LEN 2

typedef struct 
{
    char quec_flag_type;
    char use_flag;
}quec_rawdata_data_type;


//quectel function
//quectel get flag from rawdata
boolean quectel_get_flag_to_rawdata(quec_rawdata_data_type * data_buf,quec_rawdata_flag_type flag_data);

//quectel add flag to rawdata
boolean quectel_add_flag_to_rawdata(quec_rawdata_data_type * data_buf,quec_rawdata_flag_type flag_data);

//quectel modify flag from rawdata
boolean quectel_modify_flag_from_rawdata(quec_rawdata_data_type * data_buf,quec_rawdata_flag_type flag_data);

//init quectel rawdata flag
boolean quectel_rawdata_flag_init();
#endif
