.. _limits:

======
Limits
======

The limit protection may be necessary to ensure that any output of controllers, filters, or measurements stays in the pre-defined
range of values. This feature can be used to ensure that the input does not overflow an ADC or limit the actuation of a controller
to the rate the actuator can provide.

The VSlib provides four types of limits protection :ref:`Components <component_api>`: :code:`LimitRange` for keeping
the input in the pre-defined range, :code:`LimitRate` for keeping the input within the defined rate of change,
:code:`LimitIntegral` for informing the user that a cumulative over a time window has crossed the limit,
and :code:`LimitRms` fof letting the user know that a root-mean square of the inputs crossed the defined value.

General interface
-----------------

All implemented limits are derived from the :ref:`Component <component_api>` and implements all its features.
All limits provide a consistent interface of a single main access method to be called to access its functionality:
:code:`limit`. Details then differ what are the function arguments and what does it return, that is dependent on
the particular limit. Limits that use history to check the provided values also have a :code:`reset` method to
clear the history and bring itself to the starting state.

.. _limitRange_component:

LimitRange
----------

:code:`LimitRange` implements a saturation limiting feature with an optional exclusion range, called :code:`dead zone`.
The component is type-dependent, and its type needs to be defined at the time of construction using template parameter.
All numerical types are supported.

The component has three Parameters: :code:`min`, :code:`max`, and :code:`dead_zone`. The :code:`dead_zone` is a two-element
array of the :code:`LimitRange` numerical type. The first element is the lower edge of the zone, while the second element is
the upper edge. Both :code:`min`-:code:`max`` and :code:`dead_zone`` pairs are verified for logical consistency,
i.e. the minimum cannot be larger than maximum. The optionality of the :code:`dead_zone` is expressed by using it only if
the lower and upper edges have different values, so if this feature is no to be used, the values need to be set to an arbitrary
but same value for both elements of the array.

The :code:`limit` function takes one argument of the :code:`LimitRange` type (e.g. double), and returns one value of the same type.
The returned value is a value fitting in the specified range. In case of the underlow, the :code:`min` value is returned, and :code:`max`
for the overflow cases. If the input falls in the :code:`dead_zone`, the closest edge to the value is returned. Input value that
is exactly equal to any of the limits, does not trigger a change. In case the provided floating-point value is not a number (NaN),
the returne value is the smallest representable value of the templated type. If the input does not violate any of the limits, it is simply returned
unmodified.

For more details regarding the API, see the :ref:`API documentation for LimitRange <limitRange_api>`.

Usage example
^^^^^^^^^^^^^

.. code-block:: cpp

    #include "limitRange.h"
    #include "rootComponent.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        LimitRange<double> limit("limit", root);

        // set min, max, and optionally dead_zone Parameters
        // example below assumes min = 0, max = 1000, dead_zone = [10, 15]

        double output;

        output = limit.limit(3.14);   // output = 3.14
        output = limit.limit(-5);    // output = 0
        output = limit.limit(11);    // output = 10
        output = limit.limit(14);    // output = 15
        output = limit.limit(14000); // output = 1000
        output = limit.limit(inf);   // output = 1000
        output = limit.limit(NaN);   // output = 2.22507e-308

        return 0;
    }

.. _limitRate_component:

LimitRate
---------

:code:`LimitRate` implements a rate limiting feature. The component is type-dependent, and its type needs to be defined
at the time of construction using template parameter. All numerical types are supported.

The component has one Parameter: :code:`change_rate`. This is an absolute and maximal rate of change that is allowed.

The :code:`limit` function takes two arguments: the first is the input value of the same template :code:`LimitRate` type (e.g. double),
and the second argument is the time difference (:math:`\Delta t`) between measurements of type double. The method returns one value
of the same type as the input. The returned value is a value that does not surpass the maximal rate of change defined by the
:code:`change_rate` Parameter. The check has the following equation:

.. math::

    rate = | input - input_{i-1} | / \Delta t \\
    rate <= change\_rate

The first input after declaration of the component or after a reset always passes, unless it is an invalid number (e.g. NaN)
 to ensure that the :code:`input_{i-1}` always exists.

In case of the provided input surpasses the defined limit, a corrected value is returned. It is calculated according to the following formula:

.. math::

    output = input_{i-1} + change\_rate \cdot \Delta t

In case the provided input is not a number, the smallest number representable in that type is returned. If the input does not violate the maximal
rate of change, it is returned unmodified. However, in case the provided :math:`\Delta t` value is equal to zero, the calculated rate would be
infinite, so the returned value returns the largest representable value of the template type.

The :code:`reset` method brings the component to the intiial state, by unsetting the :code:`input_{i-1}` value. After it is executed, the next
call to the :code:`limit` method will always pass and the provided value will be used as reference.

For more details regarding the API, see the :ref:`API documentation for LimitRate <limitRate_api>`.

Usage example
^^^^^^^^^^^^^

.. code-block:: cpp

    #include "limitRate.h"
    #include "rootComponent.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        LimitRate<double> limit("limit", root);

        // set change_rate Parameter
        // example below assumes change_rate = 10

        double out_1 = limit.limit(3.14, 1.0);  // first input always passes
        double out_2 = limit.limit(3.14, 1.0);  // rate = 0.0, output = 3.14
        limit.reset(); // resets to the initial state

        out_1 = limit.limit(1.0, 1.0); // first always passes
        out_2 = limit.limit(12.0);     // output = 11
        limit.reset();

        out_1 = limit.limit(1.0, 1.0); // first always passes
        out_2 = limit.limit(inf, 1.0); // output = 11
        limit.reset();

        out_1 = limit.limit(1.0, 1.0); // first always passes
        out_2 = limit.limit(1.0, 0.0); // output = 1.79769e+308
        limit.reset();

        out_2 = limit.limit(NaN, 1.0); // output = 2.22507e-308
        limit.reset();

        return 0;
    }

.. _limitIntegral_component:

LimitIntegral
-------------

:code:`LimitIntegral` implements a limit on the cumulative value of inputs over a number of measurements. The component is type-dependent,
and its type needs to be defined at the time of construction using template parameter. All numerical types are supported. The component
has also an optional second template argument, specifying the size of the array holding provided inputs. By default it is set to 16.

The component has two Parameters: :code:`integral_limit` of the class template type and :code:`integral_limit_window_length` of unsigned integer type.
The former is the maximal value of the sum of the provided inputs up to the latter parameter number of elements.

The :code:`limit` function takes one argument: the input value of the same class template type (e.g. double). It returns one value of boolean type.
It returns true if the sum of provided inputs over the :code:`integral_limit_window_length` is less or equal than the :code:`integral_limit`,
otherwise it returns false. :code:`false` is also returned if the provided input is not a number.

The :code:`reset` method brings the component to the intiial state, clearing the array with stored inputs.

For more details regarding the API, see the :ref:`API documentation for LimitIntegral <limitIntegral_api>`.

Usage example
^^^^^^^^^^^^^

.. code-block:: cpp

    #include "limitIntegral.h"
    #include "rootComponent.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        LimitIntegral<double, 5> limit("limit", root);

        // set integral_limit, and integral_limit_window_length Parameters
        // example below assumes integral_limit = 100, window_length = 3

        bool output = limit.limit(2.0); // true, cumulative: 2.0
        limit.reset(); // resets to the initial state

        bool out_1 = limit.limit(5.0);   // true, cumulative: 5.0
        bool out_2 = limit.limit(10.0);  // true, cumulative: 15.0
        bool out_3 = limit.limit(100.0); // false, cumulative would be 115, max input: 85.0
        limit.reset();

        output = limit.limit(inf); // false, max input = 100.0
        limit.reset();

        out = limit.limit(NaN); // false, input not a number
        limit.reset();

        return 0;
    }

.. _limitRms_component:

LimitRms
--------

:code:`LimitRms` implements a second rate limit, using root-mean square calculation of the provided input. The component is not type dependent,
and the input type has been fixed to :code:`double`, for type safety given the necessary calculations to be performed.

The component has two settable Parameters: :code:`rms_limit` and :code:`rms_time_constant`, both of double type. In addition, the component has
an optional constructor argument of type double: :code:`iteration_period`, which by default is set to :math:`5e-6`.

The :code:`limit` function takes one argument: the input value of type double and returns one value of boolean type.
It returns true if the RMS of provided input is less or equal than the :code:`rms_limit`,
otherwise it returns false. :code:`false` is also returned if the provided input is not a number.

The formula used is the following:

.. math::

    cumulative_{i} = (input_{i}^{2} - cumulative_{i-1}) \cdot filter\_factor \\
    cumulative_{i} <= rms\_limit^{2},

where :math:`filter\_factor = \frac{iteration\_period}{rms\_time\_constant + 0.5 \cdot iteration\_period}`.

The :code:`reset` method brings the component to the intiial state, clearing the information about the previous input.

For more details regarding the API, see the :ref:`API documentation for LimitRms <limitRms_api>`.

Usage example
^^^^^^^^^^^^^

.. code-block:: cpp

    #include "limitRms.h"
    #include "rootComponent.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        LimitRms limit("limit", root);

        // set rms_limit and rms_time_constant Parameters
        // example below assumes rms_limit = 10.0, rms_time_constant: 1e-6, iteration_period: 1e-5

        bool out_1 = limit.limit(1.0); // first input always passes, true
        bool out_2 = limit.limit(2.0); // true
        limit.reset();                 // resets to the initial state

        out_1 = limit.limit(1.0);   // true
        out_2 = limit.limit(10.0);  // false, maximum is < 7.853
        limit.reset();

        out_1 = limit.limit(1.0); // true
        out_2 = limit.limit(inf); // false, maximum is < 7.853
        limit.reset();

        out_1 = limit.limit(1.0); // true
        out_2 = limit.limit(NaN); // false, input is not a number
        limit.reset();

        return 0;
    }
