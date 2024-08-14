#include "rootComponent.h"
#include "state.h"
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
    void setParameters(vslib::PID& controller)
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
        controller.T.setJsonValue(ts);

        controller.verifyParameters();
        controller.flipBufferState();
    }

    template<size_t N>
    void setRSTParameters(vslib::RST<N - 1>& controller)
    {
        const auto r = std::array<double, N>{1.0};
        const auto s = std::array<double, N>{1.0};
        const auto t = std::array<double, N>{1.0};

        controller.actuation_limits.min.setJsonValue(0);
        controller.actuation_limits.max.setJsonValue(0.1);
        controller.actuation_limits.dead_zone.setJsonValue(std::array<double, 2>{0, 0});
        controller.actuation_limits.verifyParameters();
        controller.actuation_limits.flipBufferState();

        controller.r.setJsonValue(r);
        controller.s.setJsonValue(s);
        controller.t.setJsonValue(t);

        controller.verifyParameters();
        controller.flipBufferState();
    }

}   // namespace user

int main()
{
    Component root("root", "root", nullptr);

    // VSlib-side initialization:
    vslib::utils::VSMachine vs_state(root);   // initial state: initalization
    vs_state.update();                        // start -> initialization

    // User-side initialization:
    std::cout << "Initializing user-converter\n";
    user::Converter converter(root);
    vs_state.setConverter(&converter);
    std::cout << "done\n";

    // No Component declarations beyond this point!
    // ************************************************************

    vs_state.update();   // initialization -> unconfigured

    // VERBOSE TEST CODE
    std::cout << std::boolalpha << "Configured? (expected false) " << vs_state.isConfigured()
              << std::endl;   // should be false
    // END OF VERBOSE TEST CODE

    // User-side configuration:
    user::setRSTParameters<3>(converter.rst_1);

    // transition to configured:
    vs_state.update();
    // now, the Parameters are configured, control has been handed over to the user FSM while still
    // running a background task

    // no code will be executed beyond this point, end of user-side FSM code

    return 0;
}
