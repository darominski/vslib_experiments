//! @file
//! @brief Defines the base class for filters.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.hpp"

namespace vslib
{
    class Filter : public Component
    {
      public:
        //! Constructor of the Filter Component, serves as the base for the Filters interface.
        //!
        //! @param type Type of this Filter Component
        //! @param name Name of this Filter Component
        //! @param parent Parent of this Filter Component
        Filter(std::string_view type, std::string_view name, Component& parent)
            : Component(type, name, parent)
        {
        }

        //! Method to implement filtering of the provided input.
        //!
        //! @param input Value to be filtered
        virtual double filter(const double input) = 0;
    };
}   // namespace vslib