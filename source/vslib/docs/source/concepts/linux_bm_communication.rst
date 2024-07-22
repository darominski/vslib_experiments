.. _linux_bare-metal_communication:

================================
Linux - bare-metal communication
================================

This section describes the Linux - bare-metal communication needed to allow
for the flexibility of having settable :code:`Parameters`. Bare-metal core
sends the :code:`Parameter map` with all the settable :code:`Parameters`
present in the binary using the message queue, and receives commands containing
name - value pairs and sets them.

.. _parameter_map:

Parameter map
-------------

Parameter map is a :code:`JSON` object sent from the Vloop binary to the Linux cores
during the startup of the binary, upon entering the :ref:`Unconfigured <unconfigured_state>` state.
The :code:`JSON` contains the information regarding the settable :code:`Parameters` and
the :code:`Components` owning them, including the :code:`Component` hierarchy.

The base unit of the :code:`Parameter map` is individual :code:`Component` declared in the user-code.
Each :code:`Component` has the following fields serialized:

- name: containing name of the :code:`Component`
- type: type of the :code:`Component`
- parameters: list of serialized :code:`Parameters` controlled by this :code:`Component`

Then, each :code:`Parameter` has the following fields:

- name: free string identifier
- type: one of the supported types :ref:`Parameter <parameter>`
- length: 1 for scalar types, length of the arrays in case of arrays
- value: value stored by the :code:`Parameter`
- limit_min: If defined, minimal numerical value that will be accepted
- limit_max: If defined, maximal numerical value that will be accepted
- fields: for enumerations, all of the options that the enum can be set to

Examples
^^^^^^^^

.. code-block:: cpp

    enum class Status
    {
        uninitialized,
        ready,
        updating,
        fault
    };

    // ************************************************************

    class Status : public Component
    {
      public:
        Status(std::string_view name, Component* parent)
          : Component("RST", name, parent),
            status(*this, "status"),
        {
        }

        parameters::Parameter<Status> status;
    };

    int main()
    {
        Component root("root", "root", nullptr);
        Status status("status_1", &root);

        return 0;
    }

The definitions above will result in the following :code:`Parameter map`:

.. code-block:: json

    [
      {
        "components": [],
        "name": "status_1",
        "parameters":
        [
          {
            "fields":
            [
              "uninitialized",
              "ready",
              "updating",
              "fault"
            ],
            "length": 4,
            "name": "status",
            "type": "Enum",
            "value": {}
            }
        ],
        "type": "Status"
      }
    ]


Schema
^^^^^^

.. code-block:: json

  "title": "ParameterMap",
  "description": "Parameter map with all settable parameters of a vloop binary",
  "type": "array",
  "items": {
    "oneOf": [
      {
        "type": "object",
        "properties": {
          "version": {
            "type": "array",
            "minItems": 3
          }
        },
        "required": ["version"]
      },
      {
        "type": "object",
        "minItems": 1,
        "properties": {
          "components": {
            "type": "array"
          },
          "name": {
            "type": "string"
          },
          "parameters": {
            "type": "array",
            "items": {
              "type": "object",
              "properties": {
                "length": {
                  "type": "integer"
                },
                "name": {
                  "type": "string"
                },
                "type": {
                  "type": "string"
                },
                "value": {
                  "type": ["array", "boolean", "number", "object", "string"]
                },
                "limit_max": {
                  "type": "number"
                },
                "limit_min": {
                  "type": "number"
                },
                "fields": {
                  "type": "array",
                  "description": "All available values for an enumeration"
                }
              },
              "required": ["name", "length", "type"]
            }
          },
          "type": {
            "type": "string"
          }
        },
        "uniqueItems": true,
        "required": ["components", "name", "parameters", "type"]
      }
    ]
  }

Commands
--------

Commands are created by Linux cores and are received by the vloop.
The command contains the full name of the :code:`Parameter` and
a new value to be set, and a version of the communication interface.

The full :code:`Parameter` name will include the full name of
the :code:`Component` hierarchy of the :code:`Component` owning this
:code:`Parameters`, separated with dots. The value must be of the same type,
fit in the (optionally defined) limits specified for this :code:`Parameters`,
have the same length if the :code:`type` is an array, or be one of the
available enumrations in case of :code:`enum`.

In case any issue during setting arises, the feedback message queue will be
filled with a :code:`Warning` message describing the reason why the setting
of the new value has failed.

Schema
^^^^^^

.. code-block:: json

    {
      "title": "Command",
      "description": "Command with a new value to be set",
      "type": "object",
      "properties": {
          "name": {
          "description": "Unique string identifier for a parameter",
          "type": "string",
          "minLength": 1
          },
          "value": {
          "description": "New value to be set to the parameter",
          "type": ["array", "boolean", "number", "string"]
          },
          "version": {
          "description": "Version of the command interface",
          "type": "string"
          }
      },
      "required": ["name", "value", "version"]
    }
