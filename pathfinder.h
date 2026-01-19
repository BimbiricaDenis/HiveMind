#pragma once
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include "enums.h"

struct Point {
    int row;
    int col;

    bool operator<(const Point& other) const {
        if(row != other.row) return row < other.row;
        return col < other.col;
    }
    bool operator==(const Point& other) const {
        return row == other.row && col == other.col;
    }
};

class Pathfinder {
    public:
        static std::vector<std::pair<int,int>> getPath(const std::vector<std::vector<char>>&map,int startRow,int startCol,int endRow,int endCol,bool canFly);
};