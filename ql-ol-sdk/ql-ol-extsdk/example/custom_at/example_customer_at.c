/**
 * @file example_customer_at.c
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

/* include Quectel library header file */
#include "ql_oe.h"

#define  QL_CMD_ATCOP_NA 0X01
#define  QL_CMD_ATCOP_EQ 0X02
#define  QL_CMD_ATCOP_QU 0X04
#define  QL_CMD_ATCOP_AR 0X08

#define QL_IS_READ_COMMAND(opcode)     (opcode == (QL_CMD_ATCOP_NA | QL_CMD_ATCOP_QU))
#define QL_IS_WRITE_COMMAND(opcode)    (opcode == (QL_CMD_ATCOP_NA | QL_CMD_ATCOP_EQ | QL_CMD_ATCOP_AR))
#define QL_IS_EXEC_COMMAND(opcode)     (opcode == (QL_CMD_ATCOP_NA))
#define QL_IS_TEST_COMMAND(opcode)     (opcode == (QL_CMD_ATCOP_NA | QL_CMD_ATCOP_EQ | QL_CMD_ATCOP_QU))

typedef struct 
{
    char qlcmd_name[32];
	char qlcmd_argstr[512];
	char  qlcmd_opcode[8];
}ql_atcmd_type;

typedef int (* ql_atcmd_handler)(int,struct sockaddr_un *, char *, char *);

typedef struct
{
    char cmd_atname[32];
	ql_atcmd_handler cmd_athandler;
}ql_atcmd_handler_type;


static int test_handler(int sockfd, struct sockaddr_un *un, char *cmd_name, char *cmd_arg)
{
	char response[512] = { 0 };
	int retval = 0;
	
	snprintf(response, sizeof(response)-1, "\"+%s\"", cmd_name);
	retval = ql_customer_at_send_response(sockfd, un, true, response, strlen(response));
	return retval;
}


static char cust_atcmd[][16] = 
{
	"TEST"
};

ql_atcmd_handler_type cust_atcmd_handler[]=
{
	{"TEST", test_handler}
};

static int quectel_check_custcmd_func(char * name)
{
	int i = 0;
	for(i = 0; i < (sizeof(cust_atcmd)/sizeof(&cust_atcmd[0])); i++)
	{
		printf("cust atcmd name %s\n", name);
		if(0 == strcasecmp(cust_atcmd[i], name))
		{
			printf("found cust at command success\n");
			return 0;
		}
	}
	return -1;
}



static int quectel_get_atcmd_name(char * str, ql_atcmd_type * atcmd)
{
    char * tmp1 = NULL;
    char * tmp2 = NULL;

	tmp1 = strchr(str, '\"');
	if(tmp1 == NULL)
	{
	    return -1;
	}

	tmp2 = strchr(tmp1 + 1, '\"');
	if(tmp2 == NULL)
	{
	    return -1;
	}

	memcpy(atcmd->qlcmd_name, tmp1+1, tmp2-tmp1-1);

    printf("cmd name: %s size: %d\n", atcmd->qlcmd_name, strlen(atcmd->qlcmd_name));
    return 0;
}

static int quectel_get_atcmd_paramter(char * str, ql_atcmd_type * atcmd)
{
    char * tmp = NULL;
	
	tmp = strchr(str, ',');
	if(tmp == NULL)
	{
	    return -1;
	}
	strcpy(atcmd->qlcmd_argstr, tmp+1);

    printf("cmd strarg: %s size: %d\n", atcmd->qlcmd_argstr, strlen(atcmd->qlcmd_argstr));
    return 0;
}


static void atcommand_handler(int sockfd, struct sockaddr_un *un, char *ptr, int len)
{
	int retval;
	char response[512] = { 0 };
    int test_flag = 0;
    ql_atcmd_type recv_atcmd;
	int i = 0;

    printf("cmd str: %s\n", ptr);

    memset(&recv_atcmd, 0, sizeof(recv_atcmd));
    //get atcmd name 
    if(0 > quectel_get_atcmd_name(ptr, &recv_atcmd))
	{
        printf("no found at command\n");
        retval = ql_customer_at_send_response(sockfd, un, false, response, strlen(response));
		return;
	}

	//get atcmd parameter
	quectel_get_atcmd_paramter(ptr, &recv_atcmd);

    if(0 > quectel_check_custcmd_func(recv_atcmd.qlcmd_name))
	{
	    printf("not found customer at cmd\n");
		retval = ql_customer_at_send_response(sockfd, un, false, NULL, 0);
		return;

	}
    else
    {
		for(i = 0; i < (sizeof(cust_atcmd_handler)/sizeof(&cust_atcmd_handler[0])); i++)
		{
	       if(0 == strcasecmp(cust_atcmd_handler[i].cmd_atname, recv_atcmd.qlcmd_name))
	       {
			   cust_atcmd_handler[i].cmd_athandler(sockfd, un, recv_atcmd.qlcmd_name, recv_atcmd.qlcmd_argstr);
		       return;
	       }
		}
       //not found customer at command handler,return false
       retval = ql_customer_at_send_response(sockfd, un, false, NULL, 0);
       return;
 
    }
}

static void listen_sock(int sockfd)
{
	int retval;
	fd_set rfds;
	struct timeval tv;
	struct sockaddr_un un;
	char buf[2048];
	
	while (1) 
	{
		FD_ZERO(&rfds);
		FD_SET(sockfd, &rfds);

		tv.tv_sec = 5;
		tv.tv_usec = 0;

		printf("wait select.... \n");
		retval = select(sockfd + 1, &rfds, NULL, NULL, &tv);
		if (retval > 0) 
		{
			if(FD_ISSET(sockfd, &rfds)) 
			{
				memset(buf, 0, sizeof(buf));
				memset(&un, 0, sizeof(un));
				if (ql_customer_at_get_request(sockfd, &un, buf, sizeof(buf))) 
				{
					printf("get at command request failed\n");
					continue;
				}
				atcommand_handler(sockfd, &un, buf, strlen(buf));
			}
		} 
		else if (retval < 0)
		{
			printf("select error: %s\n", strerror(errno));
		}
	}
}

int main(int argc, char **argv)
{
	int sockfd = -1;

	sockfd = ql_customer_at_get_socket();
	if (sockfd < 0) 
	{
		printf("Can not get socket fd\n");
		return -1;
	}

	listen_sock(sockfd);
	return 0;
}
