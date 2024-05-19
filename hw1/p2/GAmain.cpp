/***************************************************************************
 *   Copyright (C) 2004 by Tian-Li Yu                                      *
 *   tianliyu@cc.ee.ntu.edu.tw                                             *
 *                                                                         *
 *   You can redistribute it and/or modify it as you like                  *
 ***************************************************************************/

#include <cmath>
#include <cstdio>
#include <iostream>
#include <cstdlib>

#include "statistics.h"
#include "ga.h"
#include "chromosome.h"
#include "global.h"

using namespace std;

int main (int argc, char *argv[])
{

    if (argc != 9 && argc != 11) {
        printf ("SGA ell nInitial selectionPressure pc pm maxGen maxFe repeat\n");
        printf ("Or\n");
        printf ("SGA ell nInitial selectionPressure pc pm maxGen maxFe repeat selectionType crossoverType\n");
        return -1;
    }
    
    int ell = atoi (argv[1]);    // problem size
                                 // initial population size
    int nInitial = atoi (argv[2]);
                                 // selection pressure
    int selectionPressure = atoi (argv[3]);
    double pc = atof (argv[4]);  // pc
    double pm = atof (argv[5]);  // pm
    int maxGen = atoi (argv[6]); // max generation
    int maxFe = atoi (argv[7]);  // max fe
    int repeat = atoi (argv[8]); // how many time to repeat

    if (argc == 11) {
        selectionType = (SelectionType) atoi (argv[9]);
        crossoverType = (CrossoverType) atoi (argv[10]);
    }
    else {
        selectionType = TOURNAMENT;
        crossoverType = ONEPOINT;
    }

    printf("problem size: %d\n", ell);
    printf("initial population size: %d\n", nInitial);
    printf("selection pressure: %d\n", selectionPressure);
    printf("crossover probability: %f\n", pc);
    printf("mutation probability: %f\n", pm);
    printf("max generation: %d\n", maxGen);
    printf("max number of function evaluation: %d\n", maxFe);
    printf("repeat: %d\n", repeat);

    if (selectionType == TOURNAMENT)
        printf("selection type: TOURNAMENT\n");
    else if (selectionType == ROULETTEWHEEL)
        printf("selection type: ROULETTEWHEEL\n");
    else
        outputErrMsg ("Unknown selection type");
        // printf("selection type: UNKNOWN\n");
    
    if (crossoverType == ONEPOINT)
        printf("crossover type: ONEPOINT\n");
    else if (crossoverType == UNIFORM)
        printf("crossover type: UNIFORM\n");
    else if (crossoverType == POPWISESHUFFLE)
        printf("crossover type: POPWISESHUFFLE\n");
    else
        outputErrMsg ("Unknown crossover type");
        // printf("crossover type: UNKNOWN\n");
    printf("==========================================================\n");

    int i;

    Statistics stGenS, stGenF;
    int usedGen;

    int failNum = 0;

    for (i = 0; i < repeat; i++) {

        GA ga (ell, nInitial, selectionPressure, pc, pm, maxGen, maxFe);

        if (repeat == 1)
            usedGen = ga.doIt (true);
        else
            usedGen = ga.doIt (false);
        
        Chromosome ch(ell);
        if (ga.stFitness.getMax() < ch.getMaxFitness()) {
            printf ("-");
            failNum++;
            stGenF.record (usedGen);
        }
        else {
            printf ("+");
            stGenS.record (usedGen);
        }

        fflush (NULL);

    }

    printf ("\nAverage Gen of Success: %f (%f)\n", stGenS.getMean(), stGenS.getStdev());
    printf ("Average Gen of Failure: %f (%f)\n", stGenF.getMean(), stGenF.getStdev());
    printf ("Total number of Failure: %d\n", failNum);

    return EXIT_SUCCESS;
}
