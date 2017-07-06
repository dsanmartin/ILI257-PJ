import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.collections import LineCollection
from matplotlib import cm

# Chebyshev differentiation matrix construction
def cheb(N):
    if N == 0:
        D = 0
        x = 1
        return D, x
    x = np.cos(np.pi*np.arange(N+1)/N)
    c = np.hstack((2,np.ones(N-1),2))*((-1.)**np.arange(N+1))
    X = np.tile(x,(N+1,1)).T
    dX = X - X.T
    D = np.outer(c, 1./c)/(dX + np.eye(N+1))
    D = D - np.diag(np.sum(D.T, axis=0))
    return D, x


# Differentiation matrix
N = 20; D, x = cheb(N); D2 = np.dot(D, D)   

# Remove first and last row for convenience
D2[0,:] = np.zeros(N+1)
D2[-1,:] = np.zeros(N+1)

# Initial values
eps = 1e-2#0.001
#dt = min([.01, 50*N**(-4)/eps])
dt = .01
v = .53*x + .47*np.sin(-1.5*np.pi*x) # Initial condition
#v = x + np.cos(np.pi*x)
#v = np.exp(-x)

# Method's parameters
tmax = 100; tplot = 2; nplots = round(tmax/tplot)
plotgap = round(tplot/dt); dt = tplot/plotgap

# Interpolation (for make a better approximation of solution)
#xx = np.arange(-1, 1.025, .025)
#vv = np.polyval(np.polyfit(x,v,N),xx)

# Matrix and vector to save approximations
#u = np.zeros((nplots+1, len(xx)))
u = np.zeros((nplots+1, len(x)))
t = np.zeros(nplots+1)
u[0] = v#v
tt = 0

# Solving PDE using Euler's Method
for i in range(nplots):
    for n in range(plotgap):
        tt = tt + dt
        
        # The approximation of second derivative using chebyshev matrix 
        # For border's conditions is used the values of border values of the initial condition 
        # Since for convenience the first and last row is removed from the differentiation matrix, the borders
        # values (-1 and 1) are kept in all the times
        #v = v + dt*(eps*np.dot(D2, v) + v - v**3)
        
        v = v + dt*(eps*np.dot(D2, v) + v - v**3)
        
        
        
#        k1 = np.dot(D2, v) #+ v 
#        k2 = np.dot(D2, v + 0.5*k1*dt) #+ (v+0.5*k1*dt)
#        k3 = np.dot(D2, v + 0.5*k2*dt) #+ (v+0.5*k2*dt)
#        k4 = np.dot(D2, v + k3*dt) #+ (v+k3*dt)
#        v = v + (dt/6)*(k1 + 2*(k2 + k3) + k4)
    
    # Interpolation
    #vv = np.polyval(np.polyfit(x,v,N), xx)
    vv = v
    
    # Save approximation and time
    u[i+1] = vv
    t[i+1] = tt
    
#print(x)
# Plot
fig = plt.figure(figsize=(12,6))
ax = fig.gca(projection='3d')
#X, T = np.meshgrid(xx, t)
X, T = np.meshgrid(x, t)
ax.plot_surface(X, T, u, rstride=1, cstride=1, color="white")
ax.set_xlabel('x')
ax.set_ylabel('t')
ax.view_init(elev=30., azim=230.)
plt.show()