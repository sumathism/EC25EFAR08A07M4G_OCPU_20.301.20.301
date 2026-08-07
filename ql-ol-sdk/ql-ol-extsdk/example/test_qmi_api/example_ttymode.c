#include <ql_oe.h>

void ind_cb(int mode)
{
	printf("tty mode:%d\n",mode);
	switch(mode)
	{
		case QL_TTY_MODE_FULL:
			printf("full\n");
			break;

		case QL_TTY_MODE_VCO:
			printf("Voice carry over\n");
			break;

		case QL_TTY_MODE_HCO:
			printf("Hearing carry over\n");
			break;

		case QL_TTY_MODE_OFF:
			printf("off");
			break;

		default:
			printf("unsupport mode\n");
			break;
	}
}

int main(int argc, const char *argv[])
{
	int ret = -1;
	int tty_mode;
    ret = QL_Voice_Config_Init();
	if(ret != 0){
		printf("init error\n");
		return -1;
	}
	usleep(500*1000);

	QL_Voice_Config_Ind(ind_cb);
	while(1){
		printf("input tty mode:\n");
		printf("0: Full\t 1: Voice carry over\t 2: Hearing carry over\t 3: Off\n");
		scanf("%d",&tty_mode);
		QL_Voice_Config_Set(tty_mode);
	}
	
	return 0;
}
