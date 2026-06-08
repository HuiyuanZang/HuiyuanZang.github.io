

# Kalman Filter 

## Normal Distribution

$$
f(x)=\frac{1}{\sigma\sqrt{2\pi}} e^{-\frac{(x-\mu)^2}{2\sigma^2}}
$$

What each symbol means:
- x: the value of the variable
- μ (mu): the mean (center of the distribution)
- σ (sigma): the standard deviation (spread of the distribution)
- π: a constant ≈ 3.14159
- e: Euler’s number ≈ 2.718

Key properties:

- Symmetrical bell-shaped curve
- Mean = Median = Mode
About:
- 68% of data lies within μ±σ
- 95% within  μ±2σ
- 99.7% within  μ±3σ


## Kalman Filter in one dimension


- p: estimate variant 
- $σ^2$ : measurement variant. The standard deviation (σ) of the measurement
errors is the measurement uncertainty


### The Estimate Uncertainty Extrapolation for a constant velocity dynamics

$$
p_{n+1, n}^x = p_{n, n}^x + \Delta t^2 \cdot p_{n, n}^v
$$
$$
P_{n+1, n}^v = p_{n, n}^v
$$

Where 

- $p^x$ is the position estimate variance.
- $p^v$ is the velocity estimate variance.


To proof, we need to proof in probability and statistics, the variance of a random variable X measures how spread out the numbers are from the mean (expected value). That is when you multiply a variable by a constant, its variance scales by the square of that constant.

$$
Var(cX)=c^2Var(X)
$$

The Variance is a single number (a summary statistic). It tells you, on average, how far away your data points are from the center (the mean).

Because we want to know the average squared distance from the mean μ, we take the "expected value" (E) of that squared distance. That is why the definition is:

$$
Var(cX)=E[(X-\mu)^2]
$$

In statistics, the "expected value" of anything is calculated by multiplying all possible values by their probabilities (the PDF) and adding them up (integrating).So, if you have a continuous random variable X with a PDF of f(x), the formula to actually compute the variance looks like this:
$$
Var(X) = \int_{-\infty}^{\infty} (x - \mu)^2 \cdot f(x) dx
$$
where f(x) is x's PDF(probablitiy distribution function).  

Keep in mind, the $\mu$ isn't completely independed with X in the variant function $Var(X)=E[(X-\mu)^2]$. Therefore, $Var(cX)=E[(cX-c\mu)^2]$; but not $Var(cX)=E[(cX-\mu)^2]$ becuase the mean of Var(cX) should be $c\mu$

if Y = cX, change $Var(X) = \int_{-\infty}^{\infty} (x - \mu)^2 \cdot f(x) dx$ to y, change the differential to dy, but you must use the properly scaled PDF $f_Y(y) = \frac{1}{c} \, f\!\left(\frac{y}{c}\right)$


$$
Var(Y) = \int_{-\infty}^{\infty} (y - c\mu)^2 \cdot \frac{1}{c} \, f\!\left(\frac{y}{c}\right) d(y)
$$

### Kalman Filter Optimal filter


$$
\hat{x}_{n,n} = w_{1} z_{n} + (1 − w_{1})\hat{x}_{n,n−1}
$$

$$
p_{n,n} = w_{1}^2 r_{n} + (1 − w_{1})^2 p_{n,n−1}
$$

where: 
- $p_{n,n}$ is the variance of the optimum combined estimate
- $p_{n,n-1}$ is the variance of the prior estimate $\hat{x}_{n,n-1}$
- $r_{n}$ is the variance of the measurement $z_{n}$



To find $w_{1}$ that minimizes $p_{n,n}$ , we differentiate $p_{n,n}$ with respect to $w_{1}$  and set the result to zero.

$$
\frac{\partial p_{n,n}}{\partial w_{1}} = 2 w_1 r_n - 2(1 - w_1) p_{n,n-1} = 0
$$

Therefore, the Kalman Gain $(0 ≤ k_n ≤ 1)$ is 

$$
k_n = w_1 = \frac{p_{n,n-1}}{p_{n,n-1} + r_n}    
$$



Then, we have the state update equation

$$
\hat{x}_{n,n} = \hat{x}_{n,n-1} +  \frac{p_{n,n-1}}{p_{n,n-1} + r_n} (z_n - \hat{x}_{n,n−1})
$$


### Covariance Update Equation


$$
p_{n,n} = (1 − K_n ) p_{n,n−1}
$$