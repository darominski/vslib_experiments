//! @file
//! @brief Defines class for creating and storing fixed-size arrays of components, as well as methods to iterate
//! over this array.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.h"
#include "constants.h"

namespace vslib
{
    template<typename ComponentType, size_t N>
    class ComponentArray : public Component
    {
      public:
        //! Constructor for the ComponentArray component
        //!
        //! @param name Name of the object
        //! @param parent Specifies whether a parent of this object exists, optional: object independent by default
        //! @param settings All settings that are forwarded to construct Components held in the array
        template<typename... ComponentSettings>
        ComponentArray(const std::string& name, Component* parent = nullptr, ComponentSettings... settings)
            : Component("ComponentArray", name, parent)
        {
            static_assert(std::derived_from<ComponentType, Component>, "ComponentType must be derived from Component");
            createComponents<ComponentSettings...>(name, 0, settings...);   // 0-based indexing for the array
        }

        //! Provides seamless access to the value stored at the provided index
        //!
        //! @param index Index of the array element to be accessed
        //! @return Reference to the Component at the specified address
        const ComponentType& operator[](size_t index) const
        {
            return *m_components[index];
        }

        //! Provides an overloaded begin() to return an iterator that automatically handles access to the held
        //! Components
        auto begin()
        {
            return IteratorWrapper(m_components.begin());
        }

        // Provides overloaded end() to return an iterator that automatically handles access to the held Components
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
                = std::make_unique<ComponentType>(name_base + "[" + std::to_string(index) + "]", this, settings...);
            // Recursively create the rest of the components
            if (N > (index + 1))
            {
                createComponents<CurrentSettings...>(name_base, index + 1, settings...);
            }
        }

        // ************************************************************
        //! Helper class for wrapping dereferencing of the pointers stored in the std::array
        class IteratorWrapper
        {
          public:
            IteratorWrapper(typename std::array<std::unique_ptr<ComponentType>, N>::iterator iter)
                : m_iter(iter)
            {
            }

            // Overload operator* to automatically dereference the pointer to the component.
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

}   // namespace vslib
