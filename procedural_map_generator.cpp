#include "procedural_map_generator.h"
#include "simulation_config.h"
#include <iostream>
#include <queue>
#include <ctime>

int ProceduralMapGenerator::randomInt(int min,int max) {
    static std::mt19937 rng(time(0));
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}
bool ProceduralMapGenerator::isValidMap(const std::vector<std::vector<char>>& map, int startRow, int startCol) {
    int rows = map.size();
    int cols = map[0].size();
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::queue<std::pair<int, int>> q;

    q.push({startRow, startCol});
    visited[startRow][startCol] = true;

    // Directions: up, down, left, right
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};
    // BFS to check connectivity
    while(!q.empty()){
        auto [r,c] = q.front();
        q.pop();
        
        for(int i=0;i<4;i++){

            int nr = r + dr[i];
            int nc = c + dc[i];

            if(nr >= 0 && nr < rows && nc >= 0 && nc < cols 
               && !visited[nr][nc] && map[nr][nc] != WALL) {
                visited[nr][nc] = true;
                q.push({nr, nc});
            }
        }
    }
      // Check if all non-wall cells are visited
      for(int i=0;i<rows;i++){
          for(int j=0;j<cols;j++){
              if((map[i][j] == CLIENT || map[i][j] ==STATION) && !visited[i][j]) {
                  return false;
              }
            }
        }
      return true;
    }
std::vector<std::vector<char>> ProceduralMapGenerator::generateMap() {
    SimulationConfig& config = SimulationConfig::getInstance();
    int rows = config.getRows();
    int cols = config.getCols();

    std::vector<std::vector<char>> map;
    bool valid = false;
    int attempts =0;

    while(!valid && attempts < 1000) {
        map.assign(rows, std::vector<char>(cols, EMPTY));
        // Randomly place the base
        int baseR= randomInt(0, rows - 1);
        int baseC= randomInt(0, cols - 1);
        map[baseR][baseC] = BASE;

        // Randomly place walls(10% of the map for a higher chance that the map is valid)
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                if(map[i][j]==EMPTY && randomInt(0,100)<=10){
                    map[i][j]=WALL;
                }
            }
        }

        // Randomly place stations
        for (int i = 0; i < config.getMaxStations(); i++) {
            int r, c;
            do { r = randomInt(0, rows - 1); c = randomInt(0, cols - 1); } 
            while (map[r][c] != EMPTY);
            map[r][c] = STATION;
        }

        // Randomly place clients
        for (int i = 0; i < config.getClientsCount(); i++) {
            int r, c;
            do { r = randomInt(0, rows - 1); c = randomInt(0, cols - 1); } 
            while (map[r][c] != EMPTY);
            map[r][c] = CLIENT;
        }

        if(isValidMap(map, baseR, baseC)) {
            valid = true;
        }
        else {
            attempts++;
        }
    }
    if(!valid) {
        std::cerr << "Failed to generate a valid map after " << attempts << " attempts." << std::endl;
    }
    return map;
}