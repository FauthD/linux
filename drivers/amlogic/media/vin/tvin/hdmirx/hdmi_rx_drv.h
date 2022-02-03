/*
 * drivers/amlogic/media/vin/tvin/hdmirx/hdmi_rx_drv.h
 *
 * Copyright (C) 2017 Amlogic, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */

#ifndef __HDMI_RX_DRV_H__
#define __HDMI_RX_DRV_H__

#include <linux/workqueue.h>
#include <linux/extcon.h>


#include <linux/amlogic/cpu_version.h>
#include <linux/amlogic/media/frame_provider/tvin/tvin.h>
#include <linux/amlogic/iomap.h>
#include <linux/cdev.h>
#include <linux/irqreturn.h>
#include "../tvin_global.h"
#include "../tvin_frontend.h"
//#include "hdmirx_repeater.h"
//#include "hdmi_rx_pktinfo.h"
#include "hdmi_rx_edid.h"
#include "hdmi_rx_drv_ext.h"

/* modified sda sample rate */
#define RX_VER0 "ver.2021/03/30"

/*print type*/
#define	LOG_EN		0x01
#define VIDEO_LOG	0x02
#define AUDIO_LOG	0x04
#define HDCP_LOG	0x08
#define PACKET_LOG	0x10
#define EQ_LOG		0x20
#define REG_LOG		0x40
#define ERR_LOG		0x80
#define EDID_LOG	0x100
#define PHY_LOG		0x200
#define VSI_LOG		0x800
#define DBG_LOG		0x1000

/* avmute skip frame */
#define RX_VER1 "ver.2021/04/21"

/* 50ms timer for hdmirx main loop (HDMI_STATE_CHECK_FREQ is 20) */

#define ABS(x) ((x) < 0 ? -(x) : (x))

#define EDID_MIX_MAX_SIZE 64
/* reduce kill_esm waiting time when suspend, */
/* to avoid VAD wake-up failure */
#define ESM_KILL_WAIT_TIMES 25
#define str_cmp(buff, str) ((strlen((str)) == strlen((buff))) &&	\
		(strncmp((buff), (str), strlen((str))) == 0))
#define pr_var(str, index) rx_pr("%5d %-30s = %#x\n", (index), #str, (str))
#define var_to_str(var) (#var)
#define var_str_cmp(str, var) str_cmp((str), (#var))
#define set_pr_var(a, b, c, d, e)  (comp_set_pr_var((a), \
	(var_to_str(b)), (&b), (c), (d), (e), (sizeof(b))))

/* calc phy addr for old chips */
#define RX_VER2 "ver.2021/10/14"

#define PFIFO_SIZE 160
#define HDCP14_KEY_SIZE 368
enum chip_id_e {
	CHIP_ID_GXTVBB,
	CHIP_ID_TXL,
	CHIP_ID_TXLX,
	CHIP_ID_TXHD,
	CHIP_ID_TL1,
	CHIP_ID_TM2,
	CHIP_ID_T5,
	CHIP_ID_T5D,
};

enum phy_ver_e {
	PHY_VER_ORG,
	PHY_VER_TL1,
	PHY_VER_TM2,
	PHY_VER_T5,
};

struct meson_hdmirx_data {
	enum chip_id_e chip_id;
	enum phy_ver_e phy_ver;
	struct ctrl *phyctrl;
};

struct hdmirx_dev_s {
	int                         index;
	dev_t                       devt;
	struct cdev                 cdev;
	struct device               *dev;
	struct tvin_parm_s          param;
	struct timer_list           timer;
	struct tvin_frontend_s		frontend;
	unsigned int			irq;
	char					irq_name[12];
	struct mutex			rx_lock;
	/*struct mutex			pd_fifo_lock;*/
	struct clk *modet_clk;
	struct clk *cfg_clk;
	struct clk *acr_ref_clk;
	struct clk *audmeas_clk;
	struct clk *aud_out_clk;
	struct clk *esm_clk;
	struct clk *skp_clk;
	struct clk *meter_clk;
	struct clk *axi_clk;
	const struct meson_hdmirx_data *data;
};

#define HDMI_IOC_MAGIC 'H'
#define HDMI_IOC_HDCP_ON	_IO(HDMI_IOC_MAGIC, 0x01)
#define HDMI_IOC_HDCP_OFF	_IO(HDMI_IOC_MAGIC, 0x02)
#define HDMI_IOC_EDID_UPDATE	_IO(HDMI_IOC_MAGIC, 0x03)
#define HDMI_IOC_PC_MODE_ON		_IO(HDMI_IOC_MAGIC, 0x04)
#define HDMI_IOC_PC_MODE_OFF	_IO(HDMI_IOC_MAGIC, 0x05)
#define HDMI_IOC_HDCP22_AUTO	_IO(HDMI_IOC_MAGIC, 0x06)
#define HDMI_IOC_HDCP22_FORCE14 _IO(HDMI_IOC_MAGIC, 0x07)
#define HDMI_IOC_HDMI_20_SET	_IO(HDMI_IOC_MAGIC, 0x08)
#define HDMI_IOC_HDCP_GET_KSV _IOR(HDMI_IOC_MAGIC, 0x09, struct _hdcp_ksv)
#define HDMI_IOC_PD_FIFO_PKTTYPE_EN _IOW(HDMI_IOC_MAGIC, 0x0a,\
	uint32_t)
#define HDMI_IOC_PD_FIFO_PKTTYPE_DIS _IOW(HDMI_IOC_MAGIC, 0x0b,\
		uint32_t)
#define HDMI_IOC_GET_PD_FIFO_PARAM _IOWR(HDMI_IOC_MAGIC, 0x0c,\
	struct pd_infoframe_s)
#define HDMI_IOC_HDCP14_KEY_MODE _IOR(HDMI_IOC_MAGIC, 0x0d,\
	enum hdcp14_key_mode_e)
#define HDMI_IOC_HDCP22_NOT_SUPPORT _IO(HDMI_IOC_MAGIC, 0x0e)

#define IOC_SPD_INFO  _BIT(0)
#define IOC_AUD_INFO  _BIT(1)
#define IOC_MPEGS_INFO _BIT(2)
#define IOC_AVI_INFO _BIT(3)
#define ALL_PORTS ((1 << E_PORT_NUM) - 1)

enum colorspace_e {
	E_COLOR_RGB,
	E_COLOR_YUV422,
	E_COLOR_YUV444,
	E_COLOR_YUV420,
};

enum colorrange_rgb_e {
	E_RGB_RANGE_DEFAULT,
	E_RGB_RANGE_LIMIT,
	E_RGB_RANGE_FULL,
};

enum colorrange_ycc_e {
	E_YCC_RANGE_LIMIT,
	E_YCC_RANGE_FULL,
	E_YCC_RANGE_RSVD,
};

enum colordepth_e {
	E_COLORDEPTH_8 = 8,
	E_COLORDEPTH_10 = 10,
	E_COLORDEPTH_12 = 12,
	E_COLORDEPTH_16 = 16,
};

enum port_sts_e {
	E_PORT0,
	E_PORT1,
	E_PORT2,
	E_PORT3,
	E_PORT_NUM,
	E_5V_LOST = 0xfd,
	E_INIT = 0xff,
};

enum hdcp_version_e {
	HDCP_VER_NONE,
	HDCP_VER_14,
	HDCP_VER_22,
};

enum hdmirx_port_e {
	HDMIRX_PORT0,
	HDMIRX_PORT1,
	HDMIRX_PORT2,
	HDMIRX_PORT3,
};

enum map_addr_module_e {
	MAP_ADDR_MODULE_CBUS,
	MAP_ADDR_MODULE_HIU,
	MAP_ADDR_MODULE_HDMIRX_CAPB3,
	MAP_ADDR_MODULE_SEC_AHB,
	MAP_ADDR_MODULE_SEC_AHB2,
	MAP_ADDR_MODULE_APB4,
	MAP_ADDR_MODULE_TOP,
	MAP_ADDR_MODULE_CLK_CTRL,
	MAP_ADDR_MODULE_NUM
};

enum rx_cn_type_e {
	CN_GRAPHICS,
	CN_PHOTO,
	CN_CINEMA,
	CN_GAME,
};

struct rx_var_param {
	int pll_unlock_cnt;
	int pll_unlock_max;
	int pll_lock_cnt;
	int pll_lock_max;
	int dwc_rst_wait_cnt;
	int dwc_rst_wait_cnt_max;
	int sig_stable_cnt;
	int sig_stable_max;
	int sig_stable_err_cnt;
	int sig_stable_err_max;
	int max_err_cnt;
	bool clk_debug;
	int hpd_wait_cnt;
	/* increase time of hpd low, to avoid some source like */
	/* MTK box/KaiboerH9 i2c communicate error */
	int hpd_wait_max;
	int sig_unstable_cnt;
	int sig_unstable_max;
	bool vic_check_en;
	bool dvi_check_en;
	int sig_unready_cnt;
	int sig_unready_max;
	int pow5v_max_cnt;
	/* timing diff offset */
	int diff_pixel_th;
	int diff_line_th;
	int diff_frame_th;
	int err_dbg_cnt;
	int err_dbg_cnt_max;
	int force_vic;
	u32 err_chk_en;
	int aud_sr_stb_max;
	int log_level;
	bool auto_switch_off;
	int clk_unstable_cnt;
	int clk_unstable_max;
	int clk_stable_cnt;
	int clk_stable_max;
	int unnormal_wait_max;
	int wait_no_sig_max;
	int phy_retry_times;
	/* No need to judge frame rate while checking timing stable,as there are
	 * some out-spec sources whose framerate change a lot(e.g:59.7~60.16hz).
	 * Other brands of tv can support this,we also need to support.
	 */
	int stable_check_lvl;
	/* If dvd source received the frequent pulses on HPD line,
	 * It will sent a length of dirty audio data sometimes.it's TX's issues.
	 * Compared with other brands TV, delay 1.5S to avoid this noise.
	 */
	int edid_update_delay;
	int skip_frame_cnt;
	bool hdcp22_reauth_enable;
	unsigned int edid_update_flag;
	unsigned int downstream_hpd_flag;
	bool hdcp22_stop_auth_enable;
	bool hdcp22_esm_reset2_enable;
	int sm_pause;
	int pre_port;
	/* waiting time cannot be reduced */
	/* it will cause hdcp1.4 cts fail */
	int hdcp_none_wait_max;
	int esd_phy_rst_cnt;
	int esd_phy_rst_max;
	int cec_dev_info;
	bool term_flag;
	int clk_chg_cnt;
	int clk_chg_max;
	/* vpp mute when signal change, used
	 * in companion with vlock phase = 84
	 */
	bool vpp_mute_enable;
	/* mute delay cnt before vpp unmute */
	int mute_cnt;
	u8 dbg_ve;
	/* after DE stable, start DE count */
	bool de_stable;
	u32 de_cnt;
	u8 avi_chk_frames;
	u32 avi_rcv_cnt;
	bool force_pattern;
};

struct rx_aml_phy {
	int dfe_en;
	int ofst_en;
	int cdr_mode;
	int pre_int;
	int pre_int_en;
	int phy_bwth;
	int alirst_en;
	int tap1_byp;
	int eq_byp;
	int long_cable;
	int osc_mode;
	int pll_div;
	bool sqrst_en;
	int vga_dbg;
	int vga_dbg_delay;
	u8 eq_fix_val;
	int cdr_fr_en;
	bool force_sqo;
	/* add for t5 */
	int os_rate;
	u32 vga_gain;
	u32 eq_stg1;
	u32 eq_stg2;
	bool dfe_hold;
	bool eq_hold;
	int eye_delay;
	bool eq_retry;
	bool tap2_byp;
	bool long_bist_en;
};

enum scan_mode_e {
	E_UNKNOWN_SCAN,
	E_OVERSCAN,
	E_UNDERSCAN,
	E_FUTURE,
};

/**
 * @short HDMI RX controller video parameters
 *
 * For Auxiliary Video InfoFrame (AVI) details see HDMI 1.4a section 8.2.2
 */
struct rx_video_info {
	/** DVI detection status: DVI (true) or HDMI (false) */
	bool hw_dvi;

	uint8_t hdcp_type;
	/** bit'0:auth start  bit'1:enc state(0:not endrypted 1:encrypted) **/
	uint8_t hdcp14_state;
	/** 1:decrypted 0:encrypted **/
	uint8_t hdcp22_state;
	/** Deep color mode: 8, 10, 12 or 16 [bits per pixel] */
	uint8_t colordepth;
	/** Pixel clock frequency [kHz] */
	uint32_t pixel_clk;
	/** Refresh rate [0.01Hz] */
	uint32_t frame_rate;
	/** Interlaced */
	bool interlaced;
	/** Vertical offset */
	uint32_t voffset;
	/** Vertical active */
	uint32_t vactive;
	/** Vertical total */
	uint32_t vtotal;
	/** Horizontal offset */
	uint32_t hoffset;
	/** Horizontal active */
	uint32_t hactive;
	/** Horizontal total */
	uint32_t htotal;
	/** AVI Y1-0, video format */
	uint8_t colorspace;
	/** AVI VIC6-0, video identification code */
	enum hdmi_vic_e hw_vic;
	/** AVI PR3-0, pixel repetition factor */
	uint8_t repeat;
	/* for sw info */
	enum hdmi_vic_e sw_vic;
	uint8_t sw_dvi;
	unsigned int it_content;
	enum rx_cn_type_e cn_type;
	/** AVI Q1-0, RGB quantization range */
	unsigned int rgb_quant_range;
	/** AVI Q1-0, YUV quantization range */
	u8 yuv_quant_range;
	u8 active_valid;
	u8 active_ratio;
	u8 bar_valid;
	u8 scan_info;
	u8 colorimetry;
	u8 picture_ratio;
	u8 ext_colorimetry;
	u8 n_uniform_scale;
	u8 bar_end_top;
	u8 bar_start_bottom;
	u8 bar_end_left;
	u8 bar_start_right;
	bool sw_fp;
	bool sw_alternative;
};

/** Receiver key selection size - 40 bits */
#define HDCP_BKSV_SIZE	(2 *  1)
/** Encrypted keys size - 40 bits x 40 keys */
#define HDCP_KEYS_SIZE	(2 * 40)

/*emp buffer config*/
#define DUMP_MODE_EMP	0
#define DUMP_MODE_TMDS	1
#define TMDS_BUFFER_SIZE	0x2000000 /*32M*/
#define EMP_BUFFER_SIZE		0x1000	/*4k*/
#define EMP_BUFF_MAX_PKT_CNT 32 /* 1024/32 */
#define TMDS_DATA_BUFFER_SIZE	0x200000

struct rx_fastswitch_mode {
	enum hdcp_version_e hdcp_ver[E_PORT_NUM];
	/* if edid ver is the same after switch
	 * edid ver in UI, no need to update edid
	 */
	enum edid_ver_e	edid_ver[E_PORT_NUM];
	u8 hdmi5v_sts[E_PORT_NUM];
	/* uint8_t hpd_sts[E_PORT_NUM]; */
};

/**
 * @short HDMI RX controller HDCP configuration
 */
struct hdmi_rx_hdcp {
	/*hdcp auth state*/
	enum repeater_state_e state;
	/** Repeater mode else receiver only */
	bool repeat;
	bool cascade_exceed;
	bool dev_exceed;
	/*downstream depth*/
	unsigned char depth;
	/*downstream count*/
	uint32_t count;
	/** Key description seed */
	uint32_t seed;
	uint32_t delay;/*according to the timer,5s*/
	/**
	 * Receiver key selection
	 * @note 0: high order, 1: low order
	 */
	uint32_t bksv[HDCP_BKSV_SIZE];
	/**
	 * Encrypted keys
	 * @note 0: high order, 1: low order
	 */
	uint32_t keys[HDCP_KEYS_SIZE];
	struct extcon_dev *rx_excton_auth;
	enum hdcp_version_e hdcp_version;/* 0 no hdcp;1 hdcp14;2 hdcp22 */
	/* add for dv cts */
	enum hdcp_version_e hdcp_pre_ver;
	bool hdcp_source;
	unsigned char hdcp22_exception;/*esm exception code,reg addr :0x60*/
};

static const unsigned int rx22_ext[] = {
	EXTCON_DISP_HDMI,
	EXTCON_NONE,
};
struct vsi_info_s {
	unsigned int identifier;
	unsigned char vd_fmt;
	unsigned char _3d_structure;
	unsigned char _3d_ext_data;
	bool dolby_vision;
	bool low_latency;
	bool backlt_md_bit;
	unsigned int eff_tmax_pq;
	bool allm_mode;
	bool hdr10plus;
	u8 vsi_state;
	u8 emp_pkt_cnt;
	u8 timeout;
};

#define CHANNEL_STATUS_SIZE   24

struct aud_info_s {
    /* info frame*/
	/*
	 *unsigned char cc;
	 *unsigned char ct;
	 *unsigned char ss;
	 *unsigned char sf;
	 */

	int coding_type;
	int channel_count;
	int sample_frequency;
	int sample_size;
	int coding_extension;
	int auds_ch_alloc;
	int auds_layout;

	/*
	 *int down_mix_inhibit;
	 *int level_shift_value;
	 */
	int aud_hbr_rcv;
	int aud_packet_received;
	/* aud mute by gcp_avmute or aud_spflat mute */
	bool aud_mute_en;
	/* channel status */
	unsigned char channel_status[CHANNEL_STATUS_SIZE];
	unsigned char channel_status_bak[CHANNEL_STATUS_SIZE];
    /**/
	unsigned int cts;
	unsigned int n;
	unsigned int arc;
	/**/
	int real_channel_num;
	int real_sample_size;
	int real_sr;
	u32 aud_clk;
};

struct phy_sts {
	uint32_t cable_clk;
	uint32_t tmds_clk;
	uint32_t aud_div;
	uint32_t pll_rate;
	uint32_t clk_rate;
	uint32_t phy_bw;
	uint32_t pll_bw;
	uint32_t cablesel;
	ulong timestap;
	uint32_t err_sum;
	uint32_t eq_data[256];
};

struct emp_buff {
	unsigned int dump_mode;
	struct page *pg_addr;
	phys_addr_t p_addr_a;
	phys_addr_t p_addr_b;
	/*void __iomem *v_addr_a;*/
	/*void __iomem *v_addr_b;*/
	void __iomem *store_a;
	void __iomem *store_b;
	void __iomem *ready;
	unsigned long irqcnt;
	unsigned int emppktcnt;
	unsigned int tmdspktcnt;
	bool end;
	u8 ogi_id;
	unsigned int emp_tagid;
	u8 emp_content_type;
	u8 data[128];
	u8 data_ver;
};

struct rx_s {
	enum chip_id_e chip_id;
	enum phy_ver_e phy_ver;
	struct hdmirx_dev_s *hdmirxdev;
	/** HDMI RX received signal changed */
	uint8_t skip;
	/*avmute*/
	uint32_t avmute_skip;
	/** HDMI RX input port 0 (A) or 1 (B) (or 2(C) or 3 (D)) */
	uint8_t port;
	/* first boot flag */
	/* workaround for xiaomi-MTK box: */
	/* if box is under suspend and it worked at hdcp2.2 mode before, */
	/* must do hpd reset and keep hpd low at least 2S to ensure hdcp2.2 */
	/* work normally, otherwise mibox's hdcp22 module will pull down SDA */
	/* and stop EDID communication.*/
	/* compare with LG & LETV, the result is the same. */
	bool boot_flag;
	bool open_fg;
	uint8_t irq_flag;
	bool firm_change;/*hdcp2.2 rp/rx switch time*/
	/** HDMI RX controller HDCP configuration */
	struct hdmi_rx_hdcp hdcp;
	/*report hpd status to app*/
	struct extcon_dev *rx_excton_rx22;
	struct extcon_dev *rx_excton_open;

	/* wrapper */
	unsigned int state;
	unsigned int fsm_ext_state;
	unsigned int pre_state;
	struct rx_fastswitch_mode fs_mode;
	/* recovery mode */
	unsigned char err_rec_mode;
	unsigned char err_code;
	unsigned char pre_5v_sts;
	unsigned char cur_5v_sts;
	bool no_signal;
	unsigned char err_cnt;
	uint16_t wait_no_sig_cnt;
	int aud_sr_stable_cnt;
	int aud_sr_unstable_cnt;
	unsigned long int timestamp;
	unsigned long int stable_timestamp;
	unsigned long int unready_timestamp;
	/* info */
	struct rx_video_info pre;
	struct rx_video_info cur;
	struct aud_info_s aud_info;
	struct vsi_info_s vs_info_details;
	struct tvin_hdr_info_s hdr_info;
	unsigned char edid_mix_buf[EDID_MIX_MAX_SIZE];
	unsigned int pwr_sts;
	/* for debug */
	/*struct pd_infoframe_s dbg_info;*/
	struct phy_sts phy;
	struct emp_buff empbuff;
	uint32_t arc_port;
	enum edid_ver_e edid_ver;
	bool arc_5vsts;
	u32 vsync_cnt;
#ifdef CONFIG_AMLOGIC_HDMITX
	struct notifier_block tx_notify;
#endif
	struct rx_var_param var;
	struct rx_aml_phy aml_phy;
	u8 last_hdcp22_state;
};

struct _hdcp_ksv {
	uint32_t bksv0;
	uint32_t bksv1;
};

struct reg_map {
	unsigned int phy_addr;
	unsigned int size;
	void __iomem *p;
	int flag;
};

/* system */
extern struct delayed_work	eq_dwork;
extern struct workqueue_struct	*eq_wq;
extern struct delayed_work	esm_dwork;
extern struct workqueue_struct	*esm_wq;
extern struct delayed_work	repeater_dwork;
extern struct work_struct	amlphy_dwork;
extern struct workqueue_struct	*amlphy_wq;
extern struct workqueue_struct	*repeater_wq;
extern struct tasklet_struct rx_tasklet;
extern struct device *hdmirx_dev;
extern struct rx_s rx;
extern struct reg_map reg_maps[MAP_ADDR_MODULE_NUM];
extern bool downstream_repeat_support;
extern void rx_tasklet_handler(unsigned long arg);
extern void skip_frame(unsigned int cnt);

/* reg */


/* hotplug */
extern unsigned int pwr_sts;
extern int pre_port;
extern void hotplug_wait_query(void);
extern void rx_send_hpd_pulse(void);

/* irq */
extern void rx_irq_en(bool enable);
extern irqreturn_t irq_handler(int irq, void *params);
extern void cecb_irq_handle(void);

/* user interface */
extern int pc_mode_en;
extern int it_content;
extern int rgb_quant_range;
extern int yuv_quant_range;
extern int en_4k_timing;
extern int cec_dev_en;
extern bool dev_is_appletv_v2;
extern bool en_4096_2_3840;
extern int en_4k_2_2k;
extern bool hdmi_cec_en;
extern int hdmi_yuv444_enable;
extern int vdin_drop_frame_cnt;
extern int aud_compose_type;
/* debug */
extern bool hdcp_enable;
extern int log_level;
extern int sm_pause;
extern int suspend_pddq_sel;
extern int disable_port_num;
extern int disable_port_en;
extern bool video_stable_to_esm;
extern bool pwr_sts_to_esm;
extern bool enable_hdcp22_esm_log;
extern bool esm_reset_flag;
extern bool esm_auth_fail_en;
extern bool esm_error_flag;
extern bool hdcp22_stop_auth;
extern bool hdcp22_esm_reset2;
extern int esm_recovery_mode;

extern int rx_set_global_variable(const char *buf, int size);
extern void rx_get_global_variable(const char *buf);
extern int rx_pr(const char *fmt, ...);
extern unsigned int hdmirx_hw_dump_reg(unsigned char *buf, int size);
extern unsigned int hdmirx_show_info(unsigned char *buf, int size);
extern bool is_afifo_error(void);
extern bool is_aud_pll_error(void);
extern int hdmirx_debug(const char *buf, int size);
extern void dump_reg(void);
extern void dump_edid_reg(void);
extern void rx_debug_loadkey(void);
extern void rx_debug_load22key(void);
extern int rx_debug_wr_reg(const char *buf, char *tmpbuf, int i);
extern int rx_debug_rd_reg(const char *buf, char *tmpbuf);
void rx_update_sig_info(void);

/* repeater */
bool hdmirx_repeat_support(void);

/* edid-hdcp14 */
extern unsigned int edid_update_flag;
extern unsigned int downstream_hpd_flag;

extern void hdmirx_fill_edid_buf(const char *buf, int size);
void hdmirx_fill_edid_with_port_buf(const char *buf, int size);
extern unsigned int hdmirx_read_edid_buf(char *buf, int max_size);
extern void hdmirx_fill_key_buf(const char *buf, int size);
extern int rx_audio_block_len;
extern u8 rx_audio_block[MAX_AUDIO_BLK_LEN];
/* packets */
extern unsigned int packet_fifo_cfg;
extern unsigned int *pd_fifo_buf;

/* for other modules */
extern int External_Mute(int mute_flag);
extern int rx_is_hdcp22_support(void);
#endif
