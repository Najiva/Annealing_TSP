/* 
 * File:   Flight.cpp
 * Author: Juraj
 * 
 * Created on February 26, 2017, 12:11 PM
 */

#include "Flight.h"

Flight::Flight(){

}

Flight::Flight(int f, int t, int d, uint16_t pr) {
    x = f;
    y = t;
    z = d;
    price = pr;
}

Flight::~Flight() {
}

