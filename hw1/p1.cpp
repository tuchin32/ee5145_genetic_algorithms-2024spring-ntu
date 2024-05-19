/*
    Consider the case with l genes, randomly assign the fitness values for each chromosome.
    Then, test whether the deception occurs.
*/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <random>

using namespace std;

void generate_binary(int n, vector<string> &v);
void generate_schema(int n, vector<string> &v);
void compute_schema_fitness(map<string, double> &h_fit, map<string, double> &fit);
bool ckeck_deception(int n, map<string, double> &h_fit, map<string, double> &fit);

void generate_binary(int n, vector<string> &v) {
    if (n == 1) {
        v.push_back("0");
        v.push_back("1");
        // cout << "n = 1" << endl;
        // for (int i = 0; i < v.size(); i++) {
        //     cout << v[i] << endl;
        // }
        return;
    }
    generate_binary(n - 1, v);
    vector<string> temp;
    for (int i = 0; i < v.size(); i++) {
        temp.push_back(v[i] + "0");
        temp.push_back(v[i] + "1");
    }
    v = temp;
    // cout << "n = " << n << endl;
    // for (int i = 0; i < v.size(); i++) {
    //     cout << v[i] << endl;
    // }
}

void generate_schema(int n, vector<string> &v) {
    if (n == 1)
        return;

    vector <string> temp;
    for (int i = 1; i < n; i++){
        // cout << "i = " << i << endl;
        generate_binary(i, temp);
        for (int j = 0; j < temp.size(); j++) {
            string star = temp[j];
            for (int k = i; k < n; k++) {
                star += "*";
            }

            sort(star.begin(), star.end());

            if (find(v.begin(), v.end(), star) == v.end())
                v.push_back(star);
        }
        temp.clear();
    }

    // Run permutation
    for (int i = 0; i < v.size(); i++) {
        string s = v[i];
        do {
            temp.push_back(s);
        } while (next_permutation(s.begin(), s.end()));
    }

    v = temp;

    // // Print the schema
    // for (int i = 0; i < v.size(); i++) {
    //     cout << v[i] << endl;
    // }
}

void compute_schema_fitness(map<string, double> &h_fit, map<string, double> &fit) {
    // for (int i = 0; i < v.size(); i++) {
    for (map<string, double>::iterator it = h_fit.begin(); it != h_fit.end(); it++) {
        // string s = v[i];
        string s = it->first;

        // Count the number of stars
        vector<int> idx_star;
        for (int j = 0; j < s.size(); j++) {
            if (s[j] == '*')
                idx_star.push_back(j);
        }

        // Complete the schema
        vector<string> tmp_chromo;
        generate_binary(idx_star.size(), tmp_chromo);

        double sum = 0.0;
        string curr = "";
        for (int j = 0; j < tmp_chromo.size(); j++) {
            curr = s;
            for (int k = 0; k < tmp_chromo[j].size(); k++) {
                curr[idx_star[k]] = tmp_chromo[j][k];
            }
            sum += fit[curr];
        }
        sum /= tmp_chromo.size();
        h_fit[s] = sum;
        // cout << "Schema: " << it->first << " Fitness: " << it->second << endl;
    }
}

bool ckeck_deception(int n, map<string, double> &h_fit, map<string, double> &fit) {
    // Find the greatest fitness and the corresponding chormosome
    double max_fit = 0.0;
    string max_chromo = "";
    for (map<string, double>::iterator it = fit.begin(); it != fit.end(); it++) {
        if (it->second > max_fit) {
            max_fit = it->second;
            max_chromo = it->first;
        }
    }
    // cout << "Max fitness: " << max_fit << " Chormosome: " << max_chromo << endl;

    // Complemet the chormosome
    string comp_chromo = "";
    for (int i = 0; i < max_chromo.size(); i++) {
        if (max_chromo[i] == '0')
            comp_chromo += "1";
        else
            comp_chromo += "0";
    }

    // cout << "Complement chormosome: " << comp_chromo << endl;

    // Generate schema from the complement chormosome
    // -> # + * = n, 1 <= * <= n - 1 -> Generate all possible schema
    vector<string> schema;
    string s, temp;
    for (int i = 1; i < n; i++) {
        s = "";
        for (int j = 0; j < i; j++) {
            s += "#";
        }
        for (int j = i; j < n; j++) {
            s += "*";
        }

        // Permutation
        sort(s.begin(), s.end());
        do {
            temp = s;
            for (int j = 0; j < temp.size(); j++) {
                if (temp[j] == '#')
                    temp[j] = comp_chromo[j];
            }
            schema.push_back(temp);
        } while (next_permutation(s.begin(), s.end()));
        
    }

    // // Print the schema
    // for (int i = 0; i < schema.size(); i++) {
    //     cout << schema[i] << endl;
    // }

    bool is_deception = true;
    for (int i = 0; i < schema.size(); i++) {
        s = schema[i];

        map<string, double> curr;
        for (map<string, double>::iterator it = h_fit.begin(); it != h_fit.end(); it++) {
            string chromo = it->first;
            if (s == chromo)
                continue;

            bool is_candidate = true;
            for (int j = 0; j < s.size(); j++) {
                if (s[j] == '*' && chromo[j] != '*')
                    is_candidate = false;
                else if (s[j] != '*' && chromo[j] == '*')
                    is_candidate = false;
            }

            if (is_candidate)
                curr[chromo] = it->second;
            
        }

        // cout << "curr size: " << curr.size() << endl;

        // string cp;
        for (map<string, double>::iterator it = curr.begin(); it != curr.end(); it++) {
            // cp = (h_fit[s] > it->second) ? ">" : "<=";
            // cout << "Comp: (" << s << ", " << h_fit[s] << ")  " << cp;
            // cout << "  Schema: (" << it->first << ", " << it->second << ")" << endl;
            if (h_fit[s] <= it->second) {
                is_deception = false;
                break;
            }
        }

        curr.clear();

        if (!is_deception)
            break;
    }

    

    return is_deception;
}


int main(int argc, char *argv[]) {

    int ell = 3;
    int num_trials = 1000000;
    vector<double> exf{28.0, 26.0, 22.0, 0.0, 14.0, 0.0, 0.0, 30.0};

    // Read the input
    if (argc > 1) {
        ell = atoi(argv[1]);
    }
    if (argc > 2) {
        num_trials = atoi(argv[2]);
    }

    vector<string> chormosome;
    map<string, double> fitness;
    vector<string> schema;
    map<string, double> schema_fitness;

    // Random number generator and distribution
    std::default_random_engine generator( time(NULL) );
    std::uniform_real_distribution<float> unif(0.0, 1.0);

    // Trial log
    vector<bool> trial_log(num_trials, false);
    double deception_rate = 0.0;

    generate_binary(ell, chormosome);

    // cout << "End of the function" << endl;
    // for (int i = 0; i < chormosome.size(); i++) {
    //     cout << chormosome[i] << endl;
    // }
    generate_schema(ell, schema);


    for (int t = 0; t < num_trials; t++) {
        if (t % 10000 == 0)
            cout << "Trial: " << t << endl;


        // cout << "====================" << endl;
        // cout << "Fitness values for each chromosome" << endl;
        for (int i = 0; i < chormosome.size(); i++) {
            // Uniform random number between 0 and 1
            fitness[chormosome[i]] = unif(generator);
            // fitness[chormosome[i]] = exf[i];
            // cout << "fitness[" << chormosome[i] << "]: " << fitness[chormosome[i]] << endl;
        }



        // cout << "====================" << endl;
        // cout << "Fitness values for each schema" << endl;
        for (int i = 0; i < schema.size(); i++) {
            schema_fitness[schema[i]] = 0.0;
        }
        compute_schema_fitness(schema_fitness, fitness);

        // cout << "====================" << endl;
        bool is_deception;
        is_deception = ckeck_deception(ell, schema_fitness, fitness);

        // if (is_deception)
        //     cout << "Deception occurs" << endl;
        // else
        //     cout << "Deception does not occur" << endl;

        // Record the trial log
        trial_log[t] = is_deception;

    }

    // Compute the deception rate
    for (int i = 0; i < num_trials; i++) {
        if (trial_log[i])
            deception_rate += 1.0;
    }
    deception_rate /= num_trials;
    cout << "Deception rate: " << deception_rate << endl;

    return 0;
}