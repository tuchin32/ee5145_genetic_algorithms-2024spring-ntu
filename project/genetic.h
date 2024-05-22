#ifndef __GENETIC_H__
#define __GENETIC_H__


#include <vector>
#include <random>


using namespace std;

enum class crossoverType {
    // PMX,     // Partially Mapped Crossover
    OX,         // Order Crossover
    // CX,      // Cycle Crossover
    // EX,      // Edge Recombination Crossover
    FX          // Frequency Crossover
};

enum class problemType {
    TSP,        // Traveling Salesman Problem
    PFSP        // Permutation Flowshop Scheduling Problem
};


vector<vector<double>> inTxt(string path, int rows, int cols);
// vector<vector<double>> inTxt(string path, int cityNb);
// vector<vector<double>> inTxt(string path, int n_jobs, int n_machines);

vector<vector<int>> initPopulation(int route, int nb);

double evaluateFitness(vector<int> chromosome, vector<vector<double>> matrix, problemType pType);
double evaluateFitness(vector<int> sroute, vector<vector<double>> length, int cityNb);
double evaluateFitness(vector<int> flow, vector<vector<double>> processTime, int n_jobs, int n_machines);

vector<int> saveMin(vector<vector<int>> population, vector<double> fitness);



/* Update for GA project */

// Random number generator
double uniform();
int uniformInt(int a, int b);
void uniformArray(int *array, int num, int a, int b);

// Sorting by fitness
bool compareChromosomes(const pair<vector<int>, double>& a, const pair<vector<int>, double>& b);
void sortPopulationByFitness(vector<vector<int>>& population, vector<double>& fitness);

// Selection
vector<int> tournamentSelection(const vector<double> & fitness,
                         const int route, const int cityNb, const double selectionPressure);

// Crossover
vector<vector<int>> crossover(const vector<vector<int>> & population, const vector<int> & selectionIndex,
							  const int route, const int cityNb, const double pc, const crossoverType xoType);
void pairwiseXO(const vector<int> & p1, const vector<int> & p2, vector<int> & c1, vector<int> & c2, 
                const int cityNb, const double pc, const crossoverType xoType);
void orderXO(const vector<int> & p1, const vector<int> & p2, vector<int> & c1, vector<int> & c2,
			 const int first, const int second, const int cityNb);
void SimpleXO(const vector<int> & p1, const vector<int> & p2, vector<int> & c1, vector<int> & c2,
			  const int cutpoint, const int cityNb);
vector<vector<int>> FrequencyCrossover(const vector<vector<int>>& population, int cityNb);

// Mutation
void mutation(vector<vector<int>>& population, double pm, int n_ell);


// TODO
// class PermuteGA {
// public:
//     PermuteGA();
//     PermuteGA(string path, int n_ell, int n_pop, int n_maxGen, double pc, double pm);
//     ~PermuteGA();
    
//     void readTxt();
//     void initPop();
//     void computeFitness();
//     vector<int> saveMin();

//     void crossover();
//     void pairwiseXO (vector<int> & p1, vector<int> & p2, vector<int> & c1, vector<int> & c2);

//     void mutation ();

// protected:
//     string dataPath;
//     int n_ell;
//     int n_pop;
//     int n_maxGen;
//     double pc;
//     double pm;

//     int generation;
//     vector<vector<int>> distance;
//     vector<vector<int>> population;
//     vector<int> fitness;
// };




#endif  // __GENETIC_H__