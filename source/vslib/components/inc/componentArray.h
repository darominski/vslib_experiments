//! @file
//! @brief Defines class for creating and storing fixed-size arrays of components, as well as methods to iterate
//! over this array.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.h"
#include "constants.h"

namespace vslib::components
{
    template<typename ComponentType, size_t N>
    class ComponentArray : public Component
    {
      public:
        template<typename... ComponentSettings>
        ComponentArray(const std::string& name, Component* parent = nullptr, ComponentSettings... settings)
            : Component("ComponentArray", name, parent)
        {
            static_assert(std::derived_from<ComponentType, Component>, "ComponentType must be derived from Component");
            createComponents<ComponentSettings...>(name, 0, settings...);
        }

        //! Provides seamless access to the value stored at the provided index
        const ComponentType& operator[](size_t index) const
        {
            return *m_components[index];
        }

        //! Provides an overloaded begin() to return iterators that automatically dereference shared pointers
        auto begin()
        {
            return IteratorWrapper(m_components.begin());
        }

        // Provides overloaded end() to return iterators that automatically dereference shared pointers
        auto end()
        {
            return IteratorWrapper(m_components.end());
        }

      private:
        std::array<std::unique_ptr<ComponentType>, N> m_components;

        template<typename... CurrentSettings>
        void createComponents(const std::string& name_base, size_t index, CurrentSettings... settings)
        {
            // Create and initialize the current component
            m_components[index]
                = std::make_unique<ComponentType>(name_base + "_" + std::to_string(index + 1), this, settings...);
            // Recursively create the rest of the components
            if (N > (index + 1))
            {
                createComponents<CurrentSettings...>(name_base, index + 1, settings...);
            }
        }

        // ************************************************************
        //! Helper class for wrapping dereferencing of the std::unique_ptr stored in the std::array
        class IteratorWrapper
        {
          public:
            IteratorWrapper(typename std::array<std::unique_ptr<ComponentType>, N>::iterator iter)
                : m_iter(iter)
            {
            }

            // Overload operator* to automatically dereference the shared_ptr.
            ComponentType& operator*()
            {
                return *(*m_iter);
            }

            // Forward other iterator operations to the wrapped iterator.
            IteratorWrapper& operator++()
            {
                ++m_iter;
                return *this;
            }

            bool operator!=(const IteratorWrapper& other) const
            {
                return m_iter != other.m_iter;
            }

          private:
            typename std::array<std::unique_ptr<ComponentType>, N>::iterator m_iter;
        };
    };

}   // namespace components
