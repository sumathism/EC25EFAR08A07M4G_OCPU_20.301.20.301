#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ql_oe.h"
#include "ql_ble.h"
#include "ql_ble_uuid.h"
#include "ql_ble_types.h"


typedef int (*TEST)(void);

typedef struct
{
    int  cmdIdx;
    char *funcName;
} hfg_api_test_case;

typedef struct
{
    char                 *group_name;
    hfg_api_test_case    *test_cases;
    TEST                  pf_test;
} func_api_hfg_test_t;

void show_group_hfg_help(func_api_hfg_test_t *pt_test);
int hfgtest_bt_func(void);

#define GROUP_MAX 10

func_api_hfg_test_t hfg_test_list[GROUP_MAX] = {0};

void add_hfg_test_node(func_api_hfg_test_t *pt_test)
{
    int i;
    for(i=0; i<GROUP_MAX; i++)
    {
        if(hfg_test_list[i].group_name == NULL)
        {
            hfg_test_list[i].group_name   = pt_test->group_name;
            hfg_test_list[i].test_cases   = pt_test->test_cases;
            hfg_test_list[i].pf_test      = pt_test->pf_test;
            break;
        }
    }
}


void show_group_hfg_help(func_api_hfg_test_t *pt_test)
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



static void show_hfgtest_list()
{
    int i;

    printf("Test groups:\n");
    for(i = 0; i<GROUP_MAX; i++)
    {
        if(hfg_test_list[i].group_name != NULL)
        {
            printf("%d:\t%s\n", i, hfg_test_list[i].group_name);
        }
    }
}



hfg_api_test_case bt_hfg_testlist[] = 
{
    {0,     "power on"},
    {1,     "power off"},
    {2,     "client init"},
    {3,     "open scan"},
    {4,     "close scan"},
    {5,     "register av"},
    {6,     "activate av"},
    {7,     "deactivate av"},
    {8,     "connect av"},
    {9,     "disconnect av"},
    {10,    "cancel connect av"},
    {11,    "connect ag"},
    {12,    "disconnect ag"},
    {13,    "create_sco"},
    {14,    "destroy_sco"},
    {15,    "avrcp configure"},
    {16,    "avrcp connect"},
    {17,    "avrcp cancelconnect"},
    {-1,    NULL}
};


func_api_hfg_test_t hfg_bt_test = { "hfg_test", bt_hfg_testlist, hfgtest_bt_func};


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

//<2020/03/05>add colin Callback function interface
//typedef void (*reback_func)(char *buf);
int client_cb_func(QuecBtPrim type, char *data, int len)
{
    printf("recv type=0x%X\n", type);
    switch(type)
    {
        case QUEC_BT_SD_SEARCH_IND:
        {
            BtSdScanAddress* ind = (BtSdScanAddress*)data;
            printf("recv addr: \"%s\",\"%04x:%02x:%06x\"\r\n",ind->devicename,ind->address.nap, ind->address.uap, ind->address.lap);
        }
        break;
        case QUEC_BT_AV_CONNECT_IND:
        {
            BtAvConnectInd* ind = (BtAvConnectInd*)data;
            printf("av connect from: \"%d\",\"%04x:%02x:%06x\"\r\n",ind->connectId,ind->address.nap, ind->address.uap, ind->address.lap);
        }
        break;
        case QUEC_BT_AV_DISCONNECT_IND:
        {
            BtAvDisconnectInd* ind = (BtAvDisconnectInd*)data;
            printf("av disconnect from: \"%d\"\r\n",ind->connectId);
        }
        break;
        case QUEC_BT_HFG_CONNECT_IND:
        {
            BtHfgConnectInd* ind = (BtHfgConnectInd*)data;
            printf("hfg connect from: \"%d\",\"%04x:%02x:%06x\"\r\n",ind->connId,ind->address.nap, ind->address.uap, ind->address.lap);
        }
        break;
        case QUEC_BT_HFG_DISCONNECT_IND:
        {
            BtHfgDisconnectInd* ind = (BtHfgDisconnectInd*)data;
            printf("hfg disconnect from: \"%d\",\"%04x:%02x:%06x\"\r\n",ind->connId,ind->address.nap, ind->address.uap, ind->address.lap);
        }
        break;
        case QUEC_BT_HFG_ANSWER_IND:
        {
            BtHfgAnswer* ind = (BtHfgAnswer*)data;
            printf("hfg answer from: \"%d\"\r\n",ind->connid);
        }
        break;
        case QUEC_BT_HFG_REJECT_IND:
        {
            BtHfgReject* ind = (BtHfgReject*)data;
            printf("hfg reject from: \"%d\"\r\n",ind->connid);
        }
        break;
        case QUEC_BT_AVRCP_CONNECT_IND:
        {
            BtAvrcpConnectInd *ind = (BtAvrcpConnectInd*)data;
            printf("avrcp connect from: \"%d\",\"%04x:%02x:%06x\"\r\n",ind->connectId,ind->address.nap, ind->address.uap, ind->address.lap);
        }
        break;
        case QUEC_BT_AVRCP_DISCONNECT_IND:
        {
            BtAvrcpDisconnectInd* ind = (BtAvrcpDisconnectInd*)data;
            printf("avrcp disconnect from: \"%d\"\r\n",ind->connectId);
        }
        break;
        case QUEC_BT_AVRCP_PASS_THROUGH_IND:
        {
            BtAvrcpPassThroughInd* ind = (BtAvrcpPassThroughInd*)data;
            printf("avrcp pass through type: \"%d\"\r\n", ind->CmdType);
        }
        break;

    }
    return 0;
}



int hfgtest_bt_func(void)
{  
    int     cmdIdx  = 0;
    int     ret;
    char    hfgaddr[32] = {0};
    QuecBtUint32  connid = 0;
    QuecBtDeviceAddr hfgdeviceaddr;
    char nap[10] = {0};
    char uap[10] = {0};
    char lap[10] = {0};

    show_group_hfg_help(&hfg_bt_test);

    while(1)
    {
        printf("please input cmd index(-1 exit): ");
        scanf("%d", &cmdIdx);
        if(cmdIdx == -1)
        {
            break;
        }
        switch(cmdIdx)
        {
            case 0:
            {
                ret = ql_hfg_power_on();
                printf("ql_hfg_power_on, ret=%d\n", ret);
            }
            break;
            case 1:
            {
                ret = ql_ble_power_off();
                printf("ql_hfg_power_off, ret=%d\n", ret);
            }
            break;
            case 2:
            {//<2020/03/05>modify colin Callback function interface
                ret = ql_ble_client_init(client_cb_func);
                printf("ql_hfg_client_init, ret=%d\n", ret);
            }
            break;
            case 3:
            {
                ret = ql_open_scan_device();
                printf("ret=%d\n", ret);
            }
            break;
            case 4:
            {
                ret = ql_close_scan_device();
                printf("ret=%d\n", ret);
            }
            break;
            case 5:
            {
                ret = ql_av_register();
                printf("ret=%d\n", ret);
            }
            break;
            case 6:
            {
                ret = ql_av_activate();
                printf("ret=%d\n", ret);
                
            }
            break;
            case 7:
            {
                ret = ql_av_deactivate();
                printf("ret=%d\n", ret);
                
            }
            break;
            case 8:
            {
                printf("please input deviceAddr example:2000:00:00893c\n");
                scanf("%s",hfgaddr);
			
                getchar();
				printf("%s\n",hfgaddr);
	
                memcpy(nap,hfgaddr,4);
                memcpy(uap,hfgaddr+5,2);
                memcpy(lap,hfgaddr+8,6);
                //sscanf(hfgaddr,"%s:%s:%s",nap,uap,lap);
                printf("nap:%s,uap:%s,lap:%s\n",nap,uap,lap);

                memset(&hfgdeviceaddr,0,sizeof(hfgdeviceaddr));

                hfgdeviceaddr.nap = strtoul(nap, NULL, 16);
                hfgdeviceaddr.uap = strtoul(uap, NULL, 16);
                hfgdeviceaddr.lap = strtoul(lap, NULL, 16);

                ret = ql_av_connect(hfgdeviceaddr,(QuecBtUint8*)&connid);

                printf("connid = %d,ret=%d\n", connid,ret);
            }
            break;
            case 9:
            {
                printf("please input connid\n");
                scanf("%u",&connid);
                ret = ql_av_disconnect(connid);
                printf("ret=%d\n", ret);
            }
            break;
            case 10:
            {
                printf("please input deviceAddr example:2000:00:00893c\n");
                scanf("%s",hfgaddr);
                
                getchar();
				printf("%s\n",hfgaddr);

               // sscanf(hfgaddr,"%s:%s:%s",nap,uap,lap);

                memcpy(nap,hfgaddr,4);
                memcpy(uap,hfgaddr+5,2);
                memcpy(lap,hfgaddr+8,6);

                printf("nap:%s,uap:%s,lap:%s\n",nap,uap,lap);
                memset(&hfgdeviceaddr,0,sizeof(hfgdeviceaddr));

                hfgdeviceaddr.nap = strtoul(nap, NULL, 16);
                hfgdeviceaddr.uap = strtoul(uap, NULL, 16);
                hfgdeviceaddr.lap = strtoul(lap, NULL, 16);
                ret = ql_av_cancelconnect(hfgdeviceaddr);
                
                printf("ret=%d\n", ret);
            }
            break;
            case 11:
            {
                printf("please input deviceAddr example:2000:00:00893c\n");
                scanf("%s",hfgaddr);
            
                getchar();
				printf("%s\n",hfgaddr);

                memcpy(nap,hfgaddr,4);
                memcpy(uap,hfgaddr+5,2);
                memcpy(lap,hfgaddr+8,6);
               // sscanf(hfgaddr,"%s:%s:%s",nap,uap,lap);
                printf("nap:%s,uap:%s,lap:%s\n",nap,uap,lap);
				
                memset(&hfgdeviceaddr,0,sizeof(hfgdeviceaddr));
                hfgdeviceaddr.nap = strtoul(nap, NULL, 16);
                hfgdeviceaddr.uap = strtoul(uap, NULL, 16);
                hfgdeviceaddr.lap = strtoul(lap, NULL, 16);
				
                ret = ql_hfg_connect(hfgdeviceaddr);
                printf("ret=%d\n", ret);
                
            }
            break;
            case 12:
            {
                printf("please input deviceAddr example:2000:00:00893c\n");
                scanf("%s",hfgaddr);
            
                getchar();
				printf("%s\n",hfgaddr);

                memcpy(nap,hfgaddr,4);
                memcpy(uap,hfgaddr+5,2);
                memcpy(lap,hfgaddr+8,6);
               // sscanf(hfgaddr,"%s:%s:%s",nap,uap,lap);
                printf("nap:%s,uap:%s,lap:%s\n",nap,uap,lap);
				
                memset(&hfgdeviceaddr,0,sizeof(hfgdeviceaddr));

                hfgdeviceaddr.nap = strtoul(nap, NULL, 16);
                hfgdeviceaddr.uap = strtoul(uap, NULL, 16);
                hfgdeviceaddr.lap = strtoul(lap, NULL, 16);
				
                ret = ql_hfg_cancelconnect(hfgdeviceaddr);
                printf("ret=%d\n", ret);
                
            }
            break;
            case 13:
            {
                printf("please input connect_ID\n");
                scanf("%d",&connid);
                getchar();
                printf("connect_id:%d\n",connid);
                ret = ql_hfg_create_sco(connid);
                printf("ret=%d\n", ret);
                
            }
            break;
            case 14:
            {
                printf("please input connect_ID\n");
                scanf("%d",&connid);
                getchar();
                printf("connect_id:%d\n",connid);
                ret = ql_hfg_destroy_sco(connid);
                printf("ret=%d\n", ret);
                
            }
            break;
            case 15:
            {
                printf("avrcp configure\n");
                ret = ql_avrcp_config();
                printf("ret=%d\r\n", ret);
            }
            break;
            case 16:
            {
                printf("please input deviceAddr example:2000:00:00893c\n");
                scanf("%s",hfgaddr);
                
                getchar();
				printf("%s\n",hfgaddr);

                memcpy(nap,hfgaddr,4);
                memcpy(uap,hfgaddr+5,2);
                memcpy(lap,hfgaddr+8,6);
               // sscanf(hfgaddr,"%s:%s:%s",nap,uap,lap);
                printf("nap:%s,uap:%s,lap:%s\n",nap,uap,lap);
				
                memset(&hfgdeviceaddr,0,sizeof(hfgdeviceaddr));

                hfgdeviceaddr.nap = strtoul(nap, NULL, 16);
                hfgdeviceaddr.uap = strtoul(uap, NULL, 16);
                hfgdeviceaddr.lap = strtoul(lap, NULL, 16);

                ret = ql_avrcp_connect(hfgdeviceaddr,(QuecBtUint8*)&connid);
                printf("ret=%d\r\n", ret);
                
            }
            break;
            case 17:
            {
                printf("please input connect_ID\n");
                scanf("%d",&connid);
                getchar();
                printf("connect_id:%d\n",connid);
                ret = ql_avrcp_cancel_connect(connid);
                printf("ret=%d\n", ret);
            }break;
            default:
            break;
        }
    }
    return 0;
}



int main(int argc, const char** argv)
{

    int cmdIdx = 0;
    add_hfg_test_node(&hfg_bt_test);
    show_hfgtest_list();
    while(1)
    {
        printf("please input command index(-1 exit): ");
        scanf("%d", &cmdIdx);
        if(cmdIdx == -1)
        {
            break;
        }
        if((cmdIdx >= GROUP_MAX) || (hfg_test_list[cmdIdx].group_name == NULL))
        {
            show_hfgtest_list();
        }
        else
        {
            hfg_test_list[cmdIdx].pf_test();
        }
    }
    return 0;
}

