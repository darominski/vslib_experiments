.. _calculations:

============
Calculations
============

This chapter describes all calculation Components implemented in the VSlib that do not belong in other categories.

General interface
-----------------

All of the classes described below derive from and implement the :ref:`Component <component_api>` interface.
They have a single access method, called :code:`calculate`. The number of arguments each implementation
takes depends on the use-case, as well as the size of the returned structure.

.. _instantaneousPowerThreePhase_component:

Instantaneous power of a three phase system
-------------------------------------------

:code:`InstantaneousPowerThreePhase` component implements the calculation the instantaneous active and reactive power
based on the three phase voltages and currents components in abc reference frame. It has two :code:`double`-type
:code:`Parameters`: :code:`p_gain` and :code:`q_gain`, used to scale the calculated active and reactive power components,
respectively.

The :code:`calculate` method takes six obligatory :code:`double`-type arguments: one for each `a`, `b`, and `c` first
voltage and then current components of the three phase system. It outputs a :code:`tuple` of two values: active (P)
and reactive (Q) power, scaled by the appropriate gains.

The algorithm is as follows:

.. math::

    P &= v_{a} \cdot i_{a} + v_{b} \cdot i_{b} + v_{c} \cdot i_{c} \\
    \\
    v_{ab}   &= v_{a} - v_{b} \\
    v_{bc}   &= v_{b} - v_{c} \\
    v_{ca}   &= v_{c} - v_{a} \\
    Q &= \frac{1}{\sqrt{3}} \cdot \left( i_{a} \cdot v_{bc} + i_{b} \cdot v_{ca} + i_{c} \cdot v_{ab} \right) \\

For more details regarding the API, see the :ref:`API documentation for InstantaneousPowerThreePhase <instantaneousPowerThreePhase_api>`.

Usage example
^^^^^^^^^^^^^

.. code-block:: cpp

    #include "instantaneousPowerThreePhase.h"
    #include "rootComponent.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        InstantaneousPowerThreePhase power("power_3ph", root);

        // set p_gain and q_gain to your values, for no gain set them to 1.0

        std::array<double, 3> v_abc{230.0, -115.0, 115.0};
        std::array<double, 3> i_abc{10.0, -5.0, -5.0};

        const auto [p, q] = power.calculate(v_abc[0], v_abc[1], v_abc[2], i_abc[0], i_abc[1], i_abc[2]);

        return 0;
    }

