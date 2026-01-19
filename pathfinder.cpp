#include "pathfinder.h"

std::vector<std::pair<int,int>> Pathfinder::getPath(const std::vector<std::vector<char>>&map,int startRow,int startCol,int endRow,int endCol,bool canFly){
    int rows=map.size();
    int cols=map[0].size();

    std::queue<Point> q;
    q.push({startRow,startCol});

    std::map<Point,Point> parent;
    std::vector<std::vector<bool>> visited(rows,std::vector<bool>(cols,false));

    visited[startRow][startCol]=true;
    parent[{startRow,startCol}]={-1,-1};

    int dr[]={-1,1,0,0};// Up, Down
    int dc[]={0,0,-1,1};// Left, Right

    bool found=false;

    while(!q.empty()){
        Point current =q.front();
        q.pop();

        if(current.row==endRow && current.col==endCol){
            found=true;
            break;
        }

        for(int i=0;i<4;i++){
            int newRow=current.row + dr[i];
            int newCol=current.col + dc[i];

            if(newRow >= 0 && newRow < rows && newCol >=0 && newCol < cols){
                if(!visited[newRow][newCol]){
                    bool isObstacle = (map[newRow][newCol] == '#');
                    if(isObstacle && !canFly) continue;

                    visited[newRow][newCol]=true;
                    q.push({newRow,newCol});
                    parent[{newRow,newCol}]=current;
                }
            }
        }
    }
  std::vector<std::pair<int,int>> path;
   if(found){
    Point current = {endRow,endCol};
    while(!(current.row == startRow && current.col == startCol)){
        path.push_back({current.row,current.col});
        current = parent[current];
    }
    std::reverse(path.begin(),path.end());
   }
   return path;
}