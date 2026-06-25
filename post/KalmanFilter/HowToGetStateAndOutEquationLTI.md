# How to obtain the state and output equations for a continuous-time Linear Time-Invariant (LTI) system


you generally construct them from either the first-principles physics of the system (differential equations) or by converting a Transfer Function (Laplace domain) into state-space form.

The continuous LTI state-space representation is always defined by these two equations:

- **State Equation**

$$ \dot{x}(t) = Ax(t) + Bu(t) $$

- **Output Equation**
$$y(t) = Cx(t) + Du(t)$$


Here is how you derive the matrices $A$, $B$, $C$, and $D$ depending on your starting point.

## Method 1: Deriving from Physical First Principles

For physical systems (like tracking a maneuvering UAV), you derive these equations by writing out the $n$-th order linear differential equations that govern the system, and then breaking them down into a system of $n$ first-order differential equations.

**Step 1: Define the State Vector ($x$)**
Choose state variables that completely describe the system's energy or kinematics at any given time. For a mechanical system, this is usually position and velocity.

**Step 2: Write the First-Order Differential Equations**

Express the derivative of each state variable as a linear combination of the states and the inputs.

**Concrete Example: 1D Kinematic Target Tracking**

Suppose you are tracking a vehicle where you control (or model) its acceleration $a(t)$ as your input $u(t)$.
- Let position be $p(t)$.
- Let velocity be $v(t)$.

The physical laws of kinematics state:

- $\dot{p}(t) = v(t)$
- $\dot{v}(t) = a(t) = u(t)$

Define your state vector $x(t) = \begin{bmatrix} p(t) \\ v(t) \end{bmatrix}$. Now, rewrite the kinematic equations in matrix form to get the **State Equation**:
$$ \begin{bmatrix} \dot{p}(t) \\ \dot{v}(t) \end{bmatrix} = \begin{bmatrix} 0 & 1 \\ 0 & 0 \end{bmatrix} \begin{bmatrix} p(t) \\ v(t) \end{bmatrix} + \begin{bmatrix} 0 \\ 1 \end{bmatrix} u(t) $$


This gives you your system matrix $A = \begin{bmatrix} 0 & 1 \\ 0 & 0 \end{bmatrix}$ and input matrix $B = \begin{bmatrix} 0 \\ 1 \end{bmatrix}$.


** Step 3: Define the Output Equation**

The output $y(t)$ represents what you can actually measure with your sensors.
If your sensor is a radar that only measures the position $p(t)$, your output is $y(t) = p(t)$. Express this as a matrix multiplying the state vector:

$$ y(t) = \begin{bmatrix} 1 & 0 \end{bmatrix} \begin{bmatrix} p(t) \\ v(t) \end{bmatrix} + [0] u(t) $$

This gives your observation matrix $C = \begin{bmatrix} 1 & 0 \end{bmatrix}$ and direct feedthrough matrix $D = 0$. (In physical tracking systems, $D$ is almost always zero because inputs rarely bypass the system state to instantly appear at the sensor).


## Method 2: Converting from a Transfer Function

If your system is defined in the Laplace domain as a transfer function $H(s) = \frac{Y(s)}{U(s)}$, you can systematically map it to state-space using "Canonical Forms."

Suppose you have a transfer function:

$$ H(s) = \frac{b_1 s + b_2}{s^2 + a_1 s + a_2} $$

You can map this directly into Controllable Canonical Form, which dictates a specific layout for the $A$, $B$, $C$, and $D$ matrices based purely on the coefficients of your polynomials:

$$ \dot{x}(t) = \begin{bmatrix} -a_1 & -a_2 \\ 1 & 0 \end{bmatrix} x(t) + \begin{bmatrix} 1 \\ 0 \end{bmatrix} u(t) $$
$$ y(t) = \begin{bmatrix} b_1 & b_2 \end{bmatrix} x(t) + [0] u(t) $$


## Connecting to the Kalman Filter 


Once you have these continuous-time matrices:


The continuous $A$ matrix is discretized via the matrix exponential ($e^{A\Delta t}$) to become the discrete state transition matrix $F$ (or $A_k$) in your prediction step.


The continuous $C$ matrix usually maps directly to the discrete measurement matrix $H$ in your update step: $z_k = H_k x_k + v_k$. Because observation is instantaneous at the time of sampling, $C$ typically does not require integration over the time step.