#ifndef __CHEBY__IN_OUT_REGS__H__
#define __CHEBY__IN_OUT_REGS__H__
#define IN_OUT_REGS_SIZE 12 /* 0xc */

/* Data Direction Register ('0' - input, '1' - output) */
#define IN_OUT_REGS_DDR        0x0UL
#define IN_OUT_REGS_DDR_PRESET 0x0UL

/* Port Input Register (read input state when configured as input) */
#define IN_OUT_REGS_PIR 0x4UL

/* Port Output Register (set output state when configured as output) */
#define IN_OUT_REGS_POR        0x8UL
#define IN_OUT_REGS_POR_PRESET 0x0UL

struct in_out_regs
{
    /* [0x0]: REG (rw) Data Direction Register ('0' - input, '1' - output) */
    uint32_t DDR;

    /* [0x4]: REG (ro) Port Input Register (read input state when configured as input) */
    uint32_t PIR;

    /* [0x8]: REG (rw) Port Output Register (set output state when configured as output) */
    uint32_t POR;
};

#endif /* __CHEBY__IN_OUT_REGS__H__ */
