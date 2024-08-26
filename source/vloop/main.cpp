#include "bmboot/payload_runtime.hpp"
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

int main()
{
    RootComponent   root;
    user::Converter converter = user::Converter(root);

    // VSlib-side initialization:
    vslib::utils::VSMachine vs_state(root, converter);   // initial state: initalization
    while (true)
    {
        vs_state.update();
    }

    return 0;
}
