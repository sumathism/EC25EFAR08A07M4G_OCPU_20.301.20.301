/**
 * @file test_api.c
 *
 * @brief the example code for the customer at commands.
 * 
 * @note
 *
 * @copyright Copyright (c) 2009-2017 @ Quectel Wireless Solutions Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "ql_softsim_qmi.h"
/* include Quectel library header file */
#include "ql_oe.h"

void printf_help()
{
   printf("*****************************************\n");
   printf("******0    ---> help*********************\n");
   printf("******1    ---> qmi init*****************\n");
   printf("******2    ---> softsim connect report***\n");
   printf("******3    ---> atr reset ***************\n");
   printf("******4    ---> apdu rsp*****************\n");
   printf("******5    ---> get operator*************\n");
   printf("******exit ---> exit app ****************\n");
   printf("*****************************************\n");
}

void ql_softsim_event_cb(unsigned int msg_id,void * ind_buf, unsigned int ind_len)
{
    switch(msg_id)
	{
	case QL_SOFTSIM_APDU_IND:
	{
		ql_softsim_apdu_msg_t *ind = (ql_softsim_apdu_msg_t*)ind_buf;
        printf("softsim apdu ind .... slot: %d\n", ind->slot);
		break;
	}
	case QL_SOFTSIM_CONNECT_IND:
	{
		ql_softsim_connect_t *ind = (ql_softsim_connect_t*)ind_buf;
        printf("softsim connect ind.... slot: %d\n", ind->slot);
		break;
	}
	case QL_SOFTSIM_DISCONECT_IND:
	{
		ql_softsim_disconnect_t *ind = (ql_softsim_disconnect_t*)ind_buf;
		printf("softsim disconnet ind.... slot: %d\n", ind->slot);
        break;
	}
	case QL_SOFTSIM_POWER_UP_IND:
	{
		ql_softsim_card_powerup_t *ind = (ql_softsim_card_powerup_t*)ind_buf;
		printf("softsim power up ind.... slot: %d\n", ind->slot);
        break;
	}	 
	default:
		 break;
	}

	return;
}


int main(int argc, char **argv)
{

	int ret = 0;
    printf_help();
	char tmp[32];
    int len = 0;

	while(1)
	{
	    printf("please input operator: ");
	    memset(tmp,0,sizeof(tmp));

		scanf("%s",tmp);
		getchar();
		if(0 == strncasecmp(tmp,"exit",strlen("exit")))
		{
		    printf("exit test app");
			break;
		}

		switch(atoi(tmp))
		{
        case 0:
            printf_help();
			break;
        case 1:
		    {
               quectel_softsim_client_init(ql_softsim_event_cb);
			}
			break;
		case 2:
		    {
               quectel_softsim_connect_handler();
			}
			break;
		case 3:
			{
                 char atr_buf[32] = "00112233445566778899AA";
                 len = strlen(atr_buf);
				 ret = quectel_atr_reset_handler(atr_buf,len);
				 if(ret == 0)
				 {
				     printf("atr reset failed \n");
				 }
			}
			break;
		case 4:
			{
                 char apdu_rsp_buf[1024] = "apdudata";
                 len = strlen(apdu_rsp_buf);
				 ret = quectel_atr_reset_handler(apdu_rsp_buf,len);
				 if(ret == 0)
				 {
				     printf("atr reset failed \n");
				 }
			}
			break;
		case 5:
			{
			    quectel_softsim_client_release();
			}
			break;
		default:
				printf("please input valid parameter.\n");
			break;
		}
	}

	return 0;
}
