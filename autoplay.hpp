
#ifndef AUTOPLAY_HPP
#define AUTOPLAY_HPP

#include <core.hpp>
#include <assets.hpp>
#include <utils.hpp>
#include <chess.hpp>
#include <uci_engine.hpp>

#include <raylib/raylib.h>
#include <raylib/raymath.h>

struct AutoPlay
{
    UCIEngine *engine;
    Board *board;

    bool engine_thread_done;
    bool engine_thread_started;
    std::string engine_move;
    float engine_time;

    AutoPlay();
    void Reset();
    void Process();
    void Render(RenderTexture2D _render_target);
};

extern AutoPlay *autoplay;

#endif