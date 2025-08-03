#include <core.hpp>

Core::Core(uint16_t _win_width, uint16_t _win_height, float _aspect_ratio)
{
    win_width = _win_width;
    win_height = _win_height;
    float width = _win_width;
    float height = _win_height;

    if(width/height < _aspect_ratio)
    {
        vp_width = win_width;
        vp_height = vp_width*(1/_aspect_ratio);
    }
    else
    {
        vp_height = win_height;
        vp_width = vp_height*_aspect_ratio;
    }

    vp_x = (win_width-vp_width)/2.f;
    vp_y = (win_height-vp_height)/2.f;

    default_cam = {0};
    default_cam.zoom = 1;

    vp_cam.target = {0, 0};
    vp_cam.offset = {vp_x, vp_y};
    vp_cam.rotation = 0;
    vp_cam.zoom = 1;

    engine = new UCIEngine("./stockfish.exe");
    engine->RunCommand("uci", "uciok");

    board = new Board;
}