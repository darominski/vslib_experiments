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
#include "vslib.hpp"

namespace user
{
    class Converter : public vslib::IConverter
    {
      public:
        Converter(vslib::RootComponent& root) noexcept
            : vslib::IConverter("example", root),
              //   interrupt_1("aurora", *this, 121, vslib::InterruptPriority::high, RTTask),
              interrupt_1("timer", *this, std::chrono::microseconds(1000), RTTask),
              pwm_7("pwm_7", *this, 10'000),
              pwm_8("pwm_8", *this, 10'000)
        //   m_s2rcpp(reinterpret_cast<uint8_t*>(0xA0200000)),
        //   m_r2scpp(reinterpret_cast<uint8_t*>(0xA0100000))
        {
            // initialize all your objects that need initializing
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

            // pwm.stop();
            pwm_7.start();
            pwm_8.start();

            // pwm_8.m_pwm.m_regs.ctrl.enable.set(true);
            // pwm_8.m_pwm.m_regs.ctrl.reset.set(false);

            // pwm_9.m_pwm.m_regs.ctrl.enable.set(true);
            // pwm_9.m_pwm.m_regs.ctrl.reset.set(false);
            // pwm_10.m_pwm.m_regs.ctrl.enable.set(true);
            // pwm_10.m_pwm.m_regs.ctrl.reset.set(false);
            // pwm_11.m_pwm.m_regs.ctrl.enable.set(true);
            // pwm_11.m_pwm.m_regs.ctrl.reset.set(false);
            // pwm_12.m_pwm.m_regs.ctrl.enable.set(true);
            // pwm_12.m_pwm.m_regs.ctrl.reset.set(false);

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
            // collect inputs
            // for (uint32_t index = 0; index < num_data; index++)
            // {
            //     converter.m_data[index] = cast<uint64_t, double>(converter.m_s2rcpp.data[index].read());
            // }

            // const auto success = converter.pwm.setModulationIndex(converter.counter);
            converter.pwm_7.m_pwm.m_regs.cc0Sc.write(converter.counter);
            if (converter.counter % 100 == 0)
            {
                //     std::cout << std::boolalpha << converter.counter << " " <<
                //     converter.pwm.m_pwm.m_regs.cc0Sc.read()
                //               << std::endl;
                std::cout << std::boolalpha << converter.counter << " " << converter.pwm_7.m_pwm.m_regs.cc0Sc.read()
                          << " " << converter.pwm_7.m_pwm.m_regs.ctrl.reset.get() << " "
                          << converter.pwm_7.m_pwm.m_regs.ctrl.enable.get() << '\n';
            }

            // // write to output registers
            // for (uint32_t index = 0; index < num_data; index++)
            // {
            //     converter.m_r2scpp.data[index].write(cast<double, uint64_t>(converter.m_data[index]));
            // }

            // send it away
            // trigger connection
            // converter.m_r2scpp.ctrl.start.set(true);
            if (converter.count_up)
            {
                converter.counter++;
            }
            else
            {
                converter.counter--;
            }

            if (converter.counter >= 10'000)
            {
                converter.count_up = false;
            }
            if (converter.counter <= 0)
            {
                converter.count_up = true;
            }
        }

        vslib::HalfBridge<6> pwm_7;
        vslib::HalfBridge<7> pwm_8;

      private:
        int  counter{0};
        bool count_up{true};
        // constexpr static uint32_t    num_data{20};
        // std::array<double, num_data> m_data;

        // ipCores::StreamToReg m_s2rcpp;
        // ipCores::RegToStream m_r2scpp;
    };

}   // namespace user
