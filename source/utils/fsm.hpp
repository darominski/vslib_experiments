//! @file
//! @brief  Generic Finite State Machine
//! @author Dariusz Zielinski

#pragma once

#include <functional>
#include <map>
#include <utility>
#include <vector>

namespace utils
{
    //! A bool constant to be used as a semantic flag
    constexpr bool FsmCascade = true;

    //! This class is used as a return type from transition function. It carries
    //! the information whether the transition should take effect and whether to cascade.
    //!
    //! @tparam State class of a user enum with states of user's FSM
    template<class State>
    class FsmTransitionResult
    {
      public:
        //! Default constructor that creates a transition result that means: no transition
        FsmTransitionResult() = default;

        // **********************************************************

        //! Constructor used to create a transition result that means: transition to \p state
        //! The implicit conversion from State to FsmTransitionResult is intended.
        //!
        //! @param state state to which the FSM should make a transition
        //! @param cascade whether to cascade, i.e. execute the new state in the same FSM cycle
        FsmTransitionResult(const State& state, bool cascade = false)
            : m_state(state),
              m_default(false),
              m_cascade(cascade)
        {
        }

        // **********************************************************

        //! @return The transition state
        State state() const
        {
            return m_state;
        }

        // **********************************************************

        //! @return True if transition is happening
        [[nodiscard]] bool isDefault() const
        {
            return m_default;
        }

        // **********************************************************

        //! @return True if cascade transition is requested
        [[nodiscard]] bool cascade() const
        {
            return m_cascade;
        }

      private:
        State m_state;             //!< State of the transition
        bool  m_default = true;    //!< Is default constructed?
        bool  m_cascade = false;   //!< Has cascade been requested?
    };

    // **********************************************************

    //! This class is used to create Finite State Machine and it's meant to be used from within another class
    //! which type is denoted by \p Parent template parameter.
    //! The FSM contains states and transitions. The update() method will iterate over each transition for the current
    //! state and execute them to check if the state should be changed. For each state a state function is executed.
    //! When making a transition, the transition can cascade - if so, then another iteration of update() function
    //! is executed, resulting in immediate execution of a state function of the new state and checking of transitions.
    //!
    //! @tparam State Enum class representing different states of the FSM.
    //! @tparam Parent Class from within the FSM is used.
    template<class State, class Parent, bool state_first = true>
    class Fsm
    {
        //! Forward declaration of the internal structure holding a state function and its
        //! respective transition functions, for a given state.
        struct StateObj;

      public:
        //! Convenience alias representing pointer to a member function of the Parent class, for a state function.
        using StateFunc = void (Parent::*)();

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = FsmTransitionResult<State> (Parent::*)();

        // **********************************************************

        //! @param parent Pointer to the Parent class, usually this should be simply 'this' pointer.
        //! @param starting_state Initial state of the FSM
        explicit Fsm(Parent& parent, const State& starting_state)
            : m_parent(parent),
              m_state(starting_state),
              m_starting_state(starting_state)
        {
        }

        // **********************************************************

        //! This function register new state, i.e. adds mapping between State value, state function and the state's
        //! transition functions.
        //!
        //! @param state Enum value of the State enum class.
        //! @param state_func State function, that has to be member function of the Parent class.
        //! @param transitions Vector of transition functions for this state. Each function  has to be member function
        //! of the Parent class.
        void addState(const State& state, const StateFunc& state_func, const std::vector<TransitionFunc>& transitions)
        {
            m_states.emplace(state, StateObj{state_func, transitions});
        }

        // **********************************************************

        //! @return Current state of the FSM.
        State getState() const
        {
            return m_state;
        }

        //! This function will update state machine. First, it will execute state function for the
        //! current state. Then it'll iterate over transition functions for the current state, until some transition
        //! changes the state. If that happens, the returned state becomes the new state.
        //! If no cascade was requested, the function finishes. Otherwise, a state function of the new state is
        //! executed and its transitions are checked.
        //! Note: this has potential to become an infinite loop, if the FSM design is flawed.
        void update()
        {
            FsmTransitionResult<State> transition_result;
            bool                       state_changed = false;

            do
            {
                // Clear transition result and state_changed in every state loop
                transition_result = {};
                state_changed     = false;

                if constexpr (state_first)
                {
                    executeState();
                }

                // Invoke transitions functions for the current state, until a new state is found
                for (const auto& transition : m_states[m_state].transitions)
                {
                    // Invoke transition
                    transition_result = std::invoke(transition, m_parent);

                    if (not transition_result.isDefault())
                    {
                        state_changed = (m_state != transition_result.state());
                    }

                    if (state_changed)
                    {
                        // Change to the new state
                        m_state = transition_result.state();
                        break;
                    }

                    if (not transition_result.isDefault())
                    {
                        break;
                    }
                }

                if constexpr (not state_first)
                {
                    executeState();
                }
            } while (state_changed && transition_result.cascade());
        }

        void reset()
        {
            m_state = m_starting_state;
        }

      private:
        void executeState()
        {
            // Invoke state function for the current state
            auto state_func = m_states[m_state].state_func;
            if (state_func != nullptr)
            {
                std::invoke(state_func, m_parent);
            }
        }

        //! This class has been described near its forward declaration.
        struct StateObj
        {
            //! Default constructor creating empty state.
            StateObj() = default;

            //! Saves a state function and a vector of transition functions.
            //! @param state_func State function, that has to be member function of the Parent class.
            //! @param transitions Vector of transition functions for this state. Each function  has to be member
            //! function of the Parent class.
            StateObj(const StateFunc& state_func, const std::vector<TransitionFunc>& transitions)
                : state_func(state_func),
                  transitions(transitions)
            {
            }

            StateFunc                   state_func;   //!< State function
            std::function<void(void)>   function;
            std::vector<TransitionFunc> transitions;   //!< Vector of transition functions for this state.
        };

        // **********************************************************

        Parent& m_parent;   //!< Reference to the object of the Parent class, used to execute state and transition
                            //!< functions.
        State                     m_state;            //!< Current state of the FSM.
        State                     m_starting_state;   //!< Starting state of the FSM.
        std::map<State, StateObj> m_states;   //!< Mapping between each state and its state and transition functions.
    };
}

// EOF
