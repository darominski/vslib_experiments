//! @file
//! @brief Defines the Component implementing the alpha-beta-zero to dq0 (Clarke components to Park components)
//! transformation.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>

#include "component.h"
#include "cosLookupTable.h"
#include "sinLookupTable.h"

namespace vslib
{
    class ClarkeParkTransform : public Component
    {
      public:
        //! Creates the ClarkePark transformation Component
        //!
        //! @param name Name of the Component
        //! @param parent Parent of this Component
        ClarkeParkTransform(std::string_view name, Component* parent, uint64_t number_points = 1000)
            : Component("ClarkeParkTransform", name, parent),
              m_sin("sin", this, number_points),
              m_cos("cos", this, number_points)
        {
        }

        //! Performs ClarkePark transform on the provided inputs
        //!
        //! @param f_alpha a-phase value of abs-frame component
        //! @param f_beta b-phase value of abs-frame component
        //! @param f_0 c-phase value of abs-frame component
        //! @param theta theta angle (in radians) between q and alpha
        //! @param a_alignment Whether the frame alignment at t=0 is aligned with A-axis (true) or degrees behind A-axis
        //! (false)
        //! @return Tuple of d, q, 0 values
        [[nodiscard]] std::tuple<double, double, double>
        transform(double f_alpha, double f_beta, double f_0, double theta, bool a_alignment = true) noexcept;

      private:
        SinLookupTable m_sin;   //!< Lookup table holding sine function
        CosLookupTable m_cos;   //!< Lookup table holding cosine function
    };
}   // namespace vslib