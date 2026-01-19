#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "simulation_config.h"
#include "procedural_map_generator.h"
#include "concrete_agents.h"
#include "hivemind.h" 
// Helper function to find base position
void findBase(const std::vector<std::vector<char>>& map, int& r, int& c) {
    for(int i=0; i<map.size(); ++i)
        for(int j=0; j<map[0].size(); ++j)
            if(map[i][j] == BASE) { r = i; c = j; return; }
}

int main() {
    srand(time(0)); // Seed for random number generation

    // Setup
    SimulationConfig::getInstance().loadConfig("simulation_setup.txt");
    IMAPGenerator* generator = new ProceduralMapGenerator();
    auto cityMap = generator->generateMap();
     std::cout << "Generated Map:" << std::endl;
  for (const auto& row : cityMap) {
    for (const auto& cell : row) {
      std::cout << cell << ' ';
    }
    std::cout << std::endl;
  }
    delete generator;

    int baseR, baseC;
    findBase(cityMap, baseR, baseC);

    // 2. Fleet
    std::vector<Agent*> fleet;
    int globalID = 1;
    for(int i=0; i<SimulationConfig::getInstance().getDrones(); ++i) 
        fleet.push_back(new Drone(globalID++, baseR, baseC));
    for(int i=0; i<SimulationConfig::getInstance().getRobots(); ++i) 
        fleet.push_back(new Robot(globalID++, baseR, baseC));
    for(int i=0; i<SimulationConfig::getInstance().getScooters(); ++i) 
        fleet.push_back(new Scooter(globalID++, baseR, baseC));
    // 3. Init HiveMind
    HiveMind brain;
    brain.scanMap(cityMap); // Scan map for stations and clients

    // 4. Simulation Loop
    int maxTicks = SimulationConfig::getInstance().getMaxTicks(); 
    
    for (int tick = 1; tick <= maxTicks; ++tick) {
        std::cout << "\n--- TICK " << tick << " | Profit: " << brain.getProfit() << " ---\n";

        // A. Generate packages
        brain.generatePackages(tick, baseR, baseC);
        //B. Dispatch packages
        brain.dispatch(fleet, cityMap);
        //C. Monitor Agents
        brain.monitorAgents(fleet, cityMap);

        // D. Update Agents (Movement, Charging, etc.)
        for(auto agent : fleet) {
            // 1. Update Normal
            agent->update(cityMap);
            
            // 2. WATCHDOG LOGIC (Watchdog for stuck agents)
            // Had a lot of bugs with agents getting stuck forever with packages resulting in no deliveries
            if (agent->getPackage() != nullptr && !agent->isDead()) {
                
                // If you have a package but are idle
                if (!agent->isMoving() && agent->getState() != CHARGING) { 
                    
                    // Try one last time to set the destination
                    Package* p = agent->getPackage();
                    
                    // Check if already at client (rare sync case)
                    if (agent->getRow() == p->dest_row && agent->getCol() == p->dest_col) {
                         // Force delivery
                         p->isDelivered = true;
                         agent->dropPackage();
                    } 
                    else {
                        // Force set destination again
                        agent->setDestination(p->dest_row, p->dest_col, cityMap);
                        
                        // If AFTER forcing still IDLE... it means the GPS really can't find a path
                        // Even if you say there is a path, the algorithm doesn't see it.
                        // SOLUTION: We take the package away!
                        if (!agent->isMoving()) {
                             std::cout << "WATCHDOG: Agent " << agent->getId() << " is mentally stuck. Confiscating package " << p->id << "!\n";
                             agent->dropPackage();
                        }
                    }
                }
            }
        }
        brain.ProcessFinancials(fleet, tick);
        brain.printQueueStatus();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

    }
    
    brain.generateRaport("simulation_report.txt");

    // Cleanup
    for(auto a : fleet) delete a;
    return 0;
}