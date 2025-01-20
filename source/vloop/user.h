#pragma once

#include <fmt/format.h>
#include <string>
#include <unistd.h>

#include "afe.h"
#include "afe_rst.h"
#include "afe_vdc_bal.h"
#include "cheby_gen/reg_to_stream_cpp.h"
#include "cheby_gen/stream_to_reg_cpp.h"
#include "peripherals/reg_to_stream.h"
#include "peripherals/stream_to_reg.h"
#include "pops_current_balancing.h"
#include "pops_current_balancing_old.h"
#include "pops_dispatcher.h"
#include "vslib.h"

namespace user
{
    class Converter : public vslib::IConverter
    {
      public:
        Converter(vslib::RootComponent& root) noexcept
            : vslib::IConverter("example", root),
              interrupt_1("aurora", *this, 121, vslib::InterruptPriority::high, RTTask),
              m_s2rcpp(reinterpret_cast<uint8_t*>(0xA0200000)),
              m_r2scpp(reinterpret_cast<uint8_t*>(0xA0100000))
        {
            // initialize all your objects that need initializing
        }

        // Define your public Components here
        vslib::PeripheralInterrupt<Converter> interrupt_1;
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

            interrupt_1.start();
        }

        constexpr static int n_elements = 101'000;

        void backgroundTask() override
        {
#ifdef PERFORMANCE_TESTS
            if (counter > n_elements)
            {
                interrupt_1.stop();
                double const mean = interrupt_1.average() / 1.2;
                std::cout << "Average time per interrupt: (" << mean << " +- "
                          << interrupt_1.standardDeviation(mean) / 1.2 << ") ns" << std::endl;
                auto const histogram = interrupt_1.histogramMeasurements<100>(interrupt_1.min(), interrupt_1.max());
                for (auto const& value : histogram.getData())
                {
                    std::cout << value << " ";
                }
                std::cout << std::endl;
                auto const bin_with_max = histogram.getBinWithMax();
                auto const edges        = histogram.getBinEdges(bin_with_max);
                std::cout << "bin with max: " << bin_with_max
                          << ", centered at: " << 0.5 * (edges.first / 1.2 + edges.second / 1.2) << std::endl;
                const auto min = interrupt_1.min() / 1.2;
                const auto max = interrupt_1.max() / 1.2;
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
            for (uint32_t index = 0; index < num_data; index++)
            {
                converter.m_data[index] = cast<uint64_t, double>(converter.m_s2rcpp.data[index].read());
            }

            // write to output registers
            for (uint32_t index = 0; index < num_data; index++)
            {
                converter.m_r2scpp.data[index].write(cast<double, uint64_t>(converter.m_data[index]));
            }

            // send it away
            // trigger connection
            converter.m_r2scpp.ctrl.start.set(true);
        }

        // static void RTTaskPerf(Converter& converter)
        // {
        //     // for (int index=0; index<50; index++)
        //     // {
        //         asm volatile("isb; dsb sy");
        //         const volatile double meas = 0.5;//static_cast<double>((std::rand() / RAND_MAX - 1) * 100.0);
        //         const volatile double ref = 1.0;//static_cast<double>((std::rand() / RAND_MAX - 1) * 100.0);

        //         converter.m_data[0] = meas;
        //         converter.m_data[1] = ref;

        //         const volatile double act = 0.0;
        //     //     const volatile double act = converter.rst.control(ref, meas);

        //         converter.m_data[2] = act;
        //         asm volatile("isb; dsb sy");
        //     // }
        //     converter.counter++;
        // }

      private:
        int counter{0};

        constexpr static uint32_t    num_data{20};
        std::array<double, num_data> m_data;

        myModule::StreamToReg m_s2rcpp;
        myModule::RegToStream m_r2scpp;
    };

}   // namespace user
