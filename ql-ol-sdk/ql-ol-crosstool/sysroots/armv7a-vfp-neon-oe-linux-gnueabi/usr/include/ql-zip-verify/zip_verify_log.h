#ifndef __ZIP_VERIFY_LOG_H__
#define __ZIP_VERIFY_LOG_H__

#include <inttypes.h>
#include <stdio.h>

/*switch log mode, if not using logd and syslog, use printf */
// #define ANDROID    // logd
// #define SYSLOG     // syslog

#if defined(ANDROID)
/********** Start ANDROID **********/
#include <utils/Log.h>
#include <common_log.h>
#include <android/log.h>
#include <cutils/properties.h>
/** adb log */
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "ZipVerify: "

#ifdef LOG_NDDEBUG
#undef LOG_NDDEBUG
#endif
#define LOG_NDDEBUG 0 //Define to enable LOGD

#ifdef LOG_NDEBUG
#undef LOG_NDEBUG
#endif
#define LOG_NDEBUG  0 //Define to enable LOGV
/**********End ANDROID **********/

#elif defined(SYSLOG)
/********** Start SYSLOG **********/
#include <syslog.h>
#define LOGI(fmt, ...) syslog(LOG_NOTICE,   fmt, ##__VA_ARGS__)
#define LOGV(fmt, ...) syslog(LOG_NOTICE,   fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) syslog(LOG_DEBUG,    fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) syslog(LOG_ERR,      fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) syslog(LOG_WARNING,  fmt, ##__VA_ARGS__)
/**********End SYSLOG **********/

#else
#define LOGI(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define LOGV(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) printf(fmt, ##__VA_ARGS__)
#endif

#define PROJECT_NAME "ZipVerify"

#define ZIPVERIFY_CHK_MSG(cond, err, loc, msg) \
do {\
    if ( cond ) {\
        LOGE("[%s][ERROR][%s:%d] ret: %d, msg: %s", PROJECT_NAME, __func__, __LINE__, err, msg);\
        ret = err;\
        goto loc; \
    }\
}while(0)

#define ZIPVERIFY_CHK_MSG_ARG(cond,err,loc,fmt,...) \
do {\
    if ( cond ) {\
        LOGE("[%s][ERROR][%s:%d] ret: %d,", PROJECT_NAME, __func__, __LINE__, err);\
        LOGE(fmt,  ##__VA_ARGS__); \
        ret = err;\
        goto loc; \
    }\
}while(0)

#define QLOG_WARN(fmt, ...) \
do {\
	LOGW("[%s][WARNING][%s:%d]" fmt, PROJECT_NAME, __func__, __LINE__, ##__VA_ARGS__); \
} while(0)

#define QLOG_DEBUG(fmt, ...) \
do {\
	LOGD("[%s][DEBUG][%s:%d]" fmt, PROJECT_NAME, __func__, __LINE__, ##__VA_ARGS__); \
} while(0)

#define QLOG_ERROR(fmt, ...) \
do {\
	LOGE("[%s][ERROR][%s:%d]" fmt, PROJECT_NAME, __func__, __LINE__, ##__VA_ARGS__); \
} while(0)

#define IN
#define OUT
#define IN_OUT

#endif
