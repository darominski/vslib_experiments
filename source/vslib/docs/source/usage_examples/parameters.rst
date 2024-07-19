.. _parameter:

=========
Parameter
=========

This section describes the interface of the :code:`Parameter` class, along with
:ref:`Component <components>` one of two pillar concepts of the VSlib. This class
provides a representation for a runtime-settable, via specialized web-hosted GUI,
parameters of :code:`Component` class. They are strongly bound, and :code:`Parameters`
can only be defined in :code:`Component` and :code:`Component`-derived classes.

General interface
-----------------

:code:`Parameter` is a templated class, and the template parameter is the stored
type. Currently, the following types are supported:

.. list-table::
    :header-rows: 1

    * - Type class
      - Types
    * - Boolean
      - :code:`bool`
    * - Signed integer
      - :code:`int8_t`, :code:`int16_t`, :code:`int32_t`, :code:`int64_t`
    * - Unsigned integer
      - :code:`uint8_t`, :code:`uint16_t`, :code:`uint32_t`, :code:`uint64_t`
    * - Floating point
      - :code:`float` (32bit), :code:`double` (64bit)
    * - Strings
      - :code:`std::string`
    * - Enumerations
      - User-defined :code:`enum` and :code:`class enum`

:code:`Parameters` holding numerical types (integers, floating-point) can optionally have defined
lower and upper limits.

:code:`Parameter` is always defined inside a :code:`Component` class, for example:

.. code-block:: cpp

    #include "component.h"

    using namespace vslib;

    class CustomComponent : public Component
    {
        CustomComponent(std::string_view name, Component* parent)
          : Component("CustomType", name, parent),
            param(*this, "scalar", 0.0, 10.0)
        {
        }

        Parameter<double> param;
    };

In the example above, the :code:`0.0` and :code:`10.0` are lower and upper limits for :code:`param`
:code:`Parameter`, respectively.

:code:`Parameter` provides seamless interactions with scalar types, that means
you can use the :code:`Parameter` as if it was the scalar type it stores:

.. code-block:: cpp

    #include "customComponent.h"

    int main()
    {
        Component root("root", "root", nullptr);
        CustomComponent component("custom", &root);

        component.param; // returns the current double-type value of the CustomComponent's param
        component.param.value(); // returns the same value as above but explicitly

        return 0;
    }

In case of :code:`Parameters` that are holding :code:`std::arrays`, there three ways to interact with that array.
Assuming the following definition:

.. code-block:: cpp

    #include "component.h"

    using namespace vslib;

    class CustomComponent : public Component
    {
        CustomComponent(std::string_view name, Component* parent)
          : Component("CustomType", name, parent),
            param(*this, "array", 0.0, 10.0)
        {
        }

        Parameter<std::array<double, 3>> param; // the array's size is fixed at definition
    };

where limits :code:`0.0` and :code:`10.0` apply to each array element, individually.

.. code-block:: cpp

    #include "customComponent.h"

    int main()
    {
        Component root("root", "root", nullptr);
        CustomComponent component("custom", &root);

        // 1. Get the reference to the entire array:
        auto& array = component.param.value();

        // 2. Refer to an array element by operator[]:
        auto& element = component.param.value()[2]; // returns the third element

        // 3. Iterate over the array as if it was an std::array:
        for (const auto& element : component.param.value())
        {
            // use element as if it was just a double-type value
        }
    }

For more details regarding the API, see the :ref:`API documentation for Parameter <parameter_api>`.

Parameter setting
-----------------

The :code:`Parameter` value is not code-settable. The interface to set values directly is not enabled,
and they can be treated as read-only in the code. This is why it is safe to place them in the public
interface of you :code:`Component`. The :code:`Parameter` value can be only set via specialized GUI,
the `Vloop parameter setter`, a part of the FGC Commander.

The values stored by the :code:`Parameter` are double-buffered to ensure safety of read-write cycle.
Whenever you access the value, the read-buffer value is returned. The value-setter GUI interacts only
with the write-buffer. The buffers are swapped and synchronised by the :code:`Component` owning this
:code:`Parameter` when all the checks performed by the setter and :code:`Component`'s :code:`verifyParameters()`
method were successful. If any of the checks showed an issue, the new value is not accepted, and
the write buffer is re-synchronised with the read-buffer.

The value-setting logic includes the following checks:

- type correctness
- for numerical types: whether the new value fits in the limits
- for arrays: the length must agree
- for enums: whether the new value exists in the enum
- any validation logic implemented by the user in :code:`Component`'s :code:`verifyParameters()` method

Type correctness checks that there is no loss of information. For example, there will be no warning when
you set a 32-bit integer value to a :code:`float` :code:`Parameter`, but the opposite combination would
return a warning. Equally, C++ would not warn the user in case an implicit cast is done between boolean and
integer type (e.g. value of :code:`2` would be interpreted as :code:`true`), and signed and unsigned integers,
potentially leading to confusing values being set like :code:`-1` to an unsigned integer due to overflow.

:code:`Parameters` have a special method to inform whether their value has been already set by the external
GUI: :code:`isInitialized()`. Until the :code:`Parameter` is successfully set for the first time, this method
will return :code:`false`.