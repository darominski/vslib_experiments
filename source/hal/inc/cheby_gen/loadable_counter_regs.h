#ifndef __CHEBY__LOADABLE_COUNTER_REGS__H__
#define __CHEBY__LOADABLE_COUNTER_REGS__H__
#define LOADABLE_COUNTER_REGS_SIZE 16 /* 0x10 */

/* Control register */
#define LOADABLE_COUNTER_REGS_CTRL            0x0UL
#define LOADABLE_COUNTER_REGS_CTRL_RESET      0x1UL
#define LOADABLE_COUNTER_REGS_CTRL_EN         0x2UL
#define LOADABLE_COUNTER_REGS_CTRL_USE_EXT_EN 0x4UL

/* Value to be loaded */
#define LOADABLE_COUNTER_REGS_LOAD_VAL        0x4UL
#define LOADABLE_COUNTER_REGS_LOAD_VAL_PRESET 0x0UL

/* Status register */
#define LOADABLE_COUNTER_REGS_STATUS               0x8UL
#define LOADABLE_COUNTER_REGS_STATUS_API_VER_MASK  0xfUL
#define LOADABLE_COUNTER_REGS_STATUS_API_VER_SHIFT 0
#define LOADABLE_COUNTER_REGS_STATUS_USES_EXT_CLK  0x10UL

/* Current counter value */
#define LOADABLE_COUNTER_REGS_COUNTER 0xcUL

struct loadable_counter_regs
{
    /* [0x0]: REG (rw) Control register */
    uint32_t ctrl;

    /* [0x4]: REG (wo) Value to be loaded */
    uint32_t load_val;

    /* [0x8]: REG (ro) Status register */
    uint32_t status;

    /* [0xc]: REG (ro) Current counter value */
    uint32_t counter;
};

#endif /* __CHEBY__LOADABLE_COUNTER_REGS__H__ */
