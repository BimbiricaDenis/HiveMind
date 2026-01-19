#pragma once
#include "agent.h"

class Drone : public Agent {
     public:
        Drone(int id,int r,int c) : Agent(id,r,c) {
            name = "Drone";
            symbol = '^';
            speed = 3;
            max_battery = 100;
            consumption = 10;
            cost_per_tick = 15;
            capacity = 1;
            current_battery = max_battery;
        }
       bool canPassWalls() const override {
            return true;
        } 
};

class Robot : public Agent {
    public:
        Robot(int id,int r,int c) : Agent(id,r,c) {
            name = "Robot";
            symbol = 'R';
            speed = 1;
            max_battery = 300;
            consumption = 2;
            cost_per_tick = 1;
            capacity = 4;
            current_battery = max_battery;
        }
       bool canPassWalls() const override {
            return false;
        }
};

class Scooter : public Agent {
    public:
        Scooter(int id,int r,int c) : Agent(id,r,c) {
            name = "Scooter";
            symbol = 'S';
            speed = 2;
            max_battery = 200;
            consumption = 5;
            cost_per_tick = 4;
            capacity = 2;
            current_battery = max_battery;
        }
       bool canPassWalls() const override {
            return false;
        }
};