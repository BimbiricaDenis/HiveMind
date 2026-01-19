#pragma once
#include <string>

class SimulationConfig {

  private:

   SimulationConfig();
   int mapRows,mapCols;
   int maxTicks;
   int maxStations;
   int clientsCount;
   int drones,robots,scooters;
   int totalPackages;
   int spawnFrequency;

  public:
   static SimulationConfig& getInstance();
   SimulationConfig(const SimulationConfig&) = delete;
   void operator=(const SimulationConfig&) = delete;
   
   void loadConfig(const std::string& filename);

    int getRows() const;
    int getCols() const;
    int getMaxTicks() const;
    int getMaxStations() const;
    int getClientsCount() const;
    int getDrones() const;
    int getRobots() const;
    int getScooters() const;
    int getTotalPackages() const;
    int getSpawnFrequency() const;
};