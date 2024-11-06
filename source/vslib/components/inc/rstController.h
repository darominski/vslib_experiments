//! @file
//! @brief Class defining the algorithm behind the two-degrees-of-freedom RST controller.
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
        //! Default constructor for RSTController class.
        RSTController(std::string_view name)
            : m_name(name)
        {
        }

        //! Updates histories of measurements and references and moves the head of the history buffer.
        //!
        //! @param reference Current value of the set-point reference
        //! @param measurement Current value of the process value
        void updateInputHistories(const double reference, const double measurement) noexcept
        {
            m_references[m_head]   = reference;
            m_measurements[m_head] = measurement;

            m_head++;
            if (m_head == (ControllerLength - 1))
            {
                m_history_ready = true;
                m_head          = 0;
            }
        }

        //! Calculates one iteration of the controller algorithm.
        //!
        //! @param reference Reference value for the controller
        //! @param measurement Current process value (measurement)
        //! @return Controller output of the iteration
        [[nodiscard]] double control(const double reference, const double measurement) noexcept
        {
            // based on logic in regRstCalcActRT from CCLIBS libreg regRst.c
            m_references[m_head]   = reference;
            m_measurements[m_head] = measurement;

            double actuation = m_t[0] * m_references[m_head] - m_r[0] * m_measurements[m_head];
            for (int64_t index = 1; index < ControllerLength; index++)
            {
                int64_t buffer_index = (m_head - index);
                if (buffer_index < 0)
                {
                    buffer_index += ControllerLength;
                }
                actuation += m_t[index] * m_references[buffer_index] - m_r[index] * m_measurements[buffer_index]
                             - m_s[index] * m_actuations[buffer_index];
            }
            actuation /= m_s[0];

            m_actuations[m_head] = actuation;   // update actuations

            m_head++;
            if (m_head == ControllerLength)
            {
                m_head = 0;
            }

            return actuation;
        }

        //! Updates the most recent reference in the history, used in cases actuation goes over the limit.
        //!
        //! @param updated_actuation Actuation that actually took place after clipping of the calculated actuation
        void updateReference(const double updated_actuation)
        {
            // based on simplified logic of regRstCalcRefRT from CCLIBS libreg's regRst.c for closed-loop
            size_t index = m_head - 1;
            if (m_head == 0)
            {
                index = ControllerLength - 1;
            }
            const double delta_actuation = updated_actuation - m_actuations[index];
            m_actuations[index]          = updated_actuation;
            m_references[index]          += delta_actuation * m_s[0] / m_t[0];
        }

        //! Updates the most recent reference in the history, used in cases actuation goes over the limit in the
        //! open-loop case.
        //!
        //! @param updated_actuation Actuation that actually took place after clipping of the calculated actuation
        void updateReferenceOpenLoop(const double updated_actuation)
        {
            // based on logic of regRstCalcRefRT from CCLIBS libreg's regRst.c for open loop calculation
            size_t prev_head = m_head - 1;
            if (m_head == 0)
            {
                prev_head = ControllerLength - 1;
            }
            m_actuations[prev_head] = updated_actuation;

            double reference = m_s[0] * updated_actuation + m_r[0] * m_measurements[prev_head];
            for (int64_t index = 1; index < ControllerLength; index++)
            {
                int64_t buffer_index = (prev_head - index);
                if (buffer_index < 0)
                {
                    buffer_index += ControllerLength;
                }
                reference += m_s[index] * m_actuations[buffer_index] + m_r[index] * m_measurements[buffer_index]
                             - m_t[index] * m_references[buffer_index];
            }
            m_references[prev_head] = reference / m_t[0];
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

        //! Performs the Jury's stability test on the provided array of coefficients.
        //!
        //! @param coefficients Coefficients to be tested
        //! @param name Name of the controller this test is run on
        //! @return Optionally returns a Warning with relevant information if test failed, nothing otherwise
        std::optional<fgc4::utils::Warning> jurysStabilityTest(const std::array<double, ControllerLength>& coefficients)
        {
            // Test re-implemented from CCLIBS libreg's regRst.c
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
            if (sum_odd > sum_even)
            {
                return fgc4::utils::Warning(fmt::format(
                    "{}: unstable, sum of even coefficients less or equal than of odd coefficients.\n", m_name
                ));
            }

            // Stability check 2 : Sum(coefficients) > 0 - allow for floating point rounding errors
            if (((sum_even + sum_odd) / sum_abs) < -fgc4::utils::constants::floating_point_min_threshold)
            {
                return fgc4::utils::Warning(
                    fmt::format("{}: unstable, sum of coefficients below minimal floating-point threshold.\n", m_name)
                );
            }

            // Stability check 3 : Jury's Stability Test for unstable roots
            m_b = coefficients;
            while (coefficient_length > 2)
            {
                double const d = m_b[coefficient_length - 1] / m_b[0];
                // First element of every row of Jury's array > 0 for stability
                if ((m_b[0] - d * m_b[coefficient_length]) <= 0.0F)
                {
                    return fgc4::utils::Warning(
                        fmt::format("{}: unstable, the first element of Jury's array is not above zero.\n", m_name)
                    );
                }

                coefficient_length--;
            }

            // No warning returned because coefficients are stable
            return {};
        }

        // ************************************************************
        // Getters

        //! Returns flag whether the reference and measurement histories are filled and RST is ready to regulate.
        //!
        //! @return True if reference and measurement histories are filled, false otherwise
        [[nodiscard]] bool isReady() const noexcept
        {
            return m_history_ready;
        }

        //! Returns the actuation history buffer.
        //!
        //! @return Reference to the history buffer holding previous actuations
        [[nodiscard]] const auto& getActuations() noexcept
        {
            return m_actuations;
        }

        //! Returns the reference history buffer.
        //!
        //! @return Reference to the history buffer holding previous references
        [[nodiscard]] const auto& getReferences() noexcept
        {
            return m_references;
        }

        //! Returns the measurement history buffer.
        //!
        //! @return Reference to the history buffer holding previous measurements
        [[nodiscard]] const auto& getMeasurements() noexcept
        {
            return m_measurements;
        }

        //! Gets the R polynomial.
        //!
        //! @return Array with R polynomial coefficients
        [[nodiscard]] const auto& getR() const
        {
            return m_r;
        }

        //! Gets the S polynomial.
        //!
        //! @return Array with S polynomial coefficients
        [[nodiscard]] const auto& getS() const
        {
            return m_s;
        }

        //! Gets the T polynomial.
        //!
        //! @return Array with T polynomial coefficients
        [[nodiscard]] const auto& getT() const
        {
            return m_t;
        }

        // ************************************************************
        // Setters

        //! Sets the R polynomial.
        //!
        //! @param r Array with R polynomial values to be set
        void setR(const std::array<double, ControllerLength>& r)
        {
            m_r = r;
        }

        //! Sets the S polynomial.
        //!
        //! @param s Array with S polynomial values to be set
        void setS(const std::array<double, ControllerLength>& s)
        {
            m_s = s;
        }

        //! Sets the T polynomial.
        //!
        //! @param t Array with T polynomial values to be set
        void setT(const std::array<double, ControllerLength>& t)
        {
            m_t = t;
        }

      private:
        int64_t     m_head{0};   //!< Index to oldest entry in the history
        std::string m_name;      //!< Name of this controller

        std::array<double, ControllerLength> m_r{0};   //!< R-polynomial coefficients
        std::array<double, ControllerLength> m_s{0};   //!< S-polynomial coefficients
        std::array<double, ControllerLength> m_t{0};   //!< T-polynomial coefficients

        std::array<double, ControllerLength> m_measurements{0};   //!< RST measurement history
        std::array<double, ControllerLength> m_references{0};     //!< RST reference history
        std::array<double, ControllerLength> m_actuations{0};     //!< RST actuation history

        bool m_history_ready{false};   //!< flag to mark RST ref and meas histories are filled

        // helper variables used in Jury's test
        std::array<double, ControllerLength> m_b{0};   // variable used in Jury's test, declaring them here avoids
                                                       // allocation whenever jurysStabilityTest is called
    };

    //! Control method returning the next actuation.
    //!
    //! @param measurement Current measurement value
    //! @param reference Current reference value
    //! @return Next actuation value
    template<>
    [[nodiscard]] inline double RSTController<3>::control(const double reference, const double measurement) noexcept
    {
        // This specialization allows to speed-up the calculation of the RST actuation by about 15%
        m_references[2] = m_references[1];
        m_references[1] = m_references[0];
        m_references[0] = reference;

        m_measurements[2] = m_measurements[1];
        m_measurements[1] = m_measurements[0];
        m_measurements[0] = measurement;

        m_actuations[2] = m_actuations[1];
        m_actuations[1] = m_actuations[0];
        m_actuations[0] = (m_t[0] * reference - m_r[0] * measurement + m_t[1] * m_references[1]
                           - m_r[1] * m_measurements[1] + m_t[2] * m_references[2] - m_r[2] * m_measurements[2]
                           - (m_s[1] * m_actuations[1] + m_s[2] * m_actuations[2]))
                          / m_s[0];

        return m_actuations[0];
    }


    //! Updates the most recent reference in the history, used in cases actuation goes over the limit.
    //!
    //! @param updated_actuation Actuation that actually took place after clipping of the calculated actuation
    template<>
    inline void RSTController<3>::updateReferenceOpenLoop(const double updated_actuation)
    {
        // based on logic of regRstCalcRefRT from CCLIBS libreg's regRst.c
        m_actuations[0] = updated_actuation;
        m_references[0] = (m_s[0] * updated_actuation + m_r[0] * m_measurements[0] + m_s[1] * m_actuations[1]
                           + m_r[1] * m_measurements[1] - m_t[1] * m_references[1] + m_s[2] * m_actuations[2]
                           + m_r[2] * m_measurements[2] - m_t[2] * m_references[2])
                          / m_t[0];
    }

    //! Updates the most recent reference in the history, used in cases actuation goes over the limit.
    //!
    //! @param updated_actuation Actuation that actually took place after clipping of the calculated actuation
    template<>
    inline void RSTController<3>::updateReference(const double updated_actuation)
    {
        // based on logic of regRstCalcRefRT from CCLIBS libreg's regRst.c
        const double delta_actuation = updated_actuation - m_actuations[0];
        m_actuations[0]              = updated_actuation;
        m_references[0]              += delta_actuation * m_s[0] / m_t[0];
    }

    template<>
    inline void RSTController<3>::updateInputHistories(const double reference, const double measurement) noexcept
    {
        m_references[2] = m_references[1];
        m_references[1] = m_references[0];
        m_references[0] = reference;

        m_measurements[2] = m_measurements[1];
        m_measurements[1] = m_measurements[0];
        m_measurements[0] = measurement;

        m_head++;
        if (m_head == 2)
        {
            m_history_ready = true;
            m_head          = 0;
        }
    }

}   // namespace vslib
