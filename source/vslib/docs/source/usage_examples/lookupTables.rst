.. _lookupTables:

==============
Look-up tables
==============

VSlib provides a functonality of a look-up table intended for all uses where trading memory consumption
for execution time may be favourable. The prime use-case example are the relatively expensive trigonometrical
functions, like sine or cosine. However, the look-up tables can be useful whenever the calculation time
can be significant, and longer than the look-up time and interpolation, see the :ref:`performance section <lookupTable_performance>`.

General interface
-----------------

VSlib has two :ref:`Components <component_api>` implementing of the look-up table functionality: :ref:`LookupTable <lookupTable_component>` and
:ref:`PeriodicLookupTable <periodicLookupTable_component>`, and two helper Components based on the periodic type:
:ref:`SinLookupTable <sinLookupTable_component>` and :ref:`CosLookupTable <cosLookupTable_component>`.

The :code:`LookupTable` and :code:`PeriodicLookupTable` are derived from the :ref:`Components <component_api>` class and implement its interface.
They both have two template parameters, including one optional: :code:`IndexType` and optional :code:`StoredType`, defining the types
of the stored table types for indices and values, respectively. The :code:`StoredType` is optional and does not need to be defined. By default
it will be the same type as the :code:`IndexType`. All numerical scalar types are supported, e.g. double-precision float. The :code:`SinLookupTable`
and :code:`CosLookupTable` do not have a template parameter, and are by default double-precision float.

The :code:`LookupTable` and :code:`PeriodicLookupTable` require the table to be stored to be provided at construction time. To ease the interaction
with the look-up table functionality, two dedicated classes: :code:`SinLookupTable` and :code:`CosLookupTable` are provided for your convenience.
They internalize the function generation and only the number of points in the table has to be specified. A utility function in the VSlib is provided
to ease the equally-spaced function generation in the :code:`fgc4::utils` namespace: :ref:`generateFunction <generateFunction>`.

All of the Components implement a single access method: :code:`interpolate`, taking one obligatory argument of the numerical scalar :code:`IndexType`
with the x-value (:math:`input_{x}`) of the function to be found in the stored table, with an optional boolean parameter denoting whether the access to the table
is random (:code:`true`) or monotonic (:code:`false`), and returns one value of numerical scalar type :code:`StoredType` from the stored table.
For the highest performance, the :code:`interpolate` method uses linear interpolation to calculate the returned value:

.. math::

    y = y_{1} + (x_{input} - x_{1}) \cdot \frac{y_{2} - y_{1}}{x_{2} - x_{1}},

where: :math:`x_{input}` is the user-defined input value, and :math:`(x_{1}, y_{1})` and :math:`(x_{2}, y_{2})`
are the lower and upper edge pairs, respectively, of x-y values stored in the table, such that  :math:`x_{1} < x_{input} < x_{2}`.

The edges, lower: :math:`x_{1}` and upper: :math:`x_{2}` of the so-called sector are found using one of the three methods: linear, binary,
or index-based search. The linear search uses the :code:`std::find` function, with linear complexity, it is the default choice if your stored
table does not have equal-size binning and you intend to access the data in a monotonically increasing manner. The binary search is enabled
when the optional second parameter of the :code:`interpolate` method is set to :code:`true`. It uses the :code:`std::upper_bound` binary-search
approach with logarithmic complexity. It is optimised for the case where your stored table does not have equal-size binning and you are accessing
the data in a random manner. Finally, the index search is the default if your stored table has equal binning. In that case, the optional parameter
for random access is ignored, as this approach is found to be the fastest (and deterministic) way to access the data.

Certain optimisations are undertaken, such as caching the previous sector edges and values, so that if the next requested :math:`x_{input}` falls
in the same sector, it does not need to be searched for again.

All classes implement a :code:`reset` method that bring the objects to the initial state, resetting all cached data except for the stored table.

.. _lookupTable_component:

Look-up table
-------------

The :code:`LookupTable` implements the basic look-up table functionality and the general interface described above. The constructor
accepts the vector of index-value pairs (of type :code:`std::vector<std::pair<IndexType, StoredType>>`) as input, called further simply table,
and moves it to an internal structure. The table is expected to have sorted indices and is not mutable at runtime. The constructor has also
an optional boolean argument, a flag denoting whether the provided table has equal binning (:code:`true` case) or not (:code:`false` case).
If the flag is set to :code:`true`, then the index search is always performed, regardless of the second parameter of the :code:`interpolate` method.

The constant boundary conditions are enforced. That is, in case of the underflow, when the provided :math:`x_{input}` is below
the lowest :math:`x` value of the stored table, the :math:`y` value corresponding to the first element is returned, without interpolation
or extrapolation. Analogically, in case of an overflow, the last :math:`y` value of the table is returned.

The :code:`LookupTable` provides a way to access the table's :math:`y` values by index by overriding :code:`operator[]`:

.. code-block:: cpp

    RootComponent root;
    LookupTable<int, double> lin_table("table", &root,
      fgc4::utils::generateFunction<int, double>([](const auto x){return 2*x + 1.5;}, 0.0, 10.0, 100), true);
    const auto y2 = lin_table[1]; // returns y of the second element

For more details regarding the API, see the :ref:`API documentation for LookupTable <lookupTable_api>`.

Usage examples
^^^^^^^^^^^^^^

.. code-block:: cpp

    #include <vector>

    #include "lookupTable.h"
    #include "rootComponent.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        // manually generated small table:
        std::vector<std::pair<int, double>> values{{0, 0.5}, {2, 1.5}, {4, 2.5}, {6, 3.5}};
        LookupTable<int32_t, double>             table("small_table", root, std::move(values));

        // interpolate inputs
        auto output = table.interpolate(-1); // underflow, 0.5
        output = table.interpolate(0);       // 0.5
        output = table.interpolate(2);       // 1.5
        output = table.interpolate(4);       // 2.5
        output = table.interpolate(7);       // overflow, 3.5
        // reset between not-connected uses to clear cached data
        table.reset();

        output = table.interpolate(1); // 1.0
        output = table.interpolate(3); // 2.0
        output = table.interpolate(5); // 3.0
        table.reset();

        // access values randomly using binary search:
        output = table.interpolate(5, true); // 1.0
        output = table.interpolate(3, true); // 2.0
        output = table.interpolate(1, true); // 3.0
        table.reset();

        // access value by index:
        output = table[3]; // 3.5

        // access entire table by reference
        const auto& table = table.getData();

        return 0;
    }

.. code-block:: cpp

    #include <vector>

    #include "lookupTable.h"
    #include "generateFunction.h"
    #include "rootComponent.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        // automatially generated larger table in range 0 to 10.0, with 100 points:
        LookupTable<double> lin_table("table", root,
          fgc4::utils::generateFunction<int, double>([](const auto x){return 2*x + 1.5;}, 0.0, 10.0, 100), true);

        // all access done using index-search:
        auto output = lin_table.interpolate(-1);  // underflow, 1.5
        output = lin_table.interpolate(0, true);  // flag has no effect, 1.5
        output = lin_table.interpolate(11);       // overflow, 21.5

        // reset between not-connected uses to clear cached data
        lin_table.reset();

        return 0;
    }

.. _periodicLookupTable_component:

Periodic look-up table
----------------------

The :code:`PeriodicLookupTable` class derives from the :code:`LookupTable` and provides the same interface for all interactions, including template
parameters. All the assumptions regarding the provided table are the same as in the :code:`LookupTable` class. The only difference is the behaviour
when under- and overflow is encountered. In that case, the periodic boundary conditions are enforced using standard library's
:code:`std::fmod` function. The code does not verify if the boundary conditions of the provided table are fulfilled.

This class is intended to be used with every periodic boundary condition function, for example trigonometrical functions.

For more details regarding the API, see the :ref:`API documentation for PeriodicLookupTable <periodicLookupTable_api>`.

Usage examples
^^^^^^^^^^^^^^

.. code-block:: cpp

    #include <numbers>

    #include "lookupTable.h"
    #include "generateFunction.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        auto constexpr two_pi = 2.0 * std::numbers::pi;
        // automatially generated larger table in range 0 to 2 PI, with 1000 points:
        PeriodicLookupTable<double> sin_table("table", &root,
          fgc4::utils::generateFunction<double, double>(std::sin, 0.0, two_pi, 1000), true);

        // all access done using index-search:
        auto output = sin_table.interpolate(0);  // 0.0
        output = sin_table.interpolate(std::numbers::pi * 0.5); //  1.0
        output = sin_table.interpolate(std::numbers::pi, true); // flag has no effect, 0.0

        output = sin_table.interpolate(-std::numbers::pi); // underflow, x equivalent to pi, 0.0
        output = sin_table.interpolate(3.5 * std::numbers::pi); // overflow, x equivalent to 1.5 pi, -1.0

        // reset between not-connected uses to clear cached data
        sin_table.reset();

        return 0;
    }

.. _sinLookupTable_component:

Sine look-up table
------------------

:code:`SinLookupTable` is a convenience :code:`Component` for interacting with a look-up table containing a sine function.
Internally, it owns a :code:`PeriodicLookupTable` holding a sine function with argument range from 0.0 to :math:`2\pi`.
The utility function, :ref:`generateFunction <generateFunction>`, is used to generate the equally-spaced sine function
table with the desired number of points. The number of points for the internal table is a constructor parameter.

An additional convenience method allows to call the :code:`SinLookupTable` object as if it was the standard-library function:

.. code-block:: cpp

    SinLookupTable sin_table(name, parent, 10000);
    sin_table(std::numbers::pi * 3.5); // -1.0

For more details regarding the API, see the :ref:`API documentation for SinLookupTable <sinLookupTable_api>`.

Usage examples
^^^^^^^^^^^^^^

.. code-block:: cpp

    #include <array>

    #include "sinLookupTable.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        SinLookupTable sin_table("sin_table", &root, 1000);

        auto output = sin_table.interpolate(0);  // 0.0
        output = sin_table.interpolate(std::numbers::pi * 0.5); // 1.0
        output = sin_table.interpolate(std::numbers::pi, true); // flag has no effect, 0.0

        output = sin_table.interpolate(-std::numbers::pi);      // underflow, x equivalent to pi, 0.0
        output = sin_table.interpolate(3.5 * std::numbers::pi); // overflow, x equivalent to 1.5 pi, -1.0

        // reset between not-connected uses to clear cached data
        sin_table.reset();

        // equivalent to:
        output = sin_table(0);  // 0.0
        output = sin_table(std::numbers::pi * 0.5); // 1.0
        output = sin_table(std::numbers::pi, true); // flag has no effect, 0.0
        cos_table.reset();

        output = sin_table(-std::numbers::pi);      // underflow, x equivalent to pi, 0.0
        output = sin_table(3.5 * std::numbers::pi); // overflow, x equivalent to 1.5 pi, -1.0
        cos_table.reset();

        return 0;
    }

.. _cosLookupTable_component:

Cosine lookup table
-------------------

Convenience :code:`Component` for interacting with a look-up table containing a cosine function, with argument range from 0.0 to :math:`2\pi`.
Completely analogous to the :code:`SinLookupTable` with all features except for the stored function.

For more details regarding the API, see the :ref:`API documentation for CosLookupTable <cosLookupTable_api>`.

Usage examples
^^^^^^^^^^^^^^

.. code-block:: cpp

    #include <array>

    #include "cosLookupTable.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        CosLookupTable cos_table("cos_table", root, 1000);

        auto output = cos_table.interpolate(0);  // 1.0
        output = cos_table.interpolate(std::numbers::pi * 0.5); // 0.0
        output = cos_table.interpolate(std::numbers::pi, true); // flag has no effect, 1.0

        output = cos_table.interpolate(-std::numbers::pi);      // underflow, x equivalent to pi, 1.0
        output = cos_table.interpolate(4.5 * std::numbers::pi); // overflow, x equivalent to 0.5 pi, 0.0

        // reset between not-connected uses to clear cached data
        cos_table.reset();

        // equivalent to:
        output = cos_table(0);  // 1.0
        output = cos_table(std::numbers::pi * 0.5); // 0.0
        output = cos_table(std::numbers::pi, true); // flag has no effect, 1.0
        cos_table.reset();

        output = cos_table(-std::numbers::pi);      // underflow, x equivalent to pi, 1.0
        output = cos_table(3.5 * std::numbers::pi); // overflow, x equivalent to 1.5 pi, 0.0
        cos_table.reset();

        return 0;
    }

.. _lookupTable_performance:

Performance
-----------

The performance of each of the look-up :code:`Components` depends mainly on the method the stored table
is accessed, its size, the distance between the points, and how often the same sector is accessed sequentially.

The table below gives a rough insight into the performance that can be expected from each of the :code:`Components`:

.. list-table::
    :header-rows: 1

    * - Class
      - Size
      - Access type
      - Access time [ns]
      - Difference [\%]
    * - LookupTable
      - 10
      - linear, 1 hit/section
      -
      -
    * - LookupTable
      - 100
      - linear, 1 hit/section
      -
      -
    * - LookupTable
      - 1000
      - linear, 1 hit/section
      -
      -
    * - LookupTable
      - 10000
      - linear, 1 hit/section
      -
      -
    * - LookupTable
      - 10000
      - linear, 10 hit/section
      -
      -
    * - LookupTable
      - 1000
      - random, 1 hit/section
      -
      -
    * - LookupTable
      - 100
      - index
      -
      -
    * - LookupTable
      - 1000
      - index
      -
      -
    * - LookupTable
      - 10000
      - index
      -
      -
    * - std::sin
      - N/A
      - random
      - 160
      -
    * - SinLookupTable
      - 1000
      - random
      - 140
      - -20\%