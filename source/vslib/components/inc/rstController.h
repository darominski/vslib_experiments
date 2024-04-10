//! @file
//! @brief Class definiting the algorithm behind the two-degrees-of-freedom RST controller.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <cstdint>
#include <optional>

#include "warningMessage.h"

namespace vslib
{
    template<size_t ControllerLength>
    class RSTController
    {
      public:
        RSTController()
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

            double actuation = m_t[0] * m_references[m_head] - m_r[0] * m_measurements[m_head];
            // int64_t buffer_index = m_head;
            for (int64_t index = 1; index < ControllerLength; index++)
            {
                int64_t buffer_index;
                if constexpr ((ControllerLength & (ControllerLength - 1)) == 0)
                {
                    // if ControllerLength is a power of two, the optimisation below is possible,
                    // speeding up execution by around 10%
                    buffer_index = (m_head - index) & ControllerLength;
                }
                else
                {
                    buffer_index = (m_head - index);
                    if (buffer_index < 0)
                    {
                        buffer_index += ControllerLength;
                    }
                }

                actuation += m_t[index] * m_references[buffer_index] - m_r[index] * m_measurements[buffer_index]
                             - m_s[index] * m_actuations[buffer_index];
            }
            actuation /= m_s[0];

            m_actuations[m_head] = actuation;   // update actuations

            if constexpr ((ControllerLength & (ControllerLength - 1)) == 0)
            {
                m_head = (m_head + 1) & ControllerLength;
            }
            else
            {
                m_head++;
                if (m_head == ControllerLength)
                {
                    m_head -= ControllerLength;
                }
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
            for (int64_t index = 0; index < ControllerLength; index++)
            {
                int64_t buffer_index;
                if constexpr ((ControllerLength & (ControllerLength - 1)) == 0)
                {
                    // if ControllerLength is a power of two, the optimisation below is possible,
                    // speeding up execution by around 10%
                    buffer_index = (m_head - 1 - index) & ControllerLength;
                }
                else
                {
                    buffer_index = (m_head - 1 - index);
                    if (buffer_index < 0)
                    {
                        buffer_index += ControllerLength;
                    }
                }

                reference += m_t[index] * m_references[buffer_index] - m_r[index] * m_measurements[buffer_index]
                             - m_s[index] * m_actuations[buffer_index];
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

        //! Performs the Jury's stability test on the provided array of coefficients
        //!
        //! @param coefficients Coefficients to be tested
        //! @return Optionally returns a Warning with relevant information if test failed, nothing otherwise
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
            m_b = coefficients;
            while (coefficient_length > 2)
            {
                m_a            = m_b;
                double const d = m_a[coefficient_length - 1] / m_a[0];

                for (size_t index = 0; index <= coefficient_length; index++)
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

        // ************************************************************
        // Getters

        //! Returns flag whether the reference and measurement histories are filled and RST is ready to regulate
        //!
        //! @return True if reference and measurement histories are filled, false otherwise
        [[nodiscard]] bool isReady() const noexcept
        {
            return m_history_ready;
        }

        //! Returns the actuation history buffer
        //!
        //! @return Reference to the history buffer holding previous actuations
        const auto& getActuations() noexcept
        {
            return m_actuations;
        }

        //! Returns the reference history buffer
        //!
        //! @return Reference to the history buffer holding previous references
        const auto& getReferences() noexcept
        {
            return m_references;
        }

        //! Returns the measurement history buffer
        //!
        //! @return Reference to the history buffer holding previous measurements
        const auto& getMeasurements() noexcept
        {
            return m_measurements;
        }

        //! Gets the R polynomial
        //!
        //! @return Array with R polynomial coefficients
        const auto& getR() const
        {
            return m_r;
        }

        //! Gets the S polynomial
        //!
        //! @return Array with S polynomial coefficients
        const auto& getS() const
        {
            return m_s;
        }

        //! Gets the T polynomial
        //!
        //! @return Array with T polynomial coefficients
        const auto& getT() const
        {
            return m_t;
        }

        // ************************************************************
        // Setters

        //! Sets the R polynomial
        //!
        //! @param r Array with R polynomial values to be set
        void setR(const std::array<double, ControllerLength>& r)
        {
            m_r = r;
        }

        //! Sets the S polynomial
        //!
        //! @param s Array with S polynomial values to be set
        void setS(const std::array<double, ControllerLength>& s)
        {
            m_s = s;
        }

        //! Sets the T polynomial
        //!
        //! @param t Array with T polynomial values to be set
        void setT(const std::array<double, ControllerLength>& t)
        {
            m_t = t;
        }

      private:
        int64_t m_head{0};   // Index to latest entry in the history

        std::array<double, ControllerLength> m_r{0};
        std::array<double, ControllerLength> m_s{0};
        std::array<double, ControllerLength> m_t{0};

        std::array<double, ControllerLength> m_measurements{0};   // RST measurement history
        std::array<double, ControllerLength> m_references{0};     // RST reference history
        std::array<double, ControllerLength> m_actuations{0};     // RST actuation history.

        bool m_history_ready{false};   // flag to mark RST ref and meas histories are filled

        // helper variables used in Jury's test
        std::array<double, ControllerLength> m_b{0};   // variable used in Jury's test, declaring them here avoids
                                                       // allocation whenever jurysStabilityTest is called
        std::array<double, ControllerLength> m_a{0};   // variable used in Jury's test, declaring them here avoids
                                                       // allocation whenever jurysStabilityTest is called
    };


    //! Control method returning the next actuation
    //!
    //! @param process_value Current easurment value
    //! @param reference Current reference value
    //! @return Next actuation value
    template<>
    double RSTController<3>::control(double input, double reference) noexcept
    {
        // This partial template specialization allows to speed-up the calculation of the RST actuation by about 15%
        const double actuation = (m_t[0] * reference - m_r[0] * input + m_t[1] * m_references[1]
                                  - m_r[1] * m_measurements[1] + m_t[2] * m_references[2] - m_r[2] * m_measurements[2]
                                  - (m_s[1] * m_actuations[1] + m_s[2] * m_actuations[2]))
                                 / m_s[0];

        m_actuations[2] = m_actuations[1];
        m_actuations[1] = actuation;

        m_measurements[2] = m_measurements[1];
        m_measurements[1] = input;

        m_references[2] = m_references[1];
        m_references[1] = reference;

        return actuation;
    }

}   // namespace vslib
