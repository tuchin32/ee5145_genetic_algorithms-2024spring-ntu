import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

if __name__ == "__main__":
    DIR = './p2-a/log_repeat30'
    I = np.sqrt(2 * (np.log(2) - np.log(np.sqrt(4.14 * np.log(2)))))
    print(I)

    means = np.zeros((4, 10)) # 3 xo types, 10 problem sizes
    stds = np.zeros((4, 10))

    problem_size = [50, 100, 150, 200, 250, 300, 350, 400, 450, 500]

    for i, xo in enumerate(['one-point', 'uniform', 'popwise-shuffle']):
        for j, size in enumerate(problem_size):
            with open(f'{DIR}/SGA_{size}_{i}.txt') as f:
                lines = f.readlines()

                record = lines[12][24:-2]
                record = record.split(' (')
                means[i, j] = float(record[0])
                stds[i, j] = float(record[1])

    for i, size in enumerate(problem_size):
        t = (np.pi / 2) * (np.sqrt(size) / I)
        means[3, i] = t
        stds[3, i] = 0

    df_mean = pd.DataFrame(means, columns=problem_size, index=['one-point', 'uniform', 'popwise-shuffle', 'perfect mixinig'])
    df_std = pd.DataFrame(stds, columns=problem_size, index=['one-point', 'uniform', 'popwise-shuffle', 'perfect mixinig'])
    df_mean.to_csv('./plot/p2-c-mean.csv')
    df_std.to_csv('./plot/p2-c-std.csv')

    # problem_size = [np.sqrt(ell) for ell in problem_size]
    param0 = np.polyfit(problem_size, means[0], 2)
    param1 = np.polyfit(problem_size, means[1], 2)
    param2 = np.polyfit(problem_size, means[2], 2)
    param3 = np.polyfit(problem_size, means[3], 2)

    x = np.linspace(50, 500, 100)
    # x = np.linspace(7, 23, 100)
    y0 = param0[0] * x**2 + param0[1] * x + param0[2]
    y1 = param1[0] * x**2 + param1[1] * x + param1[2]
    y2 = param2[0] * x**2 + param2[1] * x + param2[2]
    y3 = param3[0] * x**2 + param3[1] * x + param3[2]


    fig, ax = plt.subplots()
    ax.grid()


    ax.errorbar(problem_size, means[0], yerr=stds[0], label='one-point xo', capsize=5, elinewidth=2, markeredgewidth=2, fmt='o')
    ax.errorbar(problem_size, means[1], yerr=stds[1], label='uniform xo', capsize=5, elinewidth=2, markeredgewidth=2, fmt='o')
    ax.errorbar(problem_size, means[2], yerr=stds[2], label='population-wise shuffling', capsize=5, elinewidth=2, markeredgewidth=2, fmt='o')
    ax.errorbar(problem_size, means[3], yerr=stds[3], label='perfect mixinig', capsize=5, elinewidth=2, markeredgewidth=2, fmt='o')
    ax.plot(x, y0, '--', label=None, color='tab:blue', alpha=0.5)
    ax.plot(x, y1, '--', label=None, color='tab:orange', alpha=0.5)
    ax.plot(x, y2, '--', label=None, color='tab:green', alpha=0.5)
    ax.plot(x, y3, '--', label=None, color='tab:red', alpha=0.5)
    ax.xaxis.set_ticks(problem_size)
    ax.set_xlabel('Problem size')
    ax.set_ylabel('Convergence time (mean and std)')
    ax.legend()

    plt.savefig('./plot/p2-c.png', dpi=300, bbox_inches='tight')
    plt.close()

    # # Excgange the y axis and x axis
    # fig, ax = plt.subplots()
    # ax.grid()

    # ax.errorbar(means[0], problem_size, xerr=stds[0], label='one-point xo', capsize=5, elinewidth=2, markeredgewidth=2, fmt='o')
    # ax.errorbar(means[1], problem_size, xerr=stds[1], label='uniform xo', capsize=5, elinewidth=2, markeredgewidth=2, fmt='o')
    # ax.errorbar(means[2], problem_size, xerr=stds[2], label='population-wise shuffling', capsize=5, elinewidth=2, markeredgewidth=2, fmt='o')
    # ax.plot(y0, x, '--', label=None, color='tab:blue', alpha=0.5)
    # ax.plot(y1, x, '--', label=None, color='tab:orange', alpha=0.5)
    # ax.plot(y2, x, '--', label=None, color='tab:green', alpha=0.5)
    # ax.yaxis.set_ticks(problem_size)
    # ax.set_ylabel('Problem size')
    # ax.set_xlabel('Convergence time (mean and std)')
    # ax.legend()

    # plt.savefig('./plot/p2-a-exchange.png', dpi=300, bbox_inches='tight')
    # plt.close()

