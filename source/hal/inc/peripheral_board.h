#pragma once

#include <array>
#include <sleep.h>

#include "peripherals/adc_serial.h"
#include "peripherals/dac_serial.h"
#include "peripherals/in_out.h"

#define PB0_BASE 0x48'0000'0000UL
#define PB1_BASE 0x49'0000'0000UL

enum
{
    // Analog mux channel IDs
    CHAN_IN1      = 0,
    CHAN_IN2      = 1,
    CHAN_DAC2     = 2,
    CHAN_DAC1     = 3,
    CHAN_GND      = 4,   // connected to 5 too
    CHAN_REF_10VP = 6,
    CHAN_REF_10VN = 7,
};

enum
{
    SW_SRC_IN  = 0,
    SW_SRC_MUX = 1,
};

enum
{
    MPXA_EN_BIT = 23,
    MPXA_BIT    = 20,
    SWBUS_BIT   = 12,
    SWIN_BIT    = 16,
};

enum
{
    ADC_WIDTH = 20
};
enum
{
    DAC_WIDTH = 20
};

// BOARD

static void ana_mpxa_enable(InOut& mux, bool state)
{
    auto out = mux.get_output();
    if (state)
    {
        out |= (0x1 << MPXA_EN_BIT);
    }
    else
    {
        out &= ~(0x1 << MPXA_EN_BIT);
    }
    mux.set_output(out);
}

static void ana_mpxa_set_chan(InOut& mux, int chan)
{
    auto out = mux.get_output();
    out      &= ~(0b111 << MPXA_BIT);
    out      |= (chan << MPXA_BIT);
    mux.set_output(out);
    // mux is slow, give it a chance to catch up!
    usleep(1000);
}

static void sw_set_src(InOut& mux, int chan, int src)
{
    //    assert 1 <= chan <= 4
    //    assert src in (SW_SRC_IN, SW_SRC_MUX)

    auto out = mux.get_output();

    // We can't allow to have two analog drivers connected to the same wires
    // so the relay control sequence depends on whether we want to enable
    // external IN or MUX
    if (src == SW_SRC_IN)
    {
        // disconnect the MUX first
        out &= ~(0x1 << (SWBUS_BIT + chan - 1));
        mux.set_output(out);
        // and connect the IN later
        out |= (0x1 << (SWIN_BIT + chan - 1));
        mux.set_output(out);
    }
    else if (src == SW_SRC_MUX)
    {
        // disconnect the IN first
        out &= ~(0x1 << (SWIN_BIT + chan - 1));
        mux.set_output(out);
        // and connect the MUX later
        out |= (0x1 << (SWBUS_BIT + chan - 1));
        mux.set_output(out);
    }
}

struct AdcChannelCalibResult
{
    int32_t zero, pos10v, neg10v;
};

using AdcCalibResult = std::array<AdcChannelCalibResult, std::tuple_size<AdcReadout>::value>;

struct DacChannelCalibResult
{
    double offset_v, gain_p, gain_n;
};

class AnalogCalibration
{
  public:
    AnalogCalibration(AdcSerial& adc, DacSerial& dac)
        : m_adc(adc),
          m_dac(dac)
    {
    }

    AdcCalibResult        self_calibrate_adc();
    DacChannelCalibResult calibrate_dac0(AdcChannelCalibResult const& adc_calib);

  private:
    AdcSerial& m_adc;
    DacSerial& m_dac;
};

double adc_to_volts(int32_t raw, AdcChannelCalibResult const& calib);

// PERIPHERALS

constexpr inline AdcSerial peripheral_board_0_adc0()
{
    return AdcSerial(*(adc_serial_regs volatile*)(PB0_BASE + TOP_ADC_0));
}

constexpr inline DacSerial peripheral_board_0_dac0()
{
    return DacSerial(*(dac_serial_regs volatile*)(PB0_BASE + TOP_DAC_0));
}

constexpr inline DacSerial peripheral_board_0_dac1()
{
    return DacSerial(*(dac_serial_regs volatile*)(PB0_BASE + TOP_DAC_1));
}

constexpr inline InOut peripheral_board_0_gpio0()
{
    return InOut(*(in_out_regs volatile*)(PB0_BASE + TOP_GPIO_0));
}

constexpr inline InOut peripheral_board_1_gpio0()
{
    return InOut(*(in_out_regs volatile*)(PB1_BASE + TOP_GPIO_0));
}
