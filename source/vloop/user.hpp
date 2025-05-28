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
              //   bus_1(0xA0000000, pow(2, 24)),
              //   spi_1(bus_1, 0xD200),
              //   adc_1(),
              //   ad7606c_1(spi_1, 3, adc_1),
              pwm_0("pwm_0", *this, 10'000),
              //   pwm_1("pwm_1", *this, 10'000),
              //   pwm_5("pwm_5", *this, 10'000),
              pwm_6("pwm_6", *this, 10'000),
              pwm_7("pwm_7", *this, 10'000),
              //   pwm_8("pwm_8", *this, 10'000),
              //   pwm_11("pwm_11", *this, 10'000),
              //   full_bridge_1("fb_1", *this, 10'000),
              sync_trig_arr(hal::Top::instance().syncTrig),
              sync_time_ip(hal::Top::instance().syncTime)
        {
            // initialize all your objects that need initializing

            // sync_trig_arr = hal::Top::instance().syncTrig;
            // sync_time_ip = hal::Top::instance().syncTime;

            // configure all STGs
            for (int index = 0; index < 27; index++)
            {
                sync_trig_arr[index].stg.periodSc.write(2);
                sync_trig_arr[index].stg.ctrl.resync.set(true);
            }
            sync_trig_arr[22].stg.delaySc.write(5'000);
            // sync_trig_arr[23].stg.delaySc.write(5'000);
            // sync_trig_arr[15+7].stg.delaySc.write(5'000);
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

            // configure and start SyncTime
            // std::chrono::time_point currently = std::chrono::time_point_cast<std::chrono::milliseconds>(
            // std::chrono::system_clock::now()
            // );
            // std::chrono::duration miliseconds_since_utc_epoch = currently.time_since_epoch();

            // const auto now = std::chrono::system_clock::now();
            // const auto now_p_5s = now + std::chrono::seconds(5);
            // const auto epoch_seconds =
            // std::chrono::duration_cast<std::chrono::seconds>(now_p_5s.time_since_epoch()).count();

            pwm_0.start();
            // pwm_1.start();
            // pwm_5.start();
            // pwm_11.start();
            pwm_6.start();
            pwm_7.start();
            // pwm_8.start();

            // full_bridge_1.start();

            // const uint32_t current_time_w_offset = epoch_seconds; // 5 s in the future
            const uint32_t current_time_w_offset    = 0;   // 5 s in the future
            // const uint32_t current_time_w_offset = miliseconds_since_utc_epoch.count()*1000 + 5; // 5 s in the future
            const uint32_t current_time_w_offset_sc = 0;

            sync_time_ip.s.write(current_time_w_offset);
            sync_time_ip.sc.write(current_time_w_offset_sc);

            // std::cout << "current: " << epoch_seconds  << " " << current_time_w_offset<< std::endl;
            // std::cout << "set: " << sync_time_ip.s.read() << std::endl;

            // adc_1.start();
            // sleep(10);
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
            // converter.adc_1.start();
            // std::cout << converter.adc_1.readConverted(0) << " " << converter.adc_1.readConverted(1) << " "
            //   << converter.adc_1.readConverted(2) << "\n";
            const auto success0 = converter.pwm_0.setModulationIndex(static_cast<float>(converter.counter) / 10'000);
            // const auto success1 = converter.pwm_1.setModulationIndex(static_cast<float>(converter.counter) / 10'000);
            // const auto success5 = converter.pwm_5.setModulationIndex(static_cast<float>(converter.counter) / 10'000);
            // const auto success11 = converter.pwm_11.setModulationIndex(static_cast<float>(converter.counter) /
            // 10'000);
            const auto success6 = converter.pwm_6.setModulationIndex(static_cast<float>(converter.counter) / 10'000);
            const auto success7 = converter.pwm_7.setModulationIndex(static_cast<float>(converter.counter) / 10'000);
            // const auto success8 = converter.pwm_8.setModulationIndex(static_cast<float>(converter.counter) / 10'000);

            // converter.full_bridge_1.setModulationIndex2L1Fsw(static_cast<float>(converter.counter) / 10'000);
            // converter.full_bridge_1.setModulationIndex3L2Fsw(static_cast<float>(converter.counter - 5'000) / 10'000);

            if (converter.counter % 100 == 0)
            {
                std::cout << std::boolalpha << converter.counter << " " << success6 << '\n';
            }

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

        vslib::HalfBridge<0> pwm_0;
        // vslib::HalfBridge<1> pwm_1;
        // vslib::HalfBridge<5> pwm_5;
        vslib::HalfBridge<6> pwm_6;
        vslib::HalfBridge<7> pwm_7;
        // vslib::HalfBridge<8> pwm_8;
        // vslib::HalfBridge<11> pwm_11;

        // vslib::FullBridge<6> full_bridge_1;

        // hal::Bus                bus_1;
        // hal::XilAxiSpi          spi_1;
        // hal::UncalibratedADC<0> adc_1;
        // hal::AD7606C<0>         ad7606c_1;

        ipCores::Top::SyncTrigArray sync_trig_arr;
        ipCores::Top::SyncTime      sync_time_ip;

      private:
        int  counter{0};
        bool count_up{true};
        // constexpr static uint32_t    num_data{20};
        // std::array<double, num_data> m_data;

        // ipCores::StreamToReg m_s2rcpp;
        // ipCores::RegToStream m_r2scpp;
    };

}   // namespace user
