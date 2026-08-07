#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ql_oe.h"
#include "ql_ble.h"
#include "ql_ble_uuid.h"
#include "ql_ble_types.h"


typedef int (*TEST)(void);

BtGattDBWriteAccessInd write_rsp_ind;
uint8_t writeResponse = FALSE;
uint8_t connected = FALSE;

typedef struct
{
    int  cmdIdx;
    char *funcName;
} st_api_test_case;

typedef struct
{
    char                *group_name;
    st_api_test_case    *test_cases;
    TEST                pf_test;
} func_api_test_t;

void show_group_help(func_api_test_t *pt_test);
int test_bt(void);

#define FUNC_GROUP_MAX 10

typedef struct
{
    char                *group_name;
    st_api_test_case    *test_cases;
    TEST                pf_test;
} func_api_test_group_t;


func_api_test_group_t g_test_list[FUNC_GROUP_MAX] = {0};

void add_test_node(func_api_test_t *pt_test)
{
    int i;
    for(i=0; i<FUNC_GROUP_MAX; i++)
    {
        if(g_test_list[i].group_name == NULL)
        {
            g_test_list[i].group_name   = pt_test->group_name;
            g_test_list[i].test_cases   = pt_test->test_cases;
            g_test_list[i].pf_test      = pt_test->pf_test;
            break;
        }
    }
}


void show_group_help(func_api_test_t *pt_test)
{
    int i;

    printf("Group Name:%s, Supported test cases:\n", pt_test->group_name);
    for(i = 0; ; i++)
    {
        if(pt_test->test_cases[i].cmdIdx == -1)
        {
            break;
        }
        printf("%d:\t%s\n", pt_test->test_cases[i].cmdIdx, pt_test->test_cases[i].funcName);
    }
}



void show_group_list()
{
    int i;

    printf("Test groups:\n");
    for(i = 0; i<FUNC_GROUP_MAX; i++)
    {
        if(g_test_list[i].group_name != NULL)
        {
            printf("%d:\t%s\n", i, g_test_list[i].group_name);
        }
    }
}


st_api_test_case at_bt_testlist[] = 
{
    {0,     "power on"},
    {1,     "power off"},
    {2,     "client init"},
    {3,     "set local name"},
    {4,     "register gatt"},
    {5,     "unregister gatt"},
    {6,     "set adverting param"},
    {7,     "start adverting"},
    {8,     "stop adverting"},
    {9,     "db alloc"},
    {10,     "create service(add service/charact/descriptor)"},
    {11,     "db add"},
    {12,    "start peripheral role"},
    {13,    "send indication"},
    {14,    "send notification"},
    {99,    "client deinit"},
    {-1,    NULL}
};

func_api_test_t t_bt_test = {"ble_test", at_bt_testlist, test_bt};

const char leRole_str[][32] = {"undefined","master","slave"};

void demo_trace_dump_hex(char *prompt, unsigned char *buf, int len)
{
    int i;

    if (len > 0)
    {
        printf("%s:\n", prompt);
        for (i = 0; i < len; i++)
        {
            printf("%02X ", buf[i]);
            if ((i + 1) % 16 == 0)
            {
                printf("\n");
            }
        }
        if (i % 16 != 0)
        {
            printf("\n");
        }
    }
}


int gatt_db_write_access_dump(BtGattDBWriteAccessInd *ind)
{
    int len = 0, pos = 0, i;
    BtGattAttrWritePairs *unit = NULL;
    if(ind == NULL)
    {
        printf("ind == NULL\n");
        return -1;
    }
    printf("check: %d, attrHandle: 0x%02X, connInfo: 0x%04X, address: %04x:%02x:%06x\n", 
            ind->check, ind->attrHandle, ind->connInfo, ind->address.addr.nap, ind->address.addr.uap, ind->address.addr.lap);
    len += sizeof(BtGattDBWriteAccessInd);
    unit = ind->writeUnit;
    if(ind->writeUnitCount > 0)
    {
        for(i = 0; i < ind->writeUnitCount; i++)
        {
            unit = ind->writeUnit + i;
            pos += sizeof(BtGattAttrWritePairs);
            printf("attrHandle: 0x%02X, offset: %d, valueLength: %d\n", unit->attrHandle, unit->offset, unit->valueLength);
            if(unit->valueLength > 0)
            {
                demo_trace_dump_hex("data:", unit->value, unit->valueLength);
                pos += unit->valueLength;
            }

        }
    }
    
    return 0;
}



#if(1)
    struct btdevinfo{
    BtSdScanAddress kpbtaddr; // Keep Server address
    unsigned char flg;
    //
    BtGattConnectInd kpconind; // kp connectio infor
    unsigned char constate; // none-zero connection is established
    };
    struct btdevinfo g_bt_devinfo = {0};
    //#define BT_SERVER_NAME "TDWB01"
    #define BT_SERVER_NAME  "V1996"
    #define IDX_TDWB_SERVICE   0
    //#
    #define IDX_TDWB_SERVICE_T   2
    #define IDX_TDWB_SERVICE_FEA1 7
    #define NUM_TDWBSERVICE 20U
    unsigned short g_tdwbtservice[NUM_TDWBSERVICE][2] = {
        { 0x180F, 0x2A19}, //battery
        { 0xFF00, 0xFF01}, // ??
        { 0x180F, 0x2902}, //???
        { 0xFEE7, 0xFEC7}, //
        { 0xFEE7, 0xFEC8},
        { 0xFEE7, 0x2902},
        { 0xFEE7, 0xFEC9},
        { 0xFEE7, 0xFEA1},
        { 0xFEE7, 0x2902},
        { 0xFEE7, 0xFEA2},
        { 0xFEE7, 0x2902}
    };
#endif
int client_cb(QuecBtPrim type, char *data, int len)
{
    int ret;
    printf("recv type=0x%X\n", type);
    switch(type)
    {
        case QUEC_BT_SD_SEARCH_IND:
        {
            BtSdScanAddress *ptdev = (BtSdScanAddress*)data;
            printf("Info========QUEC_BT_SD_SEARCH_IND==len %d, sizeof(x) %d\n", len, sizeof(BtSdScanAddress));
            printf("BT dev %s is found; type = %d;lap 0x%x, uap 0x%x, nap 0x%x!\n", ptdev->devicename, ptdev->type,ptdev->address.lap, ptdev->address.uap, ptdev->address.nap );
            if(strcmp(BT_SERVER_NAME, ptdev->devicename)==0)
            {
                memcpy( &g_bt_devinfo.kpbtaddr,  ptdev, sizeof(BtSdScanAddress));
                g_bt_devinfo.flg = 1;
            }
            break;
        }
        case QUEC_BT_GATT_CONNECT_IND:
        {
            BtGattConnectInd *ind = (BtGattConnectInd*) data;
            if(sizeof(BtGattConnectInd) != len)
            {
                printf("BtGattConnectInd length error.\n");
                break;
            }

            printf("connect from %04x:%02x:%06x, role: %s, mtu: %d; resultcode 0x%x\n", 
                ind->address.addr.nap, ind->address.addr.uap, ind->address.addr.lap, 
                leRole_str[ind->leRole], ind->mtu, ind->resultCode);
            #if(1) // for test
                printf("Info==%s===change the type of BLE dev\n", __FUNCTION__);
                ind->address.type = QUEC_TBDADDR_RANDOM;
            #endif
            memcpy(  &g_bt_devinfo.kpconind, ind, sizeof(BtGattConnectInd));    
            g_bt_devinfo.constate = 0xAA;
	        connected=TRUE;
        }
        break;
        case QUEC_BT_GATT_DISCONNECT_IND:
        {
            QuecBtTypedDeviceAddr t_addr;
            BtGattDisconnectInd *ind = (BtGattDisconnectInd*) data;
            if(sizeof(BtGattDisconnectInd) != len)
            {
                printf("BtGattDisconnectInd length error.\n");
                break;
            }
            printf("disconnect from %04x:%02x:%06x\n", 
                ind->address.addr.nap, ind->address.addr.uap, ind->address.addr.lap);

		connected = FALSE;

        }
        break;
        case QUEC_BT_GATT_DB_READ_ACCESS_IND:
        {
            BtGattDBReadAccessInd *ind = (BtGattDBReadAccessInd*)data;
            printf("recv QUEC_BT_GATT_DB_READ_ACCESS_IND: attrHandle: %02X\n", ind->attrHandle);
            if(ind->attrHandle == 0xE)
            {
                QuecBtUint8 value[4] = {0};
                srand(time(NULL));
                snprintf(value, sizeof(value) , "Q%d",rand() % 10);
                ret = ql_ble_gatt_read_response(ind->gattId, ind->connId, ind->attrHandle, 0, 4, value);
                if(ret)
                {
                    printf("read access response faild.[attrHandle=%d]\n", ind->attrHandle);
                }
                printf("attrHandle: 0x%02X, value: %s\n", ind->attrHandle, value);
            }
            else if(ind->attrHandle == 0x11)
            {
                QuecBtUint8 value;
                srand(time(NULL));
                value = rand() % 255;
                ret = ql_ble_gatt_read_response(ind->gattId, ind->connId, ind->attrHandle, 0, 1, &value);
                if(ret)
                {
                    printf("read access response faild.[attrHandle=%d]\n", ind->attrHandle);
                }
                printf("attrHandle: 0x%02X, value: %d\n", ind->attrHandle, value);
            }
        }
        break;
        case QUEC_BT_GATT_DB_WRITE_ACCESS_IND:
        {
            int i = 0;
            BtGattDBWriteAccessInd *ind = (BtGattDBWriteAccessInd*)data;
            //demo_trace_dump_hex("write", data, len);
            //printf("recv QUEC_BT_GATT_DB_WRITE_ACCESS_IND: attrHandle: %02X\n", ind->attrHandle);
            //gatt_db_write_access_dump(ind);
            write_rsp_ind.gattId = ind->gattId;
            write_rsp_ind.connId = ind->connId;
            write_rsp_ind.attrHandle = ind->attrHandle;

	    writeResponse = TRUE ;
		printf("DBG================writeResponse is set to %d\n", writeResponse);
        }
        break;
    }
    return 0;
}
uint8 uuid_server_128[16] = {0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e};
uint8 uuid_write[16] = {0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e};
uint8 uuid_read[16] = {0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x02, 0x00, 0x40, 0x6e};
uint8 uuid_descriptor_128[16] = {0xfb, 0x34, 0x9b, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x02, 0x29, 0x00, 0x00};

int create_service128(QuecBtGattId gattId)
{
    /*for test: create db*/
    int ret;
    QuecBtUint8 name[128] = "Quectel";
    ret = ql_ble_db_service_add_128bit(gattId, 1, uuid_server_128, 1);
    if (ret)
    {
        printf("create service error.\n");
        return -1;
    }
    ret = ql_ble_db_charact_add_128bit(gattId, 1, 1, uuid_write,
                                128, QUEC_ATT_PERM_READ | QUEC_ATT_PERM_NOTIFY, QUEC_BT_GATT_ATTR_FLAGS_IRQ_READ, name);
    if (ret)
    {
        printf("create charact error.\n");
        return -1;
    }
    
    ret = ql_ble_db_charact_add_128bit(gattId, 1, 2, uuid_read,
                                128, QUEC_ATT_PERM_WRITE_CMD | QUEC_ATT_PERM_WRITE_REQ, QUEC_BT_GATT_ATTR_FLAGS_IRQ_WRITE, name);
    if (ret)
    {
        printf("create charact error.\n");
        return -1;
    }
    
    // QuecBtUint16 value = 0x02;
    // ret = ql_ble_db_descriptor_add(gattId, 1, 1, 1, 0x2902, 2, QUEC_ATT_PERM_READ | QUEC_ATT_PERM_WRITE_CMD | QUEC_ATT_PERM_NOTIFY, 
                                    // QUEC_BT_GATT_ATTR_FLAGS_NONE, (QuecBtUint8 *)&value);
                                    
     QuecBtUint16 value = 0x01;
     ret = ql_ble_db_descriptor_add_128bit(gattId, 1, 1, 1, uuid_descriptor_128, 2, QUEC_ATT_PERM_WRITE_REQ | QUEC_ATT_PERM_WRITE_CMD | QUEC_ATT_PERM_READ | QUEC_ATT_PERM_NOTIFY, 
                                     QUEC_BT_GATT_ATTR_FLAGS_IRQ_READ | QUEC_BT_GATT_ATTR_FLAGS_IRQ_WRITE, (QuecBtUint8 *)&value);
     if (ret)
     {
       printf("create descriptor error.\n");
       return -1;
     }

    ret = ql_ble_db_service_set_active(gattId, 1, 1);
    if (ret)
    {
        printf("set service active error.\n");
        return -1;
    }
    return 0;
}

int create_service(QuecBtGattId gattId)
{
    /*for test: create db*/
    int ret;
    QuecBtUint8 name[128] = "Quectel";
    ret = ql_ble_db_service_add(gattId, 1, QUEC_BT_GATT_UUID_DEVICE_INFORMATION_SERVICE, 1);
    if (ret)
    {
        printf("create service error.\n");
        return -1;
    }
    ret = ql_ble_db_charact_add(gattId, 1, 1, QUEC_BT_GATT_UUID_MANUFACTURER_NAME_STRING_CHARAC,
                                128, QUEC_ATT_PERM_READ | QUEC_ATT_PERM_INDICATE, QUEC_BT_GATT_ATTR_FLAGS_IRQ_READ, name);
    if (ret)
    {
        printf("create charact error.\n");
        return -1;
    }
    QuecBtUint16 value = 0x01;
    ret = ql_ble_db_descriptor_add(gattId, 1, 1, 1, 0x2902, 2, QUEC_ATT_PERM_READ, 
                                    QUEC_BT_GATT_ATTR_FLAGS_NONE, (QuecBtUint8 *)&value);
    if (ret)
    {
        printf("create descriptor error.\n");
        return -1;
    }

    value = 0x20;

    ret = ql_ble_db_charact_add(gattId,
                                1, 
                                2, 
                                0x2A19, 
                                2, 
                                QUEC_ATT_PERM_READ |QUEC_ATT_PERM_WRITE_REQ,
                                QUEC_BT_GATT_ATTR_FLAGS_IRQ_WRITE,
                                &value
                                );
    if(ret)
    {
        printf("ql_ble_db_charact_add failed, ret=%d\n", ret);
        return -1;
    }

    ret = ql_ble_db_service_set_active(gattId, 1, 1);
    if (ret)
    {
        printf("set service active error.\n");
        return -1;
    }
    return 0;
}


#define	MSG_KEY	1234
#define MSG_BLE_CONNECTED   0xA5
#define MSG_BLE_TYPE    0xAA
#define MSG_SIZE    1U

#define IDX_PIPE_READ 0
#define IDX_PIPE_WRITE 1


void *ql_thread_ble_write_response(void*arg)
{
    int ret = 0;
    while(1)
    {
        if(writeResponse == TRUE)
        {
           writeResponse = FALSE;
		    printf("ql_ble_gatt_write_response enter...\n");
			ret = ql_ble_gatt_write_response(write_rsp_ind.gattId, write_rsp_ind.connId,write_rsp_ind.attrHandle, 0);
			printf("ql_ble_gatt_write_response ret=%d\n", ret);
        }
        usleep(100);
    }
}

struct str_conninfo{
    QuecBtGattId gattid;
    QuecBtConnId connid;
};

void *ql_thread_ble_send_identification(void*arg)
{
    int ret = 0;
    int counter = 0;
    char str[100] = {0};
    struct str_conninfo *ptconinfo = (struct str_conninfo*)arg;
    if(ptconinfo == NULL)
    {
        printf("Error: thread ble send identification , para is invalid\n");
        return;
    }
    while(1)
    {
         if(connected == TRUE)
		{
			counter ++;
			snprintf(str, 100, "Hello,Zach! idx=%d",counter);
			//send data every 100ms
			printf("DBG=======Going to call send indication\n");
			int ret = ql_ble_gatt_send_indication(ptconinfo->gattid, ptconinfo->connid,0x0E, strlen(str), str);
			memset( str, 0, sizeof(str));
			printf("idx = %d ql_ble_gatt_send_indication ret = %d\n", counter, ret);
			usleep(100*100);//100ms
		}   
        //sleep(100*100);
        sleep(1);
    }
}

int test_bt(void)
{  
  int     cmdIdx  = 0;
  int     ret;
	QuecBtUint16 numOfAttrHandles = 10, preferredStartHandle = 1;
	  QuecBtGattId gattId;
	unsigned char str[] = "Hello Zach";	
	unsigned int counter = 0, idx = 0;	
	int hldfd[2]  = {0};
    pthread_t hldthread_writeresp = 0, hldthread_sendind = 0;

  //show_group_help(&t_bt_test);

	ret = ql_ble_power_off();
  printf("ql_ble_power_off, ret=%d\n", ret);

	ret = ql_ble_power_on();
  printf("ql_ble_power_on, ret=%d\n", ret);

	usleep(2000*1000);

	ret = ql_ble_client_init(client_cb);
  printf("ql_ble_client_init, ret=%d\n", ret);

	usleep(1000*100);


  //ret = ql_ble_gatt_register(&gattId);
  ret = ql_ble_gatt_client_init(&gattId);
  if(ret)
  {
      printf("ql_ble_gatt_register ret=%d\n", ret);
  }
  else
  {
      printf("ql_ble_gatt_register ret=%d, gattId=0x%x\n", ret, gattId);
  }
    #if(0)
         
    #else
    ret = ql_open_scan_device();
    printf("DBG=======ql_open_scan_device ret %d\n");
    
        while(!g_bt_devinfo.flg)
        {
            sleep(5);
            printf("DBG==trying to find %s\n", BT_SERVER_NAME);
        }
        ret = ql_close_scan_device();
        printf("Info====ql_close_scan dev is %d\n", ret);


        while(1)
        {
            ret = ql_ble_client_connect( g_bt_devinfo.kpbtaddr, gattId);
            printf("DBG===ret val of ql_ble_client_connect is %d\n",ret);
            if(!ret)
            {
                break;
            }
            sleep(5);
        }
        while(!g_bt_devinfo.constate)
        {
            printf("Info=waiting for Connection to be done");
            sleep(5);
        }
    #endif
    printf("Info==================gattId 0x%X, connId 0x%X\n", gattId, g_bt_devinfo.kpconind.connId);    

   


	#if(1)
        pthread_create( &hldthread_writeresp, NULL, ql_thread_ble_write_response, NULL);
        #if(0)
            struct str_conninfo hldconninfo;
            hldconninfo.connid = connId;
            hldconninfo.gattid = gattId;
            pthread_create( &hldthread_sendind, NULL, ql_thread_ble_send_identification,(void*)&hldconninfo);
        #endif
        unsigned short kpuuid =  0;   
        BtBleClientFindServiceInd tpserviceind = {0};
        
        while(1)
        {
            //ql_ble_client_find_service(QuecBtUint16 puuid, QuecBtGattId gattid, QuecBtConnId connid, BtBleClientFindServiceInd *serviceind)
            ret = ql_ble_client_find_service( kpuuid, g_bt_devinfo.kpconind.gattId, g_bt_devinfo.kpconind.connId, &tpserviceind);
            if(ret == 0)
            {
                printf("Info=type %x, puuid %x, starthandle %x, endhandle %x\n", tpserviceind.type, tpserviceind.puuid, tpserviceind.startHandle, tpserviceind.endHandle);
                break;
            }
            else
            {
                printf("Err=ql_ble_client_find_service faile!\n");
            }
            sleep(5);
        }


        sleep(5); //
        //int ql_ble_client_configure_attr(QuecBtGattId gattid, QuecBtConnId connid, QuecBtUint16 puuid, QuecBtUint16 cuuid, QuecBtUint16 config);
        ret = ql_ble_client_configure_attr(g_bt_devinfo.kpconind.gattId, g_bt_devinfo.kpconind.connId, 0x180D,
                                    0x2A37, QUEC_BT_GATT_CLIENT_CHARAC_CONFIG_INDICATION);
        printf("INFO===ret of ql_ble_client_configure_attr() is %d\n", ret);

        BtBleClientReadServiceCfm tpreadcfm = {0};
        int i = 0;
        while(1)
        {
            //ret = ql_ble_client_read_attr( gattId, g_bt_devinfo.kpconind.connId, 0x180F, 0x2A19, &tpreadcfm);
            //int ql_ble_client_read_attr( QuecBtGattId gattid, QuecBtConnId connid,QuecBtUint16 puuid, QuecBtUint16 cuuid, BtBleClientReadServiceCfm *ptreadbuf)
            ret = ql_ble_client_read_attr( g_bt_devinfo.kpconind.gattId, g_bt_devinfo.kpconind.connId, 0x181C, 0x2A8A, &tpreadcfm);
            printf("Info====ql_ble_client_read_addr ret is %d, len %d\n", ret, tpreadcfm.valueLength);
            if(ret == 0)
            {
                demo_trace_dump_hex( "IDX_TDWB_SERVICE_2222:", tpreadcfm.value, tpreadcfm.valueLength);   
            }
           
            //IDX_TDWB_SERVICE_T
            ret = ql_ble_client_read_attr(g_bt_devinfo.kpconind.gattId, g_bt_devinfo.kpconind.connId,0x181C, 0x2A90, &tpreadcfm);
            if(ret == 0)
            {
                demo_trace_dump_hex("IDX_TDWB_SERVICE_AAAA:", tpreadcfm.value, tpreadcfm.valueLength);
            } 

            // { 0xFEE7, 0xFEC7}, //
            ret = ql_ble_client_read_attr(g_bt_devinfo.kpconind.gattId, g_bt_devinfo.kpconind.connId, 0x181C, 0x2A8C, &tpreadcfm);
            if(ret == 0)
            {
                demo_trace_dump_hex("SERFICE 0xFEA2", tpreadcfm.value, tpreadcfm.valueLength);
            }

            #if(0)
                ret = ql_ble_client_unregister( g_bt_devinfo.kpconind.gattId, g_bt_devinfo.kpconind.connId);
                if(ret==0)
                {
                    printf("INFO===client unregister success\n");
                }
                else
                {
                    printf("INFO====Client unregister failed!\n");
                }
                break;
            #endif
			sleep(3);
        }

        printf("APP====BLE test is done\n"); 
        while(1)
        {
            sleep(5);
        }

	#else
		while(1)
		{
			if(writeResponse == TRUE)
			{
				writeResponse = FALSE;
				printf("ql_ble_gatt_write_response enter...\n");
				ret = ql_ble_gatt_write_response(write_rsp_ind.gattId, write_rsp_ind.connId,write_rsp_ind.attrHandle, 0);
				printf("ql_ble_gatt_write_response ret=%d\n", ret);
			}
			if(connected == TRUE)
			{
				counter ++;
				snprintf(str, 100, "Hello,Zach! idx=%d",counter);
				//send data every 100ms
				printf("DBG=======Going to call send indication\n");
				int ret = ql_ble_gatt_send_indication(gattId, connId,0x0E, strlen(str), str);
				memset( str, 0, sizeof(str));
				printf("idx = %d ql_ble_gatt_send_indication ret = %d\n", counter, ret);
				usleep(100*100);//100ms
			}
			usleep(100*1000);
		}
	#endif
    // while(1)
    // {
        // printf("please input cmd index(-1 exit): ");
        // scanf("%d", &cmdIdx);
        // if(cmdIdx == -1)
        // {
            // break;
        // }
        // switch(cmdIdx)
        // {
            // case 0:
            // {
                // ret = ql_ble_power_on();
                // printf("ql_ble_power_on, ret=%d\n", ret);
            // }
            // break;
            // case 1:
            // {
                // ret = ql_ble_power_off();
                // printf("ql_ble_power_off, ret=%d\n", ret);
            // }
            // break;
            // case 2:
            // {
                // ret = ql_ble_client_init(client_cb);
                // printf("ql_ble_client_init, ret=%d\n", ret);
            // }
            // break;
            // case 3:
            // {
                // char device_name[128];
                // printf("please input device name(length less than 128 bytes): ");
                // scanf("%s", device_name);
                // ret = ql_ble_set_local_name(device_name);
                // printf("ret=%d\n", ret);
            // }
            // break;
            // case 4:
            // {
                // QuecBtGattId gattId;
                // ret = ql_ble_gatt_register(&gattId);
                // if(ret)
                // {
                    // printf("ret=%d\n", ret);
                // }
                // else
                // {
                    // printf("ret=%d, gattId=%u\n", ret, gattId);
                // }
            // }
            // break;
            // case 5:
            // {
                // QuecBtGattId gattId;
                // printf("please input gattId: ");
                // scanf("%u", &gattId);
                // ret = ql_ble_gatt_unregister(gattId);
                // printf("ret=%d\n", ret);
            // }
            // break;
            // case 6:
            // {
                // QuecBtGattId gattId;
                // QuecBtUint16 min, max;
                // printf("please input gattId: ");
                // scanf("%u", &gattId);
                // printf("please input advertising interval(min)(32~16384): ");
                // scanf("%d", &min);
                // printf("please input advertising interval(max)(32~16384): ");
                // scanf("%d", &max);
                // ret = ql_ble_gatt_set_adverting_param(gattId, min, max);
                // printf("ret=%d\n", ret);
            // }
            // break;
            // case 7:
            // {
                // QuecBtGattId gattId;
                // printf("please input gattId: ");
                // scanf("%u", &gattId);
                // ret = ql_ble_adverting_start(gattId);
                // printf("ret=%d\n", ret);
            // }
            // break;
            // case 8:
            // {
                // QuecBtGattId gattId;
                // printf("please input gattId: ");
                // scanf("%u", &gattId);
                // ret = ql_ble_adverting_stop(gattId);
                // printf("ret=%d\n", ret);
            // }
            // break;
            // case 9:
            // {
                // QuecBtUint16 numOfAttrHandles, preferredStartHandle;
                // QuecBtGattId gattId;
                // printf("please input gattId: ");
                // scanf("%u", &gattId);
                // printf("please input numbers of attribute handle we want alloced: ");
                // scanf("%d", &numOfAttrHandles);
                // printf("please input prederred start handle when alloced: ");
                // scanf("%d", &preferredStartHandle);
                // ret = ql_ble_gatt_db_alloc(gattId, numOfAttrHandles, preferredStartHandle);
                // printf("ret=%d\n", ret);
            // }
            // break;
            // case 10:
            // {
                // QuecBtGattId gattId;
                // printf("please input gattId: ");
                // scanf("%u", &gattId);
                // //ret = create_service(gattId);
                // //printf("ret=%d\n", ret);
                // ret = create_service128(gattId);
                // printf("ret=%d\n", ret);
            // }
            // break;
            // case 11:
            // {
                // QuecBtGattId gattId;
                // printf("please input gattId: ");
                // scanf("%u", &gattId);
                // ret = ql_ble_gatt_db_add(gattId);
                // printf("ret=%d\n", ret);
            // }
            // break;
            // case 12:
            // {
                // QuecBtTypedDeviceAddr t_addr;
                // QuecBtGattId gattId;
                // QuecBtConnId connId;
                // t_addr.type = QUEC_TBDADDR_PUBLIC;
                // t_addr.addr.lap = 0;
                // t_addr.addr.uap = 0;
                // t_addr.addr.nap = 0;
                // printf("please input gattId: ");
                // scanf("%u", &gattId);
                // ret = ql_ble_gatt_peripheral(gattId, t_addr, QUEC_BT_GATT_FLAGS_UNDIRECTED, 0, &connId);
                // if(ret)
                // {
                    // printf("ret=%d\n", ret);
                // }
                // else
                // {
                    // printf("connId=%u,ret=%d\n", connId, ret);
                // }
            // }
            // break;
            // case 13:
            // {
                // QuecBtUint8 value[128];
                // QuecBtGattId gattId;
                // QuecBtConnId connId;
                // printf("please input gattId: ");
                // scanf("%u", &gattId);
                // printf("please input connId: ");
                // scanf("%u", &connId);
                // printf("please input indication value(string, 128 bytes): ");
                // scanf("%s", value);
                // ret = ql_ble_gatt_send_indication(gattId, connId, 0x0E, strlen(value), value);
                // printf("ret=%d\n", ret);
            // }
            // break;
            // case 14:
            // {
                // QuecBtUint8 value[128];
                // QuecBtGattId gattId;
                // QuecBtConnId connId;
                // printf("please input gattId: ");
                // scanf("%u", &gattId);
                // printf("please input connId: ");
                // scanf("%u", &connId);
                // printf("please input notification value(string, 128 bytes): ");
                // scanf("%s", value);
                // ret = ql_ble_gatt_send_notification(gattId, connId,0x0E, strlen(value), value);
                // printf("ret=%d\n", ret);
            // }
            // break;
            // case 15:
            // {
                // ql_ble_gatt_write_response(write_rsp_ind.gattId, write_rsp_ind.connId,write_rsp_ind.attrHandle, 0);
            // }
            // break;
            // case 99:
            // {
                // ret = ql_ble_client_deinit();
                // printf("ql_ble_client_deinit, ret=%d\n", ret);
            // }
            // break;
            // default:
            // break;
        // }
    // }
    return 0;
}



int main(int argc, const char** argv)
{

    int cmdIdx = 0;
    add_test_node(&t_bt_test);
    show_group_list();
    while(1)
    {
        printf("please input command index(-1 exit): ");
        scanf("%d", &cmdIdx);
        if(cmdIdx == -1)
        {
            break;
        }
        if((cmdIdx >= FUNC_GROUP_MAX) || (g_test_list[cmdIdx].group_name == NULL))
        {
            show_group_list();
        }
        else
        {
            g_test_list[cmdIdx].pf_test();
            /*show_group_list();*/
        }
    }
    return 0;
}
