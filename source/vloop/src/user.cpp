#include "../inc/user.hpp"

vslib::RootComponent& getRootComponent()
{
    static user::Converter instance;   // assuming you want to return a reference
    return instance;
}
