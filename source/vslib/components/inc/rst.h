//! @file
//! @brief Class definiting the Component interface of two-degrees-of-freedom RST controller.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

#include "component.h"
#include "parameter.h"
#include "rstController.h"

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
            rst.update_input_histories(measurement, reference);
        }

        //! Calculates one iteration of the controller algorithm
        //!
        //! @param process_value Current process value (measurement)
        //! @param reference Reference value for the controller
        //! @return Controller output of the iteration
        double control(double process_value, double reference) noexcept
        {
            return rst.control(process_value, reference);
        }

        //! Updates the most recent reference in the history, used in cases actuation goes over the limit
        //!
        //! @param updated_actuation Actuation that actually took place after clipping of the calculated actuation
        void update_reference(double updated_actuation)
        {
            rst.update_reference(updated_actuation);
        }

        //! Resets the controller to the initial state by zeroing the history.
        void reset() noexcept
        {
            rst.reset();
        }

        // ************************************************************
        // Getters

        //! Returns flag whether the reference and measurement histories are filled and RST is ready to regulate
        //!
        //! @return True if reference and measurement histories are filled, false otherwise
        bool isReady() const noexcept
        {
            return rst.isReady();
        }

        //! Returns the actuation history buffer
        //!
        //! @return Reference to the history buffer holding previous actuations
        const auto& getActuations() noexcept
        {
            return rst.getActuations();
        }

        //! Returns the reference history buffer
        //!
        //! @return Reference to the history buffer holding previous references
        const auto& getReferences() noexcept
        {
            return rst.getReferences();
        }

        //! Returns the measurement history buffer
        //!
        //! @return Reference to the history buffer holding previous measurements
        const auto& getMeasurements() noexcept
        {
            return rst.getMeasurements();
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
                return fgc4::utils::Warning("First element of r coefficients is zero.\n");
            }

            if (s[0] == 0)
            {
                return fgc4::utils::Warning("First element of s coefficients is zero.\n");
            }

            if (t[0] == 0)
            {
                return fgc4::utils::Warning("First element of t coefficients is zero.\n");
            }

            const auto& maybe_warning_s = rst.jurysStabilityTest(s.value());
            if (maybe_warning_s.has_value())
            {
                return maybe_warning_s.value();
            }

            const auto& maybe_warning_t = rst.jurysStabilityTest(t.value());
            if (maybe_warning_t.has_value())
            {
                return maybe_warning_t.value();
            }

            // r is not checked in CCLIBS and neither they are here

            // no issues, RST is stable, and parameters are valid

            // parameters can be propagated to the calculation RST class
            rst.setR(r.value());
            rst.setS(s.value());
            rst.setT(t.value());

            return {};
        }

      private:
        RSTController<ControllerLength> rst;
    };
}   // namespace vslib
