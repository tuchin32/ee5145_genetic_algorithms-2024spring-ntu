import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

if __name__ == "__main__":
    DIR = './p2-a/log'
    OUT = './plot'

    problem_size = [50, 100, 150, 200, 250, 300, 350, 400, 450, 500]

    all_intensities = []

    for i, size in enumerate(problem_size):
        for j, xo in enumerate(['one-point', 'uniform', 'popwise-shuffle']):
            with open(f'{DIR}/SGA_{size}_{j}.txt') as f:
                lines = f.readlines()

                intensities = []
                for line in lines:
                    if not line.startswith('Gen'):
                        continue

                    # Example: Gen: 1 Mean: 27.043478, Std: 3.218100, Mean_next: 28.822251, Selection Intensity: 0.552740
                    # Extract the selection intensity
                    record = line.split('Selection Intensity: ')[1][:-1]
                    # record = line.split('Std: ')[1].split(', ')[0]
                    if 'nan' not in record:
                        # print(record)
                        intensities.append(float(record))
                        all_intensities.append(float(record))

                # Plot the selection intensity
                fig, ax = plt.subplots()
                ax.grid()
                
                ax.plot(intensities)
                ax.plot([0, len(intensities)], [0.576291, 0.576291], '--', color='tab:blue', alpha=0.5)
                ax.set_xlabel('Generation')
                ax.set_ylabel('Selection Intensity')
                plt.savefig(f'{OUT}/p2b-{xo}-{size}.png', dpi=300, bbox_inches='tight')
                # plt.savefig('tmp.png', dpi=300, bbox_inches='tight')
                plt.close()
            
        # plt.close()
        # import ipdb; ipdb.set_trace()


    # Plot the histogram of selection intensity
    fig, ax = plt.subplots()
    ax.grid()
    counts, edges, bars = ax.hist(all_intensities, bins=30, color='tab:blue')
    print(counts, sum(counts))
    print(edges)
    print(bars)
    ax.plot([0.576291, 0.576291], [0, 1100], '--', color='black')
    ax.text(0.567291, 1130, '$I = 0.576291$', verticalalignment='top', fontsize=7.5)
    ax.set_xlabel('Selection Intensity')
    ax.set_ylabel('Frequency')
    plt.savefig(f'{OUT}/p2b-hist.png', dpi=300, bbox_inches='tight')            
                    

