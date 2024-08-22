.. _user_finite_state_machine:

====================
Finite state machine
====================

This section describes how to set up and use the finite state machine (FSM) interface
implemented in the software stack of the VSlib (not a part of VSlib).

First, you need to know what states and transitions you may require to control
your hardware. An example how startup of the VSlib is handled by the FSM can be found
:ref:`here <vloop_startup>`.

Second, you will need to turn the list of your states into an enumeration, for example:

.. code-block:: cpp

    enum class States
    {
        off,
        init,
        starting,
        precharge,
        charging,
        blocking,
        on,
        stopping,
        fault,
        fault_stopping,
        fault_off
        resetting,
    };

Then, define your class implementing state machine interface that defines transitions,
behaviour during a transition, and when entering a state:

.. code-block:: cpp

    class FSMachine
    {
        constexpr static bool execute_state_first = false; // whether to execute state before transition

        using StateMachine = utils::Fsm<States, FSMachine, execute_state_first>;

        using TransResVS = utils::FsmTransitionResult<VSStates>;

        using StateFunc = std::function<void(void)>;

        public:
            FSMachine()
            : m_fsm(*this, States::off)
        {
            // CAUTION: The order of transition method matters

            // clang-format off
            m_fsm.addState(States::off,             &FSMachine::onOff,           {&FSMachine::toFaultStopping}, &FSMachine::toStopping}, &FSMachine::toStarting});
            m_fsm.addState(States::starting,        &FSMachine::onStarting,      {&FSMachine::toFaultStopping}, &FSMachine::toStopping}, &FSMachine::toPrecharge});
            m_fsm.addState(States::precharge,       &FSMachine::onPrecharge,     {&FSMachine::toFaultStopping}, &FSMachine::toStopping}, &FSMachine::toCharging});
            m_fsm.addState(States::charging,        &FSMachine::onCharging,      {&FSMachine::toFaultStopping}, &FSMachine::toStopping}, &FSMachine::toBlocking});
            m_fsm.addState(States::blocking,        &FSMachine::onBlocking,      {&FSMachine::toFaultStopping}, &FSMachine::toStopping}, &FSMachine::toOn});
            m_fsm.addState(States::on,              &FSMachine::onOn,            {&FSMachine::toFaultStopping}, &FSMachine::toStopping}, &FSMachine::toStopping});
            m_fsm.addState(States::stopping,        &FSMachine::onStopping,      {&FSMachine::toFaultStopping}, &FSMachine::toStopping}, &FSMachine::toOff});
            m_fsm.addState(States::reset,           &FSMachine::onReset,         {&FSMachine::toFaultStopping}, &FSMachine::toStopping}, &FSMachine::toInit});
            m_fsm.addState(States::init,            &FSMachine::onInit,          {&FSMachine::toFaultStopping}, &FSMachine::toStopping}, &FSMachine::toOff});
            m_fsm.addState(States::fault,           &FSMachine::onFault,         {&FSMachine::toFaultStopping});
            m_fsm.addState(States::fault_stopping,  &FSMachine::onFaultStopping, {&FSMachine::toFaultOff});
            m_fsm.addState(States::fault_off,       &FSMachine::onFaultOff,      {});
            // clang-format on
        }

        void update()
        {
            m_fsm.update();
        }

        private:
            StateMachine m_fsm;

            void onOff()
            {
                // your logic describing what happens when entering off state
            }

            void onStarting()
            {
                // your logic describing what happens when entering starting state
            }

            void onPrecharge()
            {
                // your logic describing what happens when FSM enters precharge state
            }

            void onCharging()
            {
                // your logic describing what happens when FSM enters precharge state
            }

            void onBlocking()
            {
                // your logic describing what happens when FSM enters blocking state
            }

            void onOn()
            {
                // your logic describing what happens when FSM enters on state
            }

            void onStopping()
            {
                // your logic describing what happens when FSM enters stopping state
            }

            void onReset()
            {
                // your logic describing what happens when FSM enters reset state
            }

            void onInit()
            {
                // your logic describing what happens when FSM enters init state
            }

            void onFault()
            {
                // your logic describing what happens when FSM enters fault state
            }

            void onFaultStopping()
            {
                // your logic describing what happens when FSM enters fault_stopping state
            }

            void onFaultOff()
            {
                // your logic describing what happens when FSM enters fault_off state
            }

            TransResVS toFaultStopping()
            {
                // your logic deciding whether to transition or not, and to which state
                return {States::fault_stopping};
            }

            TransResVS toStarting()
            {
                // your logic deciding whether to transition or not, and to which state
                return {States::starting};
            }

            TransResVS toPrecharge()
            {
                // your logic deciding whether to transition or not, and to which state
                return {States::precharge};
            }

            TransResVS toCharging()
            {
                // your logic deciding whether to transition or not, and to which state
                return {States::charging};
            }

            TransResVS toBlocking()
            {
                // your logic deciding whether to transition or not, and to which state
                return {States::blocking};
            }

            TransResVS toOn()
            {
                // your logic deciding whether to transition or not, and to which state
                return {States::on};
            }

            TransResVS toStopping()
            {
                // your logic deciding whether to transition or not, and to which state
                return {States::stopping};
            }

            TransResVS toOff()
            {
                // your logic deciding whether to transition or not, and to which state
                return {States::off};
            }

            TransResVS toInit()
            {
                // your logic deciding whether to transition or not, and to which state
                return {States::init};
            }

            TransResVS toFaultOff()
            {
                // your logic deciding whether to transition or not, and to which state
                return {States::fault_off};
            }

            TransResVS toFault()
            {
                // your logic checking if a fault case happened
                // if (error_case_fulfilled)
                // {
                //   return {States::fault};
                // }
                // empty bracket = no transition, FSM stays in the same state
                return {};
            }

    };

