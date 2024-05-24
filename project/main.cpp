/*
	GA project for TSP, 2024 Spring
*/

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include "genetic.h"
using namespace std;


int main(int argc, char * argv[]) {

	// Define and read arguments
	problemType pType = problemType::TSP;	    // pType: 0 for TSP, 1 for PFSP
	string dataPath = "./data/dantzig42_d.txt";
	int n_pop = 2500;
	int maxGen = 1000;
	double pc = 0.9;
	double pm = 0.1;
	double selectionPressure = 2.0;
	crossoverType xoType = crossoverType::OX;	// xoType: 0 for Order crossover, 1 for Frequency crossover
	
	if (argc > 1 && argc == 9) {
		pType = (problemType)atoi(argv[1]);
		dataPath = argv[2];
		n_pop = atoi(argv[3]);
		maxGen = atoi(argv[4]);
		pc = atof(argv[5]);
		pm = atof(argv[6]);
		selectionPressure = atof(argv[7]);
		xoType = (crossoverType)atoi(argv[8]);
	}
	else if (argc > 1) {
		cout << "Usage: " << argv[0] << " pType dataPath n_pop maxGen pc pm selectionPressure xoType" << endl;
		cout << "pType: 0 for TSP, 1 for PFSP" << endl;
		cout << "xoType: 0 for Order crossover, 1 for Frequency crossover" << endl << endl;

		cout << "Example for TSP : " << argv[0] << " 0 ./data/dantzig42_d.txt    2500 1000 0.9 0.1 2 0" << endl;
		cout << "Example for PFSP: " << argv[0] << " 1 ./data/flowshop_j20m5.txt 2500 1000 0.9 0.1 2 0" << endl;
		exit(1);
	}


	// Read data matrix and define problem size
	int n_ell = 42;		// Number of cities for TSP, number of jobs for PFSP
	vector<vector<double>> matrix;

	matrix = inTxt(dataPath);
	n_ell = matrix.size();


	// Output the parameters
	cout << "n_ell: " << n_ell << ", n_pop: " << n_pop << ", maxGen: " << maxGen;
	cout << ", pc: " << pc << ", pm: " << pm << ", selectionPressure: " << selectionPressure << ", xoType: ";
	if (xoType == crossoverType::OX) {
		cout << "OX" << endl;
	}
	else if (xoType == crossoverType::FX) {
		cout << "FX" << endl;
	}
	else {
		cout << "Unknown" << endl;
	}


	vector<vector<int>> population;
	vector<vector<int>> offspring;
	vector<double> fitness;
	vector<int> selectionIndex;

	// Record the best route in each generation
	vector<vector<int>> mini; //(maxGen, vector<int>(n_ell));	

	// Initialize population
	population = initPopulation(n_pop, n_ell);

	// Evaluate fitness (length of route)
	for (int i = 0; i < n_pop; i++) {
		fitness.push_back(evaluateFitness(population[i], matrix, pType));
	}


	// Start GA
	for (int gen = 0; gen < maxGen; gen++) {

		// Sort the population by fitness, with both population and fitness sorted in the same way
		sortPopulationByFitness(population, fitness);

		// Check termination
		if (gen % 20 == 0) {
			if (terminate(fitness))
				break;
		}

		// Do selection
		/* tournament selection with replacement */
		selectionIndex = tournamentSelection(fitness, n_pop, n_ell, selectionPressure);

		// Do crossover
		/* xoType:	0: Order crossover,	1: Frequency crossover */
		offspring.clear();
		offspring = crossover(population, selectionIndex, n_pop, n_ell, pc, xoType);

		// Do mutation
		mutation(offspring, pm, n_ell);

		// Do elitism
		if (xoType == crossoverType::OX) {
			int eliteNb = (int)(n_pop * (1.0 / n_pop));	// 1, 2, or 10% of the population
			int eliteIndex[eliteNb];
			uniformArray(eliteIndex, eliteNb, 0, n_pop - 1);
			for (int i = 0; i < eliteNb; i++){
				offspring[eliteIndex[i]] = population[i];
			}
		}

		// Replacement
		population.clear();
		for (int i = 0; i < n_pop; i++) {
			population.push_back(offspring[i]);
		}

		// Evaluate fitness (length of route)
		fitness.clear();
		for (int i = 0; i < population.size(); i++) {
			fitness.push_back(evaluateFitness(population[i], matrix, pType));
		}

		// Record the best route in each generation
		// mini[gen] = saveMin(population, fitness);
		mini.push_back(saveMin(population, fitness));

		// Output the best route in each period of generations
		// if (gen % 20 == 0) {
		// 	cout << "Gen " << setw(3) << gen << ", ";
		// 	cout << "Fitness " << setw(5) << evaluateFitness(mini[gen], matrix, pType) << endl;
		// }

		selectionIndex.clear();
		offspring.clear();
	}

	// Output the final best route
	vector<double> finalFitness;
	string benchmark = dataPath.substr(dataPath.find_last_of("/") + 1, dataPath.find_last_of("_") - dataPath.find_last_of("/") - 1);
	string outputFile = "./out/" + benchmark + "_route.txt";
	ofstream out(outputFile, ios::out);

	for (int i = 0; i < mini.size(); i++) {
		finalFitness.push_back(evaluateFitness(mini[i], matrix, pType));
	}

	vector<double>::const_iterator fitIter = finalFitness.cbegin();
	double minFit = *fitIter;
	int index = 0;
	for (; fitIter != finalFitness.cend(); fitIter++) {
		if (*fitIter < minFit) {
			minFit = *fitIter;
			index = fitIter - finalFitness.cbegin();
		}
	}
	cout << "Minimum fitness: " << minFit << endl;
	out << minFit << endl;

	cout << "Best chromosome: ";
	for (int i = 0; i < n_ell; i++) {
		cout << mini[index][i] << " ";
		out << mini[index][i] << endl;
	}
	cout << endl;

	out.close();
	
	return 0;
}