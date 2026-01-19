#pragma once
#include <vector>
#include "enums.h"

class IMAPGenerator {
  public:
    virtual std::vector<std::vector<char>> generateMap() = 0;
    virtual ~IMAPGenerator() {};
};