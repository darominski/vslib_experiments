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

    #include "vslib.hpp"

    namespace fgc::user
    {
        class Converter : public vslib::IConverter
        {
        public:
            Converter(vslib::RootComponent& root) noexcept
            : vslib::IConverter("example", root),
              interrupt_1("stg", *this, 128, vslib::InterruptPriority::high, RTTask),
              power_3ph_instant("power_3ph_instant", *this),
            {
            }

            // Define your interrupts here
            vslib::PeripheralInterrupt<Converter> interrupt_1;

            // Define your public Components here
            vslib::InstantaneousPowerThreePhase power_3ph_instant;

            void init() override
            {
                interrupt_1.start();
            }

            void backgroundTask() override
            {
            }

            static void RTTask(Converter& converter)
            {
                // Read the 3-phase voltages and currents
                const double v_a           = converter.m_data[0];
                const double v_b           = converter.m_data[1];
                const double v_c           = converter.m_data[2];
                const double i_a           = converter.m_data[3];
                const double i_b           = converter.m_data[4];
                const double i_c           = converter.m_data[5];

                // calculate instantaneous P and Q components of power
                const auto [p_meas, q_meas] = converter.power_3ph_instant.calculate(
                    v_a, v_b, v_c, i_a, i_b, i_c
                );
            }

            private:
                std::array<double, 5> m_data{0.0};
        };
    }   // namespace fgc::user
