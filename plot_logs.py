import sys
import matplotlib.pyplot as plt
import pandas as pd


def main(file_name):
    table = pd.read_csv(file_name, sep='\t')
    # time = table['Times']
    # throughput = table[:, 3]
    # arrival_freq = table[:, 4]
    # queue_size = table[:, 5]
    # n_workers = table[:, 6]

    fig, axs = plt.subplots(4)

    axs[0].set_title('Throughput')
    axs[0].plot(table["Timestamp"], table["Average_throughput"])
    axs[0].plot(table["Timestamp"], table["Average_arrival_frequency"])

    axs[1].set_title('Queue Size')
    axs[1].plot(table["Timestamp"], table["Queue_size"])

    axs[2].set_title('Latency')
    axs[2].plot(table["Timestamp"], table["Average_worker_latency"])

    axs[3].set_title('N. Workers')
    axs[3].plot(table["Timestamp"], table["n_workers"])

    plt.show()


if __name__ == "__main__":
    main(sys.argv[1])
