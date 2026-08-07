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
} st_api_test_case;

typedef struct
{
    char                *group_name;
    st_api_test_case    *test_cases;
    TEST                pf_test;
} func_api_test_t;

typedef struct
{
    QuecBtPrim           type;
    char                 *data;
    int                  len;
    int                  flag;
    int                  thread_running;
    pthread_t            pth;
} indication_handle_t;

static indication_handle_t *indication_handle_info = NULL;

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
    {15,	"set security controller"},
	{16,    "get local address"},
	{17,	"set datapkt for broadcasting"},
	{18,	"set datapkt for broadcasting, ADV_IND"},
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

int client_cb(QuecBtPrim type, char *data, int len)
{
    indication_handle_info->type = type;
    indication_handle_info->data = data;
    indication_handle_info->len = len;
    indication_handle_info->flag = 1;
    return 0;
}

void *indication_handle(void* args)
{
    indication_handle_t *info = NULL;
    info = (indication_handle_t*)args;
    int ret;
    while (info->thread_running)
	{
        if (info->flag == 1)
        {
            printf("recv info->type=0x%X\n", info->type);
            switch(info->type)
            {
                case QUEC_BT_GATT_CONNECT_IND:
                {
                    BtGattConnectInd *ind = (BtGattConnectInd*) info->data;
                    if(sizeof(BtGattConnectInd) != info->len)
                    {
                        printf("BtGattConnectInd length error.\n");
                        break;
                    }
                    printf("connect from %04x:%02x:%06x, role: %s, mtu: %d\n", 
                        ind->address.addr.nap, ind->address.addr.uap, ind->address.addr.lap, 
                        leRole_str[ind->leRole], ind->mtu);
                }
                break;
                case QUEC_BT_GATT_DISCONNECT_IND:
                {
                    QuecBtTypedDeviceAddr t_addr;
                    BtGattDisconnectInd *ind = (BtGattDisconnectInd*) info->data;
                    if(sizeof(BtGattDisconnectInd) != info->len)
                    {
                        printf("BtGattDisconnectInd length error.\n");
                        break;
                    }
                    printf("disconnect from %04x:%02x:%06x\n", 
                        ind->address.addr.nap, ind->address.addr.uap, ind->address.addr.lap);

                }
                break;
                case QUEC_BT_GATT_MTU_CHANGED_IND:
                {
                    BtGattMtuChangedInd *ind = (BtGattMtuChangedInd*) info->data;
                    if(sizeof(BtGattMtuChangedInd) != info->len)
                    {
                        printf("BtGattConnectInd length error.\n");
                        break;
                    }
                    printf("MTU changed from connId:%u , mtu: %d\n", ind->connId, ind->mtu);
                }
                break;
                case QUEC_BT_GATT_DB_READ_ACCESS_IND:
                {
                    BtGattDBReadAccessInd *ind = (BtGattDBReadAccessInd*)info->data;
                    printf("recv QUEC_BT_GATT_DB_READ_ACCESS_IND: attrHandle: %02X\n", ind->attrHandle);
                    if(ind->attrHandle == 0x0E)
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
                        QuecBtUint8 value[4] = {0};
                        srand(time(NULL));
                        snprintf(value, sizeof(value) , "Q%d",rand() % 10);
                        ret = ql_ble_gatt_read_response(ind->gattId, ind->connId, ind->attrHandle, 0, 4, &value);
                        if(ret)
                        {
                            printf("read access response faild.[attrHandle=%d]\n", ind->attrHandle);
                        }
                        printf("attrHandle: 0x%02X, value: %d\n", ind->attrHandle, value);
                    }else if(ind->attrHandle == 0x14)
                    {
                        QuecBtUint8 value[4] = {0};
                        srand(time(NULL));
                        snprintf(value, sizeof(value) , "Q%d",rand() % 10);
                        ret = ql_ble_gatt_read_response(ind->gattId, ind->connId, ind->attrHandle, 0, 4, &value);
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
                    BtGattDBWriteAccessInd *ind = (BtGattDBWriteAccessInd*)info->data;
                    demo_trace_dump_hex("write", info->data, info->len);
                    printf("recv QUEC_BT_GATT_DB_WRITE_ACCESS_IND: attrHandle: %02X\n", ind->attrHandle);
                    gatt_db_write_access_dump(ind);
                    ql_ble_gatt_write_response(ind->gattId, ind->connId, ind->attrHandle, 0);
                }
                break;
                default:
                {
                    printf("recv Unknown type=0x%X \n", info->type);
                }
                break;
            }
            info->flag = 0;
        }else{
            usleep(10*1000);
        }
    }
}

uint8 uuid_server_128[16] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x00};
//in fact it may work as 000F0E0D0C0B0A090807060504030201 in uuid info
uint8 uuid_charact_128[16] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x01};
//in fact it may work as 010F0E0D0C0B0A090807060504030201 in uuid info
uint8 uuid_descriptor_128[16] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x29,0x02};
//in fact it may work as 02290E0D0C0B0A090807060504030201 in uuid info


int create_service128(QuecBtGattId gattId)
{
    /*for test: create db*/
    int ret;
    QuecBtUint8 name[128] = "Quectel";
    ret = ql_ble_db_service_add_128bit(gattId, 2, uuid_server_128, 1);
    if (ret)
    {
        printf("create service error.\n");
        return -1;
    }
    ret = ql_ble_db_charact_add_128bit(gattId, 2, 1, uuid_charact_128,
                                128, QUEC_ATT_PERM_READ | QUEC_ATT_PERM_INDICATE, QUEC_BT_GATT_ATTR_FLAGS_IRQ_READ, name);
    if (ret)
    {
        printf("create charact error.\n");
        return -1;
    }
    QuecBtUint16 value = 0x01;
    ret = ql_ble_db_descriptor_add_128bit(gattId, 2, 1, 1, uuid_descriptor_128, 2, QUEC_ATT_PERM_READ, 
                                    QUEC_BT_GATT_ATTR_FLAGS_NONE, (QuecBtUint8 *)&value);
    if (ret)
    {
        printf("create descriptor error.\n");
        return -1;
    }

    ret = ql_ble_db_service_set_active(gattId, 2, 1);
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
                                QUEC_ATT_PERM_READ |QUEC_ATT_PERM_WRITE_CMD,
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




int test_bt(void)
{  
    int     cmdIdx  = 0;
    int     ret;


    show_group_help(&t_bt_test);

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
                ret = ql_ble_power_on();
                printf("ql_ble_power_on, ret=%d\n", ret);
            }
            break;
            case 1:
            {
                ret = ql_ble_power_off();
                printf("ql_ble_power_off, ret=%d\n", ret);
            }
            break;
            case 2:
            {
                ret = ql_ble_client_init(client_cb);
                printf("ql_ble_client_init, ret=%d\n", ret);

                if(indication_handle_info == NULL)
                {
                    indication_handle_info = malloc(sizeof(indication_handle_t));
                }
                indication_handle_info->thread_running = 1;
                indication_handle_info->flag = 0;
                pthread_create(&indication_handle_info->pth, NULL, indication_handle, (void*)indication_handle_info);
            }
            break;
            case 3:
            {
                char device_name[128];
                printf("please input device name(length less than 128 bytes): ");
                scanf("%s", device_name);
                ret = ql_ble_set_local_name(device_name);
                printf("ret=%d\n", ret);
            }
            break;
            case 4:
            {
                QuecBtGattId gattId;
                ret = ql_ble_gatt_register(&gattId);
                if(ret)
                {
                    printf("ret=%d\n", ret);
                }
                else
                {
                    printf("ret=%d, gattId=%u\n", ret, gattId);
                }
            }
            break;
            case 5:
            {
                QuecBtGattId gattId;
                printf("please input gattId: ");
                scanf("%u", &gattId);
                ret = ql_ble_gatt_unregister(gattId);
                printf("ret=%d\n", ret);
            }
            break;
            case 6:
            {
                QuecBtGattId gattId;
                QuecBtUint16 min, max;
                printf("please input gattId: ");
                scanf("%u", &gattId);
                printf("please input advertising interval(min)(32~16384): ");
                scanf("%d", &min);
                printf("please input advertising interval(max)(32~16384): ");
                scanf("%d", &max);
                ret = ql_ble_gatt_set_adverting_param(gattId, min, max);
                printf("ret=%d\n", ret);
            }
            break;
            case 7:
            {
                QuecBtGattId gattId;
                printf("please input gattId: ");
                scanf("%u", &gattId);
                ret = ql_ble_adverting_start(gattId);
                printf("ret=%d\n", ret);
            }
            break;
            case 8:
            {
                QuecBtGattId gattId;
                printf("please input gattId: ");
                scanf("%u", &gattId);
                ret = ql_ble_adverting_stop(gattId);
                printf("ret=%d\n", ret);
            }
            break;
            case 9:
            {
                QuecBtUint16 numOfAttrHandles, preferredStartHandle;
                QuecBtGattId gattId;
                printf("please input gattId: ");
                scanf("%u", &gattId);
                printf("please input numbers of attribute handle we want alloced: ");
                scanf("%d", &numOfAttrHandles);
                printf("please input prederred start handle when alloced: ");
                scanf("%d", &preferredStartHandle);
                ret = ql_ble_gatt_db_alloc(gattId, numOfAttrHandles, preferredStartHandle);
                printf("ret=%d\n", ret);
            }
            break;
            case 10:
            {
                QuecBtGattId gattId;
                printf("please input gattId: ");
                scanf("%u", &gattId);
                ret = create_service(gattId);
                printf("ret=%d\n", ret);
                ret = create_service128(gattId);
                printf("ret=%d\n", ret);
            }
            break;
            case 11:
            {
                QuecBtGattId gattId;
                printf("please input gattId: ");
                scanf("%u", &gattId);
                ret = ql_ble_gatt_db_add(gattId);
                printf("ret=%d\n", ret);
            }
            break;
            case 12:
            {
                QuecBtTypedDeviceAddr t_addr;
                QuecBtGattId gattId;
                QuecBtConnId connId;
                t_addr.type = QUEC_TBDADDR_PUBLIC;
                t_addr.addr.lap = 0;
                t_addr.addr.uap = 0;
                t_addr.addr.nap = 0;
                printf("please input gattId: ");
                scanf("%u", &gattId);
                ret = ql_ble_gatt_peripheral(gattId, t_addr, QUEC_BT_GATT_FLAGS_UNDIRECTED, 0, &connId);
                if(ret)
                {
                    printf("ret=%d\n", ret);
                }
                else
                {
                    printf("connId=%u,ret=%d\n", connId, ret);
                }
            }
            break;
            case 13:
            {
                QuecBtUint8 value[128];
                QuecBtGattId gattId;
                QuecBtConnId connId;
                printf("please input gattId: ");
                scanf("%u", &gattId);
                printf("please input connId: ");
                scanf("%u", &connId);
                printf("please input indication value(string, 128 bytes): ");
                scanf("%s", value);
                ret = ql_ble_gatt_send_indication(gattId, connId, 0x0E, strlen(value), value);
                printf("ret=%d\n", ret);
            }
            break;
            case 14:
            {
                QuecBtUint8 value[128];
                QuecBtGattId gattId;
                QuecBtConnId connId;
                printf("please input gattId: ");
                scanf("%u", &gattId);
                printf("please input connId: ");
                scanf("%u", &connId);
                printf("please input notification value(string, 128 bytes): ");
                scanf("%s", value);
                ret = ql_ble_gatt_send_notification(gattId, connId,0x0E, strlen(value), value);
                printf("ret=%d\n", ret);
            }
            break;
			case 15:
            {
                QuecBtUint8 mode;
                QuecBtGattId gattid;
                QuecBtUint8 ioCapability; 
                printf("please input mode: "); 
                scanf("%d", &mode); 
                printf("please input gattId: "); 
                scanf("%d", &gattid);
                printf("please input ioCapability: "); 
                scanf("%d", &ioCapability);    
                ret = ql_ble_set_security_controller(mode,gattid,ioCapability);
                printf("ret=%d\n", ret);
            }break;
			case 16:
            {
                char mac[18] = {0};
				ret = ql_bt_mac_get(mac);
				if (ret == 0){
					printf("get bt mac success. mac = %s. \n", mac);
				}else{
					printf("get bt mac failure. mac = %s. \n", mac);
				}
            }break;
			case 17:
            { // set datapkt for broadcasting
	            int ret = 0;
                QuecBtGattId gattid;
	            QuecBtUint8 buf_rsp[] = {0x03, 0x09, 'q', 'l'};
	            QuecBtUint8 buf_adv[] = {
	                0x1b, 0xff, 0xff, 0xff,
	                0x59, 0xfe, 0x5f, 0x5e, 0xae, 0xeb, 0x54, 0x40, 0xf6, 0xad, 0xe6, 0xf5, 0x7e, 0xc1, 0x4b, 0xff,
	                0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78};
                printf("please input gattId: "); 
                scanf("%d", &gattid);

	            ret = ql_ble_gattadvdata(gattid, QUEC_BT_GATT_FLAGS_NONE, buf_adv, sizeof(buf_adv), buf_rsp, sizeof(buf_rsp));
	            printf("ret=%d\n", ret);
                if (ret != 0)
	            {
	                printf("\nErr====fail to set advdata packet! ret %d\n", ret);
	            }
			}break;
	        case 18:
	        { // set datapkt for broadcasting ADV_IND
	            int ret = 0;
	            QuecBtUint8 pktbuf[] = {0x06, 0xFF, 0xAA, 0x22, 0x33, 0x44, 0x55};
	            QuecBtGattId gattid = 0;
	            printf("Input gatt ID:\n");
	            scanf("%d", &gattid);
	            ret = ql_ble_gattadvdata_connectable(gattid, QUEC_BT_GATT_FLAGS_UNDIRECTED, 0, 200, 300, pktbuf, sizeof(pktbuf), NULL, 0);
	            printf("ret=%d\n", ret);
                if (ret != 0)
	            {
	                printf("\nErr====fail to set advdata packet! ret %d\n", ret);
	            }
	        }break;
            case 99:
            {
                ret = ql_ble_client_deinit();
                printf("ql_ble_client_deinit, ret=%d\n", ret);
                    if(indication_handle_info != NULL)
                    {
                        indication_handle_info->thread_running = 0;
                        usleep(100*1000);
                        if(indication_handle_info->pth)
                        {
                            ret = pthread_cancel(indication_handle_info->pth);
                        }
                        usleep(100*1000);
                        free(indication_handle_info);
                        indication_handle_info = NULL;
                    }
            }
            break;
            default:
            break;
        }
    }
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
