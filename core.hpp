#ifndef CORE_HPP
#define CORE_HPP

#include <uci_engine.hpp>
#include <chess.hpp>
#include <utils.hpp>

#include <raylib/raylib.h>
#include <cstdint>

struct Core
{
    uint16_t win_width;
    uint16_t win_height;

    float vp_width;
    float vp_height;
    float vp_x;
    float vp_y;

    Camera2D default_cam;
    Camera2D vp_cam;

    UCIEngine *engine;
    Board *board;

    float delta_time;

    Core(uint16_t _win_width, uint16_t _win_height, float _aspect_ratio);
};

extern Core *core;

#endif