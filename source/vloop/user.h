#pragma once

#include "component.h"
#include "converter.h"
// Fill your includes here
#include "pid.h"
#include "timerInterrupt.h"
// End of your includes

namespace user
{
    // real-time task callbacks
    class Converter;
    void RTTask1(Converter&);   // fwd declaration

    void RTTask2()
    {
        // handle interrupt
    }

    // ************************************************************

    class Converter : public IConverter
    {
      public:
        Converter(Component& root) noexcept
            : IConverter("Example", &root),
              pid_1("pid_1", this),
              interrupt_1("timer_1", this, *this, RTTask1)
        {
            // initialize all your objects that need initializing
        }

        // Define your public Components here
        vslib::PID                       pid_1;
        vslib::TimerInterrupt<Converter> interrupt_1;

        // ...
        // end of your Components

        // Define your Parameters here

        // end of your Parameters

        void init() override
        {
            std::cout << "Initializing something\n";
        }

        void backgroundTask() override
        {
            std::cout << "Bkg task\n";
        }
    };

    void RTTask1(Converter& converter)
    {
        for (int index = 0; index < 100; index++)
        {
            // volatile double const input = 2.0 * M_PI * (std::rand() / static_cast<double>(RAND_MAX));
            volatile double const input = 1.0;
            // volatile auto         variable = converter.pid_1.control(input, input + 2);
            // std::cout << "Did a thing\n";
            // break;
        }
    }


    void userMain()
    {
        // all background-task activities that need to be handled for your converter
    }

}   // namespace user::converter