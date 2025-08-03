#ifndef SCENE_GAME_INIT_HPP
#define SCENE_GAME_INIT_HPP

#include <core.hpp>
#include <assets.hpp>
#include <utils.hpp>
#include <anim_text.hpp>
#include <chess.hpp>
#include <autoplay.hpp>

#include <raylib/raylib.h>
#include <raylib/raymath.h>

#include <string>

struct PlayerCard
{
    Vector2 position;
    Vector2 size;
    float anim_time;
    float scale_y;
    float scale;
    bool hovered;
    bool selected;

    AnimTextLetter *id;

    AnimTextLetter *age_key;
    AnimTextLetter *age_value;
    AnimTextLetter *rating_key;
    AnimTextLetter *rating_value;
    AnimTextLetter *level_key;
    AnimTextLetter *level_value;

    AnimText *desc_text;

    PlayerCard(std::string _id, std::string _age, std::string _rating, std::string _level, std::string _desc);
    void Reset();
    void Process();
    void Render(Vector2 _position);
};

struct ColorBox
{
    uint8_t color;
    Vector2 position;
    Vector2 size;
    float anim_time;
    float scale;
    bool hovered;
    bool selected;

    ColorBox(uint8_t _color, Vector2 _pos);
    void Reset();
    void Process();
    void Render(Vector2 _pos);
};

struct SceneGameInit
{
    RenderTexture2D autoplay_render;
    float autoplay_offset;

    std::string id[8];
    std::string age[8];
    std::string pronoun[8];
    std::string rating[8];
    int level[8];
    std::string level_name[8];
    std::string desc[8];

    PlayerCard *player_card[8];

    uint8_t color;
    uint8_t player_index;

    AnimTextLetter *back_btn_text;
    Vector2 back_btn_pos;
    Vector2 back_btn_size;
    bool back_btn_hover;

    AnimTextLetter *choose_player_text;
    Vector2 choose_player_pos;
    AnimTextLetter *choose_color_text;
    Vector2 choose_color_pos;

    AnimTextLetter *start_btn_text;
    Vector2 start_btn_pos;
    Vector2 start_btn_size;
    float start_btn_hover;

    ColorBox *color_w;
    ColorBox *color_b;

    bool scene_done;
    bool start_game;

    SceneGameInit();
    void Reset();
    void Process();
    void Render();
};

#endif