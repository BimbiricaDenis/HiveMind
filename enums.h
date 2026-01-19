#pragma once

enum CellType {
    EMPTY = '.',
    WALL = '#',
    BASE = 'B',
    STATION = 'S',
    CLIENT = 'D'
};

enum AgentState {
    IDLE,
    MOVING,
    CHARGING,
    DEAD
};

enum AgentType {
    DRONE,
    ROBOT,
    SCOOTER
};
