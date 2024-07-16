#ifndef __CHEBY__STREAM_TO_REG__H__
#define __CHEBY__STREAM_TO_REG__H__
#define STREAM_TO_REG_SIZE 3072 /* 0xc00 = 3KB */

/* Control register */
#define STREAM_TO_REG_CTRL                   0x0UL
#define STREAM_TO_REG_CTRL_RESET_PB          0x1UL
#define STREAM_TO_REG_CTRL_RESET_PB_PRESET   0x1UL
#define STREAM_TO_REG_CTRL_PMA_INIT          0x2UL
#define STREAM_TO_REG_CTRL_PMA_INIT_PRESET   0x1UL
#define STREAM_TO_REG_CTRL_LOOPBACK_MASK     0x70UL
#define STREAM_TO_REG_CTRL_LOOPBACK_SHIFT    4
#define STREAM_TO_REG_CTRL_LOOPBACK_PRESET   0x0UL
#define STREAM_TO_REG_CTRL_SEL_OUTPUT        0x80UL
#define STREAM_TO_REG_CTRL_SEL_OUTPUT_PRESET 0x0UL

/* Status register */
#define STREAM_TO_REG_STATUS              0x4UL
#define STREAM_TO_REG_STATUS_CHANNEL_UP   0x1UL
#define STREAM_TO_REG_STATUS_GT_PLL_LOCK  0x2UL
#define STREAM_TO_REG_STATUS_LANE_UP      0x4UL
#define STREAM_TO_REG_STATUS_PLL_LOCKED   0x8UL
#define STREAM_TO_REG_STATUS_SOFT_ERR     0x10UL
#define STREAM_TO_REG_STATUS_HARD_ERR     0x20UL
#define STREAM_TO_REG_STATUS_GT_POWERGOOD 0x40UL
#define STREAM_TO_REG_STATUS_GT_RESET     0x80UL
#define STREAM_TO_REG_STATUS_LINK_RESET   0x100UL
#define STREAM_TO_REG_STATUS_SYS_RESET    0x200UL
#define STREAM_TO_REG_STATUS_ALMOST_FULL  0x400UL
#define STREAM_TO_REG_STATUS_PROG_FULL    0x800UL

/* How many data words have been received. */
#define STREAM_TO_REG_NUM_DATA 0x8UL

/* Data received from Aurora */
#define STREAM_TO_REG_DATA      0x400UL
#define STREAM_TO_REG_DATA_SIZE 4 /* 0x4 */

/* None */
#define STREAM_TO_REG_DATA_VALUE 0x0UL

/* TKEEPs received from Aurora */
#define STREAM_TO_REG_KEEP      0x800UL
#define STREAM_TO_REG_KEEP_SIZE 4 /* 0x4 */

/* None */
#define STREAM_TO_REG_KEEP_VALUE 0x0UL

#ifndef __ASSEMBLER__
struct stream_to_reg
{
    /* [0x0]: REG (rw) Control register */
    uint32_t ctrl;

    /* [0x4]: REG (ro) Status register */
    uint32_t status;

    /* [0x8]: REG (ro) How many data words have been received. */
    uint32_t num_data;

    /* padding to: 1024 Bytes */
    uint32_t __padding_0[253];

    /* [0x400]: REPEAT Data received from Aurora */
    struct data
    {
        /* [0x0]: REG (ro) (no description) */
        uint32_t value;
    } data[200];

    /* padding to: 1024 Bytes */
    uint32_t __padding_1[56];

    /* [0x800]: REPEAT TKEEPs received from Aurora */
    struct keep
    {
        /* [0x0]: REG (ro) (no description) */
        uint32_t value;
    } keep[200];

    /* padding to: 2048 Bytes */
    uint32_t __padding_2[56];
};
#endif /* !__ASSEMBLER__*/

#endif /* __CHEBY__STREAM_TO_REG__H__ */
