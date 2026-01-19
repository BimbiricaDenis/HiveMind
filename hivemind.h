#pragma once
#include <vector>
#include "pathfinder.h"
#include "agent.h"
#include "package.h"
#include "simulation_config.h"

class HiveMind {
    private:
        std::vector<Package*> allPackages;
        int packageIdCounter;
        std::vector<std::pair<int,int>> clientLocations; // List of (row, column) pairs for clients
        std::vector<std::pair<int,int>> stationLocations; // List of (row, column) pairs for stations
        std::pair<int,int> basePosition; // (row, column) of the base
        int getDistance(int r1, int c1, int r2, int c2) {
            return abs(r1 - r2) + abs(c1 - c2);// Manhattan distance
        }
        std::pair<int,int> getNearestStation(int row,int col){
            std::pair<int,int> nearest =basePosition;
            int minDist = getDistance(row,col,basePosition.first,basePosition.second);

            for(auto station : stationLocations){
                int dist = getDistance(row,col,station.first,station.second);
                if(dist < minDist){
                    minDist = dist;
                    nearest = station;
                }
            }
            return nearest;
        }
        double TotalProfit = 0;
        int deliveredCount = 0;
        int missedDeadlines = 0;
        int agentsLost = 0;
    public:

    HiveMind();
    ~HiveMind();
    
    void scanMap(const std::vector<std::vector<char>>& map);
    void generatePackages(int currentTick,int base_row,int base_col);
    void dispatch(std::vector<Agent*>& fleet,const std::vector<std::vector<char>>& map);
    void printQueueStatus();
    void monitorAgents(std::vector<Agent*>& fleet,const std::vector<std::vector<char>>& map);
    void ProcessFinancials(std::vector<Agent*>& fleet,int current_tick);
    void reportDelivery(Package* pkg,int current_tick);
    void generateRaport(const std::string& filename);
    double getProfit() const { return TotalProfit; }
};