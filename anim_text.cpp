#include <anim_text.hpp>
#include <core.hpp>
#include <utils.hpp>

#include <sstream>

AnimText::AnimText(std::string _text, float _duration)
{
    if(_text.size() == 0)
    {
        return;
    }

    std::stringstream ss(_text);
    std::string word;
    while(ss>>word)
    {
        words.push_back(word);
        words.push_back(" ");
    }
    words.pop_back();
    duration = _duration;
    anim_time = 0;
    done = false;
}

void AnimText::Change(std::string _text, float _duration)
{
    words.clear();
    std::stringstream ss(_text);
    std::string word;
    while(ss>>word)
    {
        words.push_back(word);
        words.push_back(" ");
    }
    words.pop_back();
    duration = _duration;
    anim_time = 0;
    done = false;
}

void AnimText::Reset()
{
    anim_time = 0;
    done = false;
}

void AnimText::Forward()
{
    anim_time += core->delta_time;
    if(anim_time >= duration)
    {
        anim_time = duration;
    }
}

void AnimText::Backward()
{
    anim_time -= core->delta_time;
    if(anim_time < 0)
    {
        anim_time = 0;
    }
}

void AnimText::Process()
{
    if(!done)
    {
        anim_time += core->delta_time;
        if(anim_time >= duration)
        {
            anim_time = duration;
            done = true;
        }
    }
}

Vector2 AnimText::Render(Font _font, Vector2 _position, float _width, bool _center_h, bool _center_v, Color tint)
{
    int word_len = (anim_time/duration)*words.size()-1;
    
    float line_width = 0;
    float line_height = 0;

    float width = 0;
    float height = 0;

    for(int i = 0; i <= word_len; i++)
    {
        Vector2 word_size = MeasureTextEx(_font, words[i].c_str(), _font.baseSize, 0);
        if(line_width+word_size.x <= _width)
        {
            line_width += word_size.x;
            line_height = max(word_size.y, line_height);
        }
        else
        {
            width = max(width, line_width);
            height += line_height;

            line_width = word_size.x;
            line_height = word_size.y;
        }
    }
    width = max(width, line_width);
    height += line_height;

    Vector2 origin = _position;
    if(_center_h)
    {
        origin.x -= width*.5f;
    }
    if(_center_v)
    {
        origin.y -= height*.5f;
    }

    line_width = 0;
    line_height = 0;
    Vector2 cursor = origin;

    for(int i = 0; i <= word_len; i++)
    {
        Vector2 word_size = MeasureTextEx(_font, words[i].c_str(), _font.baseSize, 0);
        if(line_width+word_size.x <= _width)
        {
            line_width += word_size.x;
            line_height = max(word_size.y, line_height);
        }
        else
        {
            cursor.x = origin.x;
            cursor.y += line_height;

            line_width = word_size.x;
            line_height = word_size.y;
        }
        DrawText(_font, words[i].c_str(), cursor, tint);
        cursor.x += word_size.x;
    }

    return {width, height};
}


Vector2 AnimText::RenderStroked(Font _font, Vector2 _position, float _width, bool _center_h, bool _center_v, Color fill, Color stroke)
{
    int word_len = (anim_time/duration)*words.size()-1;
    
    float line_width = 0;
    float line_height = 0;

    float width = 0;
    float height = 0;

    for(int i = 0; i <= word_len; i++)
    {
        Vector2 word_size = MeasureTextEx(_font, words[i].c_str(), _font.baseSize, 0);
        if(line_width+word_size.x <= _width)
        {
            line_width += word_size.x;
            line_height = max(word_size.y, line_height);
        }
        else
        {
            width = max(width, line_width);
            height += line_height;

            line_width = word_size.x;
            line_height = word_size.y;
        }
    }
    width = max(width, line_width);
    height += line_height;

    Vector2 origin = _position;
    if(_center_h)
    {
        origin.x -= width*.5f;
    }
    if(_center_v)
    {
        origin.y -= height*.5f;
    }

    line_width = 0;
    line_height = 0;
    Vector2 cursor = origin;

    for(int i = 0; i <= word_len; i++)
    {
        Vector2 word_size = MeasureTextEx(_font, words[i].c_str(), _font.baseSize, 0);
        if(line_width+word_size.x <= _width)
        {
            line_width += word_size.x;
            line_height = max(word_size.y, line_height);
        }
        else
        {
            cursor.x = origin.x;
            cursor.y += line_height;

            line_width = word_size.x;
            line_height = word_size.y;
        }
        DrawTextStroked(_font, words[i].c_str(), cursor, fill, stroke);
        cursor.x += word_size.x;
    }

    return {width, height};
}

AnimTextLetter::AnimTextLetter(std::string _text, float _duration)
{
    text = _text;
    duration = _duration;
    Reset();
}

void AnimTextLetter::Change(std::string _text, float _duration)
{
    text = _text;
    duration = _duration;
    Reset();
}

void AnimTextLetter::Reset()
{
    anim_time = 0;
    done = false;
    closing = false;
}

void AnimTextLetter::Close()
{
    anim_time = duration;
    done = false;
    closing = true;
}

void AnimTextLetter::Forward()
{
    anim_time += core->delta_time;
    if(anim_time >= duration)
    {
        anim_time = duration;
    }
}

void AnimTextLetter::Backward()
{
    anim_time -= core->delta_time;
    if(anim_time < 0)
    {
        anim_time = 0;
    }
}

void AnimTextLetter::Process()
{
    if(!done)
    {
        if(closing)
        {
            anim_time -= core->delta_time;
            if(anim_time <= 0)
            {
                done = true;
                anim_time = 0;
            }
        }
        else
        {
            anim_time += core->delta_time;
            if(anim_time >= duration)
            {
                done = true;
                anim_time = duration;
            }
        }
    }
}

Vector2 AnimTextLetter::Render(Font _font, Vector2 _position, bool _center_h, bool _center_v, Color tint)
{
    int word_len = (anim_time/duration)*text.size();
    std::string sub_text = text.substr(0, word_len);
    DrawText(_font, sub_text.c_str(), _position, _center_h, _center_v, tint);
    return MeasureTextEx(_font, sub_text.c_str(), _font.baseSize, 0);
}

Vector2 AnimTextLetter::RenderRB(Font _font, Vector2 _position, Color tint)
{
    int word_len = (anim_time/duration)*text.size();
    std::string sub_text = text.substr(0, word_len);
    Vector2 size = MeasureTextEx(_font, sub_text.c_str(), _font.baseSize, 0);
    DrawTextPro(_font, sub_text.c_str(), _position, size, 0, _font.baseSize, 0, tint);
    return size;
}

Vector2 AnimTextLetter::Render(Font _font, Vector2 _position, float _font_size, bool _center_h, bool _center_v, Color tint)
{
    int word_len = (anim_time/duration)*text.size();
    std::string sub_text = text.substr(0, word_len);
    DrawText(_font, sub_text.c_str(), _position, _font_size, _center_h, _center_v, tint);
    return MeasureTextEx(_font, sub_text.c_str(), _font_size, 0);
}