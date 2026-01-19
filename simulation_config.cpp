#include "simulation_config.h"
#include <fstream>
#include <iostream>

// Private constructor just for safety reasons(sa nu crape programul daca nu e initializat corect sau daca nu citeste valorile bine din fisierul text)
SimulationConfig::SimulationConfig()
    : mapRows(20), mapCols(20), maxTicks(1000), maxStations(5),
      clientsCount(10), drones(0), robots(0), scooters(0),
      totalPackages(50), spawnFrequency(10) {}

SimulationConfig& SimulationConfig::getInstance() {
    static SimulationConfig instance;
    return instance;
}
//Getting configuration from a file
void SimulationConfig::loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if(!file.is_open()) {
        std::cerr << "Error opening config file: " << filename << std::endl;
        return;
    }
    std::string key;
    while(file >> key) {
        if (key.back() == ':') key.pop_back();

        if(key == "mapRows") file >> mapRows;
        else if(key == "mapCols") file >> mapCols;
        else if(key == "MAX_TICKS") file >> maxTicks;
        else if(key == "MAX_STATIONS") file >> maxStations;
        else if(key == "CLIENTS_COUNT") file >> clientsCount;
        else if(key == "DRONES") file >> drones;
        else if(key == "ROBOTS") file >> robots;
        else if(key == "SCOOTERS") file >> scooters;
        else if(key == "TOTAL_PACKAGES") file >> totalPackages;
        else if(key == "SPAWN_FREQUENCY") file >> spawnFrequency;
    }
    file.close();
    std::cout << "Configuration loaded from " << filename << std::endl;
    std::cout << "Map Size: " << mapRows << "x" << mapCols << std::endl;
}
//Getters implementation
int SimulationConfig::getRows() const {
    return mapRows;
}
int SimulationConfig::getCols() const {
    return mapCols;
}
int SimulationConfig::getMaxTicks() const {
    return maxTicks;
}
int SimulationConfig::getMaxStations() const {
    return maxStations;
}
int SimulationConfig::getClientsCount() const {
    return clientsCount;
}
int SimulationConfig::getDrones() const {
    return drones;
}
int SimulationConfig::getRobots() const {
    return robots;
}
int SimulationConfig::getScooters() const {
    return scooters;
}
int SimulationConfig::getTotalPackages() const {
    return totalPackages;
}
int SimulationConfig::getSpawnFrequency() const {
    return spawnFrequency;
}