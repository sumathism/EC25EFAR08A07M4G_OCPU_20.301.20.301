/**
 *@file     ql_voice_cfg.h
 *@date     2019-05-05
 *@author   charles <charles.sun@quectel.com>
*/

#ifndef __QL_VOICE_CFG_H__
#define __QL_VOICE_CFG_H__

typedef void (*vcfg_ind_cb)(int mode);
#define	QL_TTY_MODE_FULL 0
#define	QL_TTY_MODE_VCO 1
#define	QL_TTY_MODE_HCO 2
#define	QL_TTY_MODE_OFF 3
#define QL_TTY_MODE_UNKOWN 4
/*
* Function:     Init Function
* 
* Description:
*               QMI Client Init
*
* Parameters:
*               NULL 
* Return:        
*               TURE or FALSE
*/
int QL_Voice_Config_Init();

/*
* Function:     set tty mode
* 
* Description:
*               • TTY_MODE_FULL (0) – Full
*               • TTY_MODE_VCO (1) – Voice carry over
*               • TTY_MODE_HCO (2) – Hearing carry over
*               • TTY_MODE_OFF (3) – Off
*
* Parameters:
*               int
* Return:        
*               TURE or FALSE
*/
int QL_Voice_Config_Set(int tty_mode);

/*
* Function:     tty mode ind
* 
* Parameters:
*               callback 
* Return:        
*               NULL
*/
void QL_Voice_Config_Ind(vcfg_ind_cb usr_cb);
#endif
