#ifndef SCENE_HOME_HPP
#define SCENE_HOME_HPP

#include <core.hpp>
#include <assets.hpp>
#include <utils.hpp>
#include <anim_text.hpp>
#include <chess.hpp>
#include <autoplay.hpp>

#include <raylib/raylib.h>
#include <raylib/raymath.h>

#include <vector>
#include <algorithm>

struct ImageAnim
{
    Vector2 position;
    float scale;
    int w, h;
    float width;
    float height;
    std::vector<float> cell_anim_time;
    std::vector<float> cell_anim_delay;
    std::vector<float> cell_scale;
    std::vector<int> cell_order;
    std::vector<Vector2> cell_pos;
    std::vector<Color> cell_color;
    float cell_size;
    bool done;

    ImageAnim(const char *path, float _width, Vector2 _pos);
    void Reset();
    void Process();
    void Render();
};

enum OptionState
{
    PLAY_ENGINE,
    PLAY_ONLINE,
    CHANGE_OPTIONS,
    HELP,
    CREDITS,
    QUIT
};

struct SceneHome
{
    RenderTexture2D autoplay_render;
    float autoplay_offset;

    ImageAnim *raylib_anim;
    ImageAnim *title;
    float raylib_anim_time = 0;
    float title_anim_time = 0;

    AnimTextLetter *play_btn_text;
    Vector2 play_btn_pos;
    Vector2 play_btn_size;
    float play_btn_hover;

    AnimTextLetter *play_online_btn_text;
    Vector2 play_online_btn_pos;

    AnimTextLetter *options_btn_text;
    Vector2 options_btn_pos;
    Vector2 options_btn_size;
    float options_btn_hover;

    AnimTextLetter *help_btn_text;
    Vector2 help_btn_pos;
    Vector2 help_btn_size;
    float help_btn_hover;

    AnimTextLetter *credits_btn_text;
    Vector2 credits_btn_pos;
    Vector2 credits_btn_size;
    float credits_btn_hover;

    AnimTextLetter *quit_btn_text;
    Vector2 quit_btn_pos;
    Vector2 quit_btn_size;
    float quit_btn_hover;

    bool scene_done;
    OptionState option_sel;

    SceneHome();
    void Reset();
    void Process();
    void Render();
};

#endif