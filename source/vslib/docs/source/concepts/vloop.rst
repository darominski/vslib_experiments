.. _vloop_design:

================
Vloop definition
================

This section describes how the voltage regulation workflow (vloop) can be defined based on the elements available in the VSlib.

General interface
-----------------

Vloop is a binary containing the voltage regulation workflow designed fully by the user for their bespoke use-case. This workflow
definition is provided to the VSlib infrastructure by filling out the skeleton class called :code:`Converter`, implementing
the :ref:`IConverter <iconverter_api>` interface.

The skeleton class has the following structure:

.. code-block:: cpp

    #pragma once

    #include "iconverter.h"

    namespace user
    {
        // Skeleton example of the Converter class to be filled

        class Converter : public vslib::IConverter
        {
        public:
            Converter(vslib::RootComponent& root) noexcept
                : vslib::IConverter("your_converter_name", root)
            {
                // initialize all your objects that need initializing
            }

            // Define your public Components here

            // end of your Components

            // Define your Parameters here

            // end of your Parameters

            void init() override
            {
                // Define the workflow that is required to start-up your converter, state machine,
                // your custom communication links, etc.
            }

            void backgroundTask() override
            {
                // Define the work that needs to be done on your Converter in the background.
                // This code will be called in a loop on the VSlib side, so please do not implement
                // infinite loops.
            }

            static void RTTask(Converter& converter)
            {
                // Define the work to be done in real-time when an interrupt is called
            }

        private:
            // Define your private members

            // Add your finite-state machine here
        };

    }   // namespace user

Before filling it out the user will need to decide what kind of :code:`Components`, :code:`Parameters` they
need to perform the control of the voltage source, as well as the number of interrupts and real-time
tasks attached to each of them. Finally, a :ref:`Finite State Machine <user_finite_state_machine>` to control
the workflow of the regulation will need to be designed and implemented.

