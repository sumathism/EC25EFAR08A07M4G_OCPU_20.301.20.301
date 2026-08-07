/**
 * @file        quectel_leagcy_config.h
 * @brief       Quectel Leagcy MACROs Define.
 * @author      Running.qian
 * @copyright   Copyright (c) 2017-2020 @ Quectel Wireless Solutions Co., Ltd.
 */

#ifndef QUECTEL_LEAGCY_CONFIG_H
#define QUECTEL_LEAGCY_CONFIG_H

#if defined (__QUECTEL_PROJECT_EC25EU_GB__)
    #define __QUECTEL_PROJECT_EC25EU__
#endif
//ec25eu project add at+qringback at command,control play local ringback or send urc(+qringbackMode: mode) 
#if defined (__QUECTEL_PROJECT_EC25EU__) && ! defined (__QUECTEL_PROJECT_EC25EU_GB__)
#define QUECTEL_SEND_RINGBACK_URC
#endif

#if defined (__QUECTEL_PROJECT_EC20CE_CT__)
    #define QUECTEL_CTCC_SUPPORT
    #define QUECTEL_CTCC_PRIVATE
#endif

#if defined(__QUECTEL_PROJECT_EM05E__)
    #define QUECTEL_EC05_FEATURE_SUPPORT  //open the EM05 project features
#endif

#if defined(__QUECTEL_PROJECT_EM05CE__)
    #define QUECTEL_EC05_FEATURE_SUPPORT  //open the EM05 project features
#endif

#if defined (__QUECTEL_PROJECT_EC25J_SBK__) || defined (__QUECTEL_PROJECT_EC21J_SBK__)
    #define QUECTEL_SBM_FOTA
#endif

#if defined(__QUECTEL_PROJECT_EC20CE_FDG_CTVT__)
#define __QUECTEL_PROJECT_EC20CE_FDG__
#endif

#if defined(__QUECTEL_PROJECT_EG91AUX_GAD__)
#define __QUECTEL_PROJECT_EG91AUX__
#endif

#if defined(__QUECTEL_PROJECT_EG95AUX_DP__)
#define __QUECTEL_PROJECT_EG95AUX__
#endif

#if defined(__QUECTEL_PROJECT_EG95AUX_GAD__)
#define __QUECTEL_PROJECT_EG95AUX__
#endif

#if defined(__QUECTEL_PROJECT_EC20CE_FASG_CTVT__)
#define __QUECTEL_PROJECT_EC20CE_FASG__
#endif
 
#if defined(__QUECTEL_PROJECT_EC20CE_FASG_HW__)
#define __QUECTEL_PROJECT_EC20CE_FASG__
#endif   

#if defined(__QUECTEL_PROJECT_EC25E_EMBMS__)
#define __QUECTEL_PROJECT_EC25E__
#define CONFIG_PROJECT_EC25E
#define QL_G_PRODUCT_EC25E    1
#endif 

#if defined(__QUECTEL_PROJECT_EC25EM__)
#define __QUECTEL_PROJECT_EC25E__
#define CONFIG_PROJECT_EC25E
#define QL_G_PRODUCT_EC25E    1
#define QL_G_PRODUCT_EC25EM    1
#define QUECTEL_RINGBACK_GAIN_DEBUG
#define QUECTEL_SLIC_EVENT_REPORT_SUPPORT
#define QUECTEL_SLIC_FLASH_AND_SLICAT_CUSTOM_MADE //bolian costom made flash key time and at+qslic=1,1 function
#endif

#if defined(__QUECTEL_PROJECT_EC25E_KR__)
#define __QUECTEL_PROJECT_EC25E__
#define CONFIG_PROJECT_EC25E
#define QL_G_PRODUCT_EC25E    1
#define QUECTEL_KR_FLAG 
#endif 

#if defined(__QUECTEL_PROJECT_EC21KL__) || defined (__QUECTEL_PROJECT_EC21KL_KR__)
#define QUECTEL_KR_FLAG 
#endif 

#if defined(__QUECTEL_PROJECT_EC20CE_FASG_OCL__)
#define __QUECTEL_PROJECT_EC20CE_FASG__
#endif   
#ifdef QUECTEL_FEATURE_OPENLINUX
    #define QUECTEL_OPEN_LINUX_SUPPORT       1
#else
    #define QUECTEL_OPEN_LINUX_SUPPORT       0
#endif
#if defined (__QUECTEL_PROJECT_EC20CE_FARSG__)  
     #define CONFIG_PROJECT_EC20CE_FARSG
     #define CONFIG_PROJECT_EC20CE_FASG	
     #define __QUECTEL_PROJECT_EC20CE_FASG__
     #define CONFIG_NO_MODEM_PARTITION
     //#define QUEC_AP_STATE_TO_MODEM  //Chaim.qian@20200407 send usb state to modem 
#endif

#if defined (__QUECTEL_PROJECT_EC25EB_GB__)
	#define CONFIG_PROJECT_EC25EB_GB
	#define CONFIG_NO_MODEM_PARTITION
#endif
#if defined(__QUECTEL_PROJECT_EC25EUC__)
#define CONFIG_PROJECT_EC25EUC
#define QL_G_PRODUCT_EC25EUC    1
#endif
 
#if defined(__QUECTEL_PROJECT_EC25EUC__TP)
#define CONFIG_PROJECT_EC25EUC_TP
#define QL_G_PRODUCT_EC25EUC_TP    1
#endif 

/*** OMA-DM Features Macro Area ***/

//#define QUECTEL_OMA_DM //Running.qian-2018-04-03, coment this macro for disabling OMA-DM feature in the R06 Branch.
#ifdef QUECTEL_OMA_DM
#if defined(__QUECTEL_PROJECT_EC21V__) || defined(__QUECTEL_PROJECT_EC21NA__) || defined(__QUECTEL_PROJECT_EC25NA__)
#define QUECTEL_ODM_UPDATE_AUTO
#endif

#define QUECTEL_ODM_UPDATE_AUTOREPORT     1
#endif//QUECTEL_OMA_DM


/******quectel modem rootfs backup restore function**********/
#if defined (__QUECTEL_PROJECT_EC20CE_FAG__) || defined (__QUECTEL_PROJECT_EC20C_FAG__) || \
    defined (__QUECTEL_PROJECT_EC20CE_FDG__) || defined (__QUECTEL_PROJECT_EC20C_FDG__) || \
    defined (__QUECTEL_PROJECT_EC20C_FTG__) ||  defined (__QUECTEL_PROJECT_EC21C_FDG__) || \
    defined (__QUECTEL_PROJECT_EC20CFDG_HIK__) || defined(__QUECTEL_PROJECT_EC25M__) || \
    defined (__QUECTEL_PROJECT_EG91EB__) || defined (__QUECTEL_PROJECT_EG95EB__) || defined (__QUECTEL_PROJECT_EG92EU__) || \
    defined (__QUECTEL_PROJECT_EC21E_FB__) || defined (__QUECTEL_PROJECT_EC20CE_FARG__) ||\
    defined (__QUECTEL_PROJECT_EC20CE_FRG__) || defined (__QUECTEL_PROJECT_EG92EU_DP__) || \
    defined (__QUECTEL_PROJECT_EC21E__) || defined (__QUECTEL_PROJECT_EC25E__) || \
    defined (__QUECTEL_PROJECT_EC21AU__) || defined (__QUECTEL_PROJECT_EC25AU__) || \
    defined (__QUECTEL_PROJECT_EC25E_GB__) || defined (__QUECTEL_PROJECT_EC25AU_GB__) || \
    defined (__QUECTEL_PROJECT_EC21AUT__) || defined (__QUECTEL_PROJECT_EC25AUT__) || \
    defined (__QUECTEL_PROJECT_EC21EU__) || defined (__QUECTEL_PROJECT_EC25EU__) || \
    defined (__QUECTEL_PROJECT_EC21A_GB__) || defined (__QUECTEL_PROJECT_EC25A_GB_) || \
    defined (__QUECTEL_PROJECT_EC21AU_GB__) || defined (__QUECTEL_PROJECT_EC25AU_GB__) || \
    defined (__QUECTEL_PROJECT_EC21AUV_GB__) || defined (__QUECTEL_PROJECT_EC21KL_KT__) || \
    defined (__QUECTEL_PROJECT_EC20C_FTB__)  || defined (__QUECTEL_PROJECT_EC25J_DCM__) || \
    defined (__QUECTEL_PROJECT_EC21AUV__) || defined (__QUECTEL_PROJECT_EC21J__) || \
    defined (__QUECTEL_PROJECT_EC25J__) || defined (__QUECTEL_PROJECT_EC21KL_SKT__) || \
    defined (__QUECTEL_PROJECT_EC25E_LGU__) || defined (__QUECTEL_PROJECT_EC25E_SKT__) || \
	defined (__QUECTEL_PROJECT_EC25E_KR__) || defined (__QUECTEL_PROJECT_EC21KL_KR__) || defined (__QUECTEL_PROJECT_EC21KL__)|| \
    defined (__QUECTEL_PROJECT_EM05E__) || defined (__QUECTEL_PROJECT_EM05CE__)|| \
    defined (__QUECTEL_PROJECT_EC25J_SS__) || defined (__QUECTEL_PROJECT_EG91EX_GB__) || \
    defined (__QUECTEL_PROJECT_EC25EX_GC__) 
    #define QUECTEL_MODEM_RESTORE_UBI
#endif

/************************************************************
 Ramos.zhang-20180507, EC20CEFDNG  2+2 MCP, no modem backup, use QUECTEL_MODEM_RESTORE_UBI process.
 this process will do nothing when set modem backup flag if there is no modem backup  partition.
************************************************************/
#if defined (__QUECTEL_PROJECT_EC20CE_FDNG__) 
    #define QUECTEL_MODEM_RESTORE_UBI
#endif

#ifdef __QUECTEL_PROJECT_EG92EU_DP__
#define QUECTEL_AT_CODEC_INIT_SUPPORT //add colin support codec init 
#define QUECTEL_SEND_RINGBACK_URC
/* lucus.lin 20250924 Add xtwifi control to solve the timer period wake up module for the "Loc" thread during sleep. */
#define QUECTEL_AT_XTWIFI_CONTROL
#endif

#define QUECTEL_THERMAL //maxcodeflag20172106
#define QUECTEL_AT_SOFTSIM_SWITCH   // Wayne.wei-2018/4/4:add an AT command as switch to control the start and stop of the process of Linux-side softsim.

#define QUECTEL_MBIM_FEATURE
#ifdef QUECTEL_MBIM_FEATURE
    #define QUECTEL_MBIM_LOG
#endif
#define QUECTEL_CONTROL_ICCID_DISPLAY //lory2017/05/31 fix QC3655 if ICCID include hex A to F ,cannot display content after that
#define QUECTEL_USB_MODE_SUPPORT //ford 2017.08.28 add for usbmode change URC
//#define QUECTEL_EC05_FEATURE_SUPPORT  //ford 2017.09.28 add for EM05 Feature

#define QUECTEL_AT_AP_MODEM_NEW_VERSION //  2018/11/08 sync AP version to modem   
#define QUECTEL_SBL_PROTECTION

#define QUECTEL_AT_Q_VERSION //2017.12.04 add command AT+QVERSION by wayne.wei

#define QUECTEL_QC_AUTOCONNECT_CTL //add by maxTANG to make qcautoconnect more reasonable //maxcodeflag20170929

//#define QUECTEL_QCFG_SBFOTA             //Navy 2017/07/18 for enable/disable sbfota polling

#if defined (__QUECTEL_PROJECT_EC25AUTL__)
#define QUECTEL_PDC_MBN_UPDATE
#endif

#if defined (__QUECTEL_PROJECT_EC20CE_HCLG__) || defined (__QUECTEL_PROJECT_EC20CE_HDLG__ ) || defined (__QUECTEL_PROJECT_EC20CE_HKLG__ ) || \
    defined (__QUECTEL_PROJECT_EC20CE_FILG__) || defined (__QUECTEL_PROJECT_EC20CE_FALG__ ) || defined (__QUECTEL_PROJECT_EC20CE_FDLG__ ) || \
    defined (__QUECTEL_PROJECT_EC20CE_FHLG__) || defined (__QUECTEL_PROJECT_EC20CE_HHLG__ ) || defined (__QUECTEL_PROJECT_EM05CN_FD__ ) 
#define  QUECTEL_MBIM_CLOSE_CONTROL
#endif 

#if defined (__QUECTEL_PROJECT_EG91EX_GAA__)
#define  __QUECTEL_PROJECT_EG91EX__
#endif

#if defined (__QUECTEL_PROJECT_EG91EX_REL__)
#define  __QUECTEL_PROJECT_EG91EX__
#endif

// Auberon.li-20190306, porting VoLTE ring tone for telstra
#if defined (__QUECTEL_PROJECT_EC21AUX_GAA__) || defined (__QUECTEL_PROJECT_EG91EX_GAA__) || defined (__QUECTEL_PROJECT_EC21AUX_GAD__)  || \
    defined (__QUECTEL_PROJECT_EG91AUX_GAD__) || defined (__QUECTEL_PROJECT_EG95AUX_GAD__) || defined (__QUECTEL_PROJECT_EG95EX_GAA__) || defined (__QUECTEL_PROJECT_EC25AUX_GAA__)
#define QUECTEL_AUDIO_TONE_TELSTRA
#endif

#define QUECTEL_RMNET_OPTIMIZATION

#ifdef QUECTEL_RMNET_OPTIMIZATION
#define QUECTEL_MODEM_RDY_CHECK //maxcodeflag20171121
#define QUECTEL_MEND_DTR_QCRMCALL //haroldcodeflag20190520
//when modem reset, QTI will detect it and be restarted by quectel monitor process
#define QUECTEL_QTI_RECOVERY_SELF //maxcodeflag20191002
#endif

#define QUECTEL_AP_PROCESS_READY_URC  //add by MaxTANG to generate URC to modem so that MCU can detect if some processes in AP is ready

#define QUEC_FWD_HTTP_FEATURE //domingo.deng@2020.01.06 support http tranfer in ap side

#if defined (__QUECTEL_PROJECT_EG91EX__) || defined (__QUECTEL_PROJECT_EG91AUX__)
    #define QUECTEL_URC_WITH_LEN_SUPPORT //<2023/11/26-[FAE-95543][Harry.gong] Transplant changelist <121218 121453> for EG91X and EG91AUX
    #define QUECTEL_HTTPS_WITH_USER_NAME_AND_PASSWORD_SUPPORT //<2023/11/26-[FAE-95543][Harry.gong] Transplant changelist <110955 111006 113644 113678 > for EG91X and EG91AUX.
#endif

#if defined (__QUECTEL_PROJECT_EC25EUX_CZZ__)
#define __QUECTEL_PROJECT_EC25EUX__
#define QL_G_PRODUCT_EC25EUX
#define CONFIG_PROJECT_EC25EUX
#endif

#if defined (__QUECTEL_PROJECT_EC25EUX_BDA__)
#define __QUECTEL_PROJECT_EC25EUX__
#define QL_G_PRODUCT_EC25EUX
#define CONFIG_PROJECT_EC25EUX
#endif


#if defined (__QUECTEL_PROJECT_EM05CN_FDMBIM__)
#define QL_G_PRODUCT_EM05CN_FD
#define __QUECTEL_PROJECT_EM05CN_FD__
#endif

#define QUECTEL_SECCFG_CONTROL_FEATURE   //SECCFG CONTROL

#if defined (QUECTEL_FEATURE_OPENLINUX)
#define QUECTEL_OPEN_REDDA
#endif

#endif //ifdef QUECTEL_LEAGCY_CONFIG_H
