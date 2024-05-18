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

	problemType pType = problemType::TSP;	    // pType: 0 for TSP, 1 for PFSP
	string dataPath = "./data/dantzig42_d.txt";
	int n_pop = 1000;
	int maxGen = 1000;
	double pc = 1.0;
	double selectionPressure = 2.0;
	crossoverType xoType = crossoverType::OX;	// xoType: 0 for Order crossover, 1 for Frequency crossover
	
	int n_ell = 42;
	int n_cities = 42;
	int n_jobs = 20;
	int n_machines = 5;


	// Read arguments
	if (argc > 1 && argc == 9 && atoi(argv[1]) == 0) {
		// TSP
		pType = (problemType)atoi(argv[1]);
		dataPath = argv[2];
		n_cities = atoi(argv[3]);
		n_pop = atoi(argv[4]);
		maxGen = atoi(argv[5]);
		pc = atof(argv[6]);
		selectionPressure = atof(argv[7]);
		xoType = (crossoverType)atoi(argv[8]);
		
		n_ell = n_cities;
	}
	else if (argc > 1 && argc == 10 && atoi(argv[1]) == 1) {
		// PFSP
		pType = (problemType)atoi(argv[1]);
		dataPath = argv[2];
		n_jobs = atoi(argv[3]);
		n_machines = atoi(argv[4]);
		n_pop = atoi(argv[5]);
		maxGen = atoi(argv[6]);
		pc = atof(argv[7]);
		selectionPressure = atof(argv[8]);
		xoType = (crossoverType)atoi(argv[9]);

		n_ell = n_jobs;
	}
	else if (argc > 1) {
		cout << "TSP: 9 arguments, PFSP: 10 arguments\n";
		cout << "pType: 0 for TSP, 1 for PFSP" << endl;
		cout << "xoType: 0 for Order crossover, 1 for Frequency crossover" << endl << endl;

		cout << "Usage for TSP: " << argv[0] << " pType dataPath n_cities n_pop maxGen pc selectionPressure xoType" << endl;
		cout << "Example: " << argv[0] << " 0 ./data/dantzig42_d.txt 42 1000 1000 1.0 2 0" << endl << endl;
		cout << "Usage for PFSP: " << argv[0] << " pType dataPath n_jobs n_machines n_pop maxGen pc selectionPressure xoType" << endl;
		cout << "Example: " << argv[0] << " 1 ./data/flowshop_j20m5.txt 20 5 1000 1000 1.0 2 0" << endl;
		exit(1);
	}

	cout << "n_ell: " << n_ell << ", n_pop: " << n_pop << ", maxGen: " << maxGen;
	cout << ", pc: " << pc << ", selectionPressure: " << selectionPressure << ", xoType: ";
	if (xoType == crossoverType::OX) {
		cout << "OX" << endl;
	}
	else if (xoType == crossoverType::FX) {
		cout << "FX" << endl;
	}
	else {
		cout << "Unknown" << endl;
	}


	vector<vector<double>> matrix;
	vector<vector<int>> population;
	vector<double> fitness;

	vector<int> selectionIndex;
	vector<vector<int>> offspring;

	// Record the best route in each generation
	vector<vector<int>> mini(maxGen, vector<int>(n_ell));	

	// Read distance matrix
	if (pType == problemType::TSP) {
		matrix = inTxt(dataPath, n_cities, n_cities);
	}
	else if (pType == problemType::PFSP) {
		matrix = inTxt(dataPath, n_jobs, n_machines);
	}

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

		// Do selection
		/* tournament selection with replacement */
		selectionIndex = tournamentSelection(fitness, n_pop, n_ell, selectionPressure);

		// Do crossover
		/* xoType:	0: Order crossover,	1: Frequency crossover */
		offspring.clear();
		offspring = crossover(population, selectionIndex, n_pop, n_ell, pc, xoType);

		// Do elitism
		if (xoType == crossoverType::OX) {
			int eliteNb = (int)(n_pop / 10);	// 1, 2, or 10% of the population
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
		mini[gen] = saveMin(population, fitness);

		// Output the best route in each period of generations
		if (gen % 20 == 0) {
			cout << "Gen " << setw(3) << gen << ", ";
			cout << "Fitness " << setw(5) << evaluateFitness(mini[gen], matrix, pType) << endl;
		}

		selectionIndex.clear();
		offspring.clear();
	}

	// Output the final best route
	vector<double> finalFitness;
	string benchmark = dataPath.substr(dataPath.find_last_of("/") + 1, dataPath.find_last_of("_") - dataPath.find_last_of("/") - 1);
	string outputFile = "./out/" + benchmark + "_route.txt";
	ofstream out(outputFile, ios::out);

	for (int i = 0; i < maxGen; i++) {
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