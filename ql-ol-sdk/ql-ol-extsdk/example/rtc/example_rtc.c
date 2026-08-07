/*-----------------------------------------------------------------------------*/
/**
  @file  example_rtc.c
  @brief   rtc API test daemon
*/
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Copyright (c) 2018 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
-------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  EDIT HISTORY
  This section contains comments describing changes made to the file.
  Notice that changes are listed in reverse chronological order.

  when       	who       	what, where, why
  --------		---			---------------------------------------------------
  20181229   	Rambo.shan  Created
--------------------------------------------------------------------------------*/

#include <stdlib.h>
#include "ql_oe.h"

/*-----------------------------------------------------------------------------------------------*/
/**
  @brief Displays the utc time for the current value
  @param[in] cur_time Storage for microsecond
  @return 
  */
/*-----------------------------------------------------------------------------------------------*/
void cur_time_dump(int64_t cur_time)
{
	time_t time_tmp = (time_t)(cur_time / 1000);
	struct tm tm_tmp = {0};
	localtime_r(&time_tmp, &tm_tmp); 

	printf("current Rtc Time:%lld(ms) -> ", cur_time);
	printf("%04d-%02d-%02d ", tm_tmp.tm_year + 1900, tm_tmp.tm_mon+1, tm_tmp.tm_mday);  
	printf("%02d:%02d:%02d\n", tm_tmp.tm_hour, tm_tmp.tm_min, tm_tmp.tm_sec); 
}

/*-----------------------------------------------------------------------------------------------*/
/**
  @brief example_rtc instructions
  @return 
  */
/*-----------------------------------------------------------------------------------------------*/
void example_usage(void)
{
	printf("usage: 	example_rtc [OPTIONS] [VALUE]\n");
	printf("OPTIONS	Operation RTC time\n");
	printf("	-w, Set the VALUE of UTC time to RTC time\n");
	printf("	-r, View the current RTC time, VALUE is invalid at this time\n");
	printf("	VALUE, The value of the time operation is in milliseconds\n");
	printf("for example: ./example_rtc -w 1546408989000\n");
}

/*-----------------------------------------------------------------------------------------------*/
/**
  @brief Test main program
  @return 
  0 - succeed
  -1 - error ocurred
  */
/*-----------------------------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
	char *p_tmp_cmd = NULL;
	char cmd = '0';
	int64_t set_time = 0, rtcTime = 0;

	if((2 == argc) && (0 == strcmp("-r", argv[1])))
	{
		p_tmp_cmd = argv[1];
	}
	else if((3 == argc) && (0 == strcmp("-w", argv[1])))
	{
		p_tmp_cmd 	= argv[1];
		set_time 	= atoll(argv[2]);
	}
	else
	{
		printf("input argument invalid\n");
		example_usage();
		return -1;
	}
	
	cmd = p_tmp_cmd[1];

	//printf("cmd:%c	file:%s 	set_time:%lld\n", cmd, rtc_file_path, set_time);

	switch(cmd)
	{
		case 'w':
		{
			//printf("cur Time + %lldms\n", set_time);
			if(0 != ql_rtc_time_set(set_time))
			{
				printf("ql_rtc_time_set failed \n");
				return -1;
			}

			if(0 != ql_rtc_time_get(&rtcTime))
			{
				printf("ql_rtc_time_get failed \n");
				return -1;
			}
			cur_time_dump(rtcTime);
			break;
		}
		case 'r':
		{
			if(0 != ql_rtc_time_get(&rtcTime))
			{
				printf("ql_rtc_time_get failed \n");
				return -1;
			}
			cur_time_dump(rtcTime);
			break;
		}
		default:
		{
			printf("input OPTIONS argument invalid\n");
			example_usage();
			return -1;
		}
	}
	
	return 0;
}

