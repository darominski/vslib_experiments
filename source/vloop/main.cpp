#include "bmboot/payload_runtime.hpp"
#include "rootComponent.hpp"
#include "state.hpp"
#include "user.hpp"

using namespace vslib;
using namespace fgc4;

int main()
{
    // As soon as main starts bmboot should be notified that the payload is alive.
    bmboot::notifyPayloadStarted();

    RootComponent   root;
    user::Converter converter = user::Converter(root);
    bmboot::startCycleCounter();

    // VSlib-side initialization:
    vslib::utils::VSMachine vs_state(root, converter);   // initial state: initalization
    while (true)
    {
        vs_state.update();
    }

    return 0;
}
