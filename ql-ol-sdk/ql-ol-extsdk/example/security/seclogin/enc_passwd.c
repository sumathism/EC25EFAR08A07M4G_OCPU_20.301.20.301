#include<stdio.h>
#include<unistd.h>
#include<getopt.h>
#include<string.h>
#include<stdlib.h>
#include<crypt.h>

#define PASSWD_MAX 20
#define FIXED_SALT "$1$EAIaRTnR$"

static struct option opts[] = {
    {"help",         no_argument,          0,  'h' },
    {"plain_passwd",    required_argument, 0,  'p' },
    {0,              0,                    0,  '0' },
};

const char* helptext = 
    "encrypt the passwd for linux login, this tool is used for PC\n"
    "-p|--plain_passwd      input your expect passwd\n"
    "-h|--help              show this help text,and exit\n"
;

int main(int argc,  char* argv[]) 
{
    int ret = -1;
    int arg;
    int index = 0;
    int passwd_valid = -1;
    char passwd[21] = {0};

    while ((arg = getopt_long(argc, argv, "p:h", opts, &index)) != -1) 
    {
        switch(arg) {
            case 'p' :
                if(strlen(optarg) > PASSWD_MAX) {
                    fprintf(stderr, "%s it too long, the passwd must be less than 20 charater\n", optarg);
                    return ret;
                }
                strcpy(passwd, optarg);
                passwd_valid = 1;
                break;
            case 'h':
                printf("%s", helptext);
                exit(0);
            default:
                printf("Invaild Argument\n");
                exit(-1);
        }      
    }
    
    if(passwd_valid == 1) {
	printf("encrpyt passwd : %s\n", crypt(passwd, FIXED_SALT));    
    } 

    return 0;
} 
