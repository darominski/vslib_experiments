#include <algorithm>
#include <array>
#include <bmboot/payload_runtime.hpp>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <tuple>
#include <unistd.h>
#include <vector>

#include "alphaBetaZeroToDq0Transform.h"
#include "boxFilter.h"
#include "clarkeTransform.h"
#include "componentArray.h"
#include "firFilter.h"
#include "iirFilter.h"
#include "limitRange.h"
#include "logString.h"
#include "lookupTable.h"
#include "parameterRegistry.h"
#include "parkTransform.h"
#include "periodicLookupTable.h"
#include "pid.h"
#include "rst.h"
#include "state.h"
#include "timerInterrupt.h"
#include "user.h"

// This is one way to stop users from creating objects on the heap and explicit memory allocations
#ifdef __GNUC__
// poisons dynamic memory functions
#pragma GCC poison malloc new
#endif

using namespace vslib;
using namespace fgc4;

namespace user
{
    template<class Converter>
    void setParameters(vslib::PID& controller, TimerInterrupt<Converter>& timer)
    {
        const double p  = 52.79;
        const double i  = 0.0472;
        const double d  = 0.04406;
        const double ff = 6.1190;
        const double b  = 0.03057;
        const double c  = 0.8983;
        const double N  = 17.79;
        const double ts = 1.0e-3;
        const double f0 = 1e-15;

        controller.actuation_limits.min.setJsonValue(-100);
        controller.actuation_limits.max.setJsonValue(100);
        controller.actuation_limits.dead_zone.setJsonValue(std::array<double, 2>{0, 0});
        controller.actuation_limits.verifyParameters();
        controller.actuation_limits.flipBufferState();

        controller.kp.setJsonValue(p);
        controller.kd.setJsonValue(d);
        controller.ki.setJsonValue(i);
        controller.kff.setJsonValue(ff);
        controller.b.setJsonValue(b);
        controller.c.setJsonValue(c);
        controller.N.setJsonValue(N);
        controller.f0.setJsonValue(f0);
        controller.ts.setJsonValue(ts);

        controller.verifyParameters();
        controller.flipBufferState();

        const int      interrupt_delay = 100;   // us
        nlohmann::json value           = {interrupt_delay};
        timer.delay.setJsonValue(value[0]);
        timer.flipBufferState();
        timer.delay.syncWriteBuffer();
        timer.verifyParameters();
    }

}   // namespace user

// extern user::Converter converter;

int main()
{
    Component root("root", "root", nullptr);

    // VSlib-side initialization:
    vslib::utils::VSMachine vs_state(root);   // initial state: initalization

    // User-side initialization:

    user::Converter converter(root);

    // No Component declarations beyond this point!
    // ************************************************************

    // transition to unconfigured:
    vs_state.update();

    // VERBOSE TEST CODE
    std::cout << std::boolalpha << "Configured? (expected false) " << vs_state.isConfigured()
              << std::endl;   // should be false
    // END OF VERBOSE TEST CODE

    // User-side configuration:
    user::setParameters(converter.pid_1, converter.interrupt_1);

    // transition to configured:
    do
    {
        vs_state.update();
        // VERBOSE TEST CODE
        std::cout << std::boolalpha << "Configured? (expected true) " << vs_state.isConfigured()
                  << std::endl;   // should be true
        usleep(500'000);          // 500 ms
        // END OF VERBOSE TEST CODE
    } while (!vs_state.isConfigured());

    // now, the Parameters are configured, control can be handed over to the user FSM while still
    // running a background task

    // this will be handled by user-side state machine
    if (vs_state.isConfigured())
    {
        converter.interrupt_1.start();
    }

    int           counter        = 0;
    int           expected_delay = 210;
    int           time_range_min = expected_delay - 20;   // in clock ticks
    int           time_range_max = expected_delay + 20;   // in clock ticks
    constexpr int n_elements     = 1000;

    // end of user-side FSM code

    // this while loop can also be likely transferred to VSlib FSM
    while (true)
    {
        if (counter == n_elements + 50)
        {
            converter.interrupt_1.stop();
#ifdef PERFORMANCE_TESTS
            double const mean = converter.interrupt_1.average();
            std::cout << "Average time per interrupt: " << mean << " +- "
                      << converter.interrupt_1.standardDeviation(mean) << std::endl;
            auto const histogram = converter.interrupt_1.histogramMeasurements<100>(time_range_min, time_range_max);
            for (auto const& value : histogram.getData())
            {
                std::cout << value << " ";
            }
            std::cout << std::endl;
            auto const bin_with_max = histogram.getBinWithMax();
            auto const edges        = histogram.getBinEdges(bin_with_max);
            std::cout << "bin with max: " << bin_with_max << ", centered at: " << 0.5 * (edges.first + edges.second)
                      << std::endl;
#endif
            break;
        }
        __asm volatile("wfi");
        counter++;
        // parameter_setting_task.receiveJsonCommand();
        // converter.backgroundTask();
        // usleep(500'000);   // 500 ms
    }

    return 0;
}
