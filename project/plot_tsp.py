import argparse
import matplotlib.pyplot as plt

if __name__ == '__main__':
    # Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('--route_path', '-r', type=str, default='./out/eil51_route.txt')
    parser.add_argument('--xy_path', '-xy', type=str, default='./data/eil51_xy.txt')
    parser.add_argument('--plot_path', '-p', type=str, default='./image/eil51_route.png')
    args = parser.parse_args()

    args.plot_path = f'./image/{args.route_path.split("/")[-1].split("_")[0]}_route.png'

    # Read route
    with open(args.route_path) as f:
        route = f.readlines()
    length = float(route[0])
    route = [int(city) for city in route[1:]]
    tsp_name = args.route_path.split('/')[-1].split('_')[0]

    # Read cities coordinates
    # Format: [node, x, y], e.g. 1 170.0 85.0
    with open(args.xy_path) as f:
        cities = f.readlines()
    cities = [city.split() for city in cities]
    cities = [[int(city[0]), float(city[1]), float(city[2])] for city in cities]

    # Plot the route
    plt.figure(figsize=(10, 10))
    plt.scatter([city[1] for city in cities], [city[2] for city in cities], c='b')
    for i, city in enumerate(cities):
        plt.annotate(city[0], (city[1], city[2]), color='r', fontsize=12)
    for i in range(len(route)-1):
        plt.plot([cities[route[i]][1], cities[route[i+1]][1]], [cities[route[i]][2], cities[route[i+1]][2]], 'r')
    plt.plot([cities[route[-1]][1], cities[route[0]][1]], [cities[route[-1]][2], cities[route[0]][2]], 'r')
    plt.title(f'TSP: {tsp_name}, Distance: {length:.2f}')
    plt.savefig(args.plot_path, bbox_inches='tight', dpi=300)

