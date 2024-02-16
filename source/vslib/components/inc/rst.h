//! @file
//! @brief Class definiting two-degrees-of-freedom controller (RST).
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

#include "component.h"
#include "parameter.h"

namespace vslib
{
    template<size_t ControllerLength>
    class RST : public Component
    {
      public:
        RST(std::string_view name, Component* parent = nullptr)
            : Component("RST", name, parent),
              r(*this, "r", -12.0, 10.0),
              s(*this, "s", -12.0, 10.0),
              t(*this, "t", -12.0, 10.0)
        {
        }

        //! Updates histories of measurements and references and moves the head of the history buffer
        //!
        //! @param measurement Current value of the process value
        //! @param reference Current value of the set-point reference
        void update_input_histories(double measurement, double reference) noexcept
        {
            m_measurements[m_head] = measurement;
            m_references[m_head]   = reference;

            m_head++;
            if (m_head == ControllerLength)
            {
                m_history_ready = true;
                m_head          -= ControllerLength;
            }
        }

        //! Calculates one iteration of the controller algorithm
        //!
        //! @param process_value Current process value (measurement)
        //! @param reference Reference value for the controller
        //! @return Controller output of the iteration
        double control(double process_value, double reference) noexcept
        {
            // based on logic in regRstCalcActRT from CCLIBS libreg regRst.c
            m_measurements[m_head] = process_value;
            m_references[m_head]   = reference;

            double actuation = t[0] * m_references[m_head] - r[0] * m_measurements[m_head];
            for (size_t index = 1; index < ControllerLength; index++)
            {
                int64_t buffer_index = (m_head - index);
                if (buffer_index < 0)
                {
                    buffer_index += ControllerLength;
                }

                actuation += t[index] * m_references[buffer_index] - r[index] * m_measurements[buffer_index]
                             - s[index] * m_actuations[buffer_index];
            }

            actuation /= s[0];

            m_actuations[m_head] = actuation;   // update actuations

            m_head++;
            if (m_head == ControllerLength)
            {
                m_head -= ControllerLength;
            }
            return actuation;
        }

        //! Updates the most recent reference in the history, used in cases actuation goes over the limit
        //!
        //! @param updated_actuation Actuation that actually took place after clipping of the calculated actuation
        void update_reference(double updated_actuation)
        {
            // based on logic of regRstCalcRefRT from CCLIBS libreg's regRst.c
            m_actuations[m_head - 1] = updated_actuation;

            double reference = 0;
            for (size_t index = 0; index < ControllerLength; index++)
            {
                int64_t buffer_index = (m_head - 1 - index);
                if (buffer_index < 0)
                {
                    buffer_index += ControllerLength;
                }

                reference += t[index] * m_references[buffer_index] - r[index] * m_measurements[buffer_index]
                             - s[index] * m_actuations[buffer_index];
            }
            m_references[m_head - 1] = reference;
        }

        //! Resets the controller to the initial state by zeroing the history.
        void reset() noexcept
        {
            std::fill(std::begin(m_measurements), std::end(m_measurements), 0);
            std::fill(std::begin(m_references), std::end(m_references), 0);
            std::fill(std::begin(m_actuations), std::end(m_actuations), 0);
            m_head          = 0;
            m_history_ready = false;
        }

        //! Returns flag whether the reference and measurement histories are filled and RST is ready to regulate
        //!
        //! @return True if reference and measurement histories are filled, false otherwise
        bool isReady() const noexcept
        {
            return m_history_ready;
        }

        // ************************************************************
        // Settable Parameters

        Parameter<std::array<double, ControllerLength>> r;   //<! reference coefficients
        Parameter<std::array<double, ControllerLength>> s;   //<! disturbance coefficients
        Parameter<std::array<double, ControllerLength>> t;   //<! control coefficients

        //! Update parameters method, called after paramaters of this component are modified
        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            // Jury's stability test, based on logic implemented in CCLIBS regRst.c
            if (r[0] == 0)
            {
                return fgc4::utils::Warning("First element of r coefficients is zero.");
            }

            if (s[0] == 0)
            {
                return fgc4::utils::Warning("First element of s coefficients is zero.");
            }

            if (t[0] == 0)
            {
                return fgc4::utils::Warning("First element of t coefficients is zero.");
            }

            const auto& maybe_warning_s = jurysStabilityTest(s.value());
            if (maybe_warning_s.has_value())
            {
                return maybe_warning_s.value();
            }

            const auto& maybe_warning_t = jurysStabilityTest(t.value());
            if (maybe_warning_t.has_value())
            {
                return maybe_warning_t.value();
            }

            // r is not checked in CCLIBS and neither they are here

            // no issues, RST is stable, and parameters are valid
            return {};
        }

      private:
        int64_t                              m_head{0};        // Index to latest entry in the history
        std::array<double, ControllerLength> m_measurements;   // RST measurement history
        std::array<double, ControllerLength> m_references;     // RST reference history
        std::array<double, ControllerLength> m_actuations;     // RST actuation history.

        bool m_history_ready{false};                   // flag to mark RST ref and meas histories are filled
        std::array<double, ControllerLength> m_b{0};   // variable used in Jury's test, declaring them here avoids
                                                       // allocation whenever jurysStabilityTest is called
        std::array<double, ControllerLength> m_a{0};   // variable used in Jury's test, declaring them here avoids
                                                       // allocation whenever jurysStabilityTest is called

        std::optional<fgc4::utils::Warning> jurysStabilityTest(const std::array<double, ControllerLength>& coefficients)
        {
            int64_t coefficient_length = 1;
            while (coefficient_length < ControllerLength && coefficients[coefficient_length] != 0.0F)
            {
                coefficient_length++;
            }
            coefficient_length--;

            double sum_even{0};
            double sum_odd{0};
            double sum_abs{0};

            for (size_t index = 0; index <= coefficient_length; index++)
            {
                const double coefficient = coefficients[index];

                sum_abs += abs(coefficient);

                if ((index & 1) == 0)
                {
                    sum_even += coefficient;
                }
                else
                {
                    sum_odd += coefficient;
                }
            }

            // Stability check 1 : Sum(even coefficients) >= Sum(odd coefficients)
            if (sum_even < sum_odd)
            {
                return fgc4::utils::Warning(
                    "RST unstable: sum of even coefficients less or equal than of odd coefficients.\n"
                );
            }

            // Stability check 2 : Sum(coefficients) > 0 - allow for floating point rounding errors
            if (((sum_even + sum_odd) / sum_abs) < -fgc4::utils::constants::floating_point_min_threshold)
            {
                return fgc4::utils::Warning(
                    "RST unstable: sum of coefficients below minimal floating-point threshold.\n"
                );
            }

            // Stability check 3 : Jury's Stability Test for unstable roots

            while (coefficient_length > 2)
            {
                for (size_t index = 0; index <= coefficient_length; index++)
                {
                    m_a[index] = m_b[index];
                }

                double const d = m_a[coefficient_length] / m_a[0];

                for (size_t index = 0; index < coefficient_length; index++)
                {
                    m_b[index] = m_a[index] - d * m_a[coefficient_length - index];
                }

                // First element of every row of Jury's array > 0 for stability
                if (m_b[0] <= 0.0F)
                {
                    return fgc4::utils::Warning("RST unstable: the first element of Jury's array is not above zero.\n");
                }

                coefficient_length--;
            }

            // No warning returned because coefficients are stable
            return {};
        }
    };
}   // namespace vslib
