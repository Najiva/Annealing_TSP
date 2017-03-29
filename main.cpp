/* 
 * File:   main.cpp
 * Author: Juraj
 *
 * Created on February 16, 2017, 6:41 PM
 */

#include <bits/stdc++.h>
#include "GraphMap.h"
#include <csignal>
#include <fstream>
#include <string>
#include <thread>
#include "GraphLoader.h"

using namespace std;
using namespace std::chrono;
high_resolution_clock::time_point t1;
high_resolution_clock::time_point t2;
unordered_map<string, int> indexes;

ostream& operator<<(ostream &o, const iResult& p) {
    string from = "";
    string to = "";
    vector<Flight>::const_iterator it;
    unordered_map<string, int>::const_iterator i;
    o << p.second << endl;
    for (it = p.first.begin(); it != p.first.end(); ++it) {
        for (i = indexes.begin(); i != indexes.end(); ++i) {
            if (i->second == it->x)
                from = i->first;
            if (i->second == it->y)
                to = i->first;
        }
        o << from << " " << to << " " << it->z << " " << it->price << endl;
    }
    return o;
}

void printResult(pair<vector<FlightS>, int>& rasultData) {
    //cout << "Printing result .... " << endl;
    cout << rasultData.second << endl;
    vector<FlightS>::const_iterator it;
    for (it = rasultData.first.begin(); it != rasultData.first.end(); ++it) {
        cout << it->from << " " << it->to << " " << it->date << " " << it->price << endl;
    }
}

//void printResultIndexes(pair<vector<Flight>, int>& rasultData) {
//    cout << "Printing result .... " << endl;
//    cout << rasultData.second << endl;
//    vector<Flight>::const_iterator it;
//    for (it = rasultData.first.begin(); it != rasultData.first.end(); ++it) {
//        cout << *it << endl;
//    }
//}

//void printResultToFile(unordered_map<string, int>& cities, pair<vector<Flight>, int>& rasultData, string fileName) {
//    ofstream myFile("testData/r" + fileName);
//    if (myFile.is_open()) {
//        cout << "Printing result to file ..." << endl;
//        myFile << rasultData.second << endl;
//        string from = "";
//        string to = "";
//        vector<Flight>::const_iterator it;
//        unordered_map<string, int>::const_iterator i;
//        for (it = rasultData.first.begin(); it != rasultData.first.end(); ++it) {
//            for (i = cities.begin(); i != cities.end(); ++i) {
//                if (i->second == it->x)
//                    from = i->first;
//                if (i->second == it->y)
//                    to = i->first;
//            }
//            myFile << from << " " << to << " " << it->z << " " << it->price << endl;
//        }
//    } else {
//        cout << "Cannot open output file!" << endl;
//    }
//}

/**
 * Metoda overí správnost výsledkov.
 * @param cityIndex
 * @param finalResult
 * @param targetCity
 * @param numberOfCities
 * @return 
 */

//bool verifyResult(unordered_map<string, int>& cityIndex, pair<vector<Flight>, int>& finalResult, string targetCity, int numberOfCities) {
//    cout << "Verifying result ..." << endl;
//    vector<Flight>::const_iterator it;
//    unordered_map<string, int>::const_iterator i;
//    set<int> departedCities;
//    string from = "";
//    string to = "";
//    int previousCity = cityIndex.find(targetCity)->second;
//    long int totalPrice = 0;
//
//    if (finalResult.first.size() != numberOfCities) {
//        cout << "Bad number of tickets! Is: " << finalResult.first.size() << " Should be: " << numberOfCities << endl;
//        return false;
//    }
//    for (it = finalResult.first.begin(); it != finalResult.first.end(); ++it) {
//        totalPrice += it->price;
//        for (i = cityIndex.begin(); i != cityIndex.end(); ++i) {
//            if (i->second == it->x)
//                from = i->first;
//            if (i->second == it->y)
//                to = i->first;
//        }
//        if (previousCity != it->x) {
//            cout << "Bad output on city: " << from << " " << to << " " << it->z << endl;
//            return false;
//        }
//        if (departedCities.find(it->x) != departedCities.end()) {
//            cout << "Duplicate departure from the city: " << from << " " << to << " " << it->z << endl;
//            return false;
//        }
//        previousCity = it->y;
//        departedCities.insert(it->x);
//    }
//    if (totalPrice != finalResult.second) {
//        cout << "Sum of prices is not valid!" << endl;
//        return false;
//    }
//    cout << "Probably everything ok!" << endl;
//    return true;
//}

/**
 * Thread task spocita niekolko krat annealing.
 */

void thread_task(GraphMap& o, int cycles) {
    //cout << "Doing " << cycles << " cycles" << endl;
    for (int i = 0; i < cycles; i++) {
        o.anneal();
    }
}

int main(int argc, char* argv[]) {

    // Do i want to see debug  comments
    bool stat = false;

    t1 = high_resolution_clock::now();

    //    signal(SIGINT, signalHandler);
    //
    //    string testDataFileName = "";
    //    if (argc == 2) {
    //        testDataFileName.assign(argv[1]);
    //    } else {
    //        testDataFileName = "data_30.txt";
    //    }
    //
    //    set<string>::const_iterator it;
    //    set<string> files;
    //    files.insert("data_15.txt");
    //    files.insert("data_20.txt");
    //    files.insert("data_30.txt");
    //    files.insert("data_40.txt");
    //    files.insert("data_50.txt");
    //    files.insert("data_60.txt");
    //    files.insert("data_70.txt");
    //    files.insert("data_100.txt");
    //    files.insert("data_200.txt");
    //    files.insert("data_300.txt");

    GraphData* data;
    vector<GraphMap> graphs;
    int cycles = 1;
    unsigned int COOLING_STEPS = 2000;
    unsigned int STEPS_PER_TEMP = 350000;
    unsigned int maxDuration = 29000; // ms
    double INITIAL_TEMPERATURE = 0.5;
    double COOLING_FRACTION = 0.986;
    static const int num_threads = 4;
    thread t[num_threads];

    // Setting the calculation
    if (stat) {
        cout << "Cooling steps: " << COOLING_STEPS << endl;
        cout << "Steps per temperature: " << STEPS_PER_TEMP << endl;
        cout << "Cycles per thread: " << cycles << endl;
        cout << "Number of threads: " << num_threads << endl;
        cout << "Max duration [ms]: " << maxDuration << endl;
        cout << "Initial temperature: " << INITIAL_TEMPERATURE << endl;
        cout << "Cooling fraction: " << COOLING_FRACTION << endl;
    }



    // for (it = files.begin(); it != files.end(); ++it) {
    GraphLoader loader;
    data = loader.loadData("graph");
    GraphMap graph(data, maxDuration, t1);
    graph.setAnnealingParameters(COOLING_STEPS, STEPS_PER_TEMP, INITIAL_TEMPERATURE, COOLING_FRACTION);
    graphs.push_back(graph);


    int numberOfCities = loader.getNumberOfCities();
    // pridane na zaklade ziadosti Tomasa :]
    if (numberOfCities > 20 && numberOfCities < 100) {
        // VIAC VLAKIEN ----------------------- (4 x 3 volania)
        GraphMap graph2(data, maxDuration, t1);
        graph2.setAnnealingParameters(300, 150000, INITIAL_TEMPERATURE, COOLING_FRACTION);
        graphs.push_back(graph2);
        GraphMap graph3(data, maxDuration, t1);
        graph3.setAnnealingParameters(300, 75000, INITIAL_TEMPERATURE, COOLING_FRACTION);
        graphs.push_back(graph3);
        GraphMap graph4(data, maxDuration, t1);
        graph4.setAnnealingParameters(300, 25000, INITIAL_TEMPERATURE, COOLING_FRACTION);
        graphs.push_back(graph4);
        graph2.initMinsAndSolutions();
        graph3.initMinsAndSolutions();
        graph4.initMinsAndSolutions();
        t[0] = thread(thread_task, ref(graph), cycles);
        t[1] = thread(thread_task, ref(graph2), 4);
        t[2] = thread(thread_task, ref(graph3), 8);
        t[3] = thread(thread_task, ref(graph4), 16);

        for (int i = 0; i < num_threads; i++) {
            t[i].join();
        }
        if (stat) {
            cout << "Stats of each thread if more then one anneling (cycles) was set: MEAN, STD, BEST" << endl;
            cout << graph.getMEAN() << " " << graph.getSTD() << " " << graph.getBestSolution() << " " << endl;
            cout << graph2.getMEAN() << " " << graph2.getSTD() << " " << graph2.getBestSolution() << " " << endl;
            cout << graph3.getMEAN() << " " << graph3.getSTD() << " " << graph3.getBestSolution() << " " << endl;
            cout << graph4.getMEAN() << " " << graph4.getSTD() << " " << graph4.getBestSolution() << " " << endl;
        }

        // Now i will choose the best result from all of the threads
        int min = 1000000;
        vector<iResultS> r;
        iResultS winner;
        vector<iResultS>::const_iterator k;
        r.push_back(graph.getBestResult());
        r.push_back(graph2.getBestResult());
        r.push_back(graph3.getBestResult());
        r.push_back(graph4.getBestResult());

        for (k = r.begin(); k != r.end(); ++k) {
            //cout << cycles << " " << k->second << endl;
            if (k->second < min) {
                winner = *k;
                min = k->second;
            }
        }
        if (stat) {
            cout << cycles << " " << winner.second << endl;
            t2 = high_resolution_clock::now();
            cout << "Total time elapsed [ms]: " << duration_cast<milliseconds>(t2 - t1).count() << endl;
        }

        //cout << "Best slution is: " << endl;
        printResult(winner);

        return 0;
    } else {

        // VIAC VLAKIEN ----------------------- (4 x 3 volania)
        GraphMap graph2(data, maxDuration, t1);
        graph2.setAnnealingParameters(COOLING_STEPS, STEPS_PER_TEMP, INITIAL_TEMPERATURE, COOLING_FRACTION);
        graphs.push_back(graph2);
        GraphMap graph3(data, maxDuration, t1);
        graph3.setAnnealingParameters(COOLING_STEPS, STEPS_PER_TEMP, INITIAL_TEMPERATURE, COOLING_FRACTION);
        graphs.push_back(graph3);
        GraphMap graph4(data, maxDuration, t1);
        graph4.setAnnealingParameters(COOLING_STEPS, STEPS_PER_TEMP, INITIAL_TEMPERATURE, COOLING_FRACTION);
        graphs.push_back(graph4);
        graph.initMinsAndSolutions();
        graph2.initMinsAndSolutions();
        graph3.initMinsAndSolutions();
        graph4.initMinsAndSolutions();
        t[0] = thread(thread_task, ref(graph), cycles);
        t[1] = thread(thread_task, ref(graph2), cycles);
        t[2] = thread(thread_task, ref(graph3), cycles);
        t[3] = thread(thread_task, ref(graph4), cycles);

        for (int i = 0; i < num_threads; i++) {
            t[i].join();
        }
        if (stat) {
            cout << "Stats of each thread if more then one anneling (cycles) was set: MEAN, STD, BEST" << endl;
            cout << graph.getMEAN() << " " << graph.getSTD() << " " << graph.getBestSolution() << " " << endl;
            cout << graph2.getMEAN() << " " << graph2.getSTD() << " " << graph2.getBestSolution() << " " << endl;
            cout << graph3.getMEAN() << " " << graph3.getSTD() << " " << graph3.getBestSolution() << " " << endl;
            cout << graph4.getMEAN() << " " << graph4.getSTD() << " " << graph4.getBestSolution() << " " << endl;
        }

        // Now i will choose the best result from all of the threads
        int min = 1000000;
        vector<iResultS> r;
        iResultS winner;
        vector<iResultS>::const_iterator k;
        r.push_back(graph.getBestResult());
        r.push_back(graph2.getBestResult());
        r.push_back(graph3.getBestResult());
        r.push_back(graph4.getBestResult());

        for (k = r.begin(); k != r.end(); ++k) {
            //cout << cycles << " " << k->second << endl;
            if (k->second < min) {
                winner = *k;
                min = k->second;
            }
        }
        if (stat) {
            cout << cycles << " " << winner.second << endl;
            t2 = high_resolution_clock::now();
            cout << "Total time elapsed [ms]: " << duration_cast<milliseconds>(t2 - t1).count() << endl;
        }

        //cout << "Best slution is: " << endl;
        printResult(winner);

        return 0;
    }
}
