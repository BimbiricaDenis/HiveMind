#pragma once
#include "enums.h"
#include "package.h"
#include <string>
#include <vector>
#include <iostream>

class Agent {
    protected:
        int id;
        int r,c; // row and column position
        float current_battery;
        AgentState state;
        std::vector<std::pair<int, int>> current_path; // path as list of (row, column) pairs
        bool processedDead = false;
        std::string name;
        char symbol;
        int speed;
        float max_battery;
        float consumption;
        int cost_per_tick;
        int capacity;
        std::vector<std::pair<int,int>> path;
        Package* current_package;

    public:
        Agent(int id,int r,int c);
        virtual ~Agent() {}

        virtual bool canPassWalls() const = 0;

        void update(const std::vector<std::vector<char>>& map);
        void charge();
        void drainBattery();
        void setDestination(int dest_row,int dest_col, const std::vector<std::vector<char>>& map);
        bool move();
        bool isMoving() const { return !path.empty(); }
        void assignPackage(Package* pkg, const std::vector<std::vector<char>>& map);
        bool isAvailable() const { return state == IDLE && current_package == nullptr; }
        Package* getPackage() const { return current_package; }
        bool isDead() const { return state == DEAD; }
        int getRow() const { return r; }
        int getCol() const { return c; }
        char getSymbol() const { return symbol; }
        std::string getName() const { return name; }
        float getBattery() const { return current_battery; }
        float getMaxBattery() const { return max_battery; }
        int getSpeed() const { return speed; }
        float getConsumption() const { return consumption; }
        void printStatus() const;
        bool isProcessedDead() const { return processedDead; }
        void markProcessedDead() { processedDead = true; }
        int getcostPerTick() const { return cost_per_tick; }
        int getId() const { return id; }
        AgentState getState() const { return state; }
        void dropPackage() {
        if (current_package) {
            current_package->isAssigned = false; // Îl eliberăm înapoi în pool
            current_package = nullptr;
        }
        state = IDLE;
    }
};
