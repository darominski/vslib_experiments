.. _transforms:

===============
Transformations
===============

This chapter describes all electrical transformations implemented in the VSlib.

General interface
-----------------

All of the classes described below derive from and implement the :ref:`Component <component_api>` interface.
They all have a single access method, called :code:`transform`. The number of arguments each implementation
takes depends on the use-case, as well as the returned structure may vary. This method is intended to perform
transformation on the set of values at a point of time rather than arrays or vectors of values in one execution.

.. _abcToDq0Transform_component:

abc to dq0 transformation
-------------------------

:code:`AbcToDq0Transform` implements the from three time-domain signals of a three-phase
system (`abc`) to an `dq0` reference frame. The algorithm follows the implementation
and nomeclature of the `Park transformation matlab implementation <https://ch.mathworks.com/help/sps/ref/parktransform.html>`_.

The :code:`transform` method takes four obligatory and one optional :code:`double`-type arguments, one for each phase,
the :math:`theta` (:math:`=\omega t`) angle in radians between the `q` and `a` axes, and an optional offset (:math:`\phi`) in radians.
The offset can be used to change the default alignment from `q` and `a` axis alignment to `d` and `a` by setting the offset
to be equal to :math:`\frac{\pi}{2}`. The method returns a tuple of d, q, zero values. The transformation algorithm is as follows:

.. math::

    d &= \frac{2}{3} \left( a \cdot sin(\theta + \phi) + b \cdot sin \left( \theta + \phi - \frac{2}{3} \pi \right) + c \cdot sin \left(\theta + \phi + \frac{2}{3} \pi \right) \right) \\
    q &= \frac{2}{3} \left(a \cdot cos(\theta + \phi) + b \cdot cos \left(\theta + \phi - \frac{2}{3} \pi \right) + c \cdot cos \left(\theta + \phi + \frac{2}{3} \pi \right) \right) \\
    zero &= \frac{1}{3} \left( a + b + c \right) \\

However, during benchmarking it was found that due to the overhead of the look-ups of the sine and cosine functions, it is preferable
to perform the `abc` to `dq0` transformation in two steps:

1. Transform `abc` to :math:`\alpha \beta 0` frame, and then
2. Transform :math:`\alpha\beta 0` to `dq0`.

For more details regarding the API, see the :ref:`API documentation for AbcToDq0Transform <abcToDq0Transform_api>`.

Usage example
^^^^^^^^^^^^^

.. code-block:: cpp

    #include <numbers>

    #include "abcToDq0Transform.h"
    #include "rootComponent.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        AbcToDq0Transform abc_to_dq0("abc_to_dq0", root);
        // no Parameter needs setting

        const double i_a    = 1.0;
        const double i_b    = -0.5;
        const double i_c    = -0.5;
        const double theta  = 0.0;
        const double offset = std::numbers::pi / 2.0;

        // no offset, q and a alignment:
        auto [d_1, q_1, zero_1]   = abc_to_dq0.transform(i_a, i_b, i_c, theta);

        // 90 degrees offset, d and a alignment:
        auto [d_2, q_2, zero_2]   = abc_to_dq0.transform(i_a, i_b, i_c, theta, offset);

        return 0;
    }

.. _dq0ToAbcTransform_component:

dq0 to abc transformation
-------------------------

:code:`Dq0ToAbcTransform` implements the transformation from `dq0` reference frame to three-phase
system (`abc`) `dq0`, an inverse of :ref:`AbcToDq0Transform <abcToDq0Transform_component>`. The algorithm follows the implementation
and nomeclature of the `Inverse Park transformation Matlab implementation <https://ch.mathworks.com/help/sps/ref/inverseparktransform.html>`_.

The :code:`transform` method takes four obligatory and one optional :code:`double`-type arguments, one for each `dq0` component,
the :math:`theta` (:math:`=\omega t`) angle in radians between the `q` and `a` axes, and an optional offset in radians.
The offset can be used to change the default alignment from `q` and `a` axis alignment to `d` and `a` by setting the offset
to be equal to :math:`\frac{\pi}{2}`. The method returns a tuple of a, b, and c values. The transformation algorithm is as follows:

.. math::

    a &= d \cdot sin(\theta + \phi) + q \cdot cos(theta + \phi) + zero \\
    b &= d \cdot sin(\theta + \phi - \frac{2}{3} \pi) + q \cdot cos(\theta + \phi - \frac{2}{3} \pi) + zero; \\
    c &= d \cdot sin(\theta + \phi + \frac{2}{3} \pi) + q \cdot cos(\theta + \phi + \frac{2}{3} \pi) + zero; \\

However, during benchmarking it was found that due to the overhead of the look-ups of the sine and cosine functions, it is preferable
to perform the `dq0` to `abc` transformation in two steps:

1. Transform `dq0` to :math:`\alpha\beta 0` frame, and then
2. Transform :math:`\alpha\beta 0` to `abc`.

For more details regarding the API, see the :ref:`API documentation for Dq0ToAbcTransform <dq0ToAbcTransform_api>`.

Usage example
^^^^^^^^^^^^^

.. code-block:: cpp

    #include <numbers>

    #include "dq0ToAbcTransform.h"
    #include "rootComponent.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        Dq0ToAbcTransform dq0_to_abc("dq0_to_abc", root);
        // no Parameter needs setting

        const double d      = 1.0;
        const double q      = 0.05;
        const double zero   = 0.05;
        const double theta  = 0.0;
        const double offset = std::numbers::pi / 2.0;

        // no offset, q and a alignment:
        auto [a_1, b_2, c_2]   = dq0_to_abc.transform(d, q, zero, theta);

        // 90 degrees offset, d and a alignment:
        auto [a_2, b_2, c_2]   = dq0_to_abc.transform(d, q, zero, theta, offset);

        return 0;
    }

.. _abcToAlphaBetaTransform_component:

abc to alpha-beta transformation
--------------------------------

:code:`AbcToAlphaBetaTransform` implements the `abc` to :math:`\alpha\beta0` (Clarke) transformation from three-phase components in the `abc`
reference frame to the rotating :math:`\alpha\beta0` frame. The algorithm follows the implementation
and nomeclature of the `Inverse Clarke Matlab implementation <https://ch.mathworks.com/help/mcb/ref/inverseclarketransform.html>`_.

The :code:`transform` method takes three obligatory :code:`double`-type arguments, one for each `a`, `b`, and `c` component in the `abc`
frame of reference. The method returns a tuple of :math:`\alpha`, :math:`\beta`, and `0` values. The calculation is as follows:

.. math::

    \alpha &= \frac{2}{3} \left( a - \frac{b+c}{2} \right) \\
    \beta  &= \frac{\sqrt{3}}{3} (b - c) \\
    zero   &= \frac{1}{3} (a + b + c)

For more details regarding the API, see the :ref:`API documentation for AbcToAlphaBetaTransform <abcToAlphaBetaTransform_api>`.

Usage example
^^^^^^^^^^^^^

.. code-block:: cpp

    #include "abcToAlphaBetaTransform.h"
    #include "rootComponent.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        AbcToAlphaBetaTransform  abc_to_alphabeta("abc_to_alphabeta", root);
        // no Parameters need setting

        const double i_a  = 2.0;
        const double i_b  = -1.0;
        const double i_c  = -1.0;

        auto [alpha, beta, zero] = abc_to_alphabeta.transform(i_a, i_b, i_c);
        // alpha = 2.0, beta = 0, zero = 0

        return 0;
    }

.. _alphaBetaToAbcTransform_component:

alpha-beta to abc transformation
--------------------------------

:code:`AbcToAlphaBetaTransform` implements the :math:`\alpha\beta0` to abc (inverse Clarke) transformation from
the rotating :math:`\alpha\beta0` frame to the three-phase components in the time domain. The algorithm follows the implementation
and nomeclature of the `Inverse Clarke Matlab implementation <https://ch.mathworks.com/help/mcb/ref/clarketransform.html>`_.

The :code:`transform` method takes three obligatory :code:`double`-type arguments, one for each :math:`\alpha`, :math:`\beta`, and `zero`
components in the rotating :math:`\alpha\beta0` frame of reference. The method returns a tuple of `a`, `b`, and `c` values.
The calculation is as follows:

.. math::

    a &=  \left( \alpha + zero \right) \\
    b &= -\frac{1}{2} \alpha + \frac{\sqrt{3}}{2} \beta + zero \\
    c &= -\frac{1}{2} \alpha - \frac{\sqrt{3}}{2} \beta + zero

For more details regarding the API, see the :ref:`API documentation for AlphaBetaToAbcTransform <abcToAlphaBetaTransform_api>`.

Usage example
^^^^^^^^^^^^^

.. code-block:: cpp

    #include "alphaBetaToAbcTransform.h"
    #include "rootComponent.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        AlphaBetaToAbcTransform  transform("alphabeta_to_abc", root);
        // no Parameters need setting

        const double alpha  = 2.0;
        const double beta  = 0.0;
        const double zero  = 0.0;

        auto [a, b, c] = transform.transform(alpha, beta, zero);
        // a = 2.0, b = -1.0, c = -1.0

        return 0;
    }

.. _alphaBetaToDq0Transform_component:

Alpha-beta to dq0
-----------------

:code:`alphaBetaToDq0Transform` implements the :math:`\alpha\beta0` transformation from three stationary
components in the :math:`\alpha\beta0` reference frame to the rotating `dq0` reference frame, an equivalent
of the inverse Clarke and then Park transform.

The :code:`transform` method takes four obligatory :code:`double`-type arguments and one optional boolean argument:
one for each :math:`\alpha`, :math:`\beta`, and `0` component in the :math:`\alpha\beta0` frame of reference,
the :math:`theta` angle (in radians) between `q` and :math:`alpha`, and optionally specify alignment: :code:`true`
for a-axis alignment or :code:`false` for 90 degrees behind a-axis. The method returns a tuple of `d`, `q`, and `0` values.
The algorithm follows the implementation and nomeclature  of the
`alpha-beta to dq0 Matlab implementation <https://ch.mathworks.com/help/sps/powersys/ref/alphabetazerotodq0dq0toalphabetazero.html>`_.
The calculation is as follows if the a-axis alignment is chosen:

.. math::

    d &= \alpha \cdot cos(\theta) + \beta \cdot sin(\theta)  \\
    q &= -\alpha \cdot sin(\theta) + \beta \cdot cos(\theta) \\
    zero &= zero

and if the the 90-degrees behind a-axis alignment is preferred:

.. math::

    d &= \alpha \cdot sin(\theta) - \beta \cdot cos(\theta) \\
    q &= \alpha \cdot cos(\theta) + \beta \cdot sin(\theta) \\
    zero &= zero

For more details regarding the API, see the :ref:`API documentation for AlphaBetaToDq0Transform <alphaBetaToDq0Transform_api>`.

Usage example
^^^^^^^^^^^^^

.. code-block:: cpp

    #include <numbers>

    #include "alphaBetaToDq0Transform.h"
    #include "rootComponent.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        AlphaBetaToDq0Transform transform("alpha-beta_to_dq0", root);

        const double i_alpha     = 1.0;
        const double i_beta      = -0.5;
        const double i_zero      = 0.0;
        const double theta       = std::numbers::pi / 6;   // 30 degrees in radians
        bool   a_alignment       = true;
        auto [d, q, zero]        = transform.transform(i_alpha, i_beta, i_zero, theta, a_alignment);

        return 0;
    }


.. _dq0ToAlphaBetaTransform_component:

dq0 to alpha-beta
-----------------

:code:`Dq0ToAlphaBetaTransform` implements the transformation of components in `dq0` frame of reference to
the :math:`\alpha\beta0` reference frame, an inverse of :ref:`AlphaBetaToDq0Transformation <Dq0ToAlphaBetaTransform_component>`.

The :code:`transform` method takes four obligatory :code:`double`-type arguments and one optional boolean argument:
one for each `d`, `q`, `zero`, and `theta`, and optionally a boolean alignment argument: :code:`true` for a-axis
alignment or :code:`false` for 90 degrees behind a-axis. :math:`theta` is the angle (in radians) between `q`
and :math:`alpha`. The method returns a tuple of :math:`\alpha`, :math:`\beta`, and `zero` values.
The algorithm follows the implementation and nomeclature  of the
`alpha-beta to dq0 Matlab implementation (inverse) <https://ch.mathworks.com/help/sps/powersys/ref/alphabetazerotodq0dq0toalphabetazero.html>`_.

The calculation is as follows if the a-axis alignment is chosen:

.. math::

    \alpha &= d \cdot cos(\theta) - q \cdot sin(\theta) \\
    \beta  &= d \cdot sin(\theta) + q \cdot cos(\theta) \\
    zero   &= i_zero

and if the the 90-degrees behind a-axis alignment is preferred:

.. math::

    \alpha &= d \cdot sin(\theta) + q \cdot cos(\theta) \\
    \beta  &= -d \cdot cos(\theta) + q \cdot sin(\theta) \\
    zero   &= i_zero

For more details regarding the API, see the :ref:`API documentation for Dq0ToAlphaBetaTransform <dq0ToAlphaBetaTransform_api>`.

Usage example
^^^^^^^^^^^^^

.. code-block:: cpp

    #include <numbers>

    #include "dq0ToAlphaBetaTransform.h"
    #include "rootComponent.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        dq0ToAlphaBetaTransform transform("dq0_to_alphabeta", root);

        const double d           = 1.0;
        const double q           = -0.5;
        const double i_zero      = 0.0;
        const double theta       = std::numbers::pi / 6;   // 30 degrees in radians
        bool   a_alignment       = true;
        auto [alpha, beta, zero] = transform.transform(d, q, i_zero, theta, a_alignment);

        return 0;
    }

.. _instantaneousPowerThreePhase_component:


Instantaneous power of a three phase system
-------------------------------------------

:code:`InstantaneousPowerThreePhase` component implements the calculation the instantaneous active and reactive power
based on the three phase voltages and currents components in abc reference frame. It has two :code:`double`-type
:code:`Parameters`: :code:`p_gain` and :code:`q_gain`, used to scale the calculated active and reactive power components,
respectively.

The :code:`transform` method takes six obligatory :code:`double`-type arguments: one for each `a`, `b`, and `c` first
voltage and then current components of the three phase system. It outputs a :code:`tuple` of two values: active (P)
and reactive (Q) power, scaled by the appropriate gains.

The algorithm is as follows:

.. math::

    P &= v_{a} \cdot i_{a} + v_{b} \cdot i_{b} + v_{c} \cdot i_{c} \\
    \\
    v_{ab}   &= v_{a} - v_{b} \\
    v_{bc}   &= v_{b} - v_{c} \\
    v_{ca}   &= v_{c} - v_{a} \\
    Q &= \frac{1}{\sqrt{3}} \cdot \left( i_{a} \cdot v_{bc} + i_{b} \cdot v_{ca} + i_{c} \cdot v_{ab} \right) \\

For more details regarding the API, see the :ref:`API documentation for InstantaneousPowerThreePhase <instantaneousPowerThreePhase_api>`.

Usage example
^^^^^^^^^^^^^

.. code-block:: cpp

    #include "instantaneousPowerThreePhase.h"
    #include "rootComponent.h"

    using namespace vslib;

    int main() {
        RootComponent root;
        InstantaneousPowerThreePhase power("power_3ph", root);

        // set p_gain and q_gain to your values, for no gain set them to 1.0

        std::array<double, 3> v_abc{230.0, -115.0, 115.0};
        std::array<double, 3> i_abc{10.0, -5.0, -5.0};

        const auto [p, q] = power.transform(v_abc[0], v_abc[1], v_abc[2], i_abc[0], i_abc[1], i_abc[2]);

        return 0;
    }


Performance
-----------

The execution time of each :code:`Component` depends on a number of factors. In the case of :ref:`AbcToAlphaBetaTransform <abcToAlphaBetaTransform_component>`,
there no look-up tables and the execution time is independent of the inputs. For :ref:`AbcToDq0Transform <abcToDq0Transform_component>` and
:ref:`AlphaBetaToDq0Transform <alphaBetaToDq0Transform_component>`, the execution will depend on the size of the internal look-up tables.
The table below gives an overlook of the execution time that can be expected for each of the :code:`Components`.

.. list-table::
    :header-rows: 1

    * - Class
      - Access time [ns]
    * - AbcToAlphaBetaTransform
      - 33
    * - AbcToDq0Transform
      - 263
    * - AlphaBetaToDq0Transform
      - 197
    * - AlphaBetaToAbcTransform
      - 27
    * - Dq0ToAbcTransform
      - 270
    * - Dq0ToAlphaBetaTransform
      - 230
