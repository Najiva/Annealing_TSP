/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GraphLoader.h
 * Author: Juraj
 *
 * Created on March 7, 2017, 9:21 PM
 */

#include <unordered_map>
#include "Flight.h"
#include "csv.h"
#include <chrono>
#include <utility>
#include <stdlib.h>
#include <assert.h>


#ifndef GRAPHLOADER_H
#define GRAPHLOADER_H

using namespace std;
using namespace std::chrono;

struct GraphData {
    string fn;
    int nc;
    uint16_t *** cube;
    unordered_map<string, int>* ci;
    string tc;
};

class GraphLoader {
public:

    GraphLoader() {

    }

    ~GraphLoader() {
        delete cityIndex;
        for (unsigned int i = 0; i < numberOfCities; i++) {
            for (unsigned int j = 0; j < numberOfCities; j++) {
                delete[] arr[i][j];
            }
            delete[] arr[i];
        }
        delete[] arr;
    }
    
    int getNumberOfCities(){
        return numberOfCities;
    }

    GraphData* loadData(string fn) {
        //high_resolution_clock::time_point t1 = high_resolution_clock::now();
        // budem citat z cin takze si musim pocas mapovania ukladat lety co som uz nacital
        vector<Flight> initialFlights;
        this->fileName = fn;
        //io::CSVReader<4> in("testData/" + this->fileName);
        io::CSVReader<4> in(this->fileName, cin);
        in.set_header("from", "to", "day", "price");
        int i = 0;
        std::string from = "";
        std::string to = "";
        string firstCity = "NONE";
        bool foundSecond = false;
        int day = 0;
        uint16_t price = 0;
        cityIndex = new unordered_map<string, int>();
        targetCity = in.next_line();
        //cout << "Target city: " << targetCity << endl;
        // predpokladam, ze pokial najdem ine odletove mesto tak uz budem mat vsetky namapovane!!!
        do {
            in.read_row(from, to, day, price);
            if (firstCity == from || firstCity == "NONE") {
                firstCity = from;
                if (cityIndex->find(from) == cityIndex->end()) {
                    int c = i++;
                    (*cityIndex)[from] = c;
                }
                if (cityIndex->find(to) == cityIndex->end()) {
                    int c = i++;
                    (*cityIndex)[to] = c;
                }
            } else {
                foundSecond = true;
            }
            initialFlights.push_back(Flight(cityIndex->find(from)->second, cityIndex->find(to)->second, day, price));
        } while (!foundSecond);
        numberOfCities = cityIndex->size();
        
        //cout << "Number of cities: " << numberOfCities << endl;
        
        // inicializujem maticu
        this->initDataMatrix();

//        high_resolution_clock::time_point t2 = high_resolution_clock::now();
//        auto duration = duration_cast<microseconds>( t2 - t1 ).count();
        //cout << "Duration of header loading: " << duration  << endl;
        
        //high_resolution_clock::time_point t3 = high_resolution_clock::now();

        // vložím do nej data
        vector<Flight>::const_iterator it;
        for (it = initialFlights.begin(); it != initialFlights.end(); ++it) {
            arr[it->x][it->y][it->z] = it->price;
        }
        
//        high_resolution_clock::time_point t4 = high_resolution_clock::now();
//        auto duration2 = duration_cast<microseconds>( t4 - t3 ).count();
//        cout << "Duration of storing header: " << duration2  << endl;
        
        //high_resolution_clock::time_point t5 = high_resolution_clock::now();

        while (in.read_row(from, to, day, price)) {
            arr[cityIndex->find(from)->second][cityIndex->find(to)->second][day] = price;
        }
        
//        high_resolution_clock::time_point t6 = high_resolution_clock::now();
//        auto duration3 = duration_cast<microseconds>( t6 - t5 ).count();
        //cout << "Duration of reading and storing rest: " << duration3  << endl;
        

        // vytlacim si namapovanie
        //        unordered_map<string, int>::const_iterator ir;
        //        cout << "Mapovanie miest na indexy" << endl;
        //        for (ir = cityIndex.begin(); ir != cityIndex.end(); ++ir) {
        //            cout << ir->second << "-" << ir->first << endl;
        //        }


        // zabalím ich a pošlem postou

        GraphData* data = new GraphData();
        data->cube = arr;
        data->fn = fileName;
        data->nc = numberOfCities;
        data->ci = cityIndex;
        data->tc = targetCity;
        return data;
    }

    void initDataMatrix() {
        //cout << "Data matrix initialization ..." << endl;
        arr = new uint16_t**[numberOfCities];
        for (unsigned int i = 0; i < numberOfCities; i++) {
            arr[i] = new uint16_t*[numberOfCities];
            for (unsigned int j = 0; j < numberOfCities; j++) {
                arr[i][j] = new uint16_t[numberOfCities];
                for (unsigned int k = 0; k < numberOfCities; k++) {
                    arr[i][j][k] = INF;
                }
            }
        }
    }
private:
    const int INF = 65535;
    string fileName;
    string targetCity;
    uint16_t *** arr;
    long unsigned int numberOfCities;
    unordered_map<string, int>* cityIndex;
};

#endif /* GRAPHLOADER_H */

