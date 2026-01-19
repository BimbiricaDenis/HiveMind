#pragma once
#include "imap_generator.h"
#include <random>

class ProceduralMapGenerator : public IMAPGenerator {
    private:
        int randomInt(int min,int max);
        bool isValidMap(const std::vector<std::vector<char>>& map,int startRow,int startCol);
    public:
        std::vector<std::vector<char>> generateMap() override;    
};