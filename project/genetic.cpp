#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include <algorithm>

#include "genetic.h"

using namespace std;
using std::vector;

vector<vector<double>> inTxt(string path, int rows, int cols) {
	/*
		1. TSP
		    - matrix: distance between cities
			- rows: number of cities
			- cols: number of cities
		2. PFSP
			- matrix: processing time of jobs on machines
			- rows: number of jobs
			- cols: number of machines
	*/

	ifstream infile(path);

	string line;
	vector<vector<double>> matrix(rows, vector<double>(cols));

	int lineCount = 0;
	double element = 0.0;

	while (getline(infile, line)) {

		int nb = 0;

		istringstream iss(line);
		while (iss >> element && nb < cols) {

			matrix[lineCount][nb] = double(element);
			nb = nb + 1;

		}

		lineCount = lineCount + 1;

	}

	return matrix;
}


// vector<vector<double>> inTxt(string path, int cityNb) {

// 	ifstream infile(path);

// 	string line;
// 	vector<vector<double>> distance(cityNb, vector<double>(cityNb));

// 	int lineCount = 0;
// 	double tempDis = 0.0;


// 	while (getline(infile, line)) {

// 		int nb = 0;

// 		istringstream iss(line);
// 		while (iss >> tempDis && nb < cityNb) {

// 			distance[lineCount][nb] = double(tempDis);
// 			nb = nb + 1;

// 		}

// 		lineCount = lineCount + 1;

// 	}

// 	return distance;
// }

// vector<vector<double>> inTxt(string path, int n_jobs, int n_machines) {

// 	ifstream infile(path);

// 	string line;
// 	vector<vector<double>> processTime(n_jobs, vector<double>(n_machines));

// 	int lineCount = 0;
// 	double element = 0.0;

// 	while (getline(infile, line)) {

// 		int nb = 0;

// 		istringstream iss(line);
// 		while (iss >> element && nb < n_machines) {

// 			processTime[lineCount][nb] = double(element);
// 			nb = nb + 1;

// 		}

// 		lineCount = lineCount + 1;

// 	}

// 	return processTime;
// }

vector<vector<int>> initPopulation(int route, int nb) {

	vector<vector<int>> population(route, vector<int>(nb));

	default_random_engine generator(std::random_device{}());

	int city;
	vector<int> place;
	for (int i = 0; i < route; i++) {

		for (int k = 0; k < nb; k++) {
			place.push_back(k);
		}

		int s = nb;
		for (int j = 0; j < nb; j++) {


			uniform_int_distribution<int> distribution(0, s - 1);

			city = distribution(generator);
			population[i][j] = place[city];
			
			int temp = place[s - 1];
			place[city] = temp;
			place.pop_back();
			s = s - 1;
			
		}
		place.clear();
	}
	
	return population;
}

double evaluateFitness(vector<int> chromosome, vector<vector<double>> matrix, problemType pType) {
	/*
		1. TSP
			- chromosome: the sequence of cities
			- matrix: distance between cities
		2. PFSP
			- chromosome: the sequence of jobs
			- matrix: processing time of jobs on machines
	*/

	if (pType == problemType::TSP) {
		int n_cities = matrix.size();
		return evaluateFitness(chromosome, matrix, n_cities);
	}
	else if (pType == problemType::PFSP) {
		int n_jobs = matrix.size();
		int n_machines = matrix[0].size();
		return evaluateFitness(chromosome, matrix, n_jobs, n_machines);
	}
	else {
		cout << "Problem type not supported." << endl;
		exit(1);
	}
}

double evaluateFitness(vector<int> sroute, vector<vector<double>> length, int cityNb) {

	double routeLength = 0;
	for (int i = 1; i < cityNb; i++) {
		routeLength += length[sroute[i - 1]][sroute[i]];
	}
	routeLength += length[sroute[cityNb - 1]][sroute[0]];

	return routeLength;
}

double evaluateFitness(vector<int> flow, vector<vector<double>> processTime, int n_jobs, int n_machines) {
	/*
		Flow shop scheduling problem
		- n_jobs: number of jobs
		- n_machines: number of machines
		- flow: the sequence of jobs
		- processTime: processing time of each job on each machine
	
		Reference: Bo-Wei Huang, Improving EHBSA with Relational Decomposition and Entropy-aware Sampling, p.42, http://dx.doi.org/10.6342/NTU202303061
	*/

	vector<vector<double>> completion(n_jobs, vector<double>(n_machines, 0));
	double makespan = 0;


	// Dynamic programming for completion time
	for (int i = 0; i < n_jobs; i++) {
		for (int j = 0; j < n_machines; j++) {
			if (i == 0 && j == 0) {
				completion[i][j] = processTime[flow[i]][j];
			}
			else if (i == 0 && j > 0) {
				completion[i][j] = completion[i][j - 1] + processTime[flow[i]][j];
			}
			else if (i > 0 && j == 0) {
				completion[i][j] = completion[i - 1][j] + processTime[flow[i]][j];
			}
			else {
				completion[i][j] = max(completion[i - 1][j], completion[i][j - 1]) + processTime[flow[i]][j];
			}
		}
	}

	/*
		Different fitness functions can be used here. For example:
		1. Makespan: the completion time of the last job on the last machine
		2. Total completion time of all jobs on the last machine
		3. Total completion time of all jobs on all machines
		4. Total idle time of all machines
	*/
	
	makespan = completion[n_jobs - 1][n_machines - 1];
	// for (int i = 0; i < n_machines; i++) {
	// 	makespan += completion[n_jobs - 1][i];
	// }

	return makespan;
}

vector<double> choseRange(vector<double> fitness, int ep) {

	vector<double>::const_iterator fitIter = fitness.cbegin();
	int minFit = *fitIter;
	for (; fitIter != fitness.cend(); fitIter++) {
		if (*fitIter < minFit) {
			minFit = *fitIter;
		}
	}

	//cout << "Minimun distance: " << minFit << endl;

	vector<double> trueFitness;
	double total = 0;
	for (fitIter = fitness.cbegin(); fitIter != fitness.cend(); fitIter++) {
		double ratio = double(*fitIter) / double(minFit);
		double exponent = -pow(ratio, ep);
		total += exp(exponent);
		trueFitness.push_back(total);
	}

	vector<double> range(fitness.size());
	for (int i = 0; i < (int)fitness.size(); i++) {
		range[i] = trueFitness[i] / total;
	}

	return range;
}

vector<int> inheritance(vector<int> father, vector<int> mother) {

	if (father.size() != mother.size()) {
		cout << "Fuck! Length is different." << endl;
	}

	int s = father.size();
	default_random_engine generator(std::random_device{}());
	uniform_int_distribution<int> distribution(0, s - 1);

	int p1, p2;
	p1 = distribution(generator);
	p2 = distribution(generator);

	while (p1 == p2) {
		p2 = distribution(generator);
	}

	if (p1 > p2) {
		int temp = p1;
		p1 = p2;
		p2 = temp;
	}

	vector<int> child(s);

	for (int i = p1; i <= p2; i++) {
		for (int j = 0; j < (int)mother.size(); j++) {
			if (mother[j] == father[i]) {
				mother.erase(mother.begin() + j);
				break;
			}
		}
	}

	vector<int>::const_iterator mIter = mother.cbegin();
	for (int i = 0; i < s; i++) {
		if (i >= p1 && i <= p2) {
			child[i] = father[i];
		}
		else {
			child[i] = *mIter;
			if (mIter != mother.cend()) {
				mIter++;
			}
		}

	}

	//cout << "p1: " << p1 << endl;
	//cout << "p2: " << p2 << endl;

	return child;
}

vector<vector<int>> mutate(vector<vector<int>> population, vector<double> range, vector<double> fitness) {

	int routeNb = population.size();

	default_random_engine generator(std::random_device{}());

	vector<vector<int>> childGen;
	uniform_real_distribution<double> distribution(0, 1);
	for (int i = 0; i < routeNb; i++) {
		double randNb;
		randNb = distribution(generator);

		int parFather;
		for (int j = 1; j < range.size(); j++){
			if (randNb <= range[0]) {
				parFather = 0;
				break;
			}
			else if (randNb > range[j - 1] && randNb <= range[j]){
				parFather = j;
				break;
			}
		}

		randNb = distribution(generator);

		int parMother;
		for (int j = 1; j < range.size(); j++){
			if (randNb <= range[0]) {
				parMother = 0;
				break;
			}
			else if (randNb > range[j - 1] && randNb <= range[j]){
				parMother = j;
				break;
			}
		}


		while (parFather == parMother){
			randNb = distribution(generator);
			for (int j = 1; j < range.size(); j++){
				if (randNb <= range[0]) {
					parMother = 0;
					break;
				}
				else if (randNb > range[j - 1] && randNb <= range[j]){
					parMother = j;
					break;
				}
			}
		}

		vector<int> child = inheritance(population[parFather], population[parMother]);
		childGen.push_back(child);

	}

	vector<double>::const_iterator fitIter = fitness.cbegin();
	double minFit = *fitIter;
	int index = 0;
	for (; fitIter != fitness.cend(); fitIter++) {
		if (*fitIter < minFit) {
			minFit = *fitIter;
			index = fitIter - fitness.cbegin();
		}
	}
	childGen.push_back(population[index]);

	return childGen;
}

vector<vector<int>> change(vector<vector<int>> population, double changeRate, int cityNb) {

	default_random_engine generator(std::random_device{}());
	uniform_real_distribution<double> dis(0, 1);
	uniform_int_distribution<int> disInt(0, cityNb - 1);

	for (int i = 0; i < population.size(); i++) {
		double r = dis(generator);
		if (r >= changeRate) {
			int first, second;
			first = disInt(generator);
			second = disInt(generator);

			while (first == second) {
				second = disInt(generator);
			}

			int temp = population[i][first];
			population[i][first] = population[i][second];
			population[i][second] = temp;
		}
	}

	return population;
}

vector<int> saveMin(vector<vector<int>> population, vector<double> fitness) {

	vector<double>::const_iterator fitIter = fitness.cbegin();
	double minFit = *fitIter;
	int index = 0;
	for (; fitIter != fitness.cend(); fitIter++) {
		if (*fitIter < minFit) {
			minFit = *fitIter;
			index = fitIter - fitness.cbegin();
		}
	}

	return population[index];
}



/* Update for GA project */


double uniform() {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen);
}

int uniformInt(int a, int b) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(a, b);
    return dis(gen);
}

void uniformArray(int *array, int num, int a, int b) {
    int *base = new int[b - a + 1];
    int i;
    int r;

    for (i = 0; i < b - a + 1; i++)
        base[i] = a + i;

    for (i = 0; i < num; i++) {
        r = uniformInt(0, b - a - i);
        array[i] = base[r];
        base[r] = base[b - a - i];
    }

    delete[] base;
}


bool compareChromosomes(const pair<vector<int>, double>& a, const pair<vector<int>, double>& b) {
    return a.second < b.second;  // the lower the better
}

void sortPopulationByFitness(vector<vector<int>>& population, vector<double>& fitness) {
    vector<pair<vector<int>, double>> popWithFitness;

    for (size_t i = 0; i < population.size(); ++i) {
        popWithFitness.push_back(make_pair(population[i], fitness[i]));
    }

    sort(popWithFitness.begin(), popWithFitness.end(), compareChromosomes);

    for (size_t i = 0; i < population.size(); ++i) {
        population[i] = popWithFitness[i].first;
        fitness[i] = popWithFitness[i].second;
    }
}


vector<int> tournamentSelection(const vector<double> & fitness,
                         const int route, const int cityNb, const double selectionPressure) {
    int i, j;
	int intSelectionPressure = ceil(selectionPressure);
	double pwinner;
	vector<int> selectionIndex;

	if (intSelectionPressure == int(selectionPressure)) {
		// integer selection pressure -> deterministic to choose the winner
		pwinner = 1.0;
	}
	else if (1.0 < selectionPressure && selectionPressure < 2.0) {
		// non-integer selection pressure -> choose the winner with pwinner, and the loser with 1 - pwinner
		pwinner = selectionPressure / 2.0;
	}
	else {
		cout << "Selection pressure not supported." << endl;
		exit(1);
	}

    // int randArray[intSelectionPressure * route];
    // for (i = 0; i < intSelectionPressure; i++){
    //     uniformArray (randArray + (i * route), route, 0, route - 1);
	// }

    for (i = 0; i < route; i++) {

        int winner = 0;
        double winnerFitness = -10000000.0;

		// With replacement
		int rand_arr[intSelectionPressure];
		uniformArray(rand_arr, intSelectionPressure, 0, route - 1);

        for (j = 0; j < intSelectionPressure; j++) {
            // int challenger = randArray[intSelectionPressure * i + j];
			int challenger = rand_arr[j];
			double challengerFitness = fitness[challenger];

            if (challengerFitness > winnerFitness) {
				if (uniform() < pwinner || winnerFitness < -999999.0) {
					winner = challenger;
					winnerFitness = challengerFitness;
				}
            }

        }
        selectionIndex.push_back(winner);
    }
	return selectionIndex;
}


vector<vector<int>> crossover(const vector<vector<int>> & population, const vector<int> & selectionIndex,
							  const int route, const int cityNb, const double pc, const crossoverType xoType) {

	int i;
	vector<vector<int>> offspring(route, vector<int>(cityNb));

	if (xoType == crossoverType::OX) {
		// Pairwise crossover
		if ((route & 0x1) == 0) { 
			// Number of routes is even
			for (i = 0; i < route; i += 2)
				pairwiseXO (population[selectionIndex[i]], population[selectionIndex[i + 1]],
					offspring[i], offspring[i + 1], cityNb, pc, xoType);

		}
		else {
			for (i = 0; i < route - 1; i += 2) {
				pairwiseXO (population[selectionIndex[i]], population[selectionIndex[i + 1]],
					offspring[i], offspring[i + 1], cityNb, pc, xoType);
			}
			offspring[route - 1] =
				population[selectionIndex[route - 1]];
		}
	}
	else if (xoType == crossoverType::FX) {
		// Population-wise crossover
		offspring = FrequencyCrossover(population, cityNb);
	}
	else {
		cout << "Crossover type not supported." << endl;
	}


	return offspring;

}

void pairwiseXO(const vector<int> & p1, const vector<int> & p2, vector<int> & c1, vector<int> & c2, 
                const int cityNb, const double pc, const crossoverType xoType) {

	// default_random_engine generator(std::random_device{}());
	// uniform_real_distribution<double> dis(0, 1);
	// uniform_int_distribution<int> disInt(0, cityNb - 1);

	int first, second;

	if (uniform() < pc) {
		// Do crossover

		// Find a random segment (first < second)
		// first = disInt(generator);
		// second = disInt(generator);
		first = uniformInt(0, cityNb - 1);
		// first = 0;
		second = uniformInt(0, cityNb - 1);

		while (first == second) {
			second = uniformInt(0, cityNb - 1);
		}

		if (first > second) {
			int temp = first;
			first = second;
			second = temp;
		}

		if (xoType == crossoverType::OX) {
			// Order crossover
			orderXO(p1, p2, c1, c2, first, second, cityNb);
		}
	}
	else {
		// Do reproduction
		c1.clear();
		c2.clear();
		c1 = p1;
		c2 = p2;
	}

	// // Visualize the process
	// cout << "========= After crossover =========" << endl;
	// int i;
	// cout << "First: " << first << endl;
	// cout << "Second: " << second << endl;
	// cout << "Parent 1: ";
	// for (i = 0; i < cityNb; i++) {
	// 	cout << p1[i] << " ";
	// }
	// cout << endl;
	// cout << "Parent 2: ";
	// for (i = 0; i < cityNb; i++) {
	// 	cout << p2[i] << " ";
	// }
	// cout << endl;
	// cout << "Child 1: ";
	// for (i = 0; i < cityNb; i++) {
	// 	cout << c1[i] << " ";
	// }
	// cout << endl;
	// cout << "Child 2: ";
	// for (i = 0; i < cityNb; i++) {
	// 	cout << c2[i] << " ";
	// }
	// cout << endl << endl;

}

void orderXO(const vector<int> & p1, const vector<int> & p2, vector<int> & c1, vector<int> & c2,
			 const int first, const int second, const int cityNb) {

	int i, j, item;
	vector<int>::iterator iter;

	// 1. Copy a random segment from its parent
	for (i = 0; i < cityNb; i++) {
		if (i >= first && i <= second) {
			c1[i] = p1[i];
			c2[i] = p2[i];
		}
		else {
			c1[i] = -1;
			c2[i] = -1;
		}
	}

	// 2. Fill rest alleles based on to the other parent from the 2nd cross site
	// Pair: c1 -- p2
	i = second + 1;
	// i = 0;
	j = second + 1;
	// j = 0;
	while (i != first) {
		// cout << "i: " << i << " j: " << j << endl;
		if (i == cityNb)
			i = 0;
		if (j == cityNb)
			j = 0;
		// p2[j] does not exist in c1 -> assign value
		if (find(c1.begin(), c1.end(), p2[j]) == c1.end()) {
			c1[i] = p2[j];
			i++;
		}
		j++;
	}

	// Pair: c2 -- p1
	i = second + 1;
	// i = 0;
	j = second + 1;
	// j = 0;
	while (i != first) {
		if (i == cityNb)
			i = 0;
		if (j == cityNb)
			j = 0;

		// p1[j] does not exist in c2 -> assign value
		if (find(c2.begin(), c2.end(), p1[j]) == c2.end()) {
			c2[i] = p1[j];
			i++;
		}
		j++;
	}


	// // Visualize the process
	// cout << "========= After orderXO =========" << endl;
	// cout << "First: " << first << endl;
	// cout << "Second: " << second << endl;
	// cout << "Parent 1: ";
	// for (i = 0; i < cityNb; i++) {
	// 	cout << p1[i] << " ";
	// }
	// cout << endl;
	// cout << "Parent 2: ";
	// for (i = 0; i < cityNb; i++) {
	// 	cout << p2[i] << " ";
	// }
	// cout << endl;
	// cout << "Child 1: ";
	// for (i = 0; i < cityNb; i++) {
	// 	cout << c1[i] << " ";
	// }
	// cout << endl;
	// cout << "Child 2: ";
	// for (i = 0; i < cityNb; i++) {
	// 	cout << c2[i] << " ";
	// }
	// cout << endl << endl;

	// // Validate that the childs are permutation
	// vector<int> check(cityNb);
	// // Check c1
	// for (i = 0; i < cityNb; i++) {
	// 	check[i] = 0;
	// }
	// for (i = 0; i < cityNb; i++) {
	// 	check[c1[i]]++;
	// }
	// for (i = 0; i < cityNb; i++) {
	// 	if (check[i] != 1) {
	// 		cout << "Child 1 is not a permutation." << endl;
	// 	}
	// }
	// // Check c2
	// for (i = 0; i < cityNb; i++) {
	// 	check[i] = 0;
	// }
	// for (i = 0; i < cityNb; i++) {
	// 	check[c2[i]]++;
	// }
	// for (i = 0; i < cityNb; i++) {
	// 	if (check[i] != 1) {
	// 		cout << "Child 2 is not a permutation." << endl;
	// 	}
	// }
	

}

vector<vector<int>> FrequencyCrossover(const vector<vector<int>>& population, int cityNb) {
    int Y = population.size();
    int crossoverGroupSize = Y / 2;

    vector<vector<int>> newGeneration;

	// (Y/2) populations // Question: How to keep the population size?????
	for (int i = 0; i < crossoverGroupSize+1; i++) { // crossoverGroupSize+1?????
        newGeneration.push_back(population[i]);
    }

	// (Y/2 - 1) populations
    vector<int> bestChromosome = population[0];
	for (int i = 1; i < crossoverGroupSize; i++) {
        vector<int> offspring(cityNb);
        vector<int> otherChromosome = population[i];

        for (int geneIndex = 0; geneIndex < cityNb; geneIndex++) {
            if (bestChromosome[geneIndex] == otherChromosome[geneIndex]) {
                offspring[geneIndex] = bestChromosome[geneIndex];
            } else {
                offspring[geneIndex] = -1;
            }
        }

        for (int geneIndex = 0; geneIndex < cityNb; geneIndex++) {
            if (offspring[geneIndex] == -1) {
                vector<int> availableCities;
                for (int city = 0; city < cityNb; city++) {
                    if (find(offspring.begin(), offspring.end(), city) == offspring.end()) {
                        availableCities.push_back(city);
                    }
                }
                random_shuffle(availableCities.begin(), availableCities.end());
                offspring[geneIndex] = availableCities[0];
            }
        }

        newGeneration.push_back(offspring);
    }

	// cout << "population size: " << newGeneration.size() << endl;
    return newGeneration;
}

