.. _components:

==========
Components
==========

This section describes the fundamental VSlib interface of a :code:`Component`. This interface
serves as a base to most of library's classes that you, the user, interacts with, such as filters,
controllers, limits, etc.

General interface
-----------------

A :code:`Component` is any VSlib class that needs to have runtime-settable :ref:`Parameters <parameter>`.
All of the user-defined classes are intended to be derived from the :code:`Component` interface.

:code:`Component` class defines a constructor that takes three parameters: the component's type (:code:`std::string`),
the name of this instance (:code:`std::string`, needs to be unique), and a pointer to parent of this :code:`Component`.
There should be only one parent-less :code:`Component`, a so-called root, and it is defined in the VSlib main function
and passed to the user as a reference.

Using :code:`std::string` for types and names allows for semantic identification of :code:`Components` and their
:code:`Parameters`, when the latter are registered in the :code:`ParameterRegistry`. The definition of a parent,
allows for a construction of logical hierarchy between the :code:`Components` in your control workflow. Each :code:`Component`
in your code will derive either from your-defined class, which in turn derives from a root :code:`Component`, or
other :code:`Components` defined in your code. For example, in the case of :ref:`PID <pid_component>` controller, the :code:`PID`
has a parent, while it itself is a parent to a :code:`LimitRange` :code:`Component`.

The interface implements all the logic for registering a :code:`Parameter` to itself, a public-interface method :code:`registerParameter`
is called. The access to the container holding a :code:`vector` of name-reference pairs of all :code:`Parameters` registered to this
:code:`Component` is given by :code:`getParameters` method. For interactions with the :code:`Parameters`, there is also a method
allowing to verify whether all :code:`Parameters` of this :code:`Component` have been initialized, a necessary step of the
:ref:`vloop startup procedure <vloop_startup>`.

:code:`Component` provides a method intended to be overriden in your code to define a custom :code:`Parameter` values validation
algorithm, the :code:`verifyParameters`. If all your check pass, the method should return nothing, but in case there is an issue,
it should be reported using the :code:`fgc4::utils::Warning` object with all the information necessary to identify the issue.

:code:`Component` contains the logic necessary to traverse the hierarchy of the components used in your control workflow
and its serialization necessary for the :ref:`linux - bare-metal communication <linux_bare-metal_communication>` of
settable :code:`Parameters`.

For more details regarding the API, see the :ref:`API documentation for Component <component_api>`.

Usage examples
^^^^^^^^^^^^^^

1. Interactions of base-class :code:`Component`:

.. code-block:: cpp

    #include "component.h"

    using namespace vslib;

    int main() {
        Component root("root_type", "root", nullptr);

        const std::string child_type = "child_type";
        const std::string child_name = "child_name";
        Component         child(child_type, child_name, &parent);

        auto name = child.getName();          // "child_name"
        auto full_name = child.getFullName(); // "root_type.root.child_type.child_name"

        auto parameters_size = child.getParameters().size();         // no parameters, 0
        auto parameters_initialized = child.parametersInitialized(); // no parameters, true

        root.hasParent();  // false
        child.hasParent(); // true

        auto serialized_component = parent.serialize(); // JSON containing all information needed by the parameter setting interface

        return 0;
    }

2. Deriving your own class and using it:

.. code-block:: cpp

    #include "component.h"
    #include "parameter.h"

    #include <array>

    using namespace vslib;

    enum class Enums
    {
        option1,
        option2,
        option3
    };

    class Derived : public Component
    {
        public:
            Derived(std::string_view name, Component* parent)
                : Component("Derived", name, parent),
                scalar_factor(*this, "scalar_factor", 0.0, 10.0), // min = 0.0, max = 10.0
                array_factor(*this, "array_factor"),
                enums(*this)
            {
            }

            Parameter<double> scalar_factor;  //! Scalar factor of type double
            Parameter<std::array<double, 3>> array_factor; //! Array of double-type factors of length 3
            Parameter<Enums> enums; //! Enumeration Parameter with user-defined options

            // if necessary, override the verifyParameters method to perform your custom validation
            std::optional<fgc4::utils::Warning> verifyParameters() override
            {
                // for example, the first element of the array must be larger than the last one:
                // To validate incoming values, you need to use 'toValidate' method to access the write-buffer values,
                // otherwise you would be checking the currently used valus in the read buffer.
                if (array_factor.toValidate()[0] < array_factor.toValidate()[2])
                {
                    return fgc4::utils::Warning(
                        fmt::format("First element of the array_factor: {} is lower than the last element: {}.",
                                    array_factor.toValidate()[0], array_factor.toValidate()[2]));
                }
                return {}; // checks passed, no need to return anything
            }

            // other methods, public and private, to interact with these Parameters
    };

    int main()
    {
        Component root("root", "root", nullptr);
        Derived derived("derived", &root);

        // set your defined Parameters in the parameter setting GUI,
        // each time any Parameter is changed, the verifyParameters method is called.

        // when Parameters are set: use your derived class

        return 0;
    }

Component array
---------------

:code:`ComponentArray` is special type of :code:`Component` allowing for creating a fixed-size array of :code:`Components`
of user's choice, for example :ref:`FIR filters <fir_api>`. The class has two template parameters: the type of the
:code:`Components` it contains, and the number of objects it will store.

The constructor can be passed an arbitrary number of parameters. After the obligatory :code:`name` and :code:`parent`,
the remaining parameters are passed to constructors of the relevant :code:`Component`. The names of the array elements
will follow the array-like pattern, by appending :code:`[n]` to the name defined to this component, where `n` is the index
of the :code:`Component` in the array. Each stored :code:`Component` is initialized with exactly the same parameters,
except of the name.

The :code:`ComponentArray` provides analogous interaction interface with stored :code:`Components` to the :code:`std::array`,
namely it is possible to fetch a reference to the stored :code:`Component` by overriden :code:`operator[]`,
and also iterate over the :code:`ComponentArray` in a :code:`for` loop (see usage example below)

For more details regarding the API, see the :ref:`API documentation for ComponentArray <componentArray_api>`.

Usage example
^^^^^^^^^^^^^

Using :code:`Derived` defined above.

.. code-block:: cpp

    #include "componentArray.h"
    #include "parameter.h"

    #include <array>

    using namespace vslib;

    int main()
    {
        Component                             root("root", "root", nullptr);
        const std::string                     component_name = "array";
        constexpr size_t                      array_length   = 4;
        ComponentArray<Derived, array_length> array(component_name, nullptr);

        // set all the Parameters of the Components in the array

        // now, you can access each Component, either by its index, like in a std::array:
        Derived& component = array[2]; // returns 3rd element of the array

        // you can also iterate over the array as if it is any other STL container
        for (const auto& element : array)   // tests begin() and end() operators
        {
            // you can now interact with the element object
            // each element is a Derived-type Component
        }
        return 0;
    }