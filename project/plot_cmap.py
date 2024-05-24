import argparse
import subprocess
import numpy as np
import matplotlib.pyplot as plt
from tqdm import tqdm
from joblib import Parallel, delayed
from scipy.spatial import ConvexHull

def run_ga(prob_type, data_path, n_pop, max_gen, pc, pm, selection_pressure, xo_type):
    ga_execute = "./main"  # execute filename
    cmd = [
        ga_execute,
        str(prob_type),
        data_path,
        str(n_pop),
        str(max_gen),
        str(pc),
        str(pm),
        str(selection_pressure),
        str(xo_type)
    ]
    
    # result = subprocess.run(cmd, capture_output=True, text=True)
    result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
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

def plot_map(sp, pc, fitness, savename):
    fitness = fitness[::-1, :]
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
    plt.savefig(f'./image/{savename}_heatmap.png', dpi=300, bbox_inches='tight')
    plt.close()

def plot_threshold_map(sp, pc, fitness, threshold, savename):
    # fitness = fitness[::-1, :]
    plt.figure(figsize=(10, 10))
    
    coords = []
    for i in range(len(sp)):
        for j in range(len(pc)):
            if fitness[i][j] >= threshold:
                pass
                # plt.text(j, i, int(fitness[i][j]), ha='center', va='center', color='black')
            else:
                # plt.text(j, i, int(fitness[i][j]), ha='center', va='center', color='red')
                plt.scatter(j, i, color='black', s=100)
                coords.append([j, i])

    # Find envelope based on the given points
    coords = np.array(coords)
    hull = ConvexHull(coords)
    for simplex in hull.simplices:
        plt.plot(coords[simplex, 0], coords[simplex, 1], 'k-')
    
    pc = [round(p, 2) for p in pc]
    sp = [round(s, 2) for s in sp]

    plt.yticks(np.arange(len(sp)), sp)
    plt.xticks(np.arange(len(pc)), pc)

    plt.xlabel('Selection Pressure')
    plt.ylabel('Crossover Probability')
    plt.title('Control Map of Traveling Salesman Problem')
    plt.savefig(f'./image/{savename}_ctrlmap.png', dpi=300, bbox_inches='tight')
    plt.close()

    

if __name__ == '__main__':
    # Parse arguments
    parser = argparse.ArgumentParser()
    # - GA parameters
    parser.add_argument('--prob_type', '-p', type=int, default=0, help='Problem type (0 for TSP, 1 for PFSP)')
    parser.add_argument('--data_path', '-d', type=str, default='./data/dantzig42_d.txt', help='Path to the data file')
    parser.add_argument('--n_ell', type=int, default=42, help='Problem size (City number)')
    parser.add_argument('--n_pop', type=int, default=2500, help='Population size')
    parser.add_argument('--max_gen', type=int, default=1000, help='Maximum number of generations')
    parser.add_argument('--pm', type=float, default=0.0, help='Mutation probability')
    parser.add_argument('--xo_type', type=int, default=0, help='Crossover type (0 for Order crossover, 1 for Frequency crossover)')
    # - Simulation parameters
    parser.add_argument('--output_file', '-o', type=str, default='dantzig42', help='Output file to save fitness results')
    parser.add_argument('--n_repeat', type=int, default=3, help='Number of repeats for each configuration')
    parser.add_argument('--load', action=argparse.BooleanOptionalAction, help='Load fitness results from file')
    args = parser.parse_args()


    size_pc, size_sp = 10, 13
    pcs = np.linspace(0.1, 1.0, num=size_pc, endpoint=True)
    # sps = np.linspace(1.0, 30.0, num=size_sp, endpoint=True)
    sps = np.logspace(0.0, 6.0, num=size_sp, endpoint=True, base=2.0)
    
    print(f'crossover probability: {pcs}')
    print(f'selection pressure: {sps}')

    if args.load:
        fitnesses = np.loadtxt(f'./out/{args.output_file}_fitness.txt', dtype=float)

    else:
        fitnesses = np.zeros((size_pc, size_sp), dtype=float)

        pbar = tqdm(total=(sps.shape[0] * pcs.shape[0]))
        for i, pc in enumerate(pcs):
            for j, sp in enumerate(sps):
                pc = round(pc, 2)
                sp = round(sp, 2) if sp < 2.0 else round(sp, 0)
                pbar.set_description(f'pc: {pc}, sp: {sp}')

                min_fitness_list = Parallel(n_jobs=args.n_repeat)(
                    delayed(run_ga)(args.prob_type, args.data_path, args.n_pop, args.max_gen, pc, args.pm, sp, args.xo_type) 
                    for _ in range(args.n_repeat)
                )

                fitnesses[i][j] = np.mean(min_fitness_list)

                pbar.update(1)

        print(f'fitness: {fitnesses}')

        # Save fitness results to file
        np.savetxt(f'./out/{args.output_file}_fitness.txt', fitnesses, fmt='%f')

    # Plot control map
    thres = int(699 * 1.14)
    # plot_cmap(args.n_ell, args.n_pop, pcs, sps, fitnesses, thres)
    plot_map(pcs, sps, fitnesses, savename=args.output_file)
    plot_threshold_map(pcs, sps, fitnesses, threshold=thres, savename=args.output_file)

