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

The :code:`transform` method takes four obligatory and one optional :code:`double`-type arguments, three for each phase,
the :math:`theta` (:math:`=\omega t`) angle in radians between the `q` and `a` axes, and an optional offset in radians.
The offset can be used to change the default alignment from `q` and `a` axis alignment to `d` and `a` by setting the offset
to be equal to :math:`\frac{\pi}{2}`. The method returns a tuple of d, q, zero values. The calculation is as follows:

.. math::

    d &= \frac{2}{3} \left( a \cdot sin(\theta) + b \cdot sin \left( \theta - \frac{2}{3} \pi \right) + c \cdot sin \left(\theta + \frac{2}{3} \pi \right) \right) \\
    q &= \frac{2}{3} \left(a \cdot cos(\theta) + b \cdot cos \left(\theta - \frac{2}{3} \pi \right) + c \cdot cos \left(\theta + \frac{2}{3} \pi \right) \right) \\
    zero &= \frac{1}{3} \left( a + b + c \right) \\

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

.. _abcToAlphaBetaTransform_component:

abc to alpha-beta transformation
--------------------------------

:code:`AbcToAlphaBetaTransform` implements the `abc` to :math:`\alpha\beta0` (Clarke) transformation from three-phase components in the `abc`
reference frame to the rotating :math:`\alpha\beta0` frame. The algorithm follows the implementation
and nomeclature of the `Clarke Matlab implementation <https://ch.mathworks.com/help/mcb/ref/clarketransform.html>`_.

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

        const double i_a               = 2.0;
        const double i_b               = -1.0;
        const double i_c               = -1.0;

        auto [alpha, beta, zero] = abc_to_alphabeta.transform(i_a, i_b, i_c);
        // alpha = 2.0, beta = 0, zero = 0

        return 0;
    }

.. _alphaBetaToDq0_component:

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
        Component root;
        AlphaBetaToDq0Transform transform("alpha-beta_to_dq0", root);

        const double i_alpha     = 1.0;
        const double i_beta      = -0.5;
        const double i_zero      = 0.0;
        const double theta       = std::numbers::pi / 6;   // 30 degrees in radians
        bool   a_alignment       = true;
        auto [d, q, zero]        = transform.transform(i_alpha, i_beta, i_zero, theta, a_alignment);

        return 0;
    }

Performance
-----------

The execution time of each :code:`Component` depends on a number of factors. In the case of :ref:`AbcToAlphaBetaTransform <AbcToAlphaBetaTransform_component>`,
there no look-up tables and the execution time is independent of the inputs. For :ref:`AbcToDq0Transform <AbcToDq0Transform_component>` and
:ref:`AlphaBetaToDq0Transform <alphaBetaToDq0_component>`, the execution will depend on the size of the internal look-up tables.
The table below gives an overlook of the execution time that can be expected for each of the :code:`Components`.

.. list-table::
    :header-rows: 1

    * - Class
      - Access time [ns]
    * - AbcToAlphaBetaTransform
      - 33
    * - AbcToDq0Transform
      - 797
    * - AlphaBetaToDq0Transform
      - 272

The implementation of all algorithms followed equations available in Matlab documentation. However, it can be clearly seen
that a boost in execution time can be achieved if :code:`AbcToDq0Transform` was implemented as a composite of :code:`AbcToAlphaBetaTransform`,
followed up by :code:`AlphaBetaToDq0Transform`. This is due to 6 calculations of trigonometric functions inside the :code:`AbcToDq0Transform`.