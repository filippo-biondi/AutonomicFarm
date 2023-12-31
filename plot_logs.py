import sys
import matplotlib.pyplot as plt
import pandas as pd


def main(file_names):
    for file_name in file_names:
        table = pd.read_csv(file_name, sep='\t')

        fig, axs = plt.subplots(5, figsize=(10, 10))
        fig.tight_layout()
        axs[0].set_title('Throughput')
        axs[0].plot(table["Timestamp"], table["Average_throughput"])

        axs[1].set_title('Arrival frequency')
        axs[1].plot(table["Timestamp"], table["Average_arrival_frequency"])

        axs[2].set_title('Queue Size')
        axs[2].plot(table["Timestamp"], table["Queue_size"])

        axs[3].set_title('Latency')
        axs[3].plot(table["Timestamp"], table["Average_worker_latency"])

        axs[4].set_title('N. Workers')
        axs[4].plot(table["Timestamp"], table["n_workers"])

        plt.show()


if __name__ == "__main__":
    main(sys.argv[1:])
