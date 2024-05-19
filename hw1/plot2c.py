import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

if __name__ == "__main__":
    DIR = './p2-a/log'
    OUT = './plot/p2c_std'
    os.makedirs(OUT, exist_ok=True)
    I = np.sqrt(2 * (np.log(2) - np.log(np.sqrt(4.14 * np.log(2)))))

    problem_size = [50, 100, 150, 200, 250, 300, 350, 400, 450, 500]


    for i, size in enumerate(problem_size):
        fig, ax = plt.subplots()
        ax.grid()
        for j, xo in enumerate(['one-point', 'uniform', 'popwise-shuffle', 'perfect-mixing']):
            stats = []
            if xo == 'perfect-mixing':
                t_conv = int((np.pi / 2) * (np.sqrt(size) / 0.576291))
                for t in range(t_conv):
                    p_t = 0.5 * (1 + np.sin(I * t / np.sqrt(size)))
                    std_t = np.sqrt(size * p_t * (1 - p_t))
                    # stats.append(p_t)
                    stats.append(std_t)

            else:
                with open(f'{DIR}/SGA_{size}_{j}.txt') as f:
                    lines = f.readlines()

                    for line in lines:
                        if not line.startswith('Gen'):
                            continue

                        # Example: Gen: 1 Mean: 27.043478, Std: 3.218100, Mean_next: 28.822251, Selection Intensity: 0.552740
                        # Extract the selection intensity
                        # record = line.split('Selection Intensity: ')[1][:-1]
                        record = line.split('Std: ')[1].split(', ')[0]
                        # record = line.split('Mean: ')[1].split(', ')[0]
                        if 'nan' not in record:
                            # print(record)
                            # stats.append(float(record) / size)
                            stats.append(float(record))
            # Plot the selection intensity
            
            ax.plot(stats, '.', label=xo)
            ax.plot(stats, label=None, color=f'tab:{["blue", "orange", "green", "red"][j]}', alpha=0.5)
            # ax.plot([0, len(stats)], [0.576291, 0.576291], '--', color='tab:blue', alpha=0.5)
            ax.set_xlabel('Generation $t$')
            ax.set_ylabel('Standard deviation $\sigma_t$')
            # ax.set_ylabel('Proportion of 1-bits $p_t$')
            # plt.savefig(f'{OUT}/p2b-{xo}-{size}.png', dpi=300, bbox_inches='tight')
            # plt.close()
        
        ax.legend()
        plt.savefig(F'{OUT}/p2c-{size}.png', dpi=300, bbox_inches='tight')
        plt.close()
        # import ipdb; ipdb.set_trace()


    # # Plot the histogram of selection intensity
    # fig, ax = plt.subplots()
    # ax.grid()
    # counts, edges, bars = ax.hist(all_stats, bins=30, color='tab:blue')
    # print(counts, sum(counts))
    # print(edges)
    # print(bars)
    # ax.plot([0.576291, 0.576291], [0, 1100], '--', color='black')
    # ax.text(0.567291, 1130, '$I = 0.576291$', verticalalignment='top', fontsize=7.5)
    # ax.set_xlabel('Selection Intensity')
    # ax.set_ylabel('Frequency')
    # plt.savefig(f'{OUT}/p2b-hist.png', dpi=300, bbox_inches='tight')            
                    

