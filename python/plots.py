import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.collections import LineCollection
from matplotlib import cm

def plot(n, t_real, t_user, t_sys):
#    plt.semilogy(n, t_real)
#    plt.semilogy(n, t_user)
#    plt.semilogy(n, t_sys)
    plt.plot(n, t_real, "b-*")
    plt.plot(n, t_user, "r-d")
    plt.plot(n, t_sys, "g-x")
    plt.grid(True)
    plt.show()
    return None

def plot3D(x, t, u):
    fig = plt.figure(figsize=(12,6))
    ax = fig.gca(projection='3d')
    #X, T = np.meshgrid(xx, t)
    X, T = np.meshgrid(x, t)
    ax.plot_surface(X, T, u, rstride=1, cstride=1, color="white")
    ax.set_xlabel('x')
    ax.set_ylabel('t')
    ax.view_init(elev=30., azim=230.)
    plt.show()

#csv_file = "../results/20170701201251.csv"
#df = pd.read_csv(csv_file)

#plot(df["n"], df["time_real"], df["time_user"], df["time_sys"])

csv_x = "../C/CSV/x.csv"
csv_t = "../C/CSV/t.csv"
csv_u = "../C/CSV/U.csv"

x = np.genfromtxt(csv_x, delimiter=',')
t = np.genfromtxt(csv_t, delimiter=',')
u = np.genfromtxt(csv_u, delimiter=',')

plot3D(x, t, u)