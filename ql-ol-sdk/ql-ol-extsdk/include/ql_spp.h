/*-----------------------------------------------------------------------------------------------*/
/**
  @file ql_spp.h
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
  20191206   Rami.zhang   Created .
-------------------------------------------------------------------------------------------------*/
#include "ql_ble_types.h"
#include "ql_bt.h"  //<2023/03/30>add Harry Obtaining or setting BT MAC is supported when bt is disenabled

//<2020/03/05>add colin Callback function interface start
typedef void (*reback_func)(QuecBtPrim type, char *data, int len);

/*******************************************************************************
 * the primitive define about indications, user should be process 
 * these messages in the client_cb callback function.
 *******************************************************************************/
#define QUEC_BT_INDICATION_BASE                     ((QuecBtUint32) (0x2000))
#define QUEC_BT_GATT_CONNECT_IND                    ((QuecBtUint32) (0x0001 + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_GATT_DISCONNECT_IND                 ((QuecBtUint32) (0x0002 + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_GATT_DB_READ_ACCESS_IND             ((QuecBtUint32) (0x0003 + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_GATT_DB_WRITE_ACCESS_IND            ((QuecBtUint32) (0x0004 + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_SPP_READ_IND                        ((QuecBtUint32) (0x0005 + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_SPP_CONNECT_IND                     ((QuecBtUint32) (0x0006 + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_SPP_DISCONNECT_IND                  ((QuecBtUint32) (0x0007 + QUEC_BT_INDICATION_BASE))
#define QUEC_BT_INDICATION_MAX                      ((QuecBtUint32) (0x00FF + QUEC_BT_INDICATION_BASE))

/************************************
             SPP
*************************************/
typedef struct
{
    QuecBtUint32 lap;   /*!< Lower Address Part 00..23 */
    QuecBtUint8  uap;   /*!< upper Address Part 24..31 */
    QuecBtUint16 nap;   /*!< Non-significant    32..47 */
}QuecDeviceAddr;

typedef struct
{
    QuecBtPrim                type;                   /* Identity */
    QuecBtUint16              valueLength;            /* Length of the Value in octects */
    QuecBtUint8               value[0];              /* The current value of the attribute */
}BtSppData;

typedef struct
{
    QuecBtPrim                type;                   /* Identity */
    QuecBtUint16              queueId;
    QuecBtUint16              channel;
    QuecDeviceAddr             address; 
}BtSppConnect;


typedef struct
{
    QuecBtPrim                type;                   /* Identity */
    QuecBtUint16              queueId;
    QuecBtUint16              channel;
    QuecDeviceAddr            address; 
}BtSppDisconnect;

//<2020/03/05>add colin Callback function interface end

/*******************************************************************************
 * ble db operate error code
 *******************************************************************************/

/*******************************************************************************
* @brief help function,start bt service, ##do not use##.
  @param:
        NULL 
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_spp_power_on();

/*******************************************************************************
* @brief help function,close bt service, ##do not use##.
  @param:
        NULL 
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_spp_power_off();

/*******************************************************************************
* @brief help function,bt client init, ##do not use##.
  @param:
        NULL 
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_spp_client_init();

/*******************************************************************************
* @brief help function,bt client deinit, ##do not use##.
  @param:
        NULL 
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_spp_client_deinit();

/*******************************************************************************
* @brief help function,activate spp function, ##do not use##.
  @param:
        NULL 
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_activate_spp();


/*******************************************************************************
* @brief help function,instance spp function, ##do not use##.
  @param:
        NULL 
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_instance_spp();


/*******************************************************************************
* @brief help function,register spp function, ##do not use##.
  @param:
        NULL 
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_register_spp();


/*******************************************************************************
* @brief help function,connect remote spp device, ##do not use##.
  @param:
        *addr       remote SPP device MAC info
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_connect_spp(QuecDeviceAddr * addr);


/*******************************************************************************
* @brief help function,disconnect spp , ##do not use##.
  @param:
        NULL 
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_disconnect_spp();


/*******************************************************************************
* @brief help function,deactive spp , ##do not use##.
  @param:
        NULL 
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_deactivate_spp();


/*******************************************************************************
* @brief help function,write spp data to remote spp device, ##do not use##.
  @param:
        valueLength     the length of info value
        *value           the value of info
  @return 
        0           success
        -1          generic error
        -2          timeout (communicate with ble service)
        other       see ble db operate error code which define in ql_ble.h
 *******************************************************************************/
int ql_write_spp(QuecBtUint16 valueLength, QuecBtUint8 *value);



/*FUN:read local BT name
@param
ptname   where name is going to be saved
len     the buffer size
@return
	0 is returned when things go well
	none-zero: error
*/

int ql_spp_read_local_name(char*ptname, int len);
/*FUN:set local BT name
@param
ptnmstr  name string terminated by 0

@return
0 returned when things goes well
*/

int ql_spp_set_local_name(char *ptnmstr);

/*Fun:read BT address
@param
ptaddr   where local address is saved
@return
0 is returned when things goes well
*/
int ql_spp_read_local_address(QuecBtDeviceAddr *ptaddr);


/*FUN:delete paring information in Security Control DateBase
TRUE is returned when information is deleted successfully or no information of the given device in the Database at all
*/
QuecBtBool ql_spp_sc_db_del(QuecBtDeviceAddr devaddr);

