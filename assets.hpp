#ifndef ASSETS_HPP
#define ASSETS_HPP

#include <raylib/raylib.h>
#include <core.hpp>

struct Assets
{
    Font micro5_ttf[5];
    Font quaver_ttf[5];

    Texture2D piece_png[12];

    Texture2D card_back_png;
    Texture2D card_plus5_png;
    Texture2D card_switch_png;

    Texture2D point_f_png;
    Texture2D point_s_png;

    Texture2D empty_rec_f_png;
    Texture2D empty_rec_s_png;
    NPatchInfo back_patch_info;

    Assets();
};

extern Assets *assets;

#endif