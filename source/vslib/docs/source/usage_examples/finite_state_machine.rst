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
        start,
        cycling,
        fault
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
            : m_fsm(*this, States::start)
        {
            // CAUTION: The order of transition method matters

            // clang-format off
            m_fsm.addState(States::start,   &FSMachine::onStart,   {&FSMachine::toCycling, &FSMachine::toFault});
            m_fsm.addState(States::cycling, &FSMachine::onCycling, {&FSMachine::toFault});
            m_fsm.addState(States::fault,   &FSMachine::onCycling, {&FSMachine::toFault});

            // clang-format on
        }

        void update()
        {
            m_fsm.update();
        }

        private:
            StateMachine m_fsm;

            void onStart()
            {
                // your logic describing what happens when FSM enters start state
            }

            void onCycling()
            {
                // your logic describing what happens when FSM enters cycling state
            }

            void onFault()
            {
                // your logic describing what happens when FSM enters fault state
            }

            TransResVS toCycling()
            {
                // your logic describing what happens during transition or to which states this method allows transition to, e.g:
                // if (error_case_fulfilled)
                // {
                //   return {States::fault};
                // }
                return {States::cycling};
            }

            TransResVS toFault()
            {
                // your logic describing what happens during transition or to which states this method allows transition to
                return {States::fault};
            }

    };

