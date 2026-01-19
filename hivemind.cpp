#include "hivemind.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
// Constructor and Destructor
HiveMind::HiveMind() : packageIdCounter(1) {}
HiveMind::~HiveMind() {
    for (auto pkg : allPackages) {
        delete pkg;
    }
}
//To be called once at the start to identify clients and stations
void HiveMind::scanMap(const std::vector<std::vector<char>>& map) {
    clientLocations.clear();
    stationLocations.clear();
    for(int i=0; i<map.size(); ++i) {
        for(int j=0; j<map[0].size(); ++j) {
            if (map[i][j] == CLIENT)  clientLocations.push_back({i, j});
            if( map[i][j] == STATION) stationLocations.push_back({i, j});
            if( map[i][j] == BASE)    basePosition = {i, j};
        }
    }
}
//Generates random packages at random locations
void HiveMind::generatePackages(int currentTick,int base_row,int base_col){
     int freq = SimulationConfig::getInstance().getSpawnFrequency();
     if(currentTick % freq ==0){
        if (clientLocations.empty()) return;
        int randIdx = rand() % clientLocations.size();
        auto client = clientLocations[randIdx];

        int reward = 200 + (rand() % 601); // 200-800
        int deadline = 10 + (rand() % 11); // 10-20 ticks

        Package* newPackage = new Package(packageIdCounter++, base_row, base_col, client.first, client.second, reward, currentTick, deadline);  
        allPackages.push_back(newPackage);

        std::cout << "[HIVEMIND] NEW PACKAGE generated! Dest: [" 
                  << client.first << "," << client.second << "] Reward: " << reward << "\n";
     }
}



void HiveMind::dispatch(std::vector<Agent*>& fleet, const std::vector<std::vector<char>>& map) {
    // For each unassigned package, try to assign it to an available agent
    for (auto pkg : allPackages) {
        if (pkg->isAssigned || pkg->isDelivered) continue;

        bool assigned = false;

        // We first start with the most economical agents(robots)
        for (auto agent : fleet) {
            if (agent->getName() != "Robot") continue; 
            if (!agent->isAvailable()) continue;

            // Check path feasibility
            auto path = Pathfinder::getPath(map, agent->getRow(), agent->getCol(), pkg->dest_row, pkg->dest_col, false);
            if (path.empty()) continue; // Cannot reach destination

            // Check battery (Rough estimate, since it charges on the way)
            // For robots we don't care about cost, it's negligible (1 credit). 
            // We take the package directly!
            agent->assignPackage(pkg, map);
            assigned = true;
            break; 
        }
        if (assigned) continue; // next package

        // We now try with scooters(medium cost)
        for (auto agent : fleet) {
            if (agent->getName() != "Scuter") continue;
            if (!agent->isAvailable()) continue;

            auto path = Pathfinder::getPath(map, agent->getRow(), agent->getCol(), pkg->dest_row, pkg->dest_col, false);
            if (path.empty()) continue;

            // For scooters it's okay, the cost is low. We accept almost anything.
            agent->assignPackage(pkg, map);
            assigned = true;
            break;
        }
        if (assigned) continue;

        // Finally, we try with drones(high cost, flying)
        for (auto agent : fleet) {
            if (agent->getName() != "Drona") continue;
            if (!agent->isAvailable()) continue;

            auto path = Pathfinder::getPath(map, agent->getRow(), agent->getCol(), pkg->dest_row, pkg->dest_col, true);
            if (path.empty()) continue; 

            // Profitability check
            int ticksNeeded = (path.size() / agent->getSpeed()) + 2;
            int costEstimativ = ticksNeeded * agent->getcostPerTick(); // Cost * 15

            if (pkg->reward < (costEstimativ * 1.2)) {
                continue; 
            }

            agent->assignPackage(pkg, map);
            assigned = true;
            std::cout << ">>> PREMIUM: Drona a preluat pachet URGENT/SCUMP " << pkg->id << "\n";
            break;
        }
    }
}

void HiveMind::monitorAgents(std::vector<Agent*>& fleet, const std::vector<std::vector<char>>& map) {
    for (auto agent : fleet) {
        if (agent->isDead()) continue;

        bool needsCharge = false;
        //If we have a package assigned
        if (agent->getPackage() != nullptr) {
            
            // I implemented this because sometimes agents would go to charge even with almost full battery while having a package
            if (agent->getBattery() > agent->getMaxBattery() * 0.50f) {
                continue; 
            }

            Package* pkg = agent->getPackage();
            // Calculate remaining path to destination
            auto remainingPath = Pathfinder::getPath(map, agent->getRow(), agent->getCol(), pkg->dest_row, pkg->dest_col, agent->canPassWalls());
            
            // Claculate battery needed to reach destination
            int ticksLeft = (remainingPath.size() / agent->getSpeed()) + 1;
            float batNeeded = ticksLeft * agent->getConsumption();

            // Abandon mechanism
            // If the agent is almost full (over 95%) AND still doesn't have enough battery to finish the trip
            // It means it's an impossible mission for him.
            if (agent->getBattery() >= agent->getMaxBattery() * 0.95f) {
                if (batNeeded > agent->getMaxBattery()) {
                    std::cout << "ABANDON: Agent " << agent->getName() << " (" << agent->getId() 
                              << ") is dropping the package " << pkg->id << " (Distance is too big!)\n";
                    
                    agent->dropPackage(); 
                    needsCharge = false; 
                    continue;             
                }
            }

           // Check if we need to charge to complete the delivery
            if (agent->getBattery() < (batNeeded + 5)) {
                needsCharge = true;
            }
        } 
        // If we don't have a package
        else {
            // Check if battery is below 95%,if yes -> go charge
            if (agent->getBattery() < agent->getMaxBattery() * 0.95f) {
                needsCharge = true;
            }
        }

        // Charging decision
        if (needsCharge) {
             // If we are not fully charged
             if (agent->getBattery() < agent->getMaxBattery()) { 
                 
                 std::pair<int, int> target = getNearestStation(agent->getRow(), agent->getCol());
                 
                 // Extra check - if we are already at the station -> charge
                 if (agent->getRow() == target.first && agent->getCol() == target.second) {
                     agent->charge();
                 } 
                 // If not, go to station
                 else {
                     // Find path to station and set as destination
                     agent->setDestination(target.first, target.second, map);
                 }
             }
        }
    }
}
// Function to print the status of the package queue for the penalty system
void HiveMind::printQueueStatus() {
    int pending = 0;
    for(auto p : allPackages) if(!p->isDelivered) pending++;
    std::cout << "Packages waiting to be delievered:" << pending << "\n";
}
// Function to process financials each tick
void HiveMind::ProcessFinancials(std::vector<Agent*>& fleet,int current_tick){
    for(auto agent : fleet){
        if(!agent->isDead() && agent->isMoving()){
            TotalProfit -= agent->getcostPerTick();
        }
        if(agent->isDead() && !agent->isProcessedDead()){
            TotalProfit -= 500;//Penalty for losing an agent
            agentsLost++;
            agent->markProcessedDead();
            std::cout << "[HIVEMIND] Penalty applied for losing Agent " << agent->getName() << "\n";
        }
    }
    for(auto pkg : allPackages){
        if(pkg->isDelivered && !pkg->isProcessed){
            reportDelivery(pkg,current_tick);
            pkg->isProcessed = true;
        }
    }
}
// Function to report a delivery and update financials,also including penalties for late deliveries
void HiveMind::reportDelivery(Package* pkg,int current_tick){
    int finalreward = pkg->reward;

    if(current_tick > pkg->deadline){
        finalreward -= 50;
        TotalProfit += finalreward;
        missedDeadlines++;
        std::cout << "[HIVEMIND] Package #" << pkg->id << " delivered LATE. Penalty applied.\n";
    }
    else{
        TotalProfit += finalreward;
        deliveredCount++;
        std::cout << "[HIVEMIND] Package #" << pkg->id << " delivered ON TIME. Reward: " << finalreward << "\n";
    }
}
// Function to generate a report at the end of the simulation
void HiveMind::generateRaport(const std::string& filename){
    std::ofstream out(filename);
    out << "=== SIMULATION REPORT ===\n";
    out << "Total Profit: " << TotalProfit << "\n";
    out << "Packages Delivered On Time: " << deliveredCount << "\n";
    out <<" Packages Delivered Late: " << missedDeadlines << "\n";
    out << "Agents Lost: " << agentsLost << "\n";
    
    int pending = 0;
    for(auto p : allPackages) if(!p->isDelivered) pending++;
    out << "Packages Pending Delivery: " << pending << "\n";
    out <<"Penalities Applied for Late Deliveries(-200 x " <<pending<< "): " << -(pending * 200) << "\n";  
    TotalProfit -= (pending * 200);
    out << "Final Profit after Penalties: " << TotalProfit << "\n";
    out.close();
    out<< "[HIVEMIND] Simulation report generated: " << filename << "\n";
}