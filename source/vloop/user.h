#pragma once

#include <fmt/format.h>
#include <string>
#include <unistd.h>

#include "peripherals/reg_to_stream.h"
#include "peripherals/stream_to_reg.h"
#include "vslib.h"

namespace user
{
    static std::array<std::string, 30> signal_name
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

    static std::array<std::string, 9> ordinal_numerals
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
                cyclic_data[signal_name[index]] = -1.0;
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
        std::map<std::string, float> cyclic_data;

        void print_cyclic_data()
        {
            std::cout << "Values received: " << std::endl;
            for (int index = 0; index < 30; index++)
            {
                std::cout << signal_name[index] << ": " << cyclic_data[signal_name[index]] << std::endl;
            }
        }

        double find_cycle_duration()
        {
            for (int index = 8; index > 0; index--)
            {
                const auto&  numeral     = ordinal_numerals[index];
                const double min_plateau = cyclic_data[fmt::format("REF.{}_PLATEAU.TIME", numeral)];
                const double max_plateau = min_plateau + cyclic_data[fmt::format("REF.{}_PLATEAU.DURATION", numeral)];
                if (min_plateau > 0 && max_plateau > 0)
                {
                    return (max_plateau > 2.3) ? 3.6 : 2.4;
                }
            }
        }

        std::vector<std::pair<double, double>> plot_ref()
        {
            const double min      = 0;
            const double max      = find_cycle_duration();
            const int    n_points = static_cast<int>((max - min) / control_period) + 1;

            const double                           step_size = (max - min) / n_points;
            std::vector<std::pair<double, double>> reference_function(n_points);

            for (int index = 0; index < n_points; index++)
            {
                double x = min + index * step_size;
                double y = 0.0;
                // 9 possible plateaux, find the right plateau:
                for (int plateau_index = 0; plateau_index < 9; plateau_index++)
                {
                    const auto&  numeral     = ordinal_numerals[plateau_index];
                    const double min_plateau = cyclic_data[fmt::format("REF.{}_PLATEAU.TIME", numeral)];
                    const double max_plateau
                        = min_plateau + cyclic_data[fmt::format("REF.{}_PLATEAU.DURATION", numeral)];
                    if (x >= min_plateau && x < max_plateau)
                    {
                        if (plateau_index == 0)
                        {
                            y = cyclic_data[fmt::format("REF.{}_PLATEAU.REF", numeral)];
                        }
                        else
                        {
                            y = cyclic_data[fmt::format("REF.PPPL.REF4_{}", plateau_index - 1)];
                        }
                        break;
                    }
                }
                reference_function.emplace_back(std::make_pair(x, y));
            }
            return reference_function;
        }

        double get_plateau(const int plateau_index)
        {
            const auto& numeral = ordinal_numerals[plateau_index];
            return (plateau_index == 0) ? cyclic_data[fmt::format("REF.{}_PLATEAU.REF", numeral)]
                                        : cyclic_data[fmt::format("REF.PPPL.REF4_{}", plateau_index - 1)];
        }

        double interpolate_to_next(const double x, const double x1, const double y1, const double x2, const double y2)
        {
            return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
        }

        double get_ref(const double current_time)
        {
            double reference         = 0.0;
            double previous_ref      = 0.0;
            double previous_max_time = 0.0;

            for (int index = 0; index < 9; index++)
            {
                const auto&  numeral       = ordinal_numerals[index];
                const double next_min_time = cyclic_data[fmt::format("REF.{}_PLATEAU.TIME", numeral)];
                const double next_max_time
                    = next_min_time + cyclic_data[fmt::format("REF.{}_PLATEAU.DURATION", numeral)];
                const auto next_ref = get_plateau(index);
                // first, if we fall between plateaux: interpolate the reference
                if (current_time < next_min_time)
                {
                    reference
                        = interpolate_to_next(current_time, previous_max_time, previous_ref, next_min_time, next_ref);
                    break;
                }
                // if we are in a plateau: reference is the given reference
                else if (current_time >= next_min_time && current_time < next_max_time)
                {
                    reference = next_ref;
                    break;
                }
                // else: reference is 0
                previous_ref      = next_ref;
                previous_max_time = next_max_time;
            }
            return reference;
        }

        unsigned long interrupt_counter{0};
        double        previous_cyclic_data{-1};

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

            const double cyclic_data_input = data_in[0];
            const double c0                = data_in[1];
            const double vdc1              = data_in[2];
            const double vdc2              = data_in[3];
            const double vdc3              = data_in[4];
            const double vdc4              = data_in[5];
            const double vdc5              = data_in[6];
            const double vdc6              = data_in[7];

            data_in[2] = 0.0;   // otherwise, spikes appear as I am reusing this channel for output

            // std::cout << "input: " <<  cyclic_data_input << ", c0: " << c0  << " " << vdc1 << std::endl;

            // detect new cycle: c0=1, c_tim arbitrary
            // if (c0 > 0.5 && (converter.c_tim != 0 && converter.c_tim != 1))
            if (cyclic_data_input > -1 && converter.previous_cyclic_data < 0)
            {
                // new cycle start
                // std::cout << "resetting " << c0 << " " << converter.c_tim << std::endl;
                converter.c_tim = 0;
            }

            if (converter.c_tim < 30)
            {
                // std::cout << "setting " << cyclic_data_input << ", to " << signal_name[converter.c_tim] << std::endl;
                converter.cyclic_data[signal_name[converter.c_tim]] = cyclic_data_input;
            }

            if (converter.c_tim > 4)   // c_tim > 4, we can start outputting reference
            {
                data_in[2] = converter.get_ref(converter.c_tim * converter.control_period);
            }

            // message received, update c_tim
            converter.c_tim++;
            converter.interrupt_counter++;
            converter.previous_cyclic_data = cyclic_data_input;

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
