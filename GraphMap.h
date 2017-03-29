/* 
 * File:   DataAnalyzer.h
 * Author: Juraj
 *
 * Created on February 19, 2017, 11:39 AM
 */

using namespace std;

#include<set>
#include<unordered_set>
#include<unordered_map>
#include<vector>
#include <random>
#include "Flight.h"
#include "GraphLoader.h"
#include "RNG.h"
#include <cmath>
#include <numeric>
#include "boost/random.hpp"
#include "boost/random/uniform_real.hpp"

struct FlightS {
    string from;
    string to;
    uint16_t price;
    int date;
};

typedef pair<vector<Flight>, int> iResult;
typedef pair<vector<FlightS>, int> iResultS;

#ifndef GRAPHMAP_H
#define GRAPHMAP_H

class GraphMap {
public:
    GraphMap();

    GraphMap(GraphData * data, unsigned int& maxDur, high_resolution_clock::time_point& start) {
        arr = data->cube;
        targetCity = data->tc;
        numberOfCities = data->nc;
        cityIndex = data->ci;
        fileName = data->fn;
        totalMin = 1000000;
        maxDuration = maxDur;
        t1 = start;

    }

    unordered_map<string, int>* getCityIndexes();
    int getNumberOfCities();
    string getTargetName();

    vector<int> getBestPerm() {
        return bestPerm.first;
    }

    pair<vector<FlightS>, int> getBestResult() {
        string from, to;
        vector<int>::const_iterator it;
        vector<FlightS> flights;
        unordered_map<string, int>::const_iterator i;
        for (it = totalBest.first.begin(); it != totalBest.first.end() - 1; ++it) {
            for (i = cityIndex->begin(); i != cityIndex->end(); ++i) {
                if (i->second == *it) {
                    from = i->first;
                } else if (i->second == *(it + 1)) {
                    to = i->first;
                }
            }
            FlightS f;
            f.from = from;
            f.to = to;
            f.price = arr[*it][*(it + 1)][it - totalBest.first.begin()];
            f.date = it - totalBest.first.begin();
            flights.push_back(f);
        }
        return make_pair(flights, totalBest.second);
    }

    void setAnnealingParameters(unsigned int a,unsigned int b, double it, double cf) {
        COOLING_STEPS = a;
        STEPS_PER_TEMP = b;
        INITIAL_TEMPERATURE = it;
        COOLING_FRACTION = cf;
        
    }

    void initializeSolution() {
        solutionCount = 0;
        permCost = 0;
        p.clear();
        int firstCity = cityIndex->find(targetCity)->second;
        // cout << "Target city index: " << targetCity << ": " << firstCity << endl;
        for (int k = 0; k < numberOfCities; k++) {
            if (k != firstCity)
                p.push_back(k);
        }
        p.insert(p.begin(), firstCity);
        p.insert(p.end(), firstCity);
        do {
            random_shuffle(p.begin() + 1, p.end() - 1);
        } while (!verifyPermutation());
    }

    bool verifyPermutation() {
        vector<int>::const_iterator it;
        for (it = p.begin(); it != p.end() - 1; ++it) {
            if (arr[*it][*(it + 1)][it - p.begin()] == INF) {
                //cout << "Flight: " << *it << "," << *(it + 1) << "," << it - p.begin() << " does not exist" << endl;
                return false;
            }
        }
        return true;
    }

    void initMinsAndSolutions() {
        totalMin = 1000000;
        solutions.clear();
    }

    inline bool verifyNewPermutation(int& a, int& b) {
        if (a == b - 1) {
            if (arr[p[a - 1]][p[a]][a - 1] != INF && arr[p[a]][p[b]][a] != INF && arr[p[b]][p[b + 1]][b] != INF) {
                return true;
            }
        } else if (a == b + 1) {
            if (arr[p[b - 1]][p[b]][b - 1] != INF && arr[p[b]][p[a]][b] != INF && arr[p[a]][p[a + 1]][a] != INF) {
                return true;
            }
        } else {
            if (arr[p[a - 1]][p[a]][a - 1] != INF && arr[p[a]][p[a + 1]][a] != INF && arr[p[b - 1]][p[b]][b - 1] != INF && arr[p[b]][p[b + 1]][b] != INF) {
                return true;
            }
        }
        return false;
    }

    inline bool verifyNewPermutationBeforeSwap(unsigned int& b,unsigned int& a) {
        if (a == b - 1) {
            if (arr[p[a - 1]][p[a]][a - 1] != INF && arr[p[a]][p[b]][a] != INF && arr[p[b]][p[b + 1]][b] != INF) {
                return true;
            }
        } else if (a == b + 1) {
            if (arr[p[b - 1]][p[b]][b - 1] != INF && arr[p[b]][p[a]][b] != INF && arr[p[a]][p[a + 1]][a] != INF) {
                return true;
            }
        } else {
            if (arr[p[a - 1]][p[a]][a - 1] != INF && arr[p[a]][p[a + 1]][a] != INF && arr[p[b - 1]][p[b]][b - 1] != INF && arr[p[b]][p[b + 1]][b] != INF) {
                return true;
            }
        }
        return false;
    }

    inline int solutionCost() {
        int sum = 0;
        vector<int>::const_iterator it;
        for (it = p.begin(); it != p.end() - 1; ++it) {
            sum += arr[*it][*(it + 1)][it - p.begin()];
            //cout << arr[*it][*(it + 1)][it - permutation.begin()] << endl;
        }
        //cout << "Sum of solutoion: " << sum << endl;
        permCost = sum;
        return sum;
    }

    inline int newSolutionCost(int& a, int& b) {
        int sub = 0;
        int plus = 0;
        // ak su vedla seba
        if (a == b - 1) {
            sub = arr[p[a - 1]][p[b]][a - 1] + arr[p[b]][p[a]][a] + arr[p[a]][p[b + 1]][b];
            plus = arr[p[a - 1]][p[a]][a - 1] + arr[p[a]][p[b]][a] + arr[p[b]][p[b + 1]][b];
        } else if (a == b + 1) {
            sub = arr[p[b - 1]][p[a]][b - 1] + arr[p[a]][p[b]][b] + arr[p[b]][p[a + 1]][a];
            plus = arr[p[b - 1]][p[b]][b - 1] + arr[p[b]][p[a]][b] + arr[p[a]][p[a + 1]][a];
        } else {
            sub = arr[p[a - 1]][p[b]][a - 1] + arr[p[b]][p[a + 1]][a] + arr[p[b - 1]][p[a]][b - 1] + arr[p[a]][p[b + 1]][b];
            plus = arr[p[a - 1]][p[a]][a - 1] + arr[p[a]][p[a + 1]][a] + arr[p[b - 1]][p[b]][b - 1] + arr[p[b]][p[b + 1]][b];
        }
        return permCost - sub + plus;
    }

    inline int newSolutionCostDifferently(unsigned int& a,unsigned int& b) {
        // ak su vedla seba
        if (a != b - 1 && a != b + 1) {
            return permCost - arr[p[a - 1]][p[b]][a - 1] - arr[p[b]][p[a + 1]][a] - arr[p[b - 1]][p[a]][b - 1] - arr[p[a]][p[b + 1]][b] + arr[p[a - 1]][p[a]][a - 1] + arr[p[a]][p[a + 1]][a] + arr[p[b - 1]][p[b]][b - 1] + arr[p[b]][p[b + 1]][b];
        } else if (a == b + 1) {
            return permCost - arr[p[b - 1]][p[a]][b - 1] - arr[p[a]][p[b]][b] - arr[p[b]][p[a + 1]][a] + arr[p[b - 1]][p[b]][b - 1] + arr[p[b]][p[a]][b] + arr[p[a]][p[a + 1]][a];
        } else {
            return permCost - arr[p[a - 1]][p[b]][a - 1] - arr[p[b]][p[a]][a] - arr[p[a]][p[b + 1]][b] + arr[p[a - 1]][p[a]][a - 1] + arr[p[a]][p[b]][a] + arr[p[b]][p[b + 1]][b];
        }

    }

    inline int newSolutionCostDifferentlyBeforeSwap(int& b, int& a) {
        // ak su vedla seba
        if (a != b - 1 && a != b + 1) {
            return permCost - arr[p[a - 1]][p[b]][a - 1] - arr[p[b]][p[a + 1]][a] - arr[p[b - 1]][p[a]][b - 1] - arr[p[a]][p[b + 1]][b] + arr[p[a - 1]][p[a]][a - 1] + arr[p[a]][p[a + 1]][a] + arr[p[b - 1]][p[b]][b - 1] + arr[p[b]][p[b + 1]][b];
        } else if (a == b + 1) {
            return permCost - arr[p[b - 1]][p[a]][b - 1] - arr[p[a]][p[b]][b] - arr[p[b]][p[a + 1]][a] + arr[p[b - 1]][p[b]][b - 1] + arr[p[b]][p[a]][b] + arr[p[a]][p[a + 1]][a];
        } else {
            return permCost - arr[p[a - 1]][p[b]][a - 1] - arr[p[b]][p[a]][a] - arr[p[a]][p[b + 1]][b] + arr[p[a - 1]][p[a]][a - 1] + arr[p[a]][p[b]][a] + arr[p[b]][p[b + 1]][b];
        }

    }

    inline void swap(unsigned int& a, unsigned int& b) {
        int temp = 0;
        //cout << "Trying to swap: " << a << " and " << b << endl;
        temp = p[a];
        p[a] = p[b];
        p[b] = temp;
    }

    void rotate(int& a, int& b) {
        reverse(p.begin() + a, p.begin() + b);
    }

    double getSTD() {
        double mean = getMEAN();
        double var = 0;
        unsigned int n = 0;
        while (n < solutions.size()) {
            var = var + (((double) solutions[n] - mean) * ((double) solutions[n] - mean));
            n++;
        }
        var = var / (double) solutions.size();
        return sqrt(var);
    }

    double getMEAN() {
        int sum = accumulate(solutions.begin(), solutions.end(), 0);
        return (double) sum / (double) solutions.size();
    }

    int getBestSolution() {
        int min = INF;
        vector<int>::const_iterator it;
        for (it = solutions.begin(); it != solutions.end(); ++it) {
            if (min > *it) {
                min = *it;
            }
        }
        return min;
    }

    void setPermutation(vector<int> perm) {
        solutionCount = 0;
        p = perm;
    }

    void printPerm() {
        vector<int>::const_iterator it;
        for (it = p.begin(); it != p.end(); ++it) {
            cout << *it << ",";
        }
        cout << endl;
    }

    void anneal() {
        unsigned int i1, i2, min, max; /* pair of items to swap */
        unsigned int i, j; /* counters */
        double temperature = INITIAL_TEMPERATURE; /* the current system temp */
        int current_value = 0;
        int new_value = 0; /* value of current state */
        int delta = 0; /* value after swap */
        double merit, flip; /* hold swap accept conditions*/
        double exponent; /* exponent for energy funct*/
        min = 1; // minimal index of cities
        max = numberOfCities - 1; // maximal index of cities
        this->initializeSolution();

        // BOOST RNG NA DOUBLE JE TROCHU RYCHLEJSI ZDA SA, AK HO NECHCEM POUZIT ZAKOMENTUJ
        // 100 000 000 random double vygeneruje za 8 s, rng.getRandomDouble() to stihne za 26 s
        boost::mt19937 boostGen;
        boost::uniform_real<> uniformDistribution(0.0, 1.0);
        boost::variate_generator< boost::mt19937&, boost::uniform_real<> >
                generateRandomNumbers(boostGen, uniformDistribution);
        
        current_value = solutionCost();
        bestPerm = make_pair(p, current_value);

        for (i = 1; i <= COOLING_STEPS; i++) {
            t2 = high_resolution_clock::now();
            if (duration_cast<milliseconds>(t2 - t1).count() > maxDuration) {
                //                cout << "Ubehlo " << duration_cast<milliseconds>(t2 - t1).count() << " ms, iteracia: " << i << " Breaking out of the cycle." << endl;
                //                cout << "Total min: " << bestPerm.second << endl;
                break;
            }
            temperature *= COOLING_FRACTION;
            for (j = 1; j <= STEPS_PER_TEMP; j++) {
                /* pick indices of elements to swap */
                do {
                    do {
                        i1 = rng.getRandomInt(min, max);
                        i2 = rng.getRandomInt(min, max);
//                         i1 = generateRandomInt();
//                        i2 = generateRandomInt();
                        //cout << "i1: " << i1 << ", i2: " << i2 << endl;
                    } while (i1 == i2);
                } while (!verifyNewPermutationBeforeSwap(i1, i2));

                swap(i1, i2);
                new_value = newSolutionCostDifferently(i1, i2);
                delta = new_value - current_value;

                //cout << "merit = " << merit << " flip= " << flip << " exponent= " << exponent << endl;
                /*if (merit >= 1.0)
                     merit = 0.0;*/ /* don't do unchanging swaps*/
                if (delta < 0) { /*ACCEPT-WIN*/
                    //swap(i1, i2);
                    permCost = new_value;
                    current_value = new_value;
                    if (bestPerm.second > current_value)
                        bestPerm = make_pair(p, current_value);
                    //                    if (TRACE_OUTPUT && (solutionCount % PRINT_FREQUENCY) == 0) {
                    //                        cout << solutionCount << " " << current_value << endl;
                    //                        cout << "swap WIN" << i1 << "--" << i2 << " value " << current_value << " temp= " << temperature << endl;
                    //                    }
                } else {
                    flip = generateRandomNumbers();
                    //flip = rng.getRandomDouble();
                    exponent = ((-1)*(double) delta * numberOfCities / current_value) / (K * temperature);
                    merit = pow(E, exponent);
                    if (merit > flip) { /*ACCEPT-LOSS*/
                        //swap(i1, i2);
                        permCost = new_value;
                        current_value = new_value;
                        //bestPerm = make_pair(p, current_value);
                        //                        if (TRACE_OUTPUT && (solutionCount % PRINT_FREQUENCY) == 0) {
                        //                            cout << solutionCount << " " << current_value << endl;
                        //                            cout << "swap LOSS " << i1 << "--" << i2  << " value " << current_value << " merit= " << merit << " flip= " << flip << endl;
                        //                        }
                    } else { /* REJECT */
                        swap(i1, i2);
                    }
                }
                //solutionCount++;
            }
            // dynamicke znizovanie teploty
            //if ((current_value - start_value) < 0) { /* rerun at this temp */
                //temperature /= COOLING_FRACTION;
                // temperature /= COOLING_FRACTION;
                //if (TRACE_OUTPUT) cout << "rerun at temperature" << temperature << endl;
            //}
        }

        // COOLING_STEPS STEPS_PER_TEMP SOLUTIONS RESULT
        //cout << COOLING_STEPS << " " << STEPS_PER_TEMP << " " << solutionCount << " " << current_value << endl;
        //cout << "Best solution: " << current_value << endl;

        // pokial mám viac iterácií anneal
        if (bestPerm.second < totalMin) {
            totalMin = bestPerm.second;
            totalBest = bestPerm;
        }
        solutions.push_back(bestPerm.second);

        
//        cout << "End temperature: " << temperature << endl;
//        cout << "End iteration: " << i << endl;
        
       // return current_value;
        //printResult();
    }

    void printBestResult() {
        cout << totalBest.second << endl;
        vector<int>::const_iterator it;
        string from, to;
        unordered_map<string, int>::const_iterator i;
        for (it = totalBest.first.begin(); it != totalBest.first.end() - 1; ++it) {
            for (i = cityIndex->begin(); i != cityIndex->end(); ++i) {
                if (i->second == *it) {
                    from = i->first;
                } else if (i->second == *(it + 1)) {
                    to = i->first;
                }
            }
            cout << from << " " << to << " " << it - totalBest.first.begin() << " " << arr[*it][*(it + 1)][it - totalBest.first.begin()] << endl;
        }
        cout << solutionCount << endl;
    }
    void printDataMatrix();

private:
    // randomNumberGenerator
    RNG rng;
    // pole permutaci
    vector<int> p;
    // cas
    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;
    // maximalna doba behu vypoctu
    unsigned int maxDuration;
    int permCost;
    pair<vector<int>, int> bestPerm, totalBest;
    // ked viac krat iterujem tak si ulozim vysledky aby som si potom spravil statistiku
    vector<int> solutions;
    string fileName;
    const uint16_t INF = 65535;
    string targetCity;
    int numberOfCities;
    unordered_map<string, int>* cityIndex;
    uint16_t *** arr;
    iResult result;
    int solutionCount;
    int totalMin;

    // constants
    bool TRACE_OUTPUT = true;
    int PRINT_FREQUENCY = 10000;
    double INITIAL_TEMPERATURE;
    double COOLING_FRACTION;
    double E = 2.718;
    double K = 1;
    //double K = 0.01;
    unsigned int COOLING_STEPS;
    unsigned int STEPS_PER_TEMP;
};

#endif /* GRAPHMAP_H */

