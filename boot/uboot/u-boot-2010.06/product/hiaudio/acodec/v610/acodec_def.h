#ifndef _ACODEC_DEF_H_
#define _ACODEC_DEF_H_

#define ACODEC_REGS_CRG  0x12010080  //aio crg
#define AIAO_CLK_TX0_CRG 0x11080140
#define AIAO_CLK_TX0_CFG 0x11080144

#define ACODEC_REGS_BASE 0x120300A0  //acodec base address

/*acodec fast power on/off*/
#define ACODE_FASTPWR0_ADDR (0x0)
#define ACODE_FASTPWR1_ADDR (0x4)
#define ACODE_FASTPWR2_ADDR (0x8)
#define ACODE_FASTPWR3_ADDR (0xC)
#define ACODE_FASTPWR4_ADDR (0x10)

/*acodec analog register*/
#define ACODEC_ANAREG0_ADDR (0x14)
#define ACODEC_ANAREG1_ADDR (0x18)
#define ACODEC_ANAREG2_ADDR (0x1C)
#define ACODEC_ANAREG3_ADDR (0x20)
#define ACODEC_ANAREG56_ADDR  (0x40)

/*acodec dig control register*/
#define ACODEC_DIGCTRL0_ADDR (0x2C) 
#define ACODEC_DIGCTRL1_ADDR (0x30)
#define ACODEC_DIGCTRL2_ADDR (0x34)
#define ACODEC_DIGCTRL3_ADDR (0x38)

#define ACODEC_ANAREG0_DEFAULT 0x6400FCFD
#define ACODEC_ANAREG1_DEFAULT 0x00000034
#define ACODEC_ANAREG2_DEFAULT 0x4018088D
#define ACODEC_ANAREG3_DEFAULT 0x00000000

typedef union hiACODEC_ANAREG0_u
{
    struct
    {
        unsigned int acodec_pd_vref                     :1;     //[0]
        unsigned int acodec_pdb_ctcm_ibias              :1;     //[1]
        unsigned int acodec_pd_adcl                     :1;     //[2]
        unsigned int acodec_pd_adcr                     :1;     //[3]
        unsigned int acodec_pd_linein_l                 :1;     //[4]
        unsigned int acodec_pd_linein_r                 :1;     //[5]
        unsigned int acodec_pd_micbias1                 :1;     //[6]
        unsigned int acodec_pd_micbias2                 :1;     //[7]
        unsigned int acodec_pd_dacl_dff                 :1;     //[8]
        unsigned int acodec_pd_dacr_dff                 :1;     //[9]
        unsigned int acodec_pd_lineout_d                :1;     //[10]
        unsigned int acodec_pd_dacl                     :1;     //[11]
        unsigned int acodec_pd_dacr                     :1;     //[12]
        unsigned int acodec_mute_dacl                   :1;     //[13]
        unsigned int acodec_mute_dacr                   :1;     //[14]
        unsigned int acodec_mute_linout_d               :1;     //[15]
        unsigned int acodec_popfreer                    :1;     //[16]
        unsigned int acodec_popfreed                    :1;     //[17]
        unsigned int acodec_popfreel                    :1;     //[18]
        unsigned int acodec_ana_loop                    :1;     //[19]
        unsigned int acodec_byp_chop_dac_vref           :1;     //[20]
        unsigned int acodec_byp_chop_lineout_d          :1;     //[21]
        unsigned int acodec_sel_clk_chop_dac_vref       :2;     //[23:22]
        unsigned int acodec_sel1_lineout_d              :1;     //[24]
        unsigned int acodec_sel2_lineout_d              :1;     //[25]
        unsigned int acodec_ibadj_dac                   :2;     //[27:26]
        unsigned int acodec_ctrl_clk_dac_ph             :1;     //[28]
        unsigned int acodec_sel_clk_chop_lineout_d      :2;     //[30:29]
        unsigned int acodec_ibadj_ctcm                  :1;     //[31]       
        
    }bits;
    unsigned int ul32;
    
}ACODEC_ANAREG0_U;

typedef union hiACODEC_ANAREG1_u
{
    struct
    {      
        unsigned int acodec_byp_chop_ctcm               :1;     //[0] 
        unsigned int acodec_sel_clk_chop_ctcm           :1;     //[1]
        unsigned int acodec_ibadj_adc                   :2;     //[3:2]
        unsigned int acodec_mute_lineinl                :1;     //[4]
        unsigned int acodec_mute_lineinr                :1;     //[5]
        unsigned int acodec_ctrl_clk_adc_ph             :1;     //[6]
        unsigned int acodec_ctrl_mclk_ph                :1;     //[7]
        unsigned int acodec_linein_l_gain_codec         :5;     //[12:8]
        unsigned int acodec_adcl_mode_sel               :1;     //[13]
        unsigned int acodec_adcl_gain_boost             :1;     //[14]
        unsigned int acodec_adcl_dwa_byp                :1;     //[15]
        unsigned int acodec_linein_r_gain_codec         :5;     //[20:16]
        unsigned int acodec_adcr_mode_sel               :1;     //[21]
        unsigned int acodec_adcr_gain_boost             :1;     //[22]
        unsigned int acodec_adcr_dwa_byp                :1;     //[23]
        unsigned int acodec_linein_l_sel                :4;     //[27:24]
        unsigned int acodec_linein_r_sel                :4;     //[31:28]
    }bits;
    unsigned int ul32;
    
}ACODEC_ANAREG1_U;

typedef union hiACODEC_ANAREG2_u
{
    struct
    {      
        unsigned int acodec_adc_flstn                   :2;     //[1:0] 
        unsigned int acodec_adcl_selclk_a2d_inv         :1;     //[2]
        unsigned int acodec_adcr_selclk_a2d_inv         :1;     //[3]
        unsigned int acodec_adc_clkdly_sel              :1;     //[4]
        unsigned int acodec_clktiming_sel               :1;     //[5]
        unsigned int acodec_adc_adatn                   :2;     //[7:6]
        unsigned int acodec_byp_chop_linein             :1;     //[8]
        unsigned int acodec_byp_chop_adc_st1            :1;     //[9]
        unsigned int acodec_byp_chop_adc_st2            :1;     //[10]
        unsigned int acodec_sel_clk_chop_adc_ph         :1;     //[11]
        unsigned int acodec_sel_clk_chop_adc_st1        :2;     //[13:12]
        unsigned int acodec_sel_clk_chop_adc_st2        :2;     //[15:14]
        unsigned int acodec_sel_clk_chop_linein         :1;     //[16]
        unsigned int acodec_vref_fs                     :1;     //[17]
        unsigned int acodec_vref_sel                    :5;     //[22:18]
        unsigned int acodec_rst                         :1;     //[23]
        unsigned int acodec_micbias1_adj                :3;     //[26:24]
        unsigned int acodec_micbias2_adj                :3;     //[29:27]
        unsigned int acodec_pd_ctcm                     :1;     //[30]
        unsigned int acodec_lineoutd_gsel               :1;     //[31]
    }bits;
    unsigned int ul32;
    
}ACODEC_ANAREG2_U;

typedef union hiACODEC_ANAREG3_u
{
    struct
    {              
        unsigned int acodec_dacr_pop_direct             :1;     //[0]
        unsigned int acodec_dacd_pop_direct             :1;     //[1]
        unsigned int acodec_dacl_pop_direct             :1;     //[2]
        unsigned int acodec_popres_sel                  :2;     //[4:3]
        unsigned int acodec_poprampclk_sel              :2;     //[6:5]
        unsigned int acodec_vref_exmode                 :1;     //[7]
        unsigned int reserved                           :24;    //[31:8]
    }bits;
    unsigned int ul32;
    
}ACODEC_ANAREG3_U;

typedef union hiACODEC_DIGCTRL0_u
{
    struct
    {        
        unsigned int ibadj_adc        :2;  //[1:0]
        unsigned int ibadj_dac        :1;  //[2]
        unsigned int ibadj_ctcm       :1;  //[3]
        unsigned int adc_adatn        :2;  //[5:4]
        unsigned int adc_flstn        :2;  //[7:6]
        unsigned int i2s2_fs_sel      :5;  //[12:8]
        unsigned int i2s1_fs_sel      :5;  //[17:13]
        unsigned int dig_loop         :1;  //[18]
        unsigned int dig_bypass       :1;  //[19]
        unsigned int i2s2_data_bits   :2;  //[21:20]
        unsigned int i2s1_data_bits   :2;  //[23:22]
        unsigned int adcr_en          :1;  //[24]
        unsigned int adcl_en          :1;  //[25]
        unsigned int dacr_en          :1;  //[26]
        unsigned int dacl_en          :1;  //[27]
        unsigned int adcr_rst_n       :1;  //[28]
        unsigned int adcl_rst_n       :1;  //[29]
        unsigned int dacr_rst_n       :1;  //[30]
        unsigned int dacl_rst_n       :1;  //[31]
    }bits;
    unsigned int ul32; 
} ACODEC_DIGCTRL0_U;
 

typedef union hiACODEC_DIGCTRL1_u
{
    struct
    {       
        unsigned int dacr_lrsel       :1;  //[0]
        unsigned int dacr_i2ssel      :1;  //[1]
        unsigned int dacl_lrsel       :1;  //[2]
        unsigned int dacl_i2ssel      :1;  //[3]
        unsigned int Reserved         :15; //[18:4]
        unsigned int dacr_deemph      :2;  //[20:19]
        unsigned int dacl_deemph      :2;  //[22:21]
        unsigned int muter_rate       :2;  //[24:23]
        unsigned int mutel_rate       :2;  //[26:25]
        unsigned int dacvu            :1;  //[27]
        unsigned int sunmuter         :1;  //[28]
        unsigned int sunmutel         :1;  //[29]
        unsigned int smuter           :1;  //[30]
        unsigned int smutel           :1;  //[31]
    } bits;
    unsigned int ul32; 
} ACODEC_DIGCTRL1_U;

typedef union hiACODEC_DIGCTRL2_u
{
    struct
    {        
        unsigned int dacl2dacr_vol    :7;  //[6:0]
        unsigned int dacl2dacr_en     :1;  //[7]
        unsigned int dacr2dacl_vol    :7;  //[14:8]
        unsigned int dacr2dacl_en     :1;  //[15]
        unsigned int dacr_vol         :7;  //[22:16]
        unsigned int dacr_mute        :1;  //[23]
        unsigned int dacl_vol         :7;  //[30:24]
        unsigned int dacl_mute        :1;  //[31]
    } bits;
    unsigned int ul32; 
} ACODEC_DIGCTRL2_U;

typedef union hiACODEC_DIGCTRL3_u
{
    struct
    {        
        unsigned int adcr_lrsel       :1;  //[0]
        unsigned int adcr_i2ssel      :1;  //[1]
        unsigned int adcl_lrsel       :1;  //[2]
        unsigned int adcl_i2ssel      :1;  //[3]
        unsigned int Reserved         :10; //[13:4]
        unsigned int adcr_hpf_en      :1;  //[14]
        unsigned int adcl_hpf_en      :1;  //[15]
        unsigned int adcr_vol         :7;  //[22:16]
        unsigned int adcr_mute        :1;  //[23]
        unsigned int adcl_vol         :7;  //[30:24]
        unsigned int adcl_mute        :1;  //[31]
    } bits;
    unsigned int ul32; 
} ACODEC_DIGCTRL3_U;

typedef union hiACODEC_DIGCTRL56_U
{
    struct
    {       
        unsigned int audio_enable      :1;  //[0]
        unsigned int i2s_pad_enable   :1;  //[1]
        unsigned int Reserved             :30; //[31:2]
    } bits;
    unsigned int ul32; 
} ACODEC_DIGCTRL56_U;

typedef struct hiACODEC_REGS_s
{
    volatile ACODEC_DIGCTRL0_U   acodec_digctrl0;
    volatile ACODEC_DIGCTRL1_U   acodec_digctrl1;
    volatile ACODEC_DIGCTRL2_U   acodec_digctrl2;
    volatile ACODEC_DIGCTRL3_U   acodec_digctrl3;
    volatile ACODEC_DIGCTRL56_U acodec_digctrl56;         /*0x18*/
} ACODEC_REGS_S;

#endif /* End of #ifndef _ACODEC_H */

