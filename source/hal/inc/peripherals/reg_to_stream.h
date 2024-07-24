#ifndef __CHEBY__REG_TO_STREAM__H__
#define __CHEBY__REG_TO_STREAM__H__
#define REG_TO_STREAM_SIZE 2048 /* 0x800 = 2KB */

/* Control register */
#define REG_TO_STREAM_CTRL              0x0UL
#define REG_TO_STREAM_CTRL_RST          0x1UL
#define REG_TO_STREAM_CTRL_RST_PRESET   0x0UL
#define REG_TO_STREAM_CTRL_START        0x2UL
#define REG_TO_STREAM_CTRL_START_PRESET 0x0UL

/* How many data words should be sent. */
#define REG_TO_STREAM_NUM_DATA 0x4UL

/* Last tkeep to be send. Only the 8 lower bits are used. */
#define REG_TO_STREAM_TKEEP 0x8UL

/* Data received from Aurora */
#define REG_TO_STREAM_DATA      0x400UL
#define REG_TO_STREAM_DATA_SIZE 4 /* 0x4 */

/* None */
#define REG_TO_STREAM_DATA_VALUE 0x0UL

#ifndef __ASSEMBLER__

#include <cstdint>

struct reg_to_stream
{
    /* [0x0]: REG (rw) Control register */
    uint32_t ctrl;

    /* [0x4]: REG (rw) How many data words should be sent. */
    uint32_t num_data;

    /* [0x8]: REG (rw) Last tkeep to be send. Only the 8 lower bits are used. */
    uint32_t tkeep;

    /* padding to: 1024 Bytes */
    uint32_t __padding_0[253];

    /* [0x400]: REPEAT Data received from Aurora */
    struct data
    {
        /* [0x0]: REG (rw) (no description) */
        uint32_t value;
    } data[200];

    /* padding to: 1024 Bytes */
    uint32_t __padding_1[56];
};
#endif /* !__ASSEMBLER__*/

#endif /* __CHEBY__REG_TO_STREAM__H__ */
