#include "ql_oe.h"
#include "DSI_ConnectManager.h"

#define BUF_SIZE 128
#define FUNC_GROUP_MAX 10

void nw_cb(int evt)
{
    switch (evt)
    {
    case QL_DSI_EVT_NET_IS_CONN:
        printf("Call is connected!!\n");
        break;
    case QL_DSI_EVT_NET_NO_NET:
        printf("Call is Disconnected!!\n");
        break;
    case QL_DSI_EVT_NET_NEWADDR:
        printf("New address is generated!!\n");
        break;
    case QL_DSI_EVT_NET_DELADDR:
        printf("Old address deleted !!\n");
        break;
    case QL_DSI_EVT_WDS_CONNECTED:
        printf("Network Ready!!\n");
        break;
    default:
        printf("Unkown evt\n");
        break;
    }
}

typedef int (*TEST)(void);

typedef struct
{
    int cmdIdx;
    char *funcName;
} st_api_test_case;

typedef struct
{
    char *group_name;
    st_api_test_case *test_cases;
    TEST pf_test;
} func_api_test_t;

typedef struct
{
    char *group_name;
    st_api_test_case *test_cases;
    TEST pf_test;
} func_api_test_group_t;

func_api_test_group_t g_test_list[FUNC_GROUP_MAX] = {0};
int test_dsi(void);
void show_group_list()
{
    int i;

    printf("Test groups:\n");
    for (i = 0; i < FUNC_GROUP_MAX; i++)
    {
        if (g_test_list[i].group_name != NULL)
        {
            printf("%d:\t%s\n", i, g_test_list[i].group_name);
        }
    }
}

st_api_test_case at_dsi_testlist[] =
    {
        {0, "ql_wan_init"},
        {1, "ql_wan_set_autoconnect"},
        {2, "ql_wan_set_ip_version"},
        {3, "ql_wan_start"},
        {4, "ql_wan_stop"},
        {5, "ql_wan_get_pkt_stats"},
        {6, "ql_wan_set_profile_index"},
        {7, "ql_wan_set_pdp_type"},
        {8, "ql_wan_setapn"},
        {9, "ql_wan_getapn"},
        {99, "ql_wan_deinit"},
        {-1, NULL}};

func_api_test_t t_dsi_test = {"dsi_test", at_dsi_testlist, test_dsi};

void show_group_help(func_api_test_t *pt_test)
{
    int i;

    printf("Group Name:%s, Supported test cases:\n", pt_test->group_name);
    for (i = 0;; i++)
    {
        if (pt_test->test_cases[i].cmdIdx == -1)
        {
            break;
        }
        printf("%d:\t%s\n", pt_test->test_cases[i].cmdIdx, pt_test->test_cases[i].funcName);
    }
}

void add_test_node(func_api_test_t *pt_test)
{
    int i;
    for (i = 0; i < FUNC_GROUP_MAX; i++)
    {
        if (g_test_list[i].group_name == NULL)
        {
            g_test_list[i].group_name = pt_test->group_name;
            g_test_list[i].test_cases = pt_test->test_cases;
            g_test_list[i].pf_test = pt_test->pf_test;
            break;
        }
    }
}

int test_dsi(void)
{
    int cmdIdx = 0;
    int ret;

    show_group_help(&t_dsi_test);

    while (1)
    {
        printf("please input cmd index(-1 exit): ");
        scanf("%d", &cmdIdx);
        if (cmdIdx == -1)
        {
            break;
        }
        switch (cmdIdx)
        {
        case 0:
        {
            //ql_wan_init
            ret = ql_wan_init();
            printf("ret: %d\n", ret);
        }
        break;
        case 1:
        {
            //ql_wan_set_autoconnect
            int reconnect, reconnect_sec;
            printf("please input auto connect status:(0 off, 1 on)\n");
            scanf("%d", &reconnect);
            printf("please input auto connect interval:(1-300 seconds)\n");
            scanf("%d", &reconnect_sec);
            ret = ql_wan_set_autoconnect(reconnect, reconnect_sec);
            printf("ret: %d\n", ret);
        }
        break;
        case 2:
        {
            //ql_wan_set_ip_version
            int ip_version;
            printf("please input ip version: (4: IPv4, 6:IPv6, 10: IPv4v6)\n");
            scanf("%d", &ip_version);
            ret = ql_wan_set_ip_version(ip_version);
            printf("ret: %d\n", ret);
        }
        break;
        case 3:
        {
            //ql_wan_start
            ret = ql_wan_start(nw_cb);
            printf("ret: %d\n", ret);
        }
        break;
        case 4:
        {
            //ql_wan_stop
            ret = ql_wan_stop();
            printf("ret: %d\n", ret);
        }
        break;
        case 5:
        {
            ql_dsi_data_pkt_stats stats;

            ret = ql_wan_get_pkt_stats(&stats);
            if (ret)
            {
                printf("ret: %d\n", ret);
            }
            else
            {
                printf("pkts_tx: %lu\npkts_rx: %lu\nbytes_tx: %lld\nbytes_rx: %lld\npkts_dropped_tx: %lu\npkts_dropped_rx: %lu\n",
                       stats.pkts_tx, stats.pkts_rx, stats.bytes_tx, stats.bytes_rx, stats.pkts_dropped_tx, stats.pkts_dropped_rx);
            }
        }
        break;
        case 6:
        {
            //ql_wan_set_profile_index
            int profile_index;
            printf("please input profile index: (1-24)\n");
            scanf("%d", &profile_index);
            ret = ql_wan_set_profile_index(profile_index);
            printf("ret: %d\n", ret);
        }
        break;
        case 7:
        {
            int pdp_type;
            printf("please input pdp type: (0: IPv4, 1: ppp, 2: IPv6, 3: IPv4v6)\n");
            scanf("%d", &pdp_type);
            ret = ql_wan_set_pdp_type(pdp_type);
            printf("ret: %d\n", ret);
        }
        break;
        case 8:
        {
            //ql_wan_setapn
            char apn[150];
            char user[127];
            char password[127];
            int auth;
            printf("please input apn name:\n");
            scanf("%s", apn);
            printf("please input apn user:\n");
            scanf("%s", user);
            printf("please input apn password:\n");
            scanf("%s", password);
            printf("please input apn auth type:(1: pap, 2: chap, 3: pap+chap)\n");
            scanf("%d", &auth);
            ret = ql_wan_setapn(apn, user, password, auth);
            printf("ret: %d\n", ret);
        }
        break;
        case 9:
        {
            char apn[127] = {0};
            char user[127] = {0};
            char password[127] = {0};
            ret = ql_wan_getapn(apn, sizeof(apn), user, sizeof(user), password, sizeof(password));
            if (strlen(apn) > 0)
            {
                printf("apn: %s\n", apn);
            }
            if (strlen(user) > 0)
            {
                printf("user: %s\n", user);
            }
            if (strlen(password) > 0)
            {
                printf("password: %s\n", password);
            }
            printf("ret: %d\n", ret);
        }
        break;
        case 99:
        {
            //ql_wan_deinit
            ql_wan_deinit();
            printf("call ql_wan_deinit\n");
        }
        break;
        }
    }
}

int main()
{
    int cmdIdx = 0;
    add_test_node(&t_dsi_test);
    show_group_list();
    while (1)
    {
        printf("please input command index(-1 exit): ");
        scanf("%d", &cmdIdx);
        if (cmdIdx == -1)
        {
            break;
        }
        if ((cmdIdx >= FUNC_GROUP_MAX) || (g_test_list[cmdIdx].group_name == NULL))
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
