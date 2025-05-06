#pragma once

#include <fmt/format.h>
#include <string>
#include <unistd.h>

#include "afe.hpp"
#include "afe_rst.hpp"
#include "afe_vdc_bal.hpp"
#include "cheby_gen/reg_to_stream.hpp"
#include "cheby_gen/stream_to_reg.hpp"
// #include "pops_current_balancing.hpp"
// #include "pops_current_balancing_old.hpp"
// #include "pops_dispatcher.hpp"
#include "halfBridge.hpp"
#include "peripherals/bus.hpp"
#include "peripherals/xil_axi_spi.hpp"
#include "vslib.hpp"

namespace user
{
    class Converter : public vslib::IConverter
    {
      public:
        Converter(vslib::RootComponent& root) noexcept
            : vslib::IConverter("example", root),
              interrupt_1("timer", *this, std::chrono::microseconds(100'000), RTTask),
              bus_1(0xA0000000, pow(2, 24)),
              spi_1(bus_1, 0xD200),
              adc_1(),
              ad7606c_1(spi_1, 3, adc_1)
        //   pwm_7("pwm_7", *this, 10'000),
        //   pwm_8("pwm_8", *this, 10'000)
        {
            // initialize all your objects that need initializing
            std::cout << "Initialized\n";
        }

        // Define your public Components here
        vslib::TimerInterrupt<Converter> interrupt_1;
        // ...
        // end of your Components

        // Define your Parameters here

        // end of your Parameters

        void init() override
        {
            // m_s2rcpp.ctrl.pmaInit.set(false);
            // sleep(2);

            // m_s2rcpp.ctrl.resetPb.set(false);
            // sleep(1);

            // m_s2rcpp.ctrl.selOutput.set(true);

            // if (!(m_s2rcpp.status.channelUp.get() && m_s2rcpp.status.gtPllLock.get() && m_s2rcpp.status.laneUp.get()
            //       && m_s2rcpp.status.pllLocked.get() && m_s2rcpp.status.gtPowergood.get()))
            // {
            //     printf("Unexpected status: 0x%#08x\n", m_s2rcpp.ctrl.read());
            // }

            // if (m_s2rcpp.status.linkReset.get() || m_s2rcpp.status.sysReset.get())
            // {
            //     printf("Link is in reset\n");
            // }

            // if (m_s2rcpp.status.softErr.get() || m_s2rcpp.status.hardErr.get())
            // {
            //     printf("Got an error\n");
            // }

            // printf("Link up and good. Ready to receive data.\n");

            // // kria transfer rate: 100us
            // m_r2scpp.numData.write(num_data * 2);
            // m_r2scpp.tkeep.write(0x0000FFFF);

            // pwm_7.start();
            // pwm_8.start();
            adc_1.start();
            interrupt_1.start();
        }

        constexpr static int n_elements = 101'000;

        void backgroundTask() override
        {
#ifdef PERFORMANCE_TESTS
            if (counter > n_elements)
            {
                interrupt_1.stop();
                const double scaling = 1.0 / 1.3333;   // 1 / 1.3333 GHz
                double const mean    = interrupt_1.average() * scaling;
                std::cout << "Average time per interrupt: (" << mean << " +- "
                          << interrupt_1.standardDeviation(interrupt_1.average()) * scaling << ") ns" << std::endl;
                auto const histogram = interrupt_1.histogramMeasurements<100>(interrupt_1.min(), interrupt_1.max());
                for (auto const& value : histogram.getData())
                {
                    std::cout << value << " ";
                }
                std::cout << std::endl;
                auto const bin_with_max = histogram.getBinWithMax();
                auto const edges        = histogram.getBinEdges(bin_with_max);
                std::cout << "bin with max: " << bin_with_max
                          << ", centered at: " << 0.5 * (edges.first * scaling + edges.second * scaling) << std::endl;
                const auto min = interrupt_1.min() * scaling;
                const auto max = interrupt_1.max() * scaling;
                std::cout << "min: " << min << " ns, max: " << max << " ns" << std::endl;
                exit(0);
            }
#endif
        }

        template<typename SourceType, typename TargetType>
        static TargetType cast(SourceType input)
        {
            return std::bit_cast<TargetType>(input);
        }

        static void RTTask(Converter& converter)
        {


            // const auto success1 = converter.pwm_7.setModulationIndex(static_cast<float>(converter.counter) / 10'000);
            // const auto success2 = converter.pwm_8.setModulationIndex(static_cast<float>(converter.counter) / 10'000);
            // if (converter.counter % 100 == 0)
            // {
            //     std::cout << std::boolalpha << converter.counter << " " << success1 << " " << success2 << '\n';
            //

            // if (converter.count_up)
            // {
            //     converter.counter++;
            // }
            // else
            // {
            //     converter.counter--;
            // }

            // if (converter.counter >= 10'000)
            // {
            //     converter.count_up = false;
            // }
            // if (converter.counter <= 0)
            // {
            //     converter.count_up = true;
            // }
        }

        // vslib::HalfBridge<6> pwm_7;
        // vslib::HalfBridge<7> pwm_8;
        hal::Bus                bus_1;
        hal::XilAxiSpi          spi_1;
        hal::UncalibratedADC<0> adc_1;
        hal::AD7606C<0>         ad7606c_1;

      private:
        int  counter{0};
        bool count_up{true};
        // constexpr static uint32_t    num_data{20};
        // std::array<double, num_data> m_data;

        // ipCores::StreamToReg m_s2rcpp;
        // ipCores::RegToStream m_r2scpp;
    };

}   // namespace user
