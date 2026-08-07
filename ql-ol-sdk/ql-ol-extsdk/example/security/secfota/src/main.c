#include<stdio.h>
#include<unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include "ql_fota_verify.h"

static struct option opts[] = {
    {"help",         no_argument,          0,  'h' },
    {"input_zip",    required_argument,    0,  'i' },
    {0,              0,                    0,  '0' },
};

const char *helptext =
"zipverify software v2.0.0 - a tool to Check the integrity and trust of the zip file\n"
"-i|--input_zip         <path str> set zip package path\n"
"-h|--help              show this help text,and exit\n"
;

const char cert_mem[] = 
    "-----BEGIN CERTIFICATE-----\n"
    "MIICoTCCAiagAwIBAgIBATAKBggqhkjOPQQDAzCBkTELMAkGA1UEBhMCQ04xDjAM\n"
    "BgNVBAgMBUFOSFVJMQ4wDAYDVQQHDAVIRUZFSTEYMBYGA1UECwwPR2VuZXJhbCBG\n"
    "T1RBIENBMR0wGwYDVQQLDBRRVUVDVEVMIFRlY2hub2xvZ2llczEQMA4GA1UECgwH\n"
    "UXVlY3RlbDEXMBUGA1UEAwwOUUNUIEZPVEEgQ0EgVjEwHhcNMjMxMDA3MDMyMTM0\n"
    "WhcNNDMxMDAyMDMyMTM0WjCBkTELMAkGA1UEBhMCQ04xDjAMBgNVBAgMBUFOSFVJ\n"
    "MQ4wDAYDVQQHDAVIRUZFSTEYMBYGA1UECwwPR2VuZXJhbCBGT1RBIENBMR0wGwYD\n"
    "VQQLDBRRVUVDVEVMIFRlY2hub2xvZ2llczEQMA4GA1UECgwHUXVlY3RlbDEXMBUG\n"
    "A1UEAwwOUUNUIEZPVEEgQ0EgVjEwdjAQBgcqhkjOPQIBBgUrgQQAIgNiAATIQXaB\n"
    "goIKiSUWC4VC9VoAeGS3eubBUeQKde+iW904BNDELQHKl22mLOo5enoduAbqcQO7\n"
    "fiSh4H9sfmPKYtbMV8EoOf98AxPm9vKEvb9YWp0zhmSc/oWaXi+Lnu6u6qqjUDBO\n"
    "MB0GA1UdDgQWBBTvieCcL6AMNCD4olPzXGWVQS7SNzAfBgNVHSMEGDAWgBTvieCc\n"
    "L6AMNCD4olPzXGWVQS7SNzAMBgNVHRMEBTADAQH/MAoGCCqGSM49BAMDA2kAMGYC\n"
    "MQCeUc7C3z8+NWW3onhV6usjGBck/PPZkiYaCcoGS4UpcUugCnNA5rPaEiTe1rkO\n"
    "+SMCMQC+XG01LKTZen8QpIg6dmjEAT6dMQYpZjzeYkSIW07GFIpSgO5ZUKZBLoXq\n"
    "fXABVpk=\n"
    "-----END CERTIFICATE-----\n";

int main(int argc, char* argv[])
{
    int ret = -1;
    int arg;
    int index = 0;
    int zip_exist = 0;
    char zip_path[1024] = {0};

    while ((arg = getopt_long(argc, argv, "i:h", opts, &index)) != -1)
    {
        switch(arg) {
        case 'i':
            if (strlen(optarg) >= sizeof(zip_path)) {
                fprintf(stderr, "%s it too long, the zip_path length must be less than 1024 charater\n", optarg);
                return -1;
            }
            strncpy(zip_path, optarg, sizeof(zip_path)-1);
            zip_exist = 1;           
            break;       
        case 'h':
            printf("%s", helptext);
            exit(0);
        default:
            printf("Invaild Argument\n");
            exit(1);
        }
    }

    if( zip_exist == 1 ) {
        ret = ql_zip_verify((const char *)zip_path, strlen(zip_path), (const char *)cert_mem, strlen(cert_mem));
        if(!ret) {
            printf("Verify OK\n");
        } else {
            printf("Verify Failed\n");
        }
    }

    return ret;
}