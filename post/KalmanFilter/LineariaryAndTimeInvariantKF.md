# why the classic Kalman Filter requires these two properties--Linearity and Time-Invariant


## Why Clasiis KF need linearity
### What is "Linear"?

In a system context, Linearity means that the system obeys the Principle of Superposition. Superposition is broken down into two strict mathematical criteria:

- $Scaling (Homogeneity)$: If an input $\mathbf{u}$ causes an output $\mathbf{y}$, then multiplying that input by a factor $k$ must multiply the output by the exact same factor $k$.

$$f(k\cdot \mathbf{u}) = k \cdot f(\mathbf{u})$$

- $Additivity$: If input $\mathbf{u}_1$ creates output $\mathbf{y}_1$ and input $\mathbf{u}_2$ creates output $\mathbf{y}_2$, then injecting both inputs together must yield the sum of both outputs.

$$f(\mathbf{u}_1 + \mathbf{u}_2) = f(\mathbf{u}_1) + f(\mathbf{u}_2)$$

### Why the Linear Kalman Filter Needs It

In a standard Linear Kalman Filter, your state transitions and measurement equations are constructed entirely out of matrix multiplications:

$$\mathbf{x}_{k+1} = \mathbf{F}\mathbf{x}_k + \mathbf{G}\mathbf{u}_k$$


Matrix multiplication is a linear operation. This property provides a massive mathematical advantage: If you pass a Gaussian distribution (a bell curve) through a linear transformation, the output is guaranteed to remain a perfect Gaussian distribution. Because the shape doesn't warp, the Kalman Filter only needs to track two things—the mean vector and the covariance matrix—to perfectly describe the system's uncertainty profile.


### Real-World Breakdowns

Most physical systems are not perfectly linear. Linearity breaks down due to geometry and physics:

- $Perspective/Coordinate Mapping$: Projecting a target's 3D real-world coordinates back into a 2D image sensor plane involves perspective division ($x/z, y/z$) and nested trigonometric Euler rotations. Division and trigonometry are fiercely nonlinear. 
- $Aerodynamics$: Drag forces scale with the square of velocity ($v^2$)


When a system is nonlinear, passing a Gaussian curve through it twists, warps, and skews the output into a non-Gaussian shape. This causes the standard Linear Kalman Filter to miscalculate the true uncertainty, resulting in track divergence. To handle this, engineers must upgrade to nonlinear variants like the Extended Kalman Filter (EKF) or the Cubature Kalman Filter (CKF), which approximate these transformations using advanced integration or calculus tricks.


## Why  "Time-Invariant"?

### What is "Time-Invariant"?

Time-Invariance means that the fundamental physical properties and behavior of the system do not change depending on absolute clock time.

If an input $\mathbf{u}(t)$ produces an output $\mathbf{y}(t)$ today, then applying that identical input tomorrow—or shifting it by a time delay $T$—must produce the exact same output, just shifted by that same delay $T$:


$$f(\mathbf{u}(t - T)) = \mathbf{y}(t - T)$$

In your state-space matrices ($\mathbf{A}, \mathbf{B}, \mathbf{C}, \mathbf{D}$ or $\mathbf{F}, \mathbf{G}, \mathbf{H}$), time-invariance means that the values inside the matrices are constant coefficients. They do not change as time $t$ marches forward.


### Why the Kalman Filter Assumes It


If a system is time-invariant, the software tracking loop can reuse the same state-transition rules ($\mathbf{F}$) at every single time-step without recalculating the underlying differential equations from scratch. This drastically reduces the computational budget on embedded edge hardware.  




### Real-World Breakdowns

Many systems are actually Linear Time-Varying (LTV). Their parameters change over time due to external or internal forces:



- $Mass Depletion$: A rocket or high-speed interceptor burns through fuel at a massive rate. Because its mass is dropping continuously, the same thrust input ($\mathbf{u}$) causes a much higher acceleration ($\mathbf{a}$) at minute 5 than it did at second 1. The system matrix changes over time.

- $Variable Sampling Windows$: If your sensor data streams arrive asynchronously or experience network latency, the time delta ($\Delta t$) between clock cycles fluctuates. Because the discrete matrix $\mathbf{F} = e^{\mathbf{A}\Delta t}$ depends directly on $\Delta t$, the matrix must be dynamically recomputed at every single frame, breaking static time-invariance.

Fortunately, the core Kalman Filter algorithm is highly adaptable. If your system is time-varying, you can simply update the matrices with a time index ($\mathbf{F}_k$, $\mathbf{H}_k$) inside your execution loop to recompute the system dynamics on the fly.