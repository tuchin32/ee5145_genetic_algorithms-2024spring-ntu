"""
    Original author: Weibu Lee
"""

import argparse
import math
import subprocess
import numpy as np
import matplotlib.pyplot as plt
from tqdm import tqdm

def run_ga(prob_type, data_path, ell, n_pop, max_gen, pc, pm, selection_pressure, xo_type):
    ga_execute = "./main"  # execute filename
    cmd = [
        ga_execute,
        str(prob_type),
        data_path,
        str(ell),
        str(n_pop),
        str(max_gen),
        str(pc),
        str(pm),
        str(selection_pressure),
        str(xo_type)
    ]
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    output_lines = result.stdout.splitlines()
    
    min_fitness = None
    for line in output_lines:
        if "Minimum fitness" in line:
            min_fitness = float(line.split(":")[1].strip())
            break

    return min_fitness

def plot_cmap(ell, n_pop, pc, sp, fitnesses, fitness_thres):

    # Successful points in fitness
    draw_pc = []
    draw_sp = []
    for i in range(len(fitnesses)):
        if (fitnesses[i] <= fitness_thres):
            draw_pc.append(pc[i])
            draw_sp.append(sp[i])
    print(f'number of datapoints for drawing control map: {len(draw_sp)}')

    x = np.log(draw_sp)
    # print(f'naturelog selection pressure: {x}')
    # print(f'crossover probability: {draw_pc}')

    # Mixing Boundary
    p_i = ((n_pop-1)/n_pop)**ell
    y_MixingBoundary = (1/(n_pop*np.log(n_pop)*p_i))*x
    # print(f'Mixing Boundary: {y_MixingBoundary}')

    # Drift Boundary
    C_d = 1.4
    # x_Drift = np.log(n_pop)/(C_d*n_pop)
    x_Drift = np.full((len(draw_pc),), (np.log(n_pop)/(C_d*n_pop)))
    # print(f'Drift Boundary: {x_Drift}')

    # Schema Theory
    y_Schema_easy = np.ones(len(draw_pc))
    # epsilon = 100/(ell-1)
    epsilon = 0.5
    y_Schema =  np.log(1/(1-np.array(draw_pc)*epsilon))
    # print(f'Schema Theory: {y_Schema}')

    # Cross Competition
    p_0 = 0.5
    alpha = 0.5
    x_CrossCompetition = np.full((len(draw_pc),), np.log(n_pop*(np.log(1-p_0))/np.log(alpha)))
    # print(f'Cross Competition: {x_CrossCompetition}')

    # Plot the control map
    # plt.figure(figsize=(10, 10))
    # plt.xticks(np.arange(0, 0.8, step=0.1))
    # plt.yticks(np.arange(0, 2.1, step=0.001))
    plt.plot(y_MixingBoundary, 'bo', label="Mixing Boundary")
    plt.plot(x_Drift, draw_pc, 'mo', label="Drift Boundary")
    plt.plot(y_Schema_easy, '--r')
    plt.plot(x, y_Schema, 'ko', label="Schema Theorem")
    plt.plot(x_CrossCompetition, draw_pc, 'yo', label="Cross Competition")
    plt.xlabel("nature log selection pressure (s)")
    plt.ylabel("crossover probability (pc)")
    plt.legend()
    plt.title("Control Map of Traveling Salesman Problem")
    plt.savefig('test.png', dpi=300, bbox_inches='tight')
    plt.close()

def plot_map(sp, pc, fitness):
    fitness = fitness[::-1, :]
    print(fitness.shape)
    plt.figure(figsize=(10, 10))
    plt.imshow(fitness, cmap='coolwarm', interpolation='nearest')
    plt.colorbar()

    pc = [round(p, 2) for p in pc]
    sp = [round(s, 2) for s in sp]

    plt.yticks(np.arange(len(sp)), sp)
    plt.xticks(np.arange(len(pc)), pc)

    plt.xlabel('Selection Pressure')
    plt.ylabel('Crossover Probability')
    plt.title('Control Map of Traveling Salesman Problem')
    plt.savefig('test.png', dpi=300, bbox_inches='tight')
    plt.close()

def plot_threshold_map(sp, pc, fitness, threshold):
    # fitness = fitness[::-1, :]
    print(fitness.shape)
    plt.figure(figsize=(10, 10))
    
    for i in range(len(sp)):
        for j in range(len(pc)):
            if fitness[i][j] >= threshold:
                pass
                # plt.text(j, i, int(fitness[i][j]), ha='center', va='center', color='black')
            else:
                # plt.text(j, i, int(fitness[i][j]), ha='center', va='center', color='red')
                plt.scatter(j, i, color='red', s=100)

    pc = [round(p, 2) for p in pc]
    sp = [round(s, 2) for s in sp]

    plt.yticks(np.arange(len(sp)), sp)
    plt.xticks(np.arange(len(pc)), pc)

    plt.xlabel('Selection Pressure')
    plt.ylabel('Crossover Probability')
    plt.title('Control Map of Traveling Salesman Problem')
    plt.savefig('test_thres.png', dpi=300, bbox_inches='tight')
    plt.close()

    

if __name__ == '__main__':
    # Parse arguments
    parser = argparse.ArgumentParser()
    # parser.add_argument('--output_file', '-o', type=str, default='fitness_results.txt', help='Output file to save fitness results')
    parser.add_argument('--prob_type', '-p', type=int, default=0, help='Problem type (0 for TSP, 1 for PFSP)')
    parser.add_argument('--data_path', '-d', type=str, default='./data/dantzig42_d.txt', help='Path to the data file')
    parser.add_argument('--n_ell', type=int, default=42, help='Problem size (City number)')
    parser.add_argument('--n_pop', type=int, default=500, help='Population size')
    parser.add_argument('--max_gen', type=int, default=500, help='Maximum number of generations')
    parser.add_argument('--pm', type=float, default=0.1, help='Mutation probability')
    parser.add_argument('--xo_type', type=int, default=0, help='Crossover type (0 for Order crossover, 1 for Frequency crossover)')
    args = parser.parse_args()

    # test_size = 21
    size_pc, size_sp = 10, 10
    # pcs = [1.0]
    # pcs = np.random.uniform(0.0, 1.0, size=test_size)
    pcs = np.linspace(0.1, 1.0, num=size_pc, endpoint=True)
    # sps = [2.0]
    # sps = np.random.uniform(1.0, 2.0, size=test_size)
    sps = np.linspace(1.0, 10.0, num=size_sp, endpoint=True)

    fitnesses = np.zeros((size_pc, size_sp), dtype=float)

    fitnesses = np.loadtxt('./out/fitness_results.txt', dtype=float)
    plot_map(pcs, sps, fitnesses)
    plot_threshold_map(pcs, sps, fitnesses, 790)
    exit()

    # if os.path.exists(args.output_file):
    #     os.remove(args.output_file)

    n_repeat = 3

    for i, pc in enumerate(pcs):
        # for sp in tqdm(sps, leave=False):
        # print(f'pc: {pc}')
        pbar = tqdm(total=len(sps))
        for j, sp in enumerate(sps):
            pbar.set_description(f'pc: {pc}, sp: {sp}')

            sum_fitness = 0
            for r in range(n_repeat):
                min_fitness = run_ga(args.prob_type, args.data_path, args.n_ell, args.n_pop, args.max_gen, pc, args.pm, sp, args.xo_type)
                sum_fitness += min_fitness

            fitnesses[i][j] = sum_fitness / n_repeat
            # fitnesses.append(min_fitness)
            # print(f'pc: {pc}')
            # print(f'selection pressure: {sp}')
            # print(f'min_fitness: {min_fitness}')
            pbar.update(1)

    print(f'crossover probability: {pcs}')
    print(f'selection pressure: {sps}')
    print(f'fitness: {fitnesses}')
    # print(f"All runs completed. Results saved to {args.output_file}")

    # Save fitness results to file
    np.savetxt('./out/fitness_results.txt', fitnesses, fmt='%f')

    plot_map(pcs, sps, fitnesses)
    plot_threshold_map(pcs, sps, fitnesses, 800)

    # solution = 699
    # successful_thres = 800
    # fitness_thres = solution + successful_thres
    # plot_cmap(42, 1000, pcs, sps, np.random.uniform(699.0, 2001.0, size=test_size), fitness_thres)
