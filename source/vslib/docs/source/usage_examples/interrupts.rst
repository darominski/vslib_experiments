.. _interrupts:

==========
Interrupts
==========

This sections describes the interface for interacting with hardware interrupt sources:
their configuration and definition of handlers to be called when an interrupt is triggered.

General interface
-----------------

There two classes in VSlib implementing the :ref:`Interrupt <interrupt_api>` interface:
:ref:`TimerInterrupt <timerInterrupt_interface>` and :ref:`PeripheralInterrupt <peripheralInterrupt_interface>`.
Those two classes thus require a unique name to identify the interrupt, require a pointer to the owning
(templated) user-defined :code:`Converter` class, and a callback.

The callback function (handler) is expected to have the following signature:
:code:`void(Converter&)`, so that the handler has the context of :code:`Components` and other objects
defined by your :code:`Converter` instance.

There are also two methods used to enable and disable interrupt handling: :code:`start` and :code:`stop`,
respectively. They are overwritten by each implementation of the interface to perform actions necessary
to enable/disable the relevant interrupt handling.

.. _timerInterrupt_interface:

TimerInterrupt
--------------

:code:`TimerInterrupt` defines an interrupt to be called every defined number of microseconds.
The constructor takes four parameters: a unique :code:`std::string` name, a pointer to the owning
parent (assumed to be your own class defining the :code:`Converter`), the delay in microseconds,
and a callback function.

The clock used to trigger this interrupt is the single CPU clock, and is not synchronised (nor synchronisable)
with the programmable logic (PL). The main use of this class is all testing purposes when interactions with the PL
is not desired.

For more details regarding the API, see the :ref:`API documentation for TimerInterrupt <timerInterrupt_api>`.

Usage example
^^^^^^^^^^^^^

.. code-block:: cpp

    #include "timerInterrupt.h"
    #include "converter.h"
    #include "rootComponent.h"

    class Converter : public vslib::IConverter
    {
      public:
        Converter(RootComponent& root) noexcept
            : vslib::IConverter("your_converter", root),
              interrupt_1("cpu_timer", this, 10.0, RTTask) // 10 microseconds
        {
            // initialize all your objects that need initializing
        }

        // Define your public Components here
        vslib::TimerInterrupt<Converter> interrupt_1;

        void init() override
        {
            interrupt_t.start(); // enables the handling of the interrupt
        }

        //! Callback used when interrupt_1 triggers
        static void RTTask(Converter& converter)
        {
            // perform real-time actions expected when interrupt_1 is triggered
        }

        private:
            int m_interrupt_id;
    };

.. _peripheralInterrupt_interface:

PeripheralInterrupt
-------------------

:code:`PeripheralInterrupt` defines an interrupt to be called based on the trigger located in the programmable
logic (PL). The constructor takes five parameters: a unique :code:`std::string` name, a pointer to the owning
parent (assumed to be your own class defining the :code:`Converter`), unique interrupt ID corresponding to the interrupt
in the PL, an interrupt priority (of type :code:`vslib::InterruptPriority`), and a callback function.
There are three possible :code:`InterruptPriority` levels: :code:`low`, :code:`medium`, and :code:`high`.


For more details regarding the API, see the :ref:`API documentation for PeripheralInterrupt <peripheralInterrupt_api>`.

Usage example
^^^^^^^^^^^^^

.. code-block:: cpp

    #include "peripheralInterrupt.h"
    #include "converter.h"
    #include "rootComponent.h"

    class Converter : public vslib::IConverter
    {
      public:
        Converter(RootComponent& root) noexcept
            : vslib::IConverter("your_converter", root),
              m_interrupt_id{121 + 0},   // interrupt ID
              interrupt_1("AuroraLink", this, m_interrupt_id, vslib::InterruptPriority::high, RTTask)
        {
            // initialize all your objects that need initializing
        }

        // Define your public Components here
        vslib::PeripheralInterrupt<Converter> interrupt_1;

        void init() override
        {
            interrupt_t.start(); // enables the handling of the interrupt
        }

        //! Callback used when interrupt_1 triggers
        static void RTTask(Converter& converter)
        {
            // perform real-time actions expected when interrupt_1 is triggered
        }

        private:
            int m_interrupt_id;
    };