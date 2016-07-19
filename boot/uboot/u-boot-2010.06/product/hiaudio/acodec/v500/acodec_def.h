#ifndef _ACODEC_DEF_H_
#define _ACODEC_DEF_H_

#define ACODEC_REGS_CRG  0x2003008c
#define ACODEC_REGS_BASE 0x201200c8                      
#define ACODEC_REGS_SIZE 0X10000

#define ACODEC_CTRL0_ADDR   (0x0 + ACODEC_REGS_BASE)     
#define ACODEC_CTRL1_ADDR   (0x4 + ACODEC_REGS_BASE) 
#define ACODEC_DACREG0_ADDR (0x8 + ACODEC_REGS_BASE)
#define ACODEC_DACREG1_ADDR (0xc + ACODEC_REGS_BASE)
#define ACODEC_ADCREG0_ADDR (0x10 + ACODEC_REGS_BASE)
#define ACODEC_ADCREG1_ADDR (0x14 + ACODEC_REGS_BASE)
#define ACODEC_CTRL56_ADDR  (0x18 + ACODEC_REGS_BASE)

typedef union hiACODEC_CTRL0_u
{
    struct
    {
        unsigned int clk_delay_sel     :1;   //[0]
        unsigned int gainboostr        :1;   //[1]
        unsigned int gain_micr         :5;   //[6:2]
        unsigned int lineinr_sel       :1;   //[7]
        unsigned int clk_timing_sel    :1;   //[8]
        unsigned int gainboostl        :1;   //[9]
        unsigned int gain_micl         :5;   //[14:10]
        unsigned int lineinl_sel       :1;   //[15]
        unsigned int dacclk_sel        :1;   //[16]
        unsigned int mclk_ana_sel      :1;   //[17]
        unsigned int mute_adcr         :1;   //[18]
        unsigned int mute_adcl         :1;   //[19]
        unsigned int popfreer          :1;   //[20]
        unsigned int popfreel          :1;   //[21]
        unsigned int mute_dacr         :1;   //[22]
        unsigned int mute_dacl         :1;   //[23]
        unsigned int ana_loop          :1;   //[24]
        unsigned int fstartup          :1;   //[25]
        unsigned int pd_vref           :1;   //[26]
        unsigned int pd_micbias        :1;   //[27]
        unsigned int pd_dacr           :1;   //[28]
        unsigned int pd_dacl           :1;   //[29]
        unsigned int pd_adcr           :1;   //[30]
        unsigned int pd_adcl           :1;   //[31]
    } bits;
    unsigned int ul32;    
} ACODEC_CTRLO_U;

 
typedef union hiACODEC_CTRL1_u
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
} ACODEC_CTRL1_U;
 

typedef union hiACODEC_DACREG0_u
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
} ACODEC_DACREG0_U;

typedef union hiACODEC_DACREG1_u
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
} ACODEC_DACREG1_U;

typedef union hiACODEC_ADCREG0_u
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
} ACODEC_ADCREG0_U;

typedef union hiACODEC_ADCREG1_u
{
    struct
    {
        unsigned int adcr2dacr_vol    :7;  //[6:0]
        unsigned int adcr2dacr_en     :1;  //[7]
        unsigned int adcl2dacr_vol    :7;  //[14:8]
        unsigned int adcl2dacr_en     :1;  //[15]
        unsigned int adcr2dacl_vol    :7;  //[22:16]
        unsigned int adcr2dacl_en     :1;  //[23]
        unsigned int adcl2dacl_vol    :7;  //[30:24]
        unsigned int adcl2dacl_en     :1;  //[31]    
    } bits;
    unsigned int ul32; 
} ACODEC_ADCREG1_U;

typedef union hiACODEC_CTRL56_U
{
    struct
    {       
        unsigned int audio_enable      :1;  //[0]
        unsigned int i2s_pad_enable    :1;  //[1]
        unsigned int audio_mclk_sel    :1;  //[2]
        unsigned int i2s_pad_mclk_sel  :1;  //[3]
        unsigned int Reserved         :28; //[31:4]
    } bits;
    unsigned int ul32; 
} ACODEC_CTRL56_U;

typedef struct hiACODEC_REGS_s
{
    volatile ACODEC_CTRLO_U     acodec_ctrl0;           /*0x0*/
    volatile ACODEC_CTRL1_U     acodec_ctrl1;           /*0x4*/
    volatile ACODEC_DACREG0_U   acodec_dacreg0;         /*0x8*/
    volatile ACODEC_DACREG1_U   acodec_dacreg1;         /*0xc*/
    volatile ACODEC_ADCREG0_U   acodec_adcreg0;         /*0x10*/
    volatile ACODEC_ADCREG1_U   acodec_adcreg1;         /*0x14*/
    volatile ACODEC_CTRL56_U    acodec_ctrl56;         /*0x18*/
} ACODEC_REGS_S;

#endif /* End of #ifndef _ACODEC_H */

