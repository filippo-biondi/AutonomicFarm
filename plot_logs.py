import sys
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd


# def moving_average(a, n=3):
#     ret = np.cumsum(a, dtype=float)
#     ret[n:] = ret[n:] - ret[:-n]
#     return ret[n - 1:] / n


def panda_lambda(x):
    if isinstance(x, str):
        return float(x.split(":")[1])
    return x


def main(file_name):
    table = pd.read_csv(file_name, sep='\t', header=None).applymap(panda_lambda).values
    time = table[:, 0]
    throughput = table[:, 3]
    arrival_freq = table[:, 4]
    queue_size = table[:, 5]
    n_workers = table[:, 6]

    fig, axs = plt.subplots(3)

    axs[0].set_title('Throughput')
    axs[0].plot(time, throughput)
    axs[0].plot(time, arrival_freq)

    axs[1].set_title('Queue Size')
    axs[1].plot(time, queue_size)

    axs[2].set_title('Number of Workers')
    axs[2].plot(time, n_workers)

    plt.show()


if __name__ == "__main__":
    main(sys.argv[1])
