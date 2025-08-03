#ifndef ANIM_TEXT_HPP
#define ANIM_TEXT_HPP

#include <string>
#include <vector>
#include <raylib/raylib.h>

struct AnimText
{
    std::vector<std::string> words;
    float duration;
    float anim_time;
    bool done;

    AnimText(std::string _text, float _duration);
    void Change(std::string _text, float _duration);
    void Reset();
    void Process();
    void Forward();
    void Backward();
    Vector2 Render(Font _font, Vector2 _position, float _width, bool _center_h, bool _center_v, Color tint);
    Vector2 RenderStroked(Font _font, Vector2 _position, float _width, bool _center_h, bool _center_v, Color fill, Color stroke);
};


struct AnimTextLetter
{
    std::string text;
    float duration;
    float anim_time;
    bool closing;
    bool done;

    AnimTextLetter(std::string _text, float _duration);
    void Change(std::string _text, float _duration);
    void Reset();
    void Close();
    void Forward();
    void Backward();
    void Process();
    Vector2 Render(Font _font, Vector2 _position, bool _center_h, bool _center_v, Color tint);
    Vector2 RenderRB(Font _font, Vector2 _position, Color tint);
    Vector2 Render(Font _font, Vector2 _position, float _font_size, bool _center_h, bool _center_v, Color tint);
};

#endif