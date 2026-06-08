# The process of the state extrapolation equation derivation

The state-space representation of a continuous-time Linear Time-Invariant (LTI) system models the system's internal dynamics using a set of first-order differential equations, rather than a single high-order differential equation or a transfer function.Mathematically, it consists of a State Equation and an Output Equation, typically written in compact matrix-vector notation:

$$\mathbf{\dot{x}}(t) = \mathbf{A}\mathbf{x}(t) + \mathbf{B}\mathbf{u}(t)$$
$$\mathbf{y}(t) = \mathbf{C}\mathbf{x}(t) + \mathbf{D}\mathbf{u}(t)$$



The discrete-time state-space architecture of an LTI system drops the explicit time variable $t$ in favor of discrete steps $k$:

$$\mathbf{x}_{k+1} = \mathbf{F}\mathbf{x}_k + \mathbf{G}\mathbf{u}_k$$
$$\mathbf{y}_k = \mathbf{H}\mathbf{x}_k + \mathbf{D}\mathbf{u}_k$$


To derive the discrete-time state extrapolation equations from the continuous-time Linear Time-Invariant (LTI) state-space model,, we must solve a system of first-order vector differential equations over a small, finite time step $\Delta t = t_{k+1} - t_k$.Here is the complete mathematical proof.


## Step 1: The Setup and Core Assumption

We begin with the continuous state equation:

$$\mathbf{\dot{x}}(t) = \mathbf{A}\mathbf{x}(t) + \mathbf{B}\mathbf{u}(t)$$

To integrate this over a digital clock cycle from $t_k$ to $t_{k+1}$, we apply the standard Zero-Order Hold (ZOH) assumption. This means that the control input vector $\mathbf{u}(t)$ is held perfectly constant by the digital-to-analog hardware throughout the sampling interval:

$$\mathbf{u}(t) = \mathbf{u}_k \quad \text{for} \quad t_k \le t < t_{k+1}$$

## Step 2: Solving the Homogeneous Equation ($\mathbf{u}(t) = \mathbf{0}$)

Before solving the full equation, let’s look at the unforced system $\mathbf{\dot{x}}(t) = \mathbf{A}\mathbf{x}(t)$. Rearranging it gives:
$$\mathbf{\dot{x}}(t) - \mathbf{A}\mathbf{x}(t) = \mathbf{0}$$
In scalar calculus, you solve $\dot{x} - ax = 0$ by multiplying by an integrating factor $e^{-at}$. In matrix calculus, we do the exact same thing using the matrix exponential $e^{-\mathbf{A}t}$ as our integrating factor.Let's multiply the full non-homogeneous state equation from the left by $e^{-\mathbf{A}t}$:
$$e^{-\mathbf{A}t}\mathbf{\dot{x}}(t) - e^{-\mathbf{A}t}\mathbf{A}\mathbf{x}(t) = e^{-\mathbf{A}t}\mathbf{B}\mathbf{u}(t)$$

By the matrix product rule for differentiation, notice that the left side collapses perfectly into a single derivative:

$$\frac{d}{dt}\left( e^{-\mathbf{A}t}\mathbf{x}(t) \right) = e^{-\mathbf{A}t}\mathbf{B}\mathbf{u}(t)$$


## Step 3: Definite Integration over the Time StepNow, 

we perform a definite integration on both sides over our discrete sampling window, from the current time $t_k$ to the next step $t_{k+1}$. To avoid variable confusion, we introduce a dummy integration variable $\tau$:
$$\int_{t_k}^{t_{k+1}} \frac{d}{d\tau}\left( e^{-\mathbf{A}\tau}\mathbf{x}(\tau) \right) d\tau = \int_{t_k}^{t_{k+1}} e^{-\mathbf{A}\tau}\mathbf{B}\mathbf{u}(\tau) d\tau$$

By the Fundamental Theorem of Calculus, the left side integrates directly to:

$$e^{-\mathbf{A}t_{k+1}}\mathbf{x}(t_{k+1}) - e^{-\mathbf{A}t_k}\mathbf{x}(t_k) = \int_{t_k}^{t_{k+1}} e^{-\mathbf{A}\tau}\mathbf{B}\mathbf{u}(\tau) d\tau$$


## Step 4: Isolating the Next State $\mathbf{x}(t_{k+1})$
Multiply the entire equation from the left by $e^{\mathbf{A}t_{k+1}}$ to isolate our target vector:
$$\mathbf{x}(t_{k+1}) = e^{\mathbf{A}(t_{k+1}-t_k)}\mathbf{x}(t_k) + \int_{t_k}^{t_{k+1}} e^{\mathbf{A}(t_{k+1}-\tau)}\mathbf{B}\mathbf{u}(\tau) d\tau$$
Let's substitute our discrete notations: $\mathbf{x}(t_{k+1}) = \mathbf{x}_{k+1}$, $\mathbf{x}(t_k) = \mathbf{x}_k$, and the sampling window duration $\Delta t = t_{k+1}-t_k$:
$$\mathbf{x}_{k+1} = e^{\mathbf{A}\Delta t}\mathbf{x}_k + \int_{t_k}^{t_{k+1}} e^{\mathbf{A}(t_{k+1}-\tau)}\mathbf{B}\mathbf{u}(\tau) d\tau$$

## Step 5: Evaluating the Input Integral via ZOHNow

we resolve the integral on the right. Because of our Zero-Order Hold assumption from Step 1, we can safely pull the constant vector $\mathbf{u}(\tau) = \mathbf{u}_k$ and the constant matrix $\mathbf{B}$ outside the integration:
$$\mathbf{x}_{k+1} = e^{\mathbf{A}\Delta t}\mathbf{x}_k + \left( \int_{t_k}^{t_{k+1}} e^{\mathbf{A}(t_{k+1}-\tau)} d\tau \right)\mathbf{B}\mathbf{u}_k$$
To evaluate this remaining integral cleanly, we perform a change of variables (substitution).Let $v = t_{k+1} - \tau$, which means $dv = -d\tau$.
- When $\tau = t_k$, the lower bound becomes $v = t_{k+1} - t_k = \Delta t$.
- When $\tau = t_{k+1}$, the upper bound becomes $v = t_{k+1} - t_{k+1} = 0$.

Substituting these into the integral flips the bounds and removes the negative sign:
$$\int_{\Delta t}^{0} e^{\mathbf{A}v} (-dv) = \int_{0}^{\Delta t} e^{\mathbf{A}v} dv$$


## Step 6: Mapping to the Final Discrete Matrices

Our state equation has now transformed into:

$$\mathbf{x}_{k+1} = \underbrace{\left(e^{\mathbf{A}\Delta t}\right)}_{\mathbf{F}}\mathbf{x}_k + \underbrace{\left(\int_{0}^{\Delta t} e^{\mathbf{A}v} dv \cdot \mathbf{B}\right)}_{\mathbf{G}}\mathbf{u}_k$$


This defines our discrete components perfectly:

1 .The State Transition Matrix ($\mathbf{F}$):

$$\mathbf{F} = e^{\mathbf{A}\Delta t} = \mathbf{I} + \mathbf{A}\Delta t + \frac{\mathbf{A}^2\Delta t^2}{2!} + \dots$$


2 .The Discrete Input Matrix ($\mathbf{G}$):

If $\mathbf{A}$ is invertible, evaluating the integral yields:
$$\mathbf{G} = \mathbf{A}^{-1}\left(e^{\mathbf{A}\Delta t} - \mathbf{I}\right)\mathbf{B}$$

Thus, the continuous state equation successfully maps to:

$$\mathbf{x}_{k+1} = \mathbf{F}\mathbf{x}_k + \mathbf{G}\mathbf{u}_k$$


## Step 7: Converting the Output Equation

The continuous output equation is memoryless (it does not contain time derivatives):
$$\mathbf{y}(t) = \mathbf{C}\mathbf{x}(t) + \mathbf{D}\mathbf{u}(t)$$

Because it depends purely on the instantaneous values of the states and inputs at the exact moment of sampling, discretization is a simple matter of renaming the variables to match the time step $k$:

$$\mathbf{y}_k = \mathbf{H}\mathbf{x}_k + \mathbf{D}\mathbf{u}_k$$

Where $\mathbf{H} = \mathbf{C}$, completing the full algebraic proof. This discrete system is precisely what code bases utilize to extrapolate states over fixed time intervals.