#include "bmboot/payload_runtime.hpp"
#include "main_vslib.hpp"
#include "rootComponent.hpp"
#include "state.hpp"

using namespace vslib;
using namespace fgc4;

int main()
{
    // bmboot::notifyPayloadStarted();
    RootComponent& root = getRootComponent();

    // VSlib-side initialization:
    vslib::utils::VSMachine vs_state(root);   // initial state: initalization
    while (true)
    {
        vs_state.update();
    }

    return 0;
}
