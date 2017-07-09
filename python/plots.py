import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.collections import LineCollection
from matplotlib import cm

def plot(df1, df2, df3, df4):
    fig, ax = plt.subplots()
    plt.plot(df1["n"], df1["time_real"], "k-x", label="1 thread")
    plt.plot(df2["n"], df2["time_real"], "b-*", label="2 threads")
    plt.plot(df3["n"], df3["time_real"], "r-d", label="3 threads")
    plt.plot(df4["n"], df4["time_real"], "g-o", label="4 threads") 
    ax.set_xticks(df1["n"])
    plt.grid(True)
    plt.legend()
    plt.xlabel("N")
    plt.ylabel("t [s]")
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

csv_file1 = "../results/n_1/20170709.csv"
csv_file2 = "../results/n_2/20170709.csv"
csv_file3 = "../results/n_3/20170709.csv"
csv_file4 = "../results/n_4/20170709.csv"
df1 = pd.read_csv(csv_file1)
df2 = pd.read_csv(csv_file2)
df3 = pd.read_csv(csv_file3)
df4 = pd.read_csv(csv_file4)
plot(df1, df2, df3, df4)

#csv_x = "../C/CSV/x.csv"
#csv_t = "../C/CSV/t.csv"
#csv_u = "../C/CSV/U.csv"
#
#x = np.genfromtxt(csv_x, delimiter=',')
#t = np.genfromtxt(csv_t, delimiter=',')
#u = np.genfromtxt(csv_u, delimiter=',')
#
#plot3D(x, t, u)