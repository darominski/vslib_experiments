#ifndef __CHEBY__ADC_SERIAL_REGS__H__
#define __CHEBY__ADC_SERIAL_REGS__H__
#define ADC_SERIAL_REGS_SIZE 128 /* 0x80 */

/* Control register */
#define ADC_SERIAL_REGS_CTRL       0x0UL
#define ADC_SERIAL_REGS_CTRL_RESET 0x1UL
#define ADC_SERIAL_REGS_CTRL_START 0x2UL

/* ADC interface configuration */
#define ADC_SERIAL_REGS_CONFIG                  0x4UL
#define ADC_SERIAL_REGS_CONFIG_CPOL             0x1UL
#define ADC_SERIAL_REGS_CONFIG_CPHA             0x2UL
#define ADC_SERIAL_REGS_CONFIG_CNV_POL          0x4UL
#define ADC_SERIAL_REGS_CONFIG_CNV_WITH_CS      0x8UL
#define ADC_SERIAL_REGS_CONFIG_BUSY_POL         0x10UL
#define ADC_SERIAL_REGS_CONFIG_BUSY_SRC_MASK    0x60UL
#define ADC_SERIAL_REGS_CONFIG_BUSY_SRC_SHIFT   5
#define ADC_SERIAL_REGS_CONFIG_BUSY_TIME_MASK   0x1f80UL
#define ADC_SERIAL_REGS_CONFIG_BUSY_TIME_SHIFT  7
#define ADC_SERIAL_REGS_CONFIG_CLK_ACT_IN_CONV  0x2000UL
#define ADC_SERIAL_REGS_CONFIG_DATA_WIDTH_MASK  0x1f0000UL
#define ADC_SERIAL_REGS_CONFIG_DATA_WIDTH_SHIFT 16

/* Status register */
#define ADC_SERIAL_REGS_STATUS                     0x8UL
#define ADC_SERIAL_REGS_STATUS_API_VER_MASK        0xfUL
#define ADC_SERIAL_REGS_STATUS_API_VER_SHIFT       0
#define ADC_SERIAL_REGS_STATUS_DIN_PORTS_MASK      0xf0UL
#define ADC_SERIAL_REGS_STATUS_DIN_PORTS_SHIFT     4
#define ADC_SERIAL_REGS_STATUS_BUSY                0x100UL
#define ADC_SERIAL_REGS_STATUS_CLK_PRESCALER_MASK  0x1f0000UL
#define ADC_SERIAL_REGS_STATUS_CLK_PRESCALER_SHIFT 16

/* Data received from ADC */
#define ADC_SERIAL_REGS_DATA      0x40UL
#define ADC_SERIAL_REGS_DATA_SIZE 4 /* 0x4 */

/* None */
#define ADC_SERIAL_REGS_DATA_VALUE 0x0UL

struct adc_serial_regs
{
    /* [0x0]: REG (rw) Control register */
    uint32_t ctrl;

    /* [0x4]: REG (rw) ADC interface configuration */
    uint32_t config;

    /* [0x8]: REG (ro) Status register */
    uint32_t status;

    /* padding to: 16 words */
    uint32_t __padding_0[13];

    /* [0x40]: REPEAT Data received from ADC */
    struct data
    {
        /* [0x0]: REG (ro) (no description) */
        uint32_t value;
    } data[16];
};

#endif /* __CHEBY__ADC_SERIAL_REGS__H__ */
