#ifndef __CHEBY__TOP__H__
#define __CHEBY__TOP__H__

#include "adc_serial_regs.h"
#include "dac_serial_regs.h"
#include "in_out_regs.h"
#include "loadable_counter_regs.h"
#define TOP_SIZE 592 /* 0x250 */

/* None */
#define TOP_ADC_0           0x0UL
#define ADDR_MASK_TOP_ADC_0 0x380UL
#define TOP_ADC_0_SIZE      128 /* 0x80 */

/* None */
#define TOP_ADC_1           0x80UL
#define ADDR_MASK_TOP_ADC_1 0x380UL
#define TOP_ADC_1_SIZE      128 /* 0x80 */

/* None */
#define TOP_DAC_0           0x100UL
#define ADDR_MASK_TOP_DAC_0 0x380UL
#define TOP_DAC_0_SIZE      128 /* 0x80 */

/* None */
#define TOP_DAC_1           0x180UL
#define ADDR_MASK_TOP_DAC_1 0x380UL
#define TOP_DAC_1_SIZE      128 /* 0x80 */

/* None */
#define TOP_LEDS           0x200UL
#define ADDR_MASK_TOP_LEDS 0x3f0UL
#define TOP_LEDS_SIZE      16 /* 0x10 */

/* None */
#define TOP_BCKPL           0x210UL
#define ADDR_MASK_TOP_BCKPL 0x3f0UL
#define TOP_BCKPL_SIZE      16 /* 0x10 */

/* None */
#define TOP_GPIO_0           0x220UL
#define ADDR_MASK_TOP_GPIO_0 0x3f0UL
#define TOP_GPIO_0_SIZE      16 /* 0x10 */

/* None */
#define TOP_GPIO_1           0x230UL
#define ADDR_MASK_TOP_GPIO_1 0x3f0UL
#define TOP_GPIO_1_SIZE      16 /* 0x10 */

/* None */
#define TOP_CNTR           0x240UL
#define ADDR_MASK_TOP_CNTR 0x3f0UL
#define TOP_CNTR_SIZE      16 /* 0x10 */

struct top
{
    /* [0x0]: SUBMAP (no description) */
    struct adc_serial_regs adc_0;

    /* [0x80]: SUBMAP (no description) */
    struct adc_serial_regs adc_1;

    /* [0x100]: SUBMAP (no description) */
    struct dac_serial_regs dac_0;

    /* [0x180]: SUBMAP (no description) */
    struct dac_serial_regs dac_1;

    /* [0x200]: SUBMAP (no description) */
    struct in_out_regs leds;

    /* padding to: 132 words */
    uint32_t __padding_0[1];

    /* [0x210]: SUBMAP (no description) */
    struct in_out_regs bckpl;

    /* padding to: 136 words */
    uint32_t __padding_1[1];

    /* [0x220]: SUBMAP (no description) */
    struct in_out_regs gpio_0;

    /* padding to: 140 words */
    uint32_t __padding_2[1];

    /* [0x230]: SUBMAP (no description) */
    struct in_out_regs gpio_1;

    /* padding to: 144 words */
    uint32_t __padding_3[1];

    /* [0x240]: SUBMAP (no description) */
    struct loadable_counter_regs cntr;
};

#endif /* __CHEBY__TOP__H__ */
