#include <iostream>
#include <string>
#include <cmath>

using namespace std;

int main(){
    int xo_type[3] = {0, 1, 2};
    int ell[10] = {50, 100, 150, 200, 250, 300, 350, 400, 450, 500};
    int pop[10];

    int i, j;
    string cmd;

    for (i = 0; i < 10; i++){
        pop[i] = int(4 * ell[i] * log(ell[i]));
        if (pop[i] % 2 == 1){
            pop[i]++;
        }
        cout << pop[i] << endl;
    }

    // Run SGA
    for (i = 2; i < 3; i++){
        for (j = 0; j < 10; j++){
            cmd = "./SGA " + to_string(ell[j]) + " " + to_string(pop[j]);   // ell = ell[j], nInitial = pop[j]
            cmd += " 2 1.0 0.0 -1 -1 30 0 ";                                // selctionPressure = 2, pc = 0.9, pm = 0.0, 
                                                                            // maxGen = -1, maxFe = -1, repeat = 30, selectionType = 0
            cmd += to_string(xo_type[i]);                                   // crossoverType = xo_type[i]
            cmd += " > ./log_repeat30/SGA_" + to_string(ell[j]) + "_" + to_string(xo_type[i]) + ".txt";

            cout << cmd << endl;
            system(cmd.c_str());
        }
    }

    return 0;
}