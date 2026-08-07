#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ql_oe.h"
#include "ql_spp.h"
#include "ql_ble_uuid.h"
#include "ql_ble_types.h"


typedef int (*TEST)(void);

typedef struct
{
    int  cmdIdx;
    char *funcName;
} spp_api_test_case;

typedef struct
{
    char                 *group_name;
    spp_api_test_case    *test_cases;
    TEST                  pf_test;
} func_api_spp_test_t;

void show_group_spp_help(func_api_spp_test_t *pt_test);
int test_bt_func(void);

#define GROUP_MAX 10

func_api_spp_test_t spp_test_list[GROUP_MAX] = {0};

void add_spp_test_node(func_api_spp_test_t *pt_test)
{
    int i;
    for(i=0; i<GROUP_MAX; i++)
    {
        if(spp_test_list[i].group_name == NULL)
        {
            spp_test_list[i].group_name   = pt_test->group_name;
            spp_test_list[i].test_cases   = pt_test->test_cases;
            spp_test_list[i].pf_test      = pt_test->pf_test;
            break;
        }
    }
}


void show_group_spp_help(func_api_spp_test_t *pt_test)
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



void show_test_list()
{
    int i;

    printf("Test groups:\n");
    for(i = 0; i<GROUP_MAX; i++)
    {
        if(spp_test_list[i].group_name != NULL)
        {
            printf("%d:\t%s\n", i, spp_test_list[i].group_name);
        }
    }
}


spp_api_test_case bt_spp_testlist[] = 
{
    {0,     "power on"},
    {1,     "power off"},
    {2,     "client init"},
    {3,     "request instances"},
    {4,     "register spp"},
    {5,     "activate spp"},
    {6,     "disconnect spp"},
    {7,     "deactivate spp"},
    {8,     "connect spp"},
    {9,     "send spp data"},
    {10,    "client deinit"},
    {11,   "set name to BT666(default:quectel_ble_test; set before active)"},
    {12,   "read BT addr"},
    {13,   "read BT name"},
    {14,   "delete pairing info"},
    {-1,    NULL}
};

func_api_spp_test_t spp_bt_test = {"spp_test", bt_spp_testlist, test_bt_func};

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
//void printf_cb(char *buf)
//{
//    printf("%s\n",buf);
//}

//<2020/03/05>add colin Callback function interface start
int client_cb(QuecBtPrim type, char *data, int len)
{
	printf("recv type=0x%X\n", type);
	switch(type)
	{
    	case QUEC_BT_SPP_READ_IND:
		{
			BtSppData* ind = (BtSppData*)data;
			int i =0 ;
			printf("read data: \"");
			for(i=0;i < ind->valueLength; i++){
				printf("%c", ind->value[i]);
			}
			printf("\",%d \n", ind->valueLength);
		}
		break;
    	case QUEC_BT_SPP_CONNECT_IND:
		{
			BtSppConnect* ind = (BtSppConnect*)data;

			printf("connect from: %d,%d,\"%04x:%02x:%06x\" \n",ind->queueId,ind->channel,
					ind->address.nap, ind->address.uap, ind->address.lap);
		}
		break;
    	case QUEC_BT_SPP_DISCONNECT_IND:
		{
			BtSppDisconnect* ind = (BtSppDisconnect*)data;

			printf("disconnect from: %d,%d,\"%04x:%02x:%06x\" \n",ind->queueId,ind->channel,
					ind->address.nap, ind->address.uap, ind->address.lap);
		}
		break;
	}
	return 0;
}
//<2020/03/05>add colin Callback function interface end

int test_bt_func(void)
{  
	int     cmdIdx  = 0;
	int     ret;

    show_group_spp_help(&spp_bt_test);

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
                ret = ql_spp_power_on();
                printf("ql_spp_power_on, ret=%d\n", ret);
            }
            break;
            case 1:
            {
                ret = ql_spp_power_off();
                printf("ql_spp_power_off, ret=%d\n", ret);
            }
            break;
            case 2:
            {//<2020/03/05>modify colin Callback function interface
                ret = ql_spp_client_init(client_cb);
                printf("ql_spp_client_init, ret=%d\n", ret);
            }
            break;
            case 3:
            {
                ret = ql_instance_spp();
                printf("ret=%d\n", ret);
            }
            break;
            case 4:
            {
                ret = ql_register_spp();
                printf("ret=%d\n", ret);
            }
            break;
            case 5:
            {
                ret = ql_activate_spp();
                printf("ret=%d\n", ret);
            }
            break;
            case 6:
            {
                ret = ql_disconnect_spp();
                printf("ret=%d\n", ret);
                
            }
            break;
            case 7:
            {
                ret = ql_deactivate_spp();
                printf("ret=%d\n", ret);
                
            }
            break;
            case 8:
            {
                unsigned int input[3];
                QuecDeviceAddr addr;
                printf("please input remote spp device addr format as aabb:cc:eeffgg\r\n:");
                scanf("%04x:%02x:%06x", &input[0], &input[1], &input[2]);
                addr.lap = input[2];
                addr.nap = input[0];
                addr.uap = input[1];
                printf("will connect %04x:%02x:%06x\r\n", addr.nap, addr.uap, addr.lap);
                ret = ql_connect_spp(&addr);
                printf("ret=%d\r\n", ret);
            }
            break;
            case 9:
            {
                unsigned char data[256];
                memset(data, 0, sizeof(data));
                printf("please input send data\r\n:");
                scanf("%s", data);
                printf("will send:%s\r\n", data);
                ret = ql_write_spp((QuecBtUint16)strlen(data), data );
                printf("ret=%d\r\n",ret);
            }
            break;
			case 10:
			{
				ret = ql_spp_client_deinit();
				printf("ql_spp_client_deinit, ret=%d\n", ret);
				break;
			}
			case 11:
			{/*set before SPP is actived, otherwise it will not take effect*/
				ret = ql_spp_set_local_name("BT666");
				printf("ql_spp_set_local_name, set to BT666, ret = %d\n", ret);
				break;
			}
			case 12:
			{
				QuecBtDeviceAddr kpaddr = {0};
				ret = ql_spp_read_local_address( &kpaddr);
				if(ret == 0)
				{
					printf("local addr:%06x:%02x:%04x\n", kpaddr.lap, kpaddr.uap, kpaddr.nap);
				}
				else
				{
					printf("ql_spp_read_local_address fail, ret %d\n", ret);
				}
				break;
			}
			case 13:
			{
				char buf[256] = {0};
				ret = ql_spp_read_local_name( buf, sizeof(buf));
				if(ret == 0)
				{
					printf("BT name read is:%s\n", buf);
				}
				else
				{
					printf("ql_spp_read_local_name fail, ret=%d\n", ret);
				}
				break;
			}
			case 14:
			{
				QuecBtBool ret = 0;
				QuecBtDeviceAddr kpaddr = {0};
				unsigned int kplap = 0, kpuap = 0, kpnap = 0;
				printf("Info===input the dev addr(xxxx-xx-xxxxxx) to delete!\n");
				scanf("%04X-%02X-%06X",&kpnap,&kpuap,&kplap);
				kpaddr.lap = kplap;
				kpaddr.uap = kpuap;
				kpaddr.nap = kpnap;
				printf("Info=dev addr input is:%04X-%02X-%06X\n",kpaddr.nap, kpaddr.uap, kpaddr.lap);
				ret = ql_spp_sc_db_del(kpaddr);
				printf("ql_spp_sc_db_del return:%s\n", ret?"TRUE":"FALSE");
				break;
			}
            default:
            break;
        }
    }
    return 0;
}



int main(int argc, const char** argv)
{

    int cmdIdx = 0;
    add_spp_test_node(&spp_bt_test);
    show_test_list();
    while(1)
    {
        printf("please input command index(-1 exit): ");
        scanf("%d", &cmdIdx);
        if(cmdIdx == -1)
        {
            break;
        }
        if((cmdIdx >= GROUP_MAX) || (spp_test_list[cmdIdx].group_name == NULL))
        {
            show_test_list();
        }
        else
        {
            spp_test_list[cmdIdx].pf_test();
        }
    }
    return 0;
}

