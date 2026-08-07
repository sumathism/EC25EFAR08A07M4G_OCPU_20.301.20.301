/*
 * es8311.c  --  ES8311/ES8312 ALSA SoC Audio Codec
 *
 * Copyright (C) 2018 Everest Semiconductor Co., Ltd
 *
 * Authors:  David Yang(yangxiaohua@everest-semi.com)
 *
 *
 * Based on es8374.c by David Yang(yangxiaohua@everest-semi.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

//#include "es8311_balong.h"
#include "es8311.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/regmap.h>
#include <linux/stddef.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/tlv.h>
#include <sound/soc.h>
#include <sound/initval.h>

struct sp_config {
	u8 spc, mmcc, spfs;
	u32 srate;
	u8 lrcdiv;
	u8 sclkdiv;
};

/* codec private data */
struct	es8311_private {
	struct snd_soc_codec *codec;
	struct regmap *regmap;
	u32 mclk;
	bool sclkinv;
	bool mclkinv;
	bool dmic_enable;
};

struct es8311_eq_init_reg {
	u8 reg;
	u16 val;
};

static const struct es8311_eq_init_reg es8311_eqreg_defaults[]= {
{0x17,0xBF},
{0x32,0x8F},
{0x1C,0x65},
{0x1D,0x01},
{0x1E,0x32},
{0x1F,0x71},
{0x20,0xFF},
{0x21,0x00},
{0x22,0x09},
{0x23,0x0E},
{0x24,0xE8},
{0x25,0x20},
{0x26,0x3c},
{0x27,0x2F},
{0x28,0xCF},
{0x29,0x21},
{0x2A,0x01},
{0x2B,0x4D},
{0x2C,0x5B},
{0x2D,0x01},
{0x2E,0x32},
{0x2F,0x71},
{0x30,0xFF},
{0x1C,0x25},
{0x1B,0x04},
};

#define ES8311_EQ_REG_LEN ARRAY_SIZE(es8311_eqreg_defaults)
static int es8311_eq_reg_init(struct snd_soc_codec *codec)
{
    int i;
    for(i=0;i < ES8311_EQ_REG_LEN;i++) {
        snd_soc_write(codec,es8311_eqreg_defaults[i].reg,es8311_eqreg_defaults[i].val);
	if(i%3==0){
            msleep(30);
        }
 }
}

static u8 SR_val;
static const DECLARE_TLV_DB_SCALE(vdac_tlv, 
				-9550, 50, true);
static const DECLARE_TLV_DB_SCALE(vadc_tlv, 
				-9550, 50, true);
static const DECLARE_TLV_DB_SCALE(mic_pga_tlv, 
				0, 300, true);
static const DECLARE_TLV_DB_SCALE(adc_scale_tlv, 
				0, 600, false);
static const DECLARE_TLV_DB_SCALE(alc_winsize_tlv, 
				0, 25, false);
static const DECLARE_TLV_DB_SCALE(alc_maxlevel_tlv, 
				-3600, 200, false);
static const DECLARE_TLV_DB_SCALE(alc_minlevel_tlv, 
				-3600, 200, false);
static const DECLARE_TLV_DB_SCALE(alc_noisegate_tlv, 
				-9600, 600, false);
static const DECLARE_TLV_DB_SCALE(alc_noisegate_winsize_tlv, 
				4200, 4200, false);
static const DECLARE_TLV_DB_SCALE(alc_automute_gain_tlv, 
				4200, 4200, false);
static const DECLARE_TLV_DB_SCALE(adc_ramprate_tlv, 
				0, 25, false);

static const char * const dmic_type_txt[] = {
	"dmic at high level",
	"dmic at low level"
};
static const struct soc_enum dmic_type =
SOC_ENUM_SINGLE(ES8311_ADC_REG15, 0, 1, dmic_type_txt);

static const char * const automute_type_txt[] = {
	"automute disabled",
	"automute enable"
};
static const struct soc_enum alc_automute_type =
SOC_ENUM_SINGLE(ES8311_ADC_REG18, 6, 1, automute_type_txt);

static const char * const dacdsm_mute_type_txt[] = {
	"mute to 8",
	"mute to 7/9"
};
static const struct soc_enum dacdsm_mute_type =
SOC_ENUM_SINGLE(ES8311_DAC_REG31, 7, 1, dacdsm_mute_type_txt);

static const char * const aec_type_txt[] = {
	"adc left, adc right",
	"adc left, null right",
	"null left, adc right",
	"null left, null right",
	"dac left, adc right",
	"adc left, dac right",
	"dac left, dac right",
	"N/A"
};
static const struct soc_enum aec_type =
SOC_ENUM_SINGLE(ES8311_GPIO_REG44, 4, 7, aec_type_txt);

static const char * const adc2dac_sel_txt[] = {
	"disable",
	"adc data to dac",
};
static const struct soc_enum adc2dac_sel =
SOC_ENUM_SINGLE(ES8311_GPIO_REG44, 7, 1, adc2dac_sel_txt);

static const char * const mclk_sel_txt[] = {
	"from mclk pin",
	"from bclk",
};
static const struct soc_enum mclk_src =
SOC_ENUM_SINGLE(ES8311_CLK_MANAGER_REG01, 7, 1, mclk_sel_txt);

/*
 * es8311 Controls
 */
static const struct snd_kcontrol_new es8311_snd_controls[] = {
	SOC_SINGLE_TLV("MIC PGA GAIN", ES8311_SYSTEM_REG14, 
			0, 10, 0, mic_pga_tlv),
	SOC_SINGLE_TLV("ADC SCALE", ES8311_ADC_REG16, 
			0, 7, 0, adc_scale_tlv),
	SOC_ENUM("DMIC TYPE", dmic_type),
	SOC_SINGLE_TLV("ADC RAMP RATE", ES8311_ADC_REG15, 
			4, 15, 0, adc_ramprate_tlv),
	SOC_SINGLE("ADC SDP MUTE", ES8311_SDPOUT_REG0A, 6, 1, 0),
	SOC_SINGLE("ADC INVERTED", ES8311_ADC_REG16, 4, 1, 0),
	SOC_SINGLE("ADC SYNC", ES8311_ADC_REG16, 5, 1, 1),
	SOC_SINGLE("ADC RAM CLR", ES8311_ADC_REG16, 3, 1, 0),
	SOC_SINGLE_TLV("ADC VOLUME", ES8311_ADC_REG17, 
			0, 255, 0, vadc_tlv),
	SOC_SINGLE("ALC ENABLE", ES8311_ADC_REG18, 7, 1, 0),
	SOC_ENUM("ALC AUTOMUTE TYPE", alc_automute_type),
	SOC_SINGLE_TLV("ALC WIN SIZE", ES8311_ADC_REG18, 
			0, 15, 0, alc_winsize_tlv),
	SOC_SINGLE_TLV("ALC MAX LEVEL", ES8311_ADC_REG19, 
			4, 15, 0, alc_maxlevel_tlv),
	SOC_SINGLE_TLV("ALC MIN LEVEL", ES8311_ADC_REG19, 
			0, 15, 0, alc_minlevel_tlv),
	SOC_SINGLE_TLV("ALC AUTOMUTE WINSIZE", ES8311_ADC_REG1A, 
			4, 15, 0, alc_noisegate_winsize_tlv),
	SOC_SINGLE_TLV("ALC AUTOMUTE GATE THRESHOLD", ES8311_ADC_REG1A, 
			0, 15, 0, alc_noisegate_tlv),
	SOC_SINGLE_TLV("ALC AUTOMUTE VOLUME", ES8311_ADC_REG1B, 
			5, 7, 0, alc_automute_gain_tlv),
	SOC_SINGLE("ADC FS MODE", ES8311_CLK_MANAGER_REG03, 6, 1, 0),
	SOC_SINGLE("ADC OSR", ES8311_CLK_MANAGER_REG03, 0, 63, 0),
	SOC_SINGLE("DAC SDP MUTE", ES8311_SDPIN_REG09, 6, 1, 0),
	SOC_SINGLE("DAC DEM  MUTE", ES8311_DAC_REG31, 5, 1, 0),
	SOC_SINGLE("DAC INVERT", ES8311_DAC_REG31, 4, 1, 0),
	SOC_SINGLE("DAC RAM CLR", ES8311_DAC_REG31, 3, 1, 0),
	SOC_ENUM("DAC DSM MUTE", dacdsm_mute_type),
	SOC_SINGLE("DAC OFFSET", ES8311_DAC_REG33, 0, 255, 0),
	SOC_SINGLE_TLV("DAC VOLUME", ES8311_DAC_REG32, 
			0, 255, 0, vdac_tlv),
	SOC_SINGLE("DRC ENABLE", ES8311_DAC_REG34, 7, 1, 0),
	SOC_SINGLE_TLV("DRC WIN SIZE",	ES8311_DAC_REG34, 
			0, 15, 0, alc_winsize_tlv),
	SOC_SINGLE_TLV("DRC MAX LEVEL",	ES8311_DAC_REG35, 
			4, 15, 0, alc_maxlevel_tlv),
	SOC_SINGLE_TLV("DRC MIN LEVEL",	ES8311_DAC_REG35, 
			0, 15, 0, alc_minlevel_tlv),
	SOC_SINGLE_TLV("DAC RAMP RATE",	ES8311_DAC_REG37, 
			4, 15, 0, adc_ramprate_tlv),
	SOC_SINGLE("DAC OSR", ES8311_CLK_MANAGER_REG04, 0, 127, 0),
	SOC_ENUM("AEC MODE", aec_type),
	SOC_ENUM("ADC DATA TO DAC TEST MODE", adc2dac_sel),
	SOC_SINGLE("MCLK INVERT", ES8311_CLK_MANAGER_REG01, 6, 1, 0),
	SOC_SINGLE("BCLK INVERT", ES8311_CLK_MANAGER_REG06, 5, 1, 0),
	SOC_ENUM("MCLK SOURCE", mclk_src),
};

/*
 * DAPM Controls
 */
static const char * const es8311_dmic_mux_txt[] = {
	"DMIC DISABLE",
	"DMIC ENABLE"
};
static const unsigned int es8311_dmic_mux_values[] = {
	0, 1
};
static const struct soc_enum es8311_dmic_mux_enum =
	SOC_VALUE_ENUM_SINGLE(ES8311_SYSTEM_REG14, 6, 1,
		ARRAY_SIZE(es8311_dmic_mux_txt),
		es8311_dmic_mux_txt,
		es8311_dmic_mux_values);
static const struct snd_kcontrol_new es8311_dmic_mux_controls =
	SOC_DAPM_ENUM("DMIC ROUTE", es8311_dmic_mux_enum);

static const char * const es8311_adc_sdp_mux_txt[] = {
	"FROM ADC OUT",
	"FROM EQUALIZER",
};
static const unsigned int es8311_adc_sdp_mux_values[] = {
	0, 1
};
static const struct soc_enum es8311_adc_sdp_mux_enum =
	SOC_VALUE_ENUM_SINGLE(ES8311_ADC_REG1C, 6, 1,
		ARRAY_SIZE(es8311_adc_sdp_mux_txt),
		es8311_adc_sdp_mux_txt,
		es8311_adc_sdp_mux_values);
static const struct snd_kcontrol_new es8311_adc_sdp_mux_controls =
	SOC_DAPM_ENUM("ADC SDP ROUTE", es8311_adc_sdp_mux_enum);

/*
* DAC data  soure
*/
static const char * const es8311_dac_data_mux_txt[] = {
	"SELECT SDP LEFT DATA",
	"SELECT SDP RIGHT DATA",
};
static const unsigned int  es8311_dac_data_mux_values[] = {
	0, 1
};
static const struct soc_enum  es8311_dac_data_mux_enum =
	SOC_VALUE_ENUM_SINGLE(ES8311_SDPIN_REG09, 7, 1,
		ARRAY_SIZE(es8311_dac_data_mux_txt),
		es8311_dac_data_mux_txt,
		es8311_dac_data_mux_values);
static const struct snd_kcontrol_new  es8311_dac_data_mux_controls =
	SOC_DAPM_ENUM("DAC SDP ROUTE", es8311_dac_data_mux_enum);

struct _coeff_div {
	u32 mclk;       /* mclk frequency */
	u32 rate;       /* sample rate */
	u8 prediv;      /* the pre divider with range from 1 to 8 */
	u8 premulti;    /* the pre multiplier with x1, x2, x4 and x8 selection */
	u8 adcdiv;      /* adcclk divider */
	u8 dacdiv;      /* dacclk divider */
	u8 fsmode;      /* double speed or single speed, =0, ss, =1, ds */
	u8 lrck_h;      /* adclrck divider and daclrck divider */
	u8 lrck_l;
	u8 bclkdiv;     /* sclk divider */
	u8 adcosr;      /* adc osr */
	u8 dacosr;      /* dac osr */
	u8 adcscale;
};

/* codec hifi mclk clock divider coefficients */
static const struct _coeff_div coeff_div[] = {
	//mclk     rate   prediv  mult  adcdiv dacdiv fsmode lrch  lrcl  bckdiv osr  adcscale
	/* 8k */
	{26000000, 8000 , 0x05, 0x08, 0x0D, 0x0D, 0x00, 0x0C, 0xB1, 0x04, 0x19, 0x19, 0x01},  	
	{25000000, 8000 , 0x06, 0x01, 0x02, 0x02, 0x00, 0x0C, 0x34, 0x02, 0x19, 0x19, 0x04},  	
	{24000000, 8000 , 0x05, 0x02, 0x03, 0x03, 0x00, 0x0B, 0xB7, 0x04, 0x19, 0x19, 0x01},  	
	{19200000, 8000 , 0x05, 0x02, 0x03, 0x03, 0x00, 0x09, 0x5f, 0x04, 0x10, 0x10, 0x02},  	
	{18432000, 8000 , 0x03, 0x02, 0x03, 0x03, 0x00, 0x05, 0xff, 0x18, 0x10, 0x10, 0x04},  	
	{16000000, 8000 , 0x05, 0x01, 0x01, 0x01, 0x00, 0x08, 0xff, 0x04, 0x19, 0x19, 0x01},  	
	{12288000, 8000 , 0x06, 0x01, 0x01, 0x01, 0x00, 0x05, 0xff, 0x04, 0x10, 0x10, 0x04},  	
	{12000000, 8000 , 0x05, 0x04, 0x03, 0x03, 0x00, 0x05, 0xDB, 0x04, 0x19, 0x19, 0x01},		
	{16384000, 8000 , 0x08, 0x01, 0x01, 0x01, 0x00, 0x07, 0xff, 0x04, 0x10, 0x10, 0x04},  	
	{8192000 , 8000 , 0x04, 0x01, 0x01, 0x01, 0x00, 0x03, 0xff, 0x04, 0x10, 0x10, 0x04},		
	{8000000 , 8000 , 0x01, 0x02, 0x05, 0x05, 0x00, 0x03, 0xE7, 0x04, 0x19, 0x19, 0x04},  		                                                                                
	{4096000 , 8000 , 0x02, 0x01, 0x01, 0x01, 0x00, 0x01, 0xff, 0x04, 0x10, 0x10, 0x04},  	
	{4000000 , 8000 , 0x01, 0x04, 0x05, 0x05, 0x00, 0x01, 0xF3, 0x02, 0x19, 0x19, 0x04},  	
	{3072000 , 8000 , 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x7f, 0x04, 0x10, 0x10, 0x04},  	
	{2048000 , 8000 , 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10, 0x04},  	
	{1536000 , 8000 , 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xBF, 0x04, 0x10, 0x10, 0x04},  	
	{1024000 , 8000 , 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x7f, 0x04, 0x10, 0x10, 0x04},		
	{6144000 , 8000 , 0x01, 0x02, 0x03, 0x02, 0x00, 0x02, 0xff, 0x04, 0x20, 0x60, 0x00}, 
	{19200000, 8000 , 0x03, 0x01, 0x02, 0x01, 0x00, 0x09, 0x5f, 0x14, 0x19, 0x64, 0x01},  
	{8192000 , 8000 , 0x01, 0x01, 0x02, 0x01, 0x00, 0x03, 0xff, 0x04, 0x20, 0x7F, 0x00}, 
	{512000  , 8000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0x3f, 0x04, 0x10, 0x10, 0x04},			                                                                                        	
	/* 11.025k */                                                                         	
	{24000000, 11025, 0x08, 0x01, 0x01, 0x01, 0x00, 0x08, 0x7f, 0x04, 0x11, 0x11, 0x03},  	
	{12000000, 11025, 0x04, 0x01, 0x01, 0x01, 0x00, 0x04, 0x3f, 0x04, 0x11, 0x11, 0x03},  	
	{11289600, 11025, 0x04, 0x01, 0x01, 0x01, 0x00, 0x03, 0xff, 0x04, 0x10, 0x10, 0x04},  	
	{5644800 , 11025, 0x02, 0x01, 0x01, 0x01, 0x00, 0x01, 0xff, 0x04, 0x10, 0x10, 0x04},  	
	{2822400 , 11025, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10, 0x04},  
	{1411200 , 11025, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x7f, 0x04, 0x10, 0x10, 0x04},  	
	/* 12k */	                                                                            	
	{24000000, 12000, 0x05, 0x01, 0x01, 0x01, 0x00, 0x07, 0xCf, 0x04, 0x19, 0x19, 0x01},  	
	{19200000, 12000, 0x01, 0x01, 0x04, 0x02, 0x00, 0x06, 0x3f, 0x14, 0x19, 0x64, 0x01}, 
	{12000000, 12000, 0x05, 0x04, 0x02, 0x02, 0x00, 0x03, 0xE7, 0x04, 0x19, 0x19, 0x01},		
	{12288000, 12000, 0x04, 0x01, 0x01, 0x01, 0x00, 0x03, 0xff, 0x04, 0x10, 0x10, 0x04},  	
  {6144000 , 12000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0xff, 0x04, 0x20, 0x40, 0x00}, 
	{6000000 , 12000, 0x01, 0x04, 0x05, 0x05, 0x00, 0x01, 0xF3, 0x04, 0x19, 0x19, 0x01},  	
	{3072000 , 12000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10, 0x04},  	
	{1536000 , 12000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x7f, 0x04, 0x10, 0x10, 0x04},  
	{8192000 , 12000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x02, 0xA9, 0x04, 0x2a, 0x55, 0x00},		
	/* 16k */                                                                             	
	{26000000, 16000, 0x05, 0x08, 0x06, 0x06, 0x00, 0x06, 0x58, 0x01, 0x1b, 0x1b, 0x01},  	
	{25000000, 16000, 0x06, 0x01, 0x01, 0x01, 0x00, 0x06, 0x19, 0x01, 0x10, 0x10, 0x04},  	
	{24000000, 16000, 0x05, 0x04, 0x03, 0x03, 0x00, 0x05, 0xDB, 0x04, 0x19, 0x19, 0x01},  	
	{6144000 , 16000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x7f, 0x04, 0x18, 0x30, 0x01}, 	
	{12288000, 16000, 0x03, 0x01, 0x01, 0x01, 0x00, 0x02, 0xff, 0x04, 0x10, 0x10, 0x04},  	
	{12000000, 16000, 0x05, 0x08, 0x03, 0x03, 0x00, 0x02, 0xed, 0x04, 0x19, 0x19, 0x01},		
	{18432000, 16000, 0x03, 0x02, 0x03, 0x03, 0x00, 0x04, 0x7f, 0x0c, 0x10, 0x10, 0x04},  	
	{16384000, 16000, 0x04, 0x01, 0x01, 0x01, 0x00, 0x03, 0xff, 0x04, 0x10, 0x10, 0x04},  
	{19200000, 16000, 0x03, 0x02, 0x02, 0x01, 0x00, 0x04, 0xaf, 0x0F, 0x19, 0x64, 0x01},  
	{8192000 , 16000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0xff, 0x04, 0x20, 0x40, 0x00},  
	{4096000 , 16000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10, 0x04},  	
	{3072000 , 16000, 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xbf, 0x04, 0x10, 0x10, 0x04},  	
	{2048000 , 16000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x7f, 0x04, 0x10, 0x10, 0x04},  	
	{1536000 , 16000, 0x03, 0x08, 0x01, 0x01, 0x00, 0x00, 0x5f, 0x04, 0x10, 0x10, 0x04},  	
	{1024000 , 16000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0x3f, 0x04, 0x10, 0x10, 0x04},		
	/* 22.05k */                                                                          	
	{24000000, 22050, 0x04, 0x01, 0x01, 0x01, 0x00, 0x04, 0x3f, 0x04, 0x11, 0x11, 0x03},  	
	{12000000, 22050, 0x02, 0x01, 0x01, 0x01, 0x00, 0x02, 0x1f, 0x04, 0x11, 0x11, 0x03},  	
	{11289600, 22050, 0x02, 0x01, 0x01, 0x01, 0x00, 0x01, 0xff, 0x04, 0x10, 0x10, 0x04},  	
	{5644800 , 22050, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10, 0x04},  	
	{2822400 , 22050, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x7f, 0x04, 0x10, 0x10, 0x04},  	
	{1411200 , 22050, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0x3f, 0x04, 0x10, 0x10, 0x04},  	
	/* 24k */                                                                             	
	{24000000, 24000, 0x05, 0x02, 0x01, 0x01, 0x00, 0x03, 0xe7, 0x04, 0x19, 0x19, 0x01},  	
 	{19200000, 24000, 0x02, 0x01, 0x01, 0x01, 0x00, 0x03, 0x1f, 0x0A, 0x19, 0x19, 0x01},  	
	{12288000, 24000, 0x02, 0x01, 0x01, 0x01, 0x00, 0x01, 0xff, 0x04, 0x10, 0x10, 0x04},  	
	{12000000, 24000, 0x05, 0x04, 0x01, 0x01, 0x00, 0x01, 0xf3, 0x04, 0x19, 0x19, 0x01},		
	{18432000, 24000, 0x03, 0x01, 0x01, 0x01, 0x00, 0x02, 0xff, 0x04, 0x10, 0x10, 0x04},  	
  {6144000 , 24000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x20, 0x40, 0x00},   
	{3072000 , 24000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x7f, 0x04, 0x10, 0x10, 0x04},  	
	{1536000 , 24000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0x3f, 0x04, 0x10, 0x10, 0x04},		
  {8192000 , 24000, 0x01, 0x02, 0x01, 0x02, 0x00, 0x01, 0x54, 0x04, 0x2a, 0x2a, 0x00},  	
	/* 32k */                                                                             	
	{24000000, 32000, 0x05, 0x08, 0x03, 0x03, 0x00, 0x02, 0xed, 0x04, 0x19, 0x19, 0x01},  	
	{26000000, 32000, 0x03, 0x01, 0x01, 0x01, 0x00, 0x03, 0x2b, 0x01, 0x11, 0x11, 0x04},  	
	{25000000, 32000, 0x03, 0x01, 0x01, 0x01, 0x00, 0x03, 0x0c, 0x01, 0x11, 0x11, 0x04},  
	{19200000, 32000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x02, 0x57, 0x05, 0x19, 0x19, 0x01},  
	{12288000, 32000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x01, 0x7f, 0x04, 0x10, 0x10, 0x04},  	
	{12000000, 32000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x76, 0x04, 0x18, 0x18, 0x02},		
	{18432000, 32000, 0x03, 0x04, 0x03, 0x03, 0x00, 0x02, 0x3f, 0x0c, 0x10, 0x10, 0x03},  	
	{16384000, 32000, 0x02, 0x01, 0x01, 0x01, 0x00, 0x01, 0xff, 0x04, 0x10, 0x10, 0x04},  	
	{6144000 , 32000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xbf, 0x04, 0x18, 0x30, 0x01}, 
	{8192000 , 32000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20, 0x04}, 
	{4096000 , 32000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x7f, 0x04, 0x10, 0x10, 0x04},  
	{3072000 , 32000, 0x03, 0x08, 0x01, 0x01, 0x00, 0x00, 0x5f, 0x04, 0x10, 0x10, 0x04},  	
	{2048000 , 32000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0x3f, 0x04, 0x10, 0x10, 0x04},  	
	{1536000 , 32000, 0x03, 0x08, 0x01, 0x01, 0x01, 0x00, 0x2f, 0x02, 0x10, 0x10, 0x04},  	
	{1024000 , 32000, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0x1f, 0x04, 0x10, 0x10, 0x04},  	
	/* 44.1k */                                                                           	
	{24000000, 44100, 0x02, 0x01, 0x01, 0x01, 0x00, 0x02, 0x1f, 0x04, 0x11, 0x11, 0x03},  	
	{12000000, 44100, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x0f, 0x04, 0x11, 0x11, 0x03},  	
	{11289600, 44100, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10, 0x04},  	
	{5644800 , 44100, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x7f, 0x04, 0x10, 0x10, 0x04},  	
	{2822400 , 44100, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0x3f, 0x04, 0x10, 0x10, 0x04},  	
	{1411200 , 44100, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0x1f, 0x04, 0x10, 0x10, 0x04},  	
	/* 48k */	                                                                            
	{25000000, 48000, 0x02, 0x01, 0x01, 0x01, 0x00, 0x02, 0x07, 0x01, 0x11, 0x11, 0x04},  	
	{24000000, 48000, 0x05, 0x04, 0x01, 0x01, 0x00, 0x01, 0xf3, 0x04, 0x19, 0x19, 0x01},  	
	{19200000, 48000, 0x05, 0x04, 0x01, 0x01, 0x00, 0x01, 0x8F, 0x04, 0x14, 0x14, 0x02},    
	{12288000, 48000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10, 0x04},  	
	{12000000, 48000, 0x05, 0x08, 0x01, 0x01, 0x00, 0x00, 0xf9, 0x04, 0x19, 0x19, 0x01},  	
	{18432000, 48000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x01, 0x7f, 0x04, 0x10, 0x10, 0x04},  
	{6144000 , 48000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x7f, 0x02, 0x10, 0x20, 0x04},	
 	{3072000 , 48000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0x3f, 0x04, 0x10, 0x10, 0x04},  	
	{1536000 , 48000, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0x1f, 0x04, 0x10, 0x10, 0x04},  
	{8192000 , 48000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xA9, 0x02, 0x15, 0x2a, 0x02},   	
	/* 64k */                                                                             	
	{12288000, 64000, 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xbf, 0x02, 0x10, 0x10, 0x04},  	
	{18432000, 64000, 0x03, 0x04, 0x03, 0x03, 0x01, 0x01, 0x1f, 0x06, 0x10, 0x10, 0x04},  	
	{16384000, 64000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10, 0x04},  	
	{8192000 , 64000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x7f, 0x04, 0x10, 0x10, 0x04},  	
	{6144000 , 64000, 0x01, 0x08, 0x03, 0x03, 0x00, 0x00, 0x5f, 0x01, 0x10, 0x10, 0x04},  	
	{4096000 , 64000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0x3f, 0x04, 0x10, 0x10, 0x04},  	
	{3072000 , 64000, 0x01, 0x08, 0x03, 0x03, 0x01, 0x00, 0x2f, 0x06, 0x10, 0x10, 0x04},  	
	{2048000 , 64000, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0x1f, 0x04, 0x10, 0x10, 0x04},  	
	{1536000 , 64000, 0x01, 0x08, 0x01, 0x01, 0x01, 0x00, 0x17, 0x03, 0x18, 0x18, 0x01},  	
	{1024000 , 64000, 0x01, 0x08, 0x01, 0x01, 0x01, 0x00, 0x0f, 0x02, 0x10, 0x10, 0x04},  	
	/* 88.2k */                                                                           	
	{11289600, 88200, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x7f, 0x04, 0x10, 0x10, 0x04},  	
	{5644800 , 88200, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0x3f, 0x04, 0x10, 0x10, 0x04},  	
	{2822400 , 88200, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0x1f, 0x04, 0x10, 0x10, 0x04},  	
	{1411200 , 88200, 0x01, 0x08, 0x01, 0x01, 0x01, 0x00, 0x0f, 0x02, 0x10, 0x10, 0x04},  	
	/* 96k */                                                                             	
	{12288000, 96000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x7f, 0x02, 0x10, 0x10, 0x04},  	
 	{6144000 , 96000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0x3f, 0x01, 0x10, 0x10, 0x04},	
	{19200000, 96000, 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xc7, 0x02, 0x10, 0x20, 0x04}, 			                                                                                   
	{8192000 , 96000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0x54, 0x01, 0x15, 0x10, 0x02},  
	{18432000, 96000, 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xbf, 0x04, 0x10, 0x10, 0x04},  		
	{3072000 , 96000, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0x1f, 0x04, 0x10, 0x10, 0x04},  	
	{1536000 , 96000, 0x01, 0x08, 0x01, 0x01, 0x01, 0x00, 0x0f, 0x02, 0x10, 0x10, 0x04},  	
		
};

static inline int get_coeff(int mclk, int rate)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(coeff_div); i++) {
		if (coeff_div[i].rate == rate && coeff_div[i].mclk == mclk)
			return i;
	}

	return -EINVAL;
}

/*
 * if PLL not be used, use internal clk1 for mclk,otherwise, use internal clk2 for PLL source.
 */
static int es8311_set_dai_sysclk(struct snd_soc_dai *dai,
			int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = dai->codec;
	struct es8311_private *es8311 = snd_soc_codec_get_drvdata(codec);
	ES8311_INFO_TRACE("Enter into %s()", __func__);
	switch (freq) {
	case 11289600:
	case 22579200:
		es8311->mclk = freq;
		return 0;

	case 6144000:
	case 12288000:
	case 16384000:
	case 18432000:
	case 24576000:
		es8311->mclk = freq;
		return 0;
	case 19200000:
		es8311->mclk = freq;
		return 0;
	}

	return EINVAL;
}

static int es8311_set_dai_fmt(struct snd_soc_dai *codec_dai, unsigned int fmt)
{
	printk("Enter into %s()\n", __func__);	
	struct snd_soc_codec *codec = codec_dai->codec;
	u8 iface = 0;
//	u8 mclk_from_bclk = 0;
	u8 adciface = 0;
	u8 daciface = 0;
	iface    = snd_soc_read(codec, ES8311_RESET_REG00);
	adciface = snd_soc_read(codec, ES8311_SDPOUT_REG0A);
	daciface = snd_soc_read(codec, ES8311_SDPIN_REG09);

	/* set master/slave audio interface */
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:    /* MASTER MODE */
		ES8311_INFO_TRACE("ES8311 in Master mode");
		iface |= 0x40;
//		mclk_from_bclk = 0;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:    /* SLAVE MODE */
		ES8311_INFO_TRACE("ES8311 in Slave mode");
		iface &= 0xBF;
//		mclk_from_bclk = 1;
		break;
	default:
		return -EINVAL;
	}
	snd_soc_write(codec, ES8311_RESET_REG00, iface);

//	snd_soc_update_bits(codec, ES8311_CLK_MANAGER_REG01, 0x80, mclk_from_bclk << 0x7);

	/* interface format */
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		ES8311_INFO_TRACE("ES8311 in I2S Format");
		adciface &= 0xFC;
		daciface &= 0xFC;
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		return -EINVAL;
	case SND_SOC_DAIFMT_LEFT_J:
		ES8311_INFO_TRACE("ES8311 in LJ Format");
		adciface &= 0xFC;
		daciface &= 0xFC;
		adciface |= 0x01;
		daciface |= 0x01;
		break;
	case SND_SOC_DAIFMT_DSP_A:
		ES8311_INFO_TRACE("ES8311 in DSP-A Format");
		adciface &= 0xDC;
		daciface &= 0xDC;
		adciface |= 0x03;
		daciface |= 0x03;
		break;
	case SND_SOC_DAIFMT_DSP_B:
		ES8311_INFO_TRACE("ES8311 in DSP-B Format");
		adciface &= 0xDC;
		daciface &= 0xDC;
		adciface |= 0x23;
		daciface |= 0x23;
		break;
	default:
		return -EINVAL;
	}

	iface = snd_soc_read(codec, ES8311_CLK_MANAGER_REG06);
	/* clock inversion */
	if(((fmt & SND_SOC_DAIFMT_FORMAT_MASK)==SND_SOC_DAIFMT_I2S) ||
       ((fmt & SND_SOC_DAIFMT_FORMAT_MASK)==SND_SOC_DAIFMT_LEFT_J)) {
		switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
		case SND_SOC_DAIFMT_NB_NF:
			iface    &= 0xDF;
			adciface &= 0xDF;
			daciface &= 0xDF;
			break;
		case SND_SOC_DAIFMT_IB_IF:
			iface    |= 0x20;
			adciface |= 0x20;
			daciface |= 0x20;
			break;
		case SND_SOC_DAIFMT_IB_NF:
			iface    |= 0x20;
			adciface &= 0xDF;
			daciface &= 0xDF;
			break;
		case SND_SOC_DAIFMT_NB_IF:
			iface    &= 0xDF;
			adciface |= 0x20;
			daciface |= 0x20;
			break;
		default:
			return -EINVAL;
		}
	}
	snd_soc_write(codec, ES8311_CLK_MANAGER_REG06, iface);
	snd_soc_write(codec, ES8311_SDPOUT_REG0A, adciface);
	snd_soc_write(codec, ES8311_SDPIN_REG09, daciface);
	return 0;
}
#define ReadReg(A)   snd_soc_read(codec,A)
#define WriteReg(A,B)   snd_soc_write(codec,A,B)
static int es8311_pcm_hw_params(struct snd_pcm_substream *substream,
			struct snd_pcm_hw_params *params,
			struct snd_soc_dai *dai)
{
	printk("Enter into %s()\n", __func__);	
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;
	struct es8311_private *es8311 = snd_soc_codec_get_drvdata(codec);
	u16 iface;
	int coeff;
	u8 regv, datmp;
#if 0
	coeff = get_coeff(es8311->mclk, params_rate(params));
	if (coeff < 0) {
		ES8311_ERR_TRACE("Unable to configure sample rate %dHz with %dHz MCLK",
			             params_rate(params), es8311->mclk);
		return coeff;
	}
	/*
	* set clock parammeters
	*/
	if(coeff >= 0) {                                      
		regv = ReadReg(ES8311_CLK_MANAGER_REG02) & 0x07;    
		regv |= (coeff_div[coeff].prediv - 1) << 5;         
		datmp = 0;                                          
		switch(coeff_div[coeff].premulti)                   
		{                                                   
		case 1:                                             
			datmp = 0;                                        
			break;                                            
		case 2:                                             
			datmp = 1;                                        
			break;                                            
		case 4:                                             
			datmp = 2;                                        
			break;                                            
		case 8:                                             
			datmp = 3;                                        
			break;                                            
		defalut:                                            
			break;                                            
		}                                                   
		regv |= (datmp) << 3;                               
		WriteReg(ES8311_CLK_MANAGER_REG02, regv);           
                                                        
		regv = ReadReg(ES8311_CLK_MANAGER_REG05) & 0x00;    
		regv |= (coeff_div[coeff].adcdiv - 1) << 4;         
		regv |= (coeff_div[coeff].dacdiv - 1) << 0;         
		WriteReg(ES8311_CLK_MANAGER_REG05, regv);           
  	                                                    
		regv = ReadReg(ES8311_CLK_MANAGER_REG03) & 0x80;    
		regv |= coeff_div[coeff].fsmode << 6;               
		regv |= coeff_div[coeff].adcosr << 0;               
		WriteReg(ES8311_CLK_MANAGER_REG03, regv);           
  	                                                    
		regv = ReadReg(ES8311_CLK_MANAGER_REG04) & 0x80;    
		regv |= coeff_div[coeff].dacosr << 0;               
		WriteReg(ES8311_CLK_MANAGER_REG04, regv);           
			                                                  
		regv = ReadReg(ES8311_CLK_MANAGER_REG07) & 0xf0;    
		regv |= coeff_div[coeff].lrck_h << 0;               
		WriteReg(ES8311_CLK_MANAGER_REG07, regv);           
  	                                                    
		regv = ReadReg(ES8311_CLK_MANAGER_REG08) & 0x00;    
		regv |= coeff_div[coeff].lrck_l << 0;               
		WriteReg(ES8311_CLK_MANAGER_REG08, regv);           
  	                                                    
  	regv = ReadReg(ES8311_CLK_MANAGER_REG06) & 0xE0;    
  	if(coeff_div[coeff].bclkdiv < 19) {                 
  		regv |= (coeff_div[coeff].bclkdiv - 1) << 0;      
  	}                                                   
  	else {                                              
  		regv |= (coeff_div[coeff].bclkdiv) << 0;          
  	}                                                   
  	WriteReg(ES8311_CLK_MANAGER_REG06, regv);           
  	                                                    
  	regv = ReadReg(ES8311_ADC_REG16) & 0x38;            
  	regv |= (coeff_div[coeff].adcscale ) << 0;          
  	WriteReg(ES8311_ADC_REG16, regv);	                  

	}
#endif	
	if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		iface = snd_soc_read(codec, ES8311_SDPIN_REG09) & 0xE3;
		/* bit size */
		switch (params_format(params)) {
		case SNDRV_PCM_FORMAT_S16_LE:
			iface |= 0x0c;
			break;
		case SNDRV_PCM_FORMAT_S20_3LE:
			iface |= 0x04;
			break;
		case SNDRV_PCM_FORMAT_S24_LE:
			break;
		case SNDRV_PCM_FORMAT_S32_LE:
			iface |= 0x10;
			break;
		}
		/* set iface */
		snd_soc_write(codec, ES8311_SDPIN_REG09, iface);
	} else {
		iface = snd_soc_read(codec, ES8311_SDPOUT_REG0A) & 0xE3;
		/* bit size */
		switch (params_format(params)) {
		case SNDRV_PCM_FORMAT_S16_LE:
			iface |= 0x0c;
			break;
		case SNDRV_PCM_FORMAT_S20_3LE:
			iface |= 0x04;
			break;
		case SNDRV_PCM_FORMAT_S24_LE:
			break;
		case SNDRV_PCM_FORMAT_S32_LE:
			iface |= 0x10;
			break;
		}
		/* set iface */
		snd_soc_write(codec, ES8311_SDPOUT_REG0A, iface);
	}
	return 0;
}

static u_char es8311_adc_value = 0XBF;
static u_char es8311_dac_value = 0xBF;

static int es8311_set_bias_level(struct snd_soc_codec *codec,
			enum snd_soc_bias_level level)
{
	int regv;
	struct es8311_private *es8311 = snd_soc_codec_get_drvdata(codec);
	printk("Enter into %s(), level = %d,codec->dapm.bias_level=%d\n", __func__, level,codec->dapm.bias_level);
	switch (level) {
	case SND_SOC_BIAS_ON:
		msleep(20);
		snd_soc_write(codec, ES8311_GP_REG45, 0x00);
		snd_soc_write(codec, ES8311_ADC_REG16, 0x24);
		snd_soc_write(codec, ES8311_SYSTEM_REG0B, 0x00);
		msleep(50);
		snd_soc_write(codec, ES8311_SYSTEM_REG0C, 0x00);
		snd_soc_write(codec, ES8311_SYSTEM_REG10, 0x1F);
		snd_soc_write(codec, ES8311_SYSTEM_REG11, 0x7F);
		snd_soc_write(codec, ES8311_RESET_REG00, 0x80);
		msleep(50);
		snd_soc_write(codec, ES8311_SYSTEM_REG0D, 0x01);
		snd_soc_write(codec, ES8311_CLK_MANAGER_REG01, 0x3F);
		if(es8311->mclkinv == true) {
			snd_soc_update_bits(codec, ES8311_CLK_MANAGER_REG01,
					0x40, 0x40);
		}
		else {
			snd_soc_update_bits(codec, ES8311_CLK_MANAGER_REG01,
					0x40, 0x00);
		}
		msleep(50);
		if(es8311->sclkinv == true) {
			snd_soc_update_bits(codec, ES8311_CLK_MANAGER_REG06,
					0x20, 0x20);
		}
		else {
			snd_soc_update_bits(codec, ES8311_CLK_MANAGER_REG06,
					0x20, 0x00);
		}
		regv = snd_soc_read(codec, ES8311_SYSTEM_REG14) & 0xCF;
		regv |= 0x1A;
		snd_soc_write(codec, ES8311_SYSTEM_REG14, regv);
		msleep(50);
		if(es8311->dmic_enable == true) {
			snd_soc_update_bits(codec, ES8311_SYSTEM_REG14,
					0x40, 0x40);
		}
		else {
			snd_soc_update_bits(codec, ES8311_SYSTEM_REG14,
					0x40, 0x00);
		}
		snd_soc_write(codec, ES8311_SYSTEM_REG13, 0x10);
		snd_soc_write(codec, ES8311_SYSTEM_REG0E, 0x02);
		snd_soc_write(codec, ES8311_ADC_REG15, 0x40);
		msleep(50);
		snd_soc_write(codec, ES8311_ADC_REG1B, 0x0A);
		snd_soc_write(codec, ES8311_ADC_REG1C, 0x6A);
		//snd_soc_write(codec, ES8311_DAC_REG37, 0x48);
		msleep(50);
		snd_soc_write(codec, ES8311_GPIO_REG44, 0x08);
		snd_soc_write(codec, ES8311_ADC_REG17, es8311_adc_value);
		snd_soc_write(codec, ES8311_DAC_REG32, es8311_dac_value);
		break;
	case SND_SOC_BIAS_PREPARE:
		break;
	case SND_SOC_BIAS_STANDBY:
		msleep(20);
		snd_soc_write(codec, ES8311_DAC_REG32, 0x00);
		snd_soc_write(codec, ES8311_ADC_REG17, 0x00);
		snd_soc_write(codec, ES8311_SYSTEM_REG0E, 0xFF);
		msleep(50);
		snd_soc_write(codec, 0x12, 0x02);
		snd_soc_write(codec, 0x14, 0x00);
		//snd_soc_write(codec, ES8311_SYSTEM_REG0D, 0xFA);
		msleep(50);
		snd_soc_write(codec, ES8311_ADC_REG15, 0x00);
		snd_soc_write(codec, ES8311_DAC_REG37, 0x08);
		//snd_soc_write(codec, 0x02, 0x10);
		msleep(50);
		snd_soc_write(codec, ES8311_RESET_REG00, 0x00);
		snd_soc_write(codec, ES8311_RESET_REG00, 0x1F);
		snd_soc_write(codec, ES8311_CLK_MANAGER_REG01, 0x30);
		msleep(50);
		snd_soc_write(codec, ES8311_CLK_MANAGER_REG01, 0x00);
		snd_soc_write(codec, ES8311_GP_REG45, 0x01);
		//snd_soc_write(codec, 0x02, 0x00);
		break;
	
	case SND_SOC_BIAS_OFF:
		  snd_soc_write(codec, ES8311_SYSTEM_REG12, 0x02);
                snd_soc_write(codec, ES8311_DAC_REG32, 0x00);
                snd_soc_write(codec, ES8311_ADC_REG17, 0x00);
                snd_soc_write(codec, ES8311_SYSTEM_REG0E, 0xFF);
          //      snd_soc_write(codec, ES8311_SYSTEM_REG0D, 0xFA);
                snd_soc_write(codec, ES8311_ADC_REG15, 0x00);
                snd_soc_write(codec, ES8311_DAC_REG37, 0x08);
                snd_soc_write(codec, ES8311_RESET_REG00, 0x00);
                snd_soc_write(codec, ES8311_RESET_REG00, 0x1F);
                snd_soc_write(codec, ES8311_CLK_MANAGER_REG01, 0x30);
                snd_soc_write(codec, ES8311_CLK_MANAGER_REG01, 0x00);
                snd_soc_write(codec, ES8311_GP_REG45, 0x01);

		break;
	}
	codec->dapm.bias_level = level;
	return 0;
}


static int es8311_set_tristate(struct snd_soc_dai *dai, int tristate)
{
	struct snd_soc_codec *codec = dai->codec;
	printk("Enter into %s()\n", __func__);	
	if(tristate) {
		snd_soc_update_bits(codec, ES8311_CLK_MANAGER_REG07,
			0x30, 0x30);
	} 
	else {
		snd_soc_update_bits(codec, ES8311_CLK_MANAGER_REG07,
			0x30, 0x00);
	}
	return 0;
}

static int es8311_mute(struct snd_soc_dai *dai, int mute)
{
	struct snd_soc_codec *codec = dai->codec;
	printk("Enter into %s()\n", __func__);	
	if (mute) {
		//snd_soc_write(codec, ES8311_SYSTEM_REG12, 0x02);
		//snd_soc_update_bits(codec, ES8311_DAC_REG31, 0x60, 0x60);
		//snd_soc_write(codec, ES8311_DAC_REG32, 0x00);
		//snd_soc_write(codec, ES8311_DAC_REG37, 0x08);
	} else {
	//	snd_soc_write(codec, ES8311_DAC_REG32, 0xA0);
		snd_soc_update_bits(codec, ES8311_DAC_REG31, 0x60, 0x00);
		snd_soc_write(codec, ES8311_SYSTEM_REG12, 0x00);
	}
	return 0;
}


static int es8311_pcm_startup(struct snd_pcm_substream *substream,
                struct snd_soc_dai *dai)
{
        printk("Enter into %s()\n", __func__);
        return 0;
}

static int es8311_pcm_shutdown(struct snd_pcm_substream *substream,
                struct snd_soc_dai *dai)
{
        printk("Enter into %s()\n", __func__);
        return 0;
}



#define es8311_RATES SNDRV_PCM_RATE_8000_96000

#define es8311_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE |\
		SNDRV_PCM_FMTBIT_S24_LE)

static struct snd_soc_dai_ops es8311_ops = {
	.startup = es8311_pcm_startup,
	.hw_params = es8311_pcm_hw_params,
	.set_fmt = es8311_set_dai_fmt,
	.set_sysclk = es8311_set_dai_sysclk,
	.digital_mute = es8311_mute,
	.set_tristate = es8311_set_tristate,
	.shutdown = es8311_pcm_shutdown,
};

static struct snd_soc_dai_driver es8311_dai = {
	.name = "es8311-hifi",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 1,
		.channels_max = 2,
		.rates = es8311_RATES,
		.formats = es8311_FORMATS,
	},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 1,
		.channels_max = 2,
		.rates = es8311_RATES,
		.formats = es8311_FORMATS,
	},
	.ops = &es8311_ops,
	.symmetric_rates = 1,
};



static ssize_t adc_value_cfg_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int cnt = 0;
 
	cnt += sprintf(buf, "%d\n", es8311_adc_value);

    if (cnt >= PAGE_SIZE)
		cnt = PAGE_SIZE - 1;
    return cnt;
}

static ssize_t adc_value_cfg_store(struct device *dev, struct device_attribute *attr, char *buf, size_t count)
{
	int mode;

	sscanf(buf, "%d", &mode);
	
	if(mode < 0 && mode > 255)
	{
		pr_err("the para is wrong, %d \n", mode);
	}
	else
	{
		es8311_adc_value = mode;	
	}
	return count;
}

static ssize_t dac_value_cfg_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int cnt = 0;
 
	cnt += sprintf(buf, "%d\n", es8311_dac_value);

    if (cnt >= PAGE_SIZE)
		cnt = PAGE_SIZE - 1;
    return cnt;
}

static ssize_t dac_value_cfg_store(struct device *dev, struct device_attribute *attr, char *buf, size_t count)
{
	int mode;

	sscanf(buf, "%d", &mode);
	
	if(mode < 0 && mode > 255)
	{
		pr_err("the para is wrong, %d \n", mode);
	}
	else 
	{
		es8311_dac_value = mode;	
	}
	return count;
}

static DEVICE_ATTR(adc_value_cfg, 0644, adc_value_cfg_show, adc_value_cfg_store);
static DEVICE_ATTR(dac_value_cfg, 0644, dac_value_cfg_show, dac_value_cfg_store);

static int es8311_probe(struct snd_soc_codec *codec)
{
	//u32 ret = 0;
	struct es8311_private *es8311 = snd_soc_codec_get_drvdata(codec);
	ES8311_INFO_TRACE("---%s--start--",__FUNCTION__);
	es8311->codec = codec;




	snd_soc_write(codec, ES8311_GP_REG45, 0x00);
	snd_soc_write(codec, ES8311_CLK_MANAGER_REG01, 0x90);
	snd_soc_write(codec, ES8311_CLK_MANAGER_REG02, 0x00);
	snd_soc_write(codec, ES8311_CLK_MANAGER_REG03, 0x10);
	snd_soc_write(codec, ES8311_ADC_REG16, 0x24);
	snd_soc_write(codec, ES8311_CLK_MANAGER_REG04, 0x10);
	snd_soc_write(codec, ES8311_CLK_MANAGER_REG05, 0x00);
	snd_soc_write(codec, ES8311_SYSTEM_REG0B, 0x00);
	snd_soc_write(codec, ES8311_SYSTEM_REG0C, 0x00);
	snd_soc_write(codec, ES8311_SYSTEM_REG10, 0x1F);
	snd_soc_write(codec, ES8311_SYSTEM_REG11, 0x7F);
	snd_soc_write(codec, ES8311_RESET_REG00, 0x80);
	snd_soc_write(codec, ES8311_SYSTEM_REG0D, 0x01);
	snd_soc_write(codec, ES8311_CLK_MANAGER_REG01, 0x9F);
	if(es8311->mclkinv == true) {
		snd_soc_update_bits(codec, ES8311_CLK_MANAGER_REG01, 0x40, 0x40);
	}
	else {
		snd_soc_update_bits(codec, ES8311_CLK_MANAGER_REG01, 0x40, 0x00);
	}
	if(es8311->sclkinv == true) {
		snd_soc_update_bits(codec, ES8311_CLK_MANAGER_REG06, 0x20, 0x20); 
	}
	else {
		snd_soc_update_bits(codec, ES8311_CLK_MANAGER_REG06, 0x20, 0x00); 
	}
	snd_soc_write(codec, ES8311_SYSTEM_REG14, 0x1A);
	if(es8311->dmic_enable == true) {
		snd_soc_update_bits(codec, ES8311_SYSTEM_REG14, 0x40, 0x40);
	}
	else {
		snd_soc_update_bits(codec, ES8311_SYSTEM_REG14, 0x40, 0x00);
	}
	snd_soc_write(codec, ES8311_SYSTEM_REG12, 0x00);
	snd_soc_write(codec, ES8311_SYSTEM_REG13, 0x10);
	snd_soc_write(codec, ES8311_SDPIN_REG09, 0x00);
	snd_soc_write(codec, ES8311_SDPOUT_REG0A, 0x00);
	snd_soc_write(codec, ES8311_SYSTEM_REG0E, 0x02);
	snd_soc_write(codec, ES8311_ADC_REG15, 0x40);
	snd_soc_write(codec, ES8311_ADC_REG1B, 0x0A);
	snd_soc_write(codec, ES8311_ADC_REG1C, 0x6A);
	snd_soc_write(codec, ES8311_DAC_REG37, 0x48);
	snd_soc_write(codec, ES8311_GPIO_REG44, 0x08);
	snd_soc_write(codec, ES8311_ADC_REG17, 0xBF);
	snd_soc_write(codec, ES8311_DAC_REG32, 0xBF);
	msleep(100);
//	es8311_set_bias_level(codec, SND_SOC_BIAS_ON);
	es8311_eq_reg_init(codec);
	return 0;
}

static struct snd_soc_codec_driver soc_codec_dev_es8311 = {
	.probe = es8311_probe,
    .set_bias_level = es8311_set_bias_level,
	.component_driver = {
		.controls = es8311_snd_controls,
		.num_controls = ARRAY_SIZE(es8311_snd_controls),
	},
};

static const struct regmap_config es8311_regmap_cfg = {
	.reg_bits = 8,
	.val_bits = 8,
	.max_register = ES8311_MAX_REGISTER,
	.cache_type = REGCACHE_RBTREE,
};


static ssize_t codec_reg_store(struct device *dev,
		struct device_attribute *attr, 
		const char *buf, size_t count)
{
	u8 flag,i2c_reg,i2c_wval,i2c_rval;
	struct i2c_client *client = to_i2c_client(dev);
        struct es8311_private *es8311 = i2c_get_clientdata(client);
        struct snd_soc_codec *codec = es8311->codec;
        sscanf(buf, "%d %x %x", &flag,&i2c_reg);	
        if (flag==1){             //read
	    i2c_rval=snd_soc_read(codec,i2c_reg);	
	    SR_val=i2c_rval;
	    printk("read%s,%d,reg=%x,value=%x\n",__func__,__LINE__,i2c_reg,i2c_rval);
	}
	if (flag==0){             //write
	    sscanf(buf, "%d %x %x\n",&flag,&i2c_reg,&i2c_wval);
	    snd_soc_write(codec,i2c_reg,i2c_wval);
	     printk("write%s,%d,reg=%x,value=%x\n",__func__,__LINE__,i2c_reg,i2c_wval);	
	}
	return count;
}


static ssize_t codec_reg_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret;
	u8 val;
	val=SR_val;
        ret = sprintf(buf, "%x\n",val);
        printk("********val = %x\n",val);
	return ret;	
}



static DEVICE_ATTR(codec_reg, 0664, codec_reg_show, codec_reg_store);
//extern void quec_set_codec_info(const char* codec_name, const char* dai_name);
static int es8311_i2c_probe(struct i2c_client *i2c_client, const struct i2c_device_id *id)
{
	int ret;
	u8 val;
	struct device *dev= &i2c_client->dev;
	struct es8311_private *es8311;

	ES8311_ERR_TRACE("---%s--start--",__FUNCTION__);

	es8311 = devm_kzalloc(&i2c_client->dev, sizeof(struct es8311_private), GFP_KERNEL);
	if (es8311 == NULL) {
		return -ENOMEM;
	}

	es8311->dmic_enable = false;     // dmic interface disabled
	/* the edge of lrck is always at the falling edge of mclk */
	es8311->mclkinv = false; 
        /* the edge of lrck is always at the falling edge of sclk */         
	es8311->sclkinv = true; 

	i2c_set_clientdata(i2c_client, es8311);
	
	es8311->regmap = devm_regmap_init_i2c(i2c_client, &es8311_regmap_cfg);
    if (IS_ERR(es8311->regmap)) {
        return PTR_ERR(es8311->regmap);
    }
	ret=regmap_read(es8311->regmap,0xFE,&val);
/*	if (val != 0x11) {
	//	kfree(es8311);
		return 0;
	}
*/	
	printk("Enter into %s---1  val=0x%x\n", __func__,val);
	ret = snd_soc_register_codec(&i2c_client->dev, &soc_codec_dev_es8311, &es8311_dai, 1);
	if (ret) {
		ES8311_ERR_TRACE("es8311 register codec failed");
		return ret;
	}
    ret = device_create_file(dev, &dev_attr_adc_value_cfg);
    if (ret != 0) {
        dev_err(dev,
        "Failed to create adc  sysfs files: %d\n", ret);
        return ret;
    }

    ret = device_create_file(dev, &dev_attr_dac_value_cfg);
    if (ret != 0) {
        dev_err(dev,
        "Failed to create dac sysfs files: %d\n", ret);
        return ret;
    }

	device_create_file(dev, &dev_attr_codec_reg);
	if (ret < 0)
		dev_err(dev, "%s fail\n", __func__);
	return ret;
}

static int es8311_i2c_remove(struct i2c_client *client)
{
	snd_soc_unregister_codec(&client->dev);
	return 0;
}

static const struct i2c_device_id es8311_i2c_id[] = {
	{"es8311", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, es8311_i2c_id);

static struct of_device_id es8311_of_match[] = {
	{ .compatible = "quec,quec-tlv320aic3x-i2c", },
	{ }
};
MODULE_DEVICE_TABLE(of, es8311_of_match);

static struct i2c_driver es8311_i2c_driver = {
	.driver = {
		.name	= "es8311",
		.of_match_table = of_match_ptr(es8311_of_match),
	},
	.probe = es8311_i2c_probe,
	.remove	= es8311_i2c_remove,
	.id_table	= es8311_i2c_id,
};

module_i2c_driver(es8311_i2c_driver);

MODULE_DESCRIPTION("Balong ES8311 ALSA SoC Codec Driver");
MODULE_LICENSE("GPL");
