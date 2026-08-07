/**
 * @file ql_network.h
 * @brief Quectel Openlinux network public function declarations.
 * 
 * @note 
 *
 * @copyright Copyright (c) 2009-2017 @ Quectel Wireless Solutions Co., Ltd.
 */

#ifndef __QL_NETWORK_H__
#define __QL_NETWORK_H__

typedef struct {
    char gw_ip[20];                      /* Gateway Address*/
    char netmask[20];                    /* netmask */
    unsigned char enable_dhcp;           /* Whether to enable DHCP; boolean value. */
    char dhcp_start_ip[20];              /*DHCP start IP address.*/
    char dhcp_end_ip[20];                /*DHCP end IP address. */
    unsigned int lease_time;             /*DHCP lease time, in seconds.*/
}ql_lan_dhcp_config_s;

typedef enum {
    QL_ETHERNET_MODE_LAN_ROUTE = 0,
    QL_ETHERNET_MODE_WAN_ROUTE
} ql_ethernet_mode_e;

typedef struct {
	uint8_t enable;
	uint8_t adaptive;
	char device_type[32];
	uint8_t mac_addr[6];
	char client_device_name[64];
}ql_ippt_config_s;


/**
* Set the network configuration: dhcp configuration.
*s
* @param [in]  lan_dhcp_config
*
* @return 
*   On success, 0 is returned.  On error, -1 is returned.
* 
*/
extern int QL_LAN_DHCP_Config_Set(ql_lan_dhcp_config_s lan_dhcp_config);

/**
* Get the network configuration: dhcp configuration.
*
* @param [out]   lan_dhcp_config
*
* @return 
*   On success, 0 is returned.  On error, -1 is returned.
* 
*/
extern int QL_LAN_DHCP_Config_Get(ql_lan_dhcp_config_s *lan_dhcp_config);

/**
* Sets Ethernet Tethering Mode.
*
* @param [in]   ethernet tethering mode
*
* @return 
*   On success, 0 is returned.  On error, -1 is returned.
* 
*/
extern int QL_Ethernet_Mode_Set(ql_ethernet_mode_e mode);

/**
* Gets Ethernet Tethering Mode.
*
* @param [out]   ethernet tethering mode
*
* @return 
*   On success, 0 is returned.  On error, -1 is returned.
* 
*/
extern int QL_Ethernet_Mode_Get(ql_ethernet_mode_e *mode);

/**
* QL_Ippt_Config_Set.
*
* @param [adaptive]   
*	0 : Disable IPPT Device Adaptation Function
*	1 : Enable IPPT ETH Newest Device Adaptation; when enabled, PublicIP will be assigned to the newest connected ETH device.
*	2 : Enable IPPT ETH First Device Adaptation; when enabled, PublicIP will be assigned to the first connected ETH device. 
*	3 : Enable IPPT USB latest device adaptation; when enabled, PublicIP will be assigned to the latest connected USB device. 
*	4 : Enable IPPT USB First Device Adaptation; when enabled, PublicIP will be assigned to the first connected USB device.
*	
* @return 
*   On success, 0 is returned.  On error, -1 is returned.
* 
*/
extern int QL_Ippt_Config_Set(uint8_t adaptive);

/**
* QL_Ippt_Config_Get.
*
* @param [ippt_conf]   
*
* @return 
*   On success, 0 is returned.  On error, -1 is returned.
* 
*/
extern int QL_Ippt_Config_Get(ql_ippt_config_s *ippt_conf);

#endif /* end of __QL_NETWORK_H__ */

