/* 
 * File:   DataAnalyzer.cpp
 * Author: Juraj
 * 
 * Created on February 19, 2017, 11:39 AM
 */
using namespace std;

#include "GraphMap.h"
#include "csv.h"
#include <string>
#include <bits/stdc++.h>
#include <algorithm>
#include <random>
#include <fstream>
#include <set>

struct flightSort {

    inline bool operator()(const Flight& struct1, const Flight& struct2) {
        if (struct1.z == struct2.z) {
            return (struct1.price < struct2.price);
        } else
            return (struct1.z < struct2.z);
    }
};

string GraphMap::getTargetName() {
    return targetCity;
}

unordered_map<string, int>* GraphMap::getCityIndexes() {
    return cityIndex;
}

void GraphMap::printDataMatrix() {
    for (int i = 0; i < numberOfCities; i++) {
        for (int j = 0; j < numberOfCities; j++) {
            for (int k = 0; k < numberOfCities; k++) {
                cout << i << "," << j << "," << k << ": " << arr[i][j][k] << " ";
            }
            cout << endl;
        }
    }
}

int GraphMap::getNumberOfCities() {
    return this->numberOfCities;
}





