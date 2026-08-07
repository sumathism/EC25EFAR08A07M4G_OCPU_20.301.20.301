#include "ql_oe.h"

int main(int argc, const char *argv[])
{
	int ret = -1;
	uint8_t eMode = 0;

	ret = QL_Nas_Set_ECBM(eMode); // 0: exit emergency mode
	printf("ret: %d\n",ret);
	return 0;
}
