#ifndef __CHEBY__DAC_SERIAL_REGS__H__
#define __CHEBY__DAC_SERIAL_REGS__H__
#define DAC_SERIAL_REGS_SIZE 128 /* 0x80 */

/* Control register */
#define DAC_SERIAL_REGS_CTRL       0x0UL
#define DAC_SERIAL_REGS_CTRL_RESET 0x1UL
#define DAC_SERIAL_REGS_CTRL_START 0x2UL

/* DAC interface configuration */
#define DAC_SERIAL_REGS_CONFIG                       0x4UL
#define DAC_SERIAL_REGS_CONFIG_CPOL                  0x1UL
#define DAC_SERIAL_REGS_CONFIG_CPHA                  0x2UL
#define DAC_SERIAL_REGS_CONFIG_MSB_FIRST             0x4UL
#define DAC_SERIAL_REGS_CONFIG_PRE_PAD_MASK          0x38UL
#define DAC_SERIAL_REGS_CONFIG_PRE_PAD_SHIFT         3
#define DAC_SERIAL_REGS_CONFIG_POST_PAD_MASK         0x1c0UL
#define DAC_SERIAL_REGS_CONFIG_POST_PAD_SHIFT        6
#define DAC_SERIAL_REGS_CONFIG_DATA_WIDTH_MASK       0x3e00UL
#define DAC_SERIAL_REGS_CONFIG_DATA_WIDTH_SHIFT      9
#define DAC_SERIAL_REGS_CONFIG_DEAD_TIME_MASK        0xfc000UL
#define DAC_SERIAL_REGS_CONFIG_DEAD_TIME_SHIFT       14
#define DAC_SERIAL_REGS_CONFIG_CLK_PRES_MASK         0x3f00000UL
#define DAC_SERIAL_REGS_CONFIG_CLK_PRES_SHIFT        20
#define DAC_SERIAL_REGS_CONFIG_FIX_ADD_CLK_CYC_MASK  0x3c000000UL
#define DAC_SERIAL_REGS_CONFIG_FIX_ADD_CLK_CYC_SHIFT 26

/* Status register */
#define DAC_SERIAL_REGS_STATUS                  0x8UL
#define DAC_SERIAL_REGS_STATUS_API_VER_MASK     0xfUL
#define DAC_SERIAL_REGS_STATUS_API_VER_SHIFT    0
#define DAC_SERIAL_REGS_STATUS_DOUT_PORTS_MASK  0xf0UL
#define DAC_SERIAL_REGS_STATUS_DOUT_PORTS_SHIFT 4
#define DAC_SERIAL_REGS_STATUS_BUSY             0x100UL

/* Data to be sent to DAC */
#define DAC_SERIAL_REGS_DATA      0x40UL
#define DAC_SERIAL_REGS_DATA_SIZE 4 /* 0x4 */

/* None */
#define DAC_SERIAL_REGS_DATA_VALUE 0x0UL

struct dac_serial_regs
{
    /* [0x0]: REG (rw) Control register */
    uint32_t ctrl;

    /* [0x4]: REG (rw) DAC interface configuration */
    uint32_t config;

    /* [0x8]: REG (ro) Status register */
    uint32_t status;

    /* padding to: 16 words */
    uint32_t __padding_0[13];

    /* [0x40]: REPEAT Data to be sent to DAC */
    struct data
    {
        /* [0x0]: REG (wo) (no description) */
        uint32_t value;
    } data[16];
};

#endif /* __CHEBY__DAC_SERIAL_REGS__H__ */
