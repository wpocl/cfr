#pragma once
#include <map>
#include <vector>
#include <algorithm>

// S partitioned into N slots
std::map<int, std::vector<int>> partitions(int S, int N) {
    std::map<int, std::vector<int>> strategy_map;
    std::vector<int> divider;
    for (int i = 0; i < (S+1)*N-2; i++)
        divider.push_back((i < S*N-1) ? 0 : 1);
    do {
        std::vector<int> strategy;
        int cache = 0;
    
        for (int i = 2; i < (S+1)*N; i++) {
            if (divider[i-2]==1) {
                strategy.push_back(cache);
                cache = 0;
            }
            cache += (i%N) ? 0 : 1; 
        }
        strategy.push_back(cache);
        for (int i = 0; i < strategy_map.size(); i++) {
            if (strategy_map[i] == strategy)
                goto next;      
        }
        strategy_map[strategy_map.size()] = strategy;
        next:
            continue;
    } while(next_permutation(divider.begin(), divider.end()));
    
    return strategy_map;
}