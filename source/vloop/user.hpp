#pragma once

#include <fmt/format.h>
#include <string>
#include <unistd.h>

#include "cheby_gen/reg_to_stream.hpp"
#include "cheby_gen/stream_to_reg.hpp"
#include "vslib.hpp"

namespace user
{
    class Converter : public vslib::IConverter
    {
        static constexpr int data_queue_size = fgc4::utils::constants::json_memory_pool_size;   // 1 MB

      public:
        Converter(vslib::RootComponent& root) noexcept
            : vslib::IConverter("example", root),
              interrupt_1("timer", *this, std::chrono::microseconds(100'000), RTTask),
              data_queue{fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueWriter<void>>(
                  reinterpret_cast<uint8_t*>(app_data_2_3_ADDRESS + 3 * data_queue_size), data_queue_size
              )},
              bus_1(0xA0000000, pow(2, 24)),
              spi_1(bus_1, 0xE400),
              adc_1(0),
              adc_2(1),
              adc_3(2),
              adc_4(3),
              adc_5(4),
              adc_6(5),
              ad7606c_1(spi_1, 3, adc_1)
        {
            // initialize all your objects that need initializing
            std::cout << "Converter initialized\n";
        }

        // Define your public Components here
        vslib::TimerInterrupt<Converter> interrupt_1;
        // ...
        // end of your Components

        // Define your Parameters here

        // end of your Parameters

        void init() override
        {
            std::cout << "Init finished\n";
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
            converter.adc_1.start();
            const auto value = converter.adc_1.readConverted(1);
            converter.data_queue.write(std::span<const uint8_t>{reinterpret_cast<const uint8_t*>(&value), sizeof(value)}
            );

            if (converter.counter++ % 1000)
            {
                std::cout << converter.adc_1.readConverted(1) << "\n";
            }
        }

        int counter{0};

        fgc4::utils::MessageQueueWriter<void> data_queue;

        hal::Bus                bus_1;
        hal::XilAxiSpi          spi_1;
        hal::UncalibratedADC<0> adc_1;
        hal::UncalibratedADC<1> adc_2;
        hal::UncalibratedADC<2> adc_3;
        hal::UncalibratedADC<3> adc_4;
        hal::UncalibratedADC<4> adc_5;
        hal::UncalibratedADC<5> adc_6;
        hal::AD7606C<0>         ad7606c_1;
    };

}   // namespace user
