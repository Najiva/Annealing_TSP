/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RNG.h
 * Author: skvarla
 *
 * Created on 6 March 2017, 11:00 AM
 */

#include <random>


#ifndef RNG_H
#define RNG_H

class RNG {
public:
    RNG() : gen(std::random_device()()) {} // Seeds the mt19937.

    unsigned int getRandomInt(unsigned int& min,unsigned int& max){  
        std::uniform_int_distribution<int> dis(min, max);
        return (unsigned int) dis(gen);
    }
    
    double getRandomDouble(){  
        std::uniform_real_distribution<double> dis(0, 1);
        double random = dis(gen);
        return random;
    }

private:
    std::mt19937 gen;
};

#endif /* RNG_H */

