//! @file
//! @brief File containing the common interface of all components to be reused.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

#include "componentRegistry.h"
#include "iparameter.h"
#include "nlohmann/json.hpp"
#include "parameterRegistry.h"

namespace vslib::component
{
    class BaseComponent
    {
      public:
        BaseComponent(std::string_view component_type, std::string_view name) noexcept
            : m_component_type(component_type),
              m_name(name)
        {
            registerComponent();
        }

        virtual ~BaseComponent() = default;

        //! Registers the parameter belonging to this component in the parameter registry, simultaneously adding it to
        //! the m_params map.
        //!
        //! @param parameter_name Name of the parameter to be added to the parameter registry
        //! @param parameter Reference to the parameter being added to the parameter registry
        void registerParameter(std::string_view parameter_name, parameters::IParameter& parameter)
        {
            parameters::ParameterRegistry::instance().addToRegistry(
                this->getFullName() + "." + std::string(parameter_name), parameter
            );
            m_params.emplace(parameter_name, parameter);
        }

        //! Registers this component in the ComponentRegistry
        void registerComponent() noexcept
        {
            ComponentRegistry::instance().addToRegistry(m_name, (*this));
        }

        //! Serializes this component to JSON
        nlohmann::json serialize() const noexcept
        {
            nlohmann::json serialized_parameters;
            std::for_each(
                std::cbegin(m_params), std::cend(m_params),
                [&serialized_parameters](const auto& parameter)
                {
                    serialized_parameters.push_back(parameter.second.get().serialize());
                }
            );
            return {{"name", m_name}, {"type", m_component_type}, {"params", serialized_parameters}};
        }

        [[nodiscard]] std::string_view getName() const noexcept
        {
            return m_name;
        }

        [[nodiscard]] std::string getFullName() const noexcept
        {
            return m_component_type + "." + m_name;
        }

        [[nodiscard]] auto const& getParameters() const noexcept
        {
            return m_params;
        }

      private:
        std::string const                                                     m_component_type;
        std::string const                                                     m_name;
        std::map<std::string, std::reference_wrapper<parameters::IParameter>> m_params;
    };
}   // namespace component