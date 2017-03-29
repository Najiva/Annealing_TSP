/* 
 * File:   Flight.h
 * Author: Juraj
 *
 * Created on February 26, 2017, 12:11 PM 
 * This class is only a structure I used to order flights.
 */
#include <bits/stdc++.h>
//#include <cstdlib>

using namespace std;

#ifndef FLIGHT_H
#define	FLIGHT_H

class Flight {
public:
    Flight();
    Flight(int from, int to, int day, uint16_t price);
    virtual ~Flight();
    uint16_t price;
    int x;
    int y;
    int z;
private:
    friend std::ostream & operator<<(std::ostream &os, const Flight& p){
        return os << p.x << " " << p.y << " " << p.z << " " << p.price;
    }
};

#endif	/* FLIGHT_H */

