#include "Agent.h"
#include "PathFinder.h"
#include <iostream>
// Constructor
Agent::Agent(int id, int startR, int startC) 
    : id(id), r(startR), c(startC), state(IDLE), current_battery(0), current_package(nullptr), processedDead(false) {
}
//Function for charging
void Agent::charge() {
    if (state != DEAD) {
        state = CHARGING;
    }
}

// Function for moving the agent one step along its path
bool Agent::move() {
    if (state == DEAD) return false;
    
    // Ckeck if there's a path to follow
    if (path.empty()) {
        state = IDLE; 
        // Check delivery
        if (current_package != nullptr) {
            // Check if we are at the destination
            if (r == current_package->dest_row && c == current_package->dest_col) {
                current_package->isDelivered = true;
                std::cout << ">>> DELIVERY : Agent " << id << " at [" << r << "," << c << "]\n";
                current_package = nullptr;
            }
        }
        return false;
    }

    std::pair<int, int> nextStep = path.front();
    path.erase(path.begin());
    
    r = nextStep.first;
    c = nextStep.second;

    return (state != DEAD);
}

void Agent::setDestination(int destR, int destC, const std::vector<std::vector<char>>& map) {
    if (r == destR && c == destC) {
        state = IDLE;
        path.clear();
        return;
    }

    path = Pathfinder::getPath(map, r, c, destR, destC, canPassWalls());
    
    if (path.empty()) {
        state = IDLE;
        // Drop package if we can't reach destination
        if (current_package != nullptr) {
             dropPackage(); 
        }
    } else {
        state = MOVING;
    }
}

void Agent::update(const std::vector<std::vector<char>>& map) {
    if (state == DEAD) return;
    // Here we are forcing the agent to update its status each tick(i had some bugs with that)
    if (state == MOVING && path.empty()) {
        state = IDLE;
    }

    if (current_package != nullptr) {
        if (r == current_package->dest_row && c == current_package->dest_col) {
            // We are at the destination
            current_package->isDelivered = true;
            std::cout << ">>> DELIVERY SUCCESS (Update): Agent " << id << " delivered package " << current_package->id << "\n";
            
            // Packet delivered, clear it
            current_package = nullptr; 
            state = IDLE;
        }
    }
    char currentTile = map[r][c];
    if ((currentTile == 'B' || currentTile == 'S') && current_battery < max_battery) {
        charge(); 
    }

    // Charging logic
    if (state == CHARGING) {
        current_battery += (max_battery * 0.25f);
        if (current_battery >= max_battery) {
            current_battery = max_battery;
            state = IDLE;
        }
        return; 
    }
    // Re-evaluate path if idle but have a package
    if (state == IDLE && current_package != nullptr) {
        if (r != current_package->dest_row || c != current_package->dest_col) {
             setDestination(current_package->dest_row, current_package->dest_col, map);
        }
    }

    // Movement logic
    if (state == MOVING && !path.empty()) {
        for (int i = 0; i < speed; i++) {
            bool keepGoing = move();
            if (!keepGoing) break;
        }
        drainBattery(); 
    }
}
// Function to drain battery and handle death
void Agent::drainBattery() {
    current_battery -= consumption;
    if (current_battery <= 0) {
        current_battery = 0;
        state = DEAD;
        if(current_package != nullptr) {
            dropPackage(); // Release the stuck package(had also some bugs without this)
            std::cout << "!!! DEATH: Agent " << id << " died with package " << current_package->id << ". Resetting!\n";
        }
        std::cout << "!!! CRITICAL: Agent " << name << " (" << id << ") has DIED !!!\n";
    }
}
// Function to assign a package to the agent
    void Agent::assignPackage(Package* pkg, const std::vector<std::vector<char>>& map) {
        if(pkg == nullptr) return;

        current_package = pkg;
        pkg->isAssigned = true;

        std::cout << "Agent " << name << " (" << id << ") assigned to Package #" << pkg->id << "\n";

        setDestination(pkg->dest_row, pkg->dest_col, map);
    }
  // For printing the status of the agent(mainly for debugging)
    void Agent::printStatus() const {
    std::string stateStr;
    switch(state) {
        case IDLE: stateStr = "IDLE"; break;
        case MOVING: stateStr = "MOVING"; break;
        case CHARGING: stateStr = "CHARGING"; break;
        case DEAD: stateStr = "DEAD"; break;
    }
    std::cout << "[" << symbol << "] " << name << " #" << id 
              << " | Bat: " << current_battery << "/" << max_battery 
              << " | State: " << stateStr << "\n";
}