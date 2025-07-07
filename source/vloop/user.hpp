#pragma once

#include <fmt/format.h>
#include <string>
#include <unistd.h>

#include "vslib.hpp"

namespace user
{

    struct DataFrame
    {
        uint64_t             clk_cycles;
        std::array<float, 9> data{};
    };

    class Converter : public vslib::IConverter
    {
        static constexpr int data_queue_size = fgc4::utils::constants::json_memory_pool_size;   // 1 MB

      public:
        Converter(vslib::RootComponent& root) noexcept
            : vslib::IConverter("svc", root),
              interrupt_1("timer", *this, std::chrono::microseconds(100), RTTask),
              data_queue{fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueWriter<DataFrame>>(
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
              ad7606c_1(spi_1, 3, adc_1),
              ad7606c_2(spi_1, 4, adc_2),
              ad7606c_3(spi_1, 5, adc_3)
        {
            // initialize all your objects that need initializing
            std::cout << "Converter initialized\n";
        }

        // Define your public Components here
        vslib::TimerInterrupt<Converter> interrupt_1;

        hal::Bus             bus_1;
        hal::XilAxiSpi       spi_1;
        hal::UncalibratedADC adc_1;
        hal::UncalibratedADC adc_2;
        hal::UncalibratedADC adc_3;
        hal::UncalibratedADC adc_4;
        hal::UncalibratedADC adc_5;
        hal::UncalibratedADC adc_6;
        hal::AD7606C         ad7606c_1;
        hal::AD7606C         ad7606c_2;
        hal::AD7606C         ad7606c_3;

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
        }

        static void RTTask(Converter& converter)
        {
            // 1 / 1.2 GHz but clock is running at half the 1.2 GHz frequency:
            constexpr double scaling = 2 * 1.0 / 1.2;

            const uint64_t clk_value = scaling * bmboot::getCycleCounterValue();
            converter.adc_values.clk_cycles = clk_value;

            converter.adc_1.start();

            for (auto index = 0; index < 8; index++)
            {
                // the first value is ground, then 7 meaningful channels
                converter.adc_values.data[index] = converter.adc_1.readConverted(index);
            }
            converter.adc_2.start();
            // the 8th signal can be read from the next ADC chip
            converter.adc_values.data[8] = converter.adc_2.readConverted(1);
            converter.data_queue.write(converter.adc_values, {});
            converter.counter++;
            // if (converter.counter > 100'000)
            // {
            //     exit(0);
            // }
        }

        DataFrame adc_values;

        int counter{0};

        fgc4::utils::MessageQueueWriter<DataFrame> data_queue;
    };

}   // namespace user
