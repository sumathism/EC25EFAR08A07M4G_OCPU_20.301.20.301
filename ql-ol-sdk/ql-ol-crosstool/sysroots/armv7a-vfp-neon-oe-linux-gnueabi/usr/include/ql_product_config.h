/**
 * @file        quectel_product_config.h
 * @brief       Quectel PRODUCT MACROs Define.
 * @author      Running.qian
 * @copyright   Copyright (c) 2017-2020 @ Quectel Wireless Solutions Co., Ltd.
 */

#ifndef QL_PRODUCT_CONFIG_H
#define QL_PRODUCT_CONFIG_H

/******************  QUECTEL PRODUCT MACRO, Priority Level: Low1  ***********************************///
//#if QL_G_PRODUCT_EC20C_CFA
/******************* ReDefine FUNCS MACRO  ********************************/
//    #undef    QL_G_FUNC_XXX
//    #define   QL_G_FUNC_XXX     1

/******************* SPECIFIC FUNCS MACRO  ********************************/
//    #define   QL_G_FUNC_YYY
//#endif


#if defined(QL_G_PRODUCT_EC20C_FA)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20CE_FA)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20CE_FD)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20C_FD)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20C_FT)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21C_FD)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20CFD_HIK)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20CE_FAG)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20C_FAG)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC20CE_FARG)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC20CE_FRG)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC20-CE-FRG"
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC20CE_FILG)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC20-CE-FILG"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20CE_FDLG)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC20-CE-FDLG"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20CE_FALG)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC20-CE-FALG"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20CE_FHLG)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC20-CE-FHLG"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20CE_HHLG)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC20-CE"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC20CE_HKLG)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC20-CE"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EM05CN_FD)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EM05-CN"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20CE_FRSG)
/******************* ReDefine FUNCS MACRO  ********************************/
#define QUECTEL_SDIO_CLK_RATE_REGULATE 1 /*Harry 20230915 AT+QCFG="SDIO_CLK" command to set the sdio clk for EC20CE_FRSG. Change the throughput of wifi.*/
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC20CE_FDG)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20CE_FDG_CTVT)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20C_FDG)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20C_FTG)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20C_FTB)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21C_FDG)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20CFDG_HIK)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC25EM)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-EM"
	#define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE

/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC21KL_KR)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0121
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC21-KL"
    
    #undef QUECTEL_LGU_AT_CMD
    #define QUECTEL_LGU_AT_CMD
    #undef QUECTEL_KR_AT_CMD
    #define QUECTEL_KR_AT_CMD

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25E_KR)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-E"
    
    #undef QUECTEL_LGU_AT_CMD
    #define QUECTEL_LGU_AT_CMD
    #undef QUECTEL_KR_AT_CMD
    #define QUECTEL_KR_AT_CMD

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25E)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-E"
    #define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE
/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25E_EMBMS)
    #define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EG91EX_GB)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0191
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG91-EX-GB"
    #define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC21E)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0121
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC21-E"
/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21E_FB)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21AUT)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef  QL_G_FUNC_OMADM
    #define QL_G_FUNC_OMADM   1

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21AUTL)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef  QL_G_FUNC_OMADM
    #define QL_G_FUNC_OMADM   1

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25AUT)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef  QL_G_FUNC_OMADM
    #define QL_G_FUNC_OMADM   1

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25AUTL)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef  QL_G_FUNC_OMADM
    #define QL_G_FUNC_OMADM   1

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25AU)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef  QL_G_FUNC_OMADM
    #define QL_G_FUNC_OMADM   1
	
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-AU"

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21AU)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC21AU"

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21AUV)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef  QL_G_FUNC_OMADM
    #define QL_G_FUNC_OMADM   1

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21A)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef  QL_G_FUNC_OMADM
    #define QL_G_FUNC_OMADM   1

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25A)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef  QL_G_FUNC_OMADM
    #define QL_G_FUNC_OMADM   1

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21AL)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef  QL_G_FUNC_OMADM
    #define QL_G_FUNC_OMADM   1

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21AS)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef  QL_G_FUNC_OMADM
    #define QL_G_FUNC_OMADM   1

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21V)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef  QL_G_FUNC_OMADM
    #define QL_G_FUNC_OMADM   1

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21VD)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef  QL_G_FUNC_OMADM
    #define QL_G_FUNC_OMADM   1

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25VD)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef  QL_G_FUNC_OMADM
    #define QL_G_FUNC_OMADM   1


/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25V)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef  QL_G_FUNC_OMADM
    #define QL_G_FUNC_OMADM   1

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20C_GW)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20CE_GW)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21CT)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21KL)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0121
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC21-KL"
    
    #undef QUECTEL_LGU_AT_CMD
    #define QUECTEL_LGU_AT_CMD
    #undef QUECTEL_KR_AT_CMD
    #define QUECTEL_KR_AT_CMD
/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25J)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21J)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20CE_CT)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EG91E)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EG95E)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EG91EB)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EG92EU)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0191
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG92-EU"

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EG92EU_DP)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0191
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG92-EU"

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EG95EB)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0195
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG95-E"
	#define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EG91NA)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EG95NA)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21NA)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EM05E)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EM05CE)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25J_SB)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21J_SB)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25M)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25E_SKT)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25E_GB)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25AU_GB)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25EU_GB)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-EUGB"
	#define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC25EU)
    #define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21EU)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0121
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC21-EU"
	#define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE
/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21AU_GB)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21AUV_GB)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC21KL_KT)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25J_DCM)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20CE_FDNG)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25J_SS)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25E_LGU)
/******************* ReDefine FUNCS MACRO  ********************************/

/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC25EUX)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-EUX"
    #define QUECTEL_1A1_FOTA
    #define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE

/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC25EUX_BDA)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-EUX"
    #define QUECTEL_1A1_FOTA

/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC21EUX)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0121
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC21-EUX"
    #define QUECTEL_1A1_FOTA
    #define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE
    #define QUECTEL_SDIO_CLK_RATE_REGULATE 1 /*Harry 20230328 AT+QCFG="SDIO_CLK" command to set the sdio clk. Change the throughput of wifi.*/

/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC25EX)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-EX"
    #define QUECTEL_1A1_FOTA

/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined (CONFIG_PROJECT_EC25EX_GC)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-EX"
    #define QUECTEL_1A1_FOTA
    #define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC20CE_HDLG_CTVT)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC20-CE-HDLG-CTVT"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC20CE_HDLG)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC20-CE-HDLG"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC20CE_HCLG)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC20-CE-HCLG"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EG95AUX_GAD)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0195
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG95-AUX"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EG91AUX_GAD)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0191
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG91-AUX"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EG95AUX)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0195
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG95-AUX"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EG95AUX_DP)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0195
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG95-AUX"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EG91AUX)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0191
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG91-AUX"
	#define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC25AUX)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-AUX"
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC25AUX_GA)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-AUX"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC25AUX_GAA)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-AUX"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC25EM_GB)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-EM"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC25EM_GC)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-EM"
    #define QUECTEL_1A1_FOTA
    #define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC21AUX)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0121
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC21-AUX"
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC21AUX_GAD)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0121
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC21-AUX-GAD"
    #define QUECTEL_1A1_FOTA
#elif defined(QL_G_PRODUCT_EC21AUX_GAA)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0121
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC21-AUX-GAA"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC21AUX_GA)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0121
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC21-AUX"
    #define QUECTEL_1A1_FOTA
	#define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC25MX)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-MX"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC25AFX)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-AFX"
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EG95EX)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0195
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG95-EX"
    #define QUECTEL_1A1_FOTA
    #define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EG95EX_GAA)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0195
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG95-EX"
    #define QUECTEL_1A1_FOTA
    #define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EG91EX_GAA)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0191
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG91-EX-GAA"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EG91EX)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0191
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG91-EX"
    #define QUECTEL_1A1_FOTA
    #define QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EG91EX_REL)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0191
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG91-EX"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EG91_JP)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0191
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG91-JP"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EG95_JP)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0195
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG95-JP"
    #define QUECTEL_1A1_FOTA
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EG95NAX)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0195
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG95-NAX"
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EG91NAX)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0191
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG91-NAX"
/******************* SPECIFIC FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EG91VX)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0191
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EG91-VX"
/******************* ReDefine FUNCS MACRO  ********************************/
#elif defined(QL_G_PRODUCT_EC25EUC_TP)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-EUC"
/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC25EUC)
/******************* ReDefine FUNCS MACRO  ********************************/
    #undef USB_PID
    #undef USB_VID
    #undef USB_MANUFAC
    #undef USB_PRODUCT
    #define USB_PID 0x0125
    #define USB_VID 0x2C7C
    #define USB_MANUFAC "Quectel"
    #define USB_PRODUCT "EC25-EUC"
/******************* SPECIFIC FUNCS MACRO  ********************************/

#elif defined(QL_G_PRODUCT_EC20CE_FASG)
/******************* ReDefine FUNCS MACRO  ********************************/
#define QUECTEL_SDIO_CLK_RATE_REGULATE 1 /*Harry 20230911 AT+QCFG="SDIO_CLK" command to set the sdio clk. Change the throughput of wifi.*/
/******************* SPECIFIC FUNCS MACRO  ********************************/

#else //error
//#error "<<<<<<<<<<<<<<<<<<Undefine PRODUCT NAME>>>>>>>>>>>>>>>>>>>>>>>>>>>"
#endif

#endif //#ifdef  QL_G_PRODUCT_CONFIG_H
