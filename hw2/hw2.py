import os
import argparse
import numpy as np

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-p', '--population', type=str, default='./population/r11921038/popu000.txt', 
                        help='Population file')
    parser.add_argument('-o', '--output', type=str, default='./mpm/r11921038/mpm000.txt', 
                        help='Output file')
    parser.add_argument('-id', '--id', type=str, default='r11921038',
                        help='Student ID')
    parser.add_argument('-t', '--iteration', type=str, default='000',
                        help='Iteration number')
    args = parser.parse_args()

    # Print arguments
    print('Arguments:')
    # print(f'Population file: {args.population}')
    # print(f'Output file: {args.output}')
    print(f'Student ID: {args.id}')
    print(f'Iteration number: {args.iteration}')

    return args


def read_population(popufile):
    """
        Read population file (txt) and return a numpy matrix.
        Population file is a matrix of 0 and 1, with (row, col) = (population_size, problem_size).
    """
    population = []

    f = open(popufile, 'r')
    for line in f:
        line = line.strip()
        population.append([x for x in line])
    f.close()

    population = np.array(population, dtype=str)

    return population

def get_combination(m, k):
    """
        Return all possible combinations of k elements from m elements.
    """
    if k == 0:
        return [[]]
    if k == m:
        return [list(range(m))]
    if k == 1:
        return [[i] for i in range(m)]
    return [c + [m - 1] for c in get_combination(m - 1, k - 1)] + get_combination(m - 1, k)

def get_binary(m):
    """
        Return all possible binary strings of length m.
    """
    if m == 0:
        return ['']
    return ['0' + b for b in get_binary(m - 1)] + ['1' + b for b in get_binary(m - 1)]

def describe_model(bb_candidates, n_pop):
    bb_lens = np.array([len(b) for b in bb_candidates])
    model = (2 ** bb_lens - 1) * np.log2(n_pop)
    return np.sum(model)

def describe_data(bb_candidates, population, schemas=None):
    n_pop, n_ell = population.shape

    dl_data = 0
    for bb in bb_candidates:
        data = population[:, bb]
        length_bb = len(bb)
        schema = schemas[length_bb] if schemas else get_binary(length_bb)

        entropy = 0
        for s in schema:
            count = np.sum(np.all(data == list(s), axis=1))
            if count == 0:
                continue
            prob = count / n_pop
            entropy += prob * np.log2(prob)
        dl_data += (-entropy)

    dl_data = dl_data * n_pop
    return dl_data


if __name__ == '__main__':
    # Read arguments
    args = get_args()
    args.population = f'./population/{args.id}/popu{args.iteration}.txt'
    args.output = f'./mpm/{args.id}/mpm{args.iteration}.txt'
    os.makedirs(f'./mpm/{args.id}', exist_ok=True)
    # id, pop_name = args.population.split('/')[-2:]
    # args.output = f'./mpm/{id}/mpm{pop_name[4:]}'


    # Read population file
    population = read_population(args.population)
    n_pop, n_ell = population.shape
    print(population)

    
    # Prepare the schema dictionary, if needed
    # schemas = {}
    # k_max = 10    # A possible maximum order of building blocks (BBs)
    # for i in range(1, k_max):
    #     schemas[i] = get_binary(i)
    schemas = None


    # Initialize BBs
    bb = get_combination(n_ell, 1)
    d_model = describe_model(bb, n_pop)
    d_data = describe_data(bb, population, schemas)
    min_dl = d_model + d_data

    curr_ell = n_ell
    should_stop = False

    while (curr_ell > 1) and (not should_stop):
        print('\nCurrent number of modules:', curr_ell)
        
        # Calculate combinations of possible BBs
        combinations = get_combination(curr_ell, 2)
        # print('comb', len(combinations), combinations)
        
        # Evaluate each new BBs
        new_bb_candidates = []
        new_bb_dls = []
        for c in combinations:
            # Merge two groups
            new_bb = []
            merge = []
            for i in c:
                # print(bb[i])
                merge += bb[i]
            new_bb.append(merge)

            for i in range(curr_ell):
                if i not in c:
                    new_bb.append(bb[i])

            # Evaluate the description length of the model
            dl_model = describe_model(new_bb, n_pop)

            # Evaluate the description length of the data
            dl_data = describe_data(new_bb, population, schemas)

            # print('new_bb', new_bb)
            # print('dl_model', dl_model)
            # print('dl_data', dl_data)
            
            new_bb_candidates.append(new_bb)
            new_bb_dls.append(dl_model + dl_data)

        # Update BBs (choose the best one)
        idx = np.argmin(new_bb_dls)
        print('new_bb_dls', new_bb_dls[idx], '\tmin_dl', min_dl)
        if new_bb_dls[idx] <= min_dl:
            bb = new_bb_candidates[idx]
            min_dl = new_bb_dls[idx]
        else:
            should_stop = True

        curr_ell -= 1

    print('Best BB:', bb)
    print('Min DL:', min_dl)

    # Write to output file
    record = f'{len(bb)}\n'
    for b in bb:
        record += f'{len(b)} {" ".join([str(x) for x in b])}\n'
    with open(args.output, 'w') as f:
        f.write(record)

