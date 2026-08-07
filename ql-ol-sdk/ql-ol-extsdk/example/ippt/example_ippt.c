#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <unistd.h>

#include "ql_network.h"
#include "ql_wwan_v2.h"

int data_call_state = 0;

#define log_dbg(fmt, ...)  printf( "%s: %d %s [DEBUG]:"fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

static void data_call_state_callback(ql_data_call_state_s *state)
{
	log_dbg("profile id %d ", state->profile_idx);
	log_dbg("IP family %s ", QL_DATA_CALL_TYPE_IPV4 == state->ip_family ? "v4" : "v6");
	if(QL_DATA_CALL_CONNECTED == state->state && QL_DATA_CALL_TYPE_IPV4 == state->ip_family) {
        data_call_state = QL_DATA_CALL_CONNECTED;
		log_dbg("is Connected");
		log_dbg("Interface Name: %s", state->name);
        log_dbg("IP address:          %s", inet_ntoa(state->v4.ip));
        log_dbg("Gateway address:     %s", inet_ntoa(state->v4.gateway));
        log_dbg("Primary DNS address: %s", inet_ntoa(state->v4.pri_dns));
        log_dbg("Second DNS address:  %s", inet_ntoa(state->v4.sec_dns));
	} else if (QL_DATA_CALL_DISCONNECTED == state->state && QL_DATA_CALL_TYPE_IPV4 == state->ip_family){
        data_call_state = QL_DATA_CALL_DISCONNECTED;
		log_dbg("is disconnected, and reason code 0x%x\n", state->err);
	}
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        log_dbg("pls input adaptive: ./example_ippt <adaptive>");
        log_dbg("<adaptive> = 0 : Disable IPPT Device Adaptation Function.");
        log_dbg("<adaptive> = 1 : Enable IPPT ETH Newest Device Adaptation; when enabled, PublicIP will be assigned to the newest connected ETH device.");
        log_dbg("<adaptive> = 2 : Enable IPPT ETH First Device Adaptation; when enabled, PublicIP will be assigned to the first connected ETH device.");
        log_dbg("<adaptive> = 3 : Enable IPPT USB latest device adaptation; when enabled, PublicIP will be assigned to the latest connected USB device.");
        log_dbg("<adaptive> = 4 : Enable IPPT USB First Device Adaptation; when enabled, PublicIP will be assigned to the first connected USB device.");
        return -1;
    }

    uint8_t adaptive = (uint8_t)atoi(argv[1]);
    log_dbg("adaptive:%d", adaptive);
    ql_ippt_config_s ippt_conf = {0};

    if (QL_Ippt_Config_Get(&ippt_conf)) {
        log_dbg("QL_Ippt_Config_Get fail.");
        return -1;
    }

    if (ippt_conf.adaptive == adaptive) {
        log_dbg("The current adaptive is already %d.", adaptive);
        log_dbg("enable = %d adaptive = %d device_type = \"%s\" mac_addr = %02x:%02x:%02x:%02x:%02x:%02x", ippt_conf.enable,
                                                                                                           ippt_conf.adaptive,
                                                                                                           ippt_conf.device_type,
                                                                                                           ippt_conf.mac_addr[0],
                                                                                                           ippt_conf.mac_addr[1],
                                                                                                           ippt_conf.mac_addr[2],
                                                                                                           ippt_conf.mac_addr[3],
                                                                                                           ippt_conf.mac_addr[4],
                                                                                                           ippt_conf.mac_addr[5]);
        return 0;
    }

    if (QL_Ippt_Config_Set(adaptive)) {
        log_dbg("QL_Ippt_Config_Set fail.");
        return -1;
    }

    int retry = 10;
    ql_data_call_s data_call = {0};
    ql_data_call_info_s data_call_info = {0};
    ql_data_call_error_e err = QL_DATA_CALL_ERROR_NONE;
    ql_data_call_default_profile_s profile = {0};
    /*
     * The dialup API relies on the Quectel Manager service. If the program is not initialized successfully, 
     * debugging these API interfaces will fail, so here is judged whether the service is started normally.
     */
    while(0 != QL_Data_Call_Init_Precondition() && 0 != retry) {
        log_dbg("The Quectel manager service is not initialized, about 500ms try again.");
        usleep(500*1000);
        retry--;
    }

    if(0 == retry) {
        log_dbg("Data call failure\n");
        exit(0);
    }

    if(QL_Data_Call_Init(data_call_state_callback)) {
        log_dbg("Initialization data call failure\n");
        exit(0);
    }

	if(QL_Data_Call_Get_Default_Profile(&profile)) {
		log_dbg("Get default profile failure");
		return -1;
	}
    log_dbg("default profile_idx:%d", profile.profile_idx);
    data_call.profile_idx = profile.profile_idx;

    if(QL_Data_Call_Info_Get(data_call.profile_idx, QL_DATA_CALL_TYPE_IPV4, &data_call_info, &err)) {
        log_dbg("get profile index %d information failure: errno 0x%x\n", data_call.profile_idx, err);
        return -1;
    }

    if(QL_DATA_CALL_CONNECTED == data_call_info.v4.state) {
        data_call_state = QL_DATA_CALL_CONNECTED;
        QL_Data_Call_Stop(profile.profile_idx, QL_DATA_CALL_TYPE_IPV4, &err);
    }

    retry = 10;
    while(data_call_state == QL_DATA_CALL_CONNECTED && retry --) {
        sleep(1);
    }

    data_call.ip_family = QL_DATA_CALL_TYPE_IPV4;
    data_call.reconnect = 1;
    data_call.profile_idx = profile.profile_idx;
    if (QL_Data_Call_Start(&data_call, &err)){
        log_dbg("the profile index %d start data call failure: 0x%x", data_call.profile_idx, err);
        return -1;
    }
    
    memset(&ippt_conf, 0, sizeof(ippt_conf));
    if (QL_Ippt_Config_Get(&ippt_conf)) {
        log_dbg("QL_Ippt_Config_Get fail.");
        return -1;
    }

    log_dbg("enable = %d adaptive = %d device_type = \"%s\" mac_addr = %02x:%02x:%02x:%02x:%02x:%02x",  ippt_conf.enable,
                                                                                                        ippt_conf.adaptive,
                                                                                                        ippt_conf.device_type,
                                                                                                        ippt_conf.mac_addr[0],
                                                                                                        ippt_conf.mac_addr[1],
                                                                                                        ippt_conf.mac_addr[2],
                                                                                                        ippt_conf.mac_addr[3],
                                                                                                        ippt_conf.mac_addr[4],
                                                                                                        ippt_conf.mac_addr[5]);
    return 0;
}