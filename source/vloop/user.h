#pragma once

#include <fmt/format.h>
#include <string>
#include <unistd.h>

#include "peripherals/reg_to_stream.h"
#include "peripherals/stream_to_reg.h"
#include "vslib.h"

namespace user
{
    std::array<std::string, 30> signal_name
        = {"REF_USER",
           "REF_USER",
           "REF.START.VREF",
           "REF.FIRST_PLATEAU.REF",
           "REF.FIRST_PLATEAU.TIME",
           "REF.FIRST_PLATEAU.DURATION",
           "REF.PPPL.REF4_0",
           "REF.SECOND_PLATEAU.TIME",
           "REF.SECOND_PLATEAU.DURATION",
           "REF.PPPL.REF4_1",
           "REF.THIRD_PLATEAU.TIME",
           "REF.THIRD_PLATEAU.DURATION",
           "REF.PPPL.REF4_2",
           "REF.FOURTH_PLATEAU.TIME",
           "REF.FOURTH_PLATEAU.DURATION",
           "REF.PPPL.REF4_3",
           "REF.FIFTH_PLATEAU.TIME",
           "REF.FIFTH_PLATEAU.DURATION",
           "REF.PPPL.REF4_4",
           "REF.SIXTH_PLATEAU.TIME",
           "REF.SIXTH_PLATEAU.DURATION",
           "REF.PPPL.REF4_5",
           "REF.SEVENTH_PLATEAU.TIME",
           "REF.SEVENTH_PLATEAU.DURATION",
           "REF.PPPL.REF4_6",
           "REF.EIGHTH_PLATEAU.TIME",
           "REF.EIGHTH_PLATEAU.DURATION",
           "REF.PPPL.REF4_7",
           "REF.NINTH_PLATEAU.TIME",
           "REF.NINTH_PLATEAU.DURATION"};

    std::array<std::string, 9> ordinal_numerals
        = {"FIRST", "SECOND", "THIRD", "FOURTH", "FIFTH", "SIXTH", "SEVENTH", "EIGHT", "NINTH"};


    class Converter : public vslib::IConverter
    {
      public:
        Converter(vslib::RootComponent& root) noexcept
            : vslib::IConverter("example", root),
              m_interrupt_id{121 + 0},   // Jonas's definition
              interrupt_1("aurora", *this, 121, vslib::InterruptPriority::high, RTTask),
              //   pll("pll", *this),
              //   pi_id_ref("pi_id_ref", *this),
              //   pi_iq_ref("pi_iq_ref", *this),
              //   pi_vd_ref("pi_vd_ref", *this),
              //   pi_vq_ref("pi_vq_ref", *this),
              //   limit("limit", *this),
              //   abc_2_dq0("abc_2_dq0", *this),
              //   dq0_2_abc("dq0_2_abc", *this),
              control_period(*this, "control_period", 0.0),
              m_s2r(reinterpret_cast<volatile stream_to_reg*>(0xA0200000)),
              m_r2s(reinterpret_cast<volatile reg_to_stream*>(0xA0100000))
        {
            // initialize all your objects that need initializing
            for (int index = 0; index < 30; index++)
            {
                cyclic_data_map[signal_name[index]] = -1.0;
            }
        }

        // Define your public Components here
        vslib::PeripheralInterrupt<Converter> interrupt_1;

        // ...
        // end of your Components

        // Define your Parameters here
        vslib::Parameter<double> control_period;
        // end of your Parameters

        void init() override
        {
            if (m_s2r->ctrl & STREAM_TO_REG_CTRL_PMA_INIT)
            {
                m_s2r->ctrl &= ~STREAM_TO_REG_CTRL_PMA_INIT;
            }
            sleep(2);
            if (m_s2r->ctrl & STREAM_TO_REG_CTRL_RESET_PB)
            {
                m_s2r->ctrl &= ~STREAM_TO_REG_CTRL_RESET_PB;
            }
            sleep(1);

            m_s2r->ctrl |= STREAM_TO_REG_CTRL_SEL_OUTPUT;

            if (!(m_s2r->status
                  & (STREAM_TO_REG_STATUS_CHANNEL_UP | STREAM_TO_REG_STATUS_GT_PLL_LOCK | STREAM_TO_REG_STATUS_LANE_UP
                     | STREAM_TO_REG_STATUS_PLL_LOCKED | STREAM_TO_REG_STATUS_GT_POWERGOOD)))
            {
                printf("Unexpected status: 0x%#08x\n", m_s2r->ctrl);
            }

            if (m_s2r->status & (STREAM_TO_REG_STATUS_LINK_RESET | STREAM_TO_REG_STATUS_SYS_RESET))
            {
                printf("Link is in reset\n");
            }

            if (m_s2r->status & (STREAM_TO_REG_STATUS_SOFT_ERR | STREAM_TO_REG_STATUS_HARD_ERR))
            {
                printf("Got an error\n");
            }

            printf("Link up and good. Ready to receive data.\n");
            interrupt_1.start();
        }

        int                  counter        = 0;
        int                  expected_delay = 210;
        int                  time_range_min = expected_delay - 20;   // in clock ticks
        int                  time_range_max = expected_delay + 20;   // in clock ticks
        constexpr static int n_elements     = 10'000;

        void backgroundTask() override
        {
#ifdef PERFORMANCE_TESTS
            if (counter > n_elements)
            {
                std::cout << "counter: " << counter << " " << n_elements << " " << (counter > n_elements) << std::endl;
                interrupt_1.stop();
                double const mean = interrupt_1.average();
                std::cout << "Average time per interrupt: " << mean << " +- " << interrupt_1.standardDeviation(mean)
                          << std::endl;
                // auto const histogram = interrupt_1.histogramMeasurements<100>(time_range_min,
                // time_range_max); for (auto const& value : histogram.getData())
                //             {
                //                 std::cout << value << " ";
                //             }
                //             std::cout << std::endl;
                //             auto const bin_with_max = histogram.getBinWithMax();
                //             auto const edges        = histogram.getBinEdges(bin_with_max);
                //             std::cout << "bin with max: " << bin_with_max
                //                       << ", centered at: " << 0.5 * (edges.first + edges.second) << std::endl;
                const auto min = interrupt_1.min();
                const auto max = interrupt_1.max();
                std::cout << "min: " << min << ", max: " << max << std::endl;
                exit(0);
            }
#endif
        }

        template<typename SourceType, typename TargetType>
        static TargetType cast(SourceType input)
        {
            return *reinterpret_cast<TargetType*>(&input);
        }

        unsigned int                 c_tim{0};
        std::map<std::string, float> cyclic_data_map;

        void print_cyclic_data_map()
        {
            std::cout << "Values received: " << std::endl;
            for (int index = 0; index < cyclic_data_map.size(); index++)
            {
                std::cout << signal_name[index] << ": " << cyclic_data_map[signal_name[index]] << std::endl;
            }
        }

        double get_ref(double current_time)
        {
            bool current_regulation = (cyclic_data_map["REF.START.VREF"] == 0.0);   // field regulation if false

            double reference = 0.0;
            if (current_time < cyclic_data_map["REF.FIRST_PLATEAU.TIME"])   // and FIRST_PLATEAU is not -1
            {
                reference = cyclic_data_map["REF.START.VREF"];
            }

            for (int index = 1; index <= 9; index++)
            {
                if (current_time >= cyclic_data_map[fmt::format("REF.{}_PLATEAU.TIME", ordinal_numerals[index])]
                    && current_time
                           < (cyclic_data_map[fmt::format("REF.{}_PLATEAU.TIME", ordinal_numerals[index])]
                              + cyclic_data_map[fmt::format("REF.{}_PLATEAU.DURATION", ordinal_numerals[index])]))
                {
                    reference = cyclic_data_map[fmt::format("REF.{}_PLATEAU.REF", ordinal_numerals[index])];
                }
            }

            return reference;
        }

        static void RTTask(Converter& converter)
        {
            // TEST 6: Active Front-End
            constexpr uint32_t                num_data      = 40;
            constexpr uint32_t                num_data_half = 20;
            std::array<double, num_data_half> data_in;

            for (std::size_t i = 0; i < num_data_half; ++i)
            {
                data_in[i] = cast<uint64_t, double>(converter.m_s2r->data[i].value);
            }

            volatile const float c0                = data_in[0];
            volatile const float cyclic_data_input = data_in[1];

            if (c0 == 1 && (converter.c_tim != 0 && converter.c_tim != 1))
            {
                // new cycle start
                converter.c_tim = 0;
            }

            if (converter.c_tim < 30)
            {
                converter.cyclic_data_map[signal_name[converter.c_tim]] = cyclic_data_input;
            }
            else if (converter.c_tim == 30)
            {
                converter.print_cyclic_data_map();
            }
            else   // c_tim > 30, full message received
            {
                data_in[2] = converter.get_ref(converter.c_tim * converter.control_period);
            }
            // message received, update c_tim
            converter.c_tim++;

            // write to output registers
            for (uint32_t index = 0; index < num_data_half; index++)
            {
                converter.m_r2s->data[index].value = cast<double, uint64_t>(data_in[index]);
            }

            // send it away
            // kria transfer rate: 100us
            converter.m_r2s->num_data = num_data;
            converter.m_r2s->tkeep    = 0x0000FFFF;

            // trigger connection
            converter.m_r2s->ctrl = REG_TO_STREAM_CTRL_START;
            converter.counter++;
        }

      private:
        int m_interrupt_id;

        volatile stream_to_reg* m_s2r;
        volatile reg_to_stream* m_r2s;
    };

}   // namespace user
