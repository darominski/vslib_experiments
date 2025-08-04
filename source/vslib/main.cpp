#include "bmboot/payload_runtime.hpp"
#include "main_vslib.hpp"
#include "rootComponent.hpp"
#include "state.hpp"

// // This is one way to stop users from creating objects on the heap and explicit memory allocations
// #ifdef __GNUC__
// // poisons dynamic memory functions
// #pragma GCC poison malloc new
// #endif

using namespace vslib;
using namespace fgc4;

int main()
{
    RootComponent& root = getRootComponent();

    // VSlib-side initialization:
    vslib::utils::VSMachine vs_state(root);   // initial state: initalization
    while (true)
    {
        vs_state.update();
    }

    return 0;
}
