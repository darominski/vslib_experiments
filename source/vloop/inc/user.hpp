#pragma once

#include <fmt/format.h>
#include <string>
#include <unistd.h>

#include "cheby_gen/reg_to_stream.hpp"
#include "cheby_gen/stream_to_reg.hpp"
#include "vslib.hpp"

namespace user
{
    class Converter : public vslib::RootComponent
    {
      public:
        Converter() noexcept
            : vslib::RootComponent("example"),
              //   interrupt_1("aurora", *this, 121, vslib::InterruptPriority::high, RTTask),
              interrupt_1("aurora", *this, std::chrono::microseconds(10), RTTask),
              pwm("pwm_1", *this),
              m_s2rcpp(reinterpret_cast<uint8_t*>(0xA0200000)),
              m_r2scpp(reinterpret_cast<uint8_t*>(0xA0100000))
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
            m_s2rcpp.ctrl.pmaInit.set(false);
            sleep(2);

            m_s2rcpp.ctrl.resetPb.set(false);
            sleep(1);

            m_s2rcpp.ctrl.selOutput.set(true);

            if (!(m_s2rcpp.status.channelUp.get() && m_s2rcpp.status.gtPllLock.get() && m_s2rcpp.status.laneUp.get()
                  && m_s2rcpp.status.pllLocked.get() && m_s2rcpp.status.gtPowergood.get()))
            {
                printf("Unexpected status: 0x%#08x\n", m_s2rcpp.ctrl.read());
            }

            if (m_s2rcpp.status.linkReset.get() || m_s2rcpp.status.sysReset.get())
            {
                printf("Link is in reset\n");
            }

            if (m_s2rcpp.status.softErr.get() || m_s2rcpp.status.hardErr.get())
            {
                printf("Got an error\n");
            }

            printf("Link up and good. Ready to receive data.\n");

            // kria transfer rate: 100us
            m_r2scpp.numData.write(num_data * 2);
            m_r2scpp.tkeep.write(0x0000FFFF);

            pwm.start();
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

            // if(converter.counter)
            // pwm.setModulationIndex()

            // // write to output registers
            // for (uint32_t index = 0; index < num_data; index++)
            // {
            //     converter.m_r2scpp.data[index].write(cast<double, uint64_t>(converter.m_data[index]));
            // }

            // send it away
            // trigger connection
            // converter.m_r2scpp.ctrl.start.set(true);
            converter.counter++;
        }

        vslib::HalfBridge<0> pwm;

      private:
        int counter{0};

        constexpr static uint32_t    num_data{20};
        std::array<double, num_data> m_data;

        ipCores::StreamToReg m_s2rcpp;
        ipCores::RegToStream m_r2scpp;
    };

}   // namespace user
