import pandas as pd
import matplotlib.pyplot as plt

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

csv_file = "../results/20170701201251.csv"
df = pd.read_csv(csv_file)

plot(df["n"], df["time_real"], df["time_user"], df["time_sys"])