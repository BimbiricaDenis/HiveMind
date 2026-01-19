#pragma once
#include <string>

struct  Package{
    int id;
    int source_row;
    int source_col;
    int dest_row;//Client destination row
    int dest_col;//Client destination col
    int reward;//200-800
    int start_tick;
    int deadline;//10-20 ticks
    bool isAssigned;
    bool isDelivered;
    bool isProcessed = false;

    Package(int id, int sR, int sC, int dR, int dC, int rew, int tick, int dead)
        : id(id), source_row(sR), source_col(sC), dest_row(dR), dest_col(dC), 
          reward(rew), start_tick(tick), deadline(dead), isAssigned(false), isDelivered(false) {}
};