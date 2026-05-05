//
// Created by Bcom_ on 05.05.2026.
//


#include "GeneralUtils.h"


namespace utils {
    std::vector<int> getNRandomIntegersFromVector(std::vector<int> v, int n){
        std::random_device rd;
        std::mt19937 gen(rd());

        std::ranges::shuffle(v, gen);

        v.resize(n);
        return v;
    }
}

