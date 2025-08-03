#include <scene_game.hpp>
#include <algorithm>
#include <pthread.h>

GameBoard::GameBoard()
{
    squares_delay = std::vector<float>(64);
    squares_scale = std::vector<float>(64);
    squares_position = std::vector<Vector2>(64);
    squares_order = std::vector<uint8_t>(64);
    squares_anim_time = std::vector<float>(64);
    for(int i = 0; i < 64; i++)
    {
        squares_order[i] = i;
    }

    letter_delay = std::vector<float>(8);
    letter_anim_time = std::vector<float>(8);
    letter_scale = std::vector<float>(8);

    position = {core->vp_width*.5f, core->vp_height*.5f};
    squares_size = core->vp_height*.9f*.125f;
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            float x = (i+.5f)*squares_size;
            float y = (j+.5f)*squares_size;
            squares_position[j*8+i] = {x, y};
        }
    }

    std::sort(squares_order.begin(), squares_order.end(), [](const uint8_t &a, const uint8_t &b){
        float dista = a/8+a%8;
        float distb = b/8+b%8;
        return dista < distb;
    });

    Reset();
}

void GameBoard::Reset()
{
    state = BOARD_OPENING;
    for(int i : squares_order)
    {
        squares_delay[i] = 0.5f*(14-(i%8+i/8))/14.f;
        squares_scale[i] = 0.f;
        squares_anim_time[i] = 0.f;
    }
    for(int i = 0; i < 8; i++)
    {
        letter_delay[i] = 0.5f*(8-i)/8.f;
        letter_anim_time[i] = 0;
        letter_scale[i] = 0;
    }

    sel_index = 100;
    moves.clear();
    moves_anim_time.clear();
    moves_delay.clear();
    moves_order.clear();
    moves_opacity.clear();
    user_moved = false;
}

void GameBoard::Process()
{
    Vector2 m_pos = GetMousePosition();
    m_pos = GetScreenToWorld2D(m_pos, core->vp_cam);

    if(state == BOARD_OPENING)
    {
        bool finished = true;
        for(int i = 0; i < 64; i++)
        {
            if(squares_delay[i] > 0)
            {
                finished = false;
                squares_delay[i] -= core->delta_time;
            }
            else
            {
                if(squares_anim_time[i] <= 0.5f)
                {
                    finished = false;
                    squares_anim_time[i] += core->delta_time;
                    squares_scale[i] = easeOutCubic(min(0.5f, squares_anim_time[i])*2);
                }
            }
        }
        for(int i = 0; i < 8; i++)
        {
            if(letter_delay[i] > 0)
            {
                finished = false;
                letter_delay[i] -= core->delta_time;
            }
            else
            {
                if(letter_anim_time[i] <= 0.5f)
                {
                    finished = false;
                    letter_anim_time[i] += core->delta_time;
                    letter_scale[i] = easeOutCubic(min(0.5f, letter_anim_time[i])*2);
                }
            }
        }
        if(finished)
        {
            state = BOARD_NORMAL;
        }
    }
    else if(state == BOARD_NORMAL)
    {
        Vector2 board_origin = {position.x-squares_size*4, position.y-squares_size*4};
        float board_size = squares_size*8;
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(m_pos, {board_origin.x, board_origin.y, board_size, board_size}) && core->board->_current_color == player_color)
        {
            uint8_t i = (m_pos.x-board_origin.x)/squares_size;
            uint8_t j = (m_pos.y-board_origin.y)/squares_size;

            uint8_t select = j*8+i;
            if(player_color == COLOR_B)
            {
                select = (7-j)*8+(7-i);
            }

            uint8_t piece = core->board->At(select);
            if(piece == EMPTY || ColorOf(piece) != player_color)
            {
                if(IsValidSquare(sel_index))
                {
                    for(Move m : moves)
                    {
                        if(m._end == select)
                        {
                            user_moved = true;
                            curr_move = m;
                        }
                    }
                }
                sel_index = 100;
                moves.clear();
                moves_anim_time.clear();
                moves_delay.clear();
                moves_order.clear();
                moves_opacity.clear();
            }
            else
            {
                if(sel_index != select)
                {
                    sel_index = select;
                    sel_anim_time = 0;
                    sel_scale = 0;

                    moves = core->board->GetLegalMoves(sel_index);
                    moves_anim_time = std::vector<float>(moves.size(), 0);
                    moves_delay = std::vector<float>(moves.size(), 0);
                    moves_opacity = std::vector<uint8_t>(moves.size(), 0);
                    moves_order = std::vector<uint8_t>(moves.size());
                    for(int i = 0; i < moves.size(); i++)
                    {
                        moves_order[i] = i;
                    }

                    std::sort(moves_order.begin(), moves_order.end(), [&](const uint8_t &a, const uint8_t &b){
                        float dista = abs(sel_index/8-moves[a]._end/8)+abs(sel_index%8-moves[a]._end%8);
                        float distb = abs(sel_index/8-moves[b]._end/8)+abs(sel_index%8-moves[b]._end%8);
                        return dista < distb;
                    });
                    for(int i : moves_order)
                    {
                        moves_delay[i] = 0.125f*(abs(sel_index/8-moves[i]._end/8)+abs(sel_index%8-moves[i]._end%8))/14.f;
                    }
                }
                else
                {
                    sel_index = 100;
                    moves.clear();
                    moves_anim_time.clear();
                    moves_delay.clear();
                    moves_order.clear();
                    moves_opacity.clear();
                }
            }
        }
        if(IsValidSquare(sel_index))
        {
            if(sel_anim_time <= 0.25f)
            {
                sel_anim_time += core->delta_time;
                sel_scale = easeOutCubic(min(.25f, sel_anim_time)*4);
            }
            for(int i = 0; i < moves.size(); i++){
                if(moves_delay[i] > 0)
                {
                    moves_delay[i] -= core->delta_time;
                }
                else
                {
                    if(moves_anim_time[i] <= 0.125f)
                    {
                        moves_anim_time[i] += core->delta_time;
                        moves_opacity[i] = easeOutCubic(min(.125f, moves_anim_time[i])*8)*255;
                    }
                }
            }
        }
    }
    else if(state == ON_MAKE_MOVE)
    {
        if((curr_move._type != PROMOTION && anim_time <= .25f) || (curr_move._type == PROMOTION &&  anim_time <= .5f))
        {
            anim_time += core->delta_time;
        }
        else
        {
            core->board->MakeMove(curr_move);
            state = BOARD_NORMAL;
        }
    }
    else if(state == BOARD_CLOSING)
    {
        bool finished = true;
        for(int i = 0; i < 64; i++)
        {
            if(squares_delay[i] > 0)
            {
                finished = false;
                squares_delay[i] -= core->delta_time;
            }
            else
            {
                if(squares_anim_time[i] <= 0.5f)
                {
                    finished = false;
                    squares_anim_time[i] += core->delta_time;
                    squares_scale[i] = 1-easeOutCubic(min(0.5f, squares_anim_time[i])*2);
                }
            }
        }
        for(int i = 0; i < 8; i++)
        {
            if(letter_delay[i] > 0)
            {
                finished = false;
                letter_delay[i] -= core->delta_time;
            }
            else
            {
                if(letter_anim_time[i] <= 0.5f)
                {
                    finished = false;
                    letter_anim_time[i] += core->delta_time;
                    letter_scale[i] = 1-easeOutCubic(min(0.5f, letter_anim_time[i])*2);
                }
            }
        }
        if(finished)
        {
            state = BOARD_CLOSED;
        }
    }
}

void GameBoard::MakeMove(Move _move)
{
    state = ON_MAKE_MOVE;
    curr_move = _move;
    anim_time = 0;
}

void GameBoard::UnMakeMove(Move _move)
{
    
}

void GameBoard::Open(){

    state = BOARD_OPENING;
    for(int i : squares_order)
    {
        squares_delay[i] = 0.5f*(14-(i%8+i/8))/14.f;
        squares_scale[i] = 0.f;
        squares_anim_time[i] = 0.f;
    }
    for(int i = 0; i < 8; i++)
    {
        letter_delay[i] = 0.5f*(8-i)/8.f;
        letter_anim_time[i] = 0;
        letter_scale[i] = 0;
    }
}

void GameBoard::Close()
{
    state = BOARD_CLOSING;
    for(int i : squares_order)
    {
        squares_delay[i] = 0.5f*(14-(i%8+i/8))/14.f;
        squares_anim_time[i] = 0.f;
        squares_scale[i] = 1.f;
    }
    for(int i = 0; i < 8; i++)
    {
        letter_delay[i] = 0.5f*(8-i)/8.f;
        letter_anim_time[i] = 0;
        letter_scale[i] = 1.f;
    }
    sel_index = 100;
    moves.clear();
    moves_anim_time.clear();
    moves_delay.clear();
    moves_opacity.clear();
    moves_order.clear();
}

void GameBoard::Render()
{
    Vector2 board_origin = {position.x-squares_size*4, position.y-squares_size*4};
    for(int i = 0; i < 64; i++)
    {
        uint8_t x = i%8;
        uint8_t y = i/8;
        Rectangle rec = {squares_position[i].x+board_origin.x-squares_size*squares_scale[i]*.5f, squares_position[i].y+board_origin.y-squares_size*squares_scale[i]*.5f, squares_size*squares_scale[i], squares_size*squares_scale[i]};
        if(x%2 == y%2)
        {
            DrawRectangleRec(rec, {202, 233, 255, 255});
        }
        else
        {
            DrawRectangleRec(rec, {95, 168, 211, 255});
        }

        int s_i = i%8;
        int s_j = i/8;
        if(player_color == COLOR_B)
        {
            s_i = 7-s_i;
            s_j = 7-s_j;
        }
        uint8_t piece = core->board->At(s_i, s_j);
        if(piece != EMPTY && s_j*8+s_i != sel_index && (state != ON_MAKE_MOVE || (s_j*8+s_i != curr_move._start && s_j*8+s_i != curr_move._end)))
        {
            if(state == ON_MAKE_MOVE && curr_move._type == CASTLING)
            {
                uint8_t start, end;
                if(curr_move._end == G8 || curr_move._end == G1)
                {
                    start = East(curr_move._end);
                    end = West(curr_move._end);
                }
                else
                {
                    start = West(West(curr_move._end));
                    end = East(curr_move._end);
                }
                if(s_j*8+s_i != start && s_j*8+s_i != end)
                {
                    DrawTexturePro(assets->piece_png[piece-1], {0, 0, 16, 16}, {rec.x+rec.width*.1f, rec.y+rec.height*.1f, rec.width*.8f, rec.height*.8f}, {0, 0}, 0, WHITE);
                }
            }
            else if(state == ON_MAKE_MOVE && curr_move._type == ENPASSANT)
            {
                if((core->board->_current_color != COLOR_W || s_j*8+s_i != South(curr_move._end)) && (core->board->_current_color != COLOR_B || s_j*8+s_i != North(curr_move._end)))
                {
                    DrawTexturePro(assets->piece_png[piece-1], {0, 0, 16, 16}, {rec.x+rec.width*.1f, rec.y+rec.height*.1f, rec.width*.8f, rec.height*.8f}, {0, 0}, 0, WHITE);
                }
            }
            else
            {
                DrawTexturePro(assets->piece_png[piece-1], {0, 0, 16, 16}, {rec.x+rec.width*.1f, rec.y+rec.height*.1f, rec.width*.8f, rec.height*.8f}, {0, 0}, 0, WHITE);
            }
        }
    }
    for(int i = 0; i < 8; i++)
    {
        float x = board_origin.x-core->vp_height*.025f;
        float y = board_origin.y+(i+.5f)*squares_size;
        std::string text;
        if(player_color == COLOR_W)
        {
            text += char('8'-i);
        }
        else
        {
            text += char('1'+i);
        }
        DrawTextCentered(assets->quaver_ttf[0], text.c_str(), {x, y}, assets->quaver_ttf[0].baseSize*letter_scale[i], {202, 233, 255, 255});
    }
    for(int i = 0; i < 8; i++)
    {
        float x = board_origin.x+(i+.5f)*squares_size;
        float y = board_origin.y+8*squares_size+core->vp_height*.025f;
        std::string text;
        if(player_color == COLOR_W)
        {
            text += char('a'+i);
        }
        else
        {
            text += char('h'-i);
        }
        DrawTextCentered(assets->quaver_ttf[0], text.c_str(), {x, y}, assets->quaver_ttf[0].baseSize*letter_scale[i], {202, 233, 255, 255});
    }
    if(IsValidSquare(sel_index))
    {
        uint8_t s_i = sel_index%8;
        uint8_t s_j = sel_index/8;
        if(player_color == COLOR_B)
        {
            s_i = 7-s_i;
            s_j = 7-s_j;
        }
        float x = board_origin.x+(s_i+.5f)*squares_size;
        float y = board_origin.y+(s_j+.5f)*squares_size;
        float sel_size = squares_size*1.25f;
        Rectangle rec = {x-sel_size*sel_scale*0.5f, y-sel_size*sel_scale*0.5f, sel_size*sel_scale, sel_size*sel_scale};
        DrawTextureNPatch(assets->empty_rec_f_png, assets->back_patch_info, rec, {0, 0}, 0, {64, 145, 108, 150});
        DrawTextureNPatch(assets->empty_rec_s_png, assets->back_patch_info, rec, {0, 0}, 0, {149, 213, 178, 255});

        uint8_t piece = core->board->At(sel_index);
        float piece_scale = max(.8f, sel_scale);
        DrawTexturePro(assets->piece_png[piece-1], {0, 0, 16, 16}, {x, y, squares_size*piece_scale, squares_size*piece_scale}, {squares_size*piece_scale*.5f, squares_size*piece_scale*.5f}, 0, WHITE);

        for(int i = 0; i < moves.size(); i++){
            s_i = moves[i]._end%8;
            s_j = moves[i]._end/8;
            if(player_color == COLOR_B)
            {
                s_i = 7-s_i;
                s_j = 7-s_j;
            }
            x = board_origin.x+(s_i+.5f)*squares_size;
            y = board_origin.y+(s_j+.5f)*squares_size;
            DrawTexturePro(assets->point_f_png, {0, 0, 8, 8}, {x, y, squares_size*.3f, squares_size*.3f}, {squares_size*.15f, squares_size*.15f}, 0, {64, 145, 108, uint8_t(moves_opacity[i]*.5f)});
            DrawTexturePro(assets->point_s_png, {0, 0, 8, 8}, {x, y, squares_size*.3f, squares_size*.3f}, {squares_size*.15f, squares_size*.15f}, 0, {64, 145, 108, moves_opacity[i]});
        }
    }
    if(state == ON_MAKE_MOVE)
    {
        if(anim_time <= .25f || curr_move._type != PROMOTION)
        {
            uint8_t s_i;
            uint8_t s_j;
            float x;
            float y;
            uint8_t piece;
            float piece_scale;
            if(core->board->At(curr_move._end) != EMPTY || curr_move._type == ENPASSANT)
            {
                uint8_t s = curr_move._end;
                if(curr_move._type == ENPASSANT)
                {
                    if(core->board->_current_color == COLOR_W)
                    {
                        s = South(curr_move._end);
                    }
                    else
                    {
                        s = North(curr_move._end);
                    }
                }
                s_i = s%8;
                s_j = s/8;
                if(player_color == COLOR_B)
                {
                    s_i = 7-s_i;
                    s_j = 7-s_j;
                }
                x = board_origin.x+(s_i+.5f)*squares_size;
                y = board_origin.y+(s_j+.5f)*squares_size;

                piece = core->board->At(s);
                piece_scale = .8f*(1-easeOutCubic(min(.25f, anim_time)*4));
                DrawTexturePro(assets->piece_png[piece-1], {0, 0, 16, 16}, {x, y, squares_size*piece_scale, squares_size*piece_scale}, {squares_size*piece_scale*.5f, squares_size*piece_scale*.5f}, 0, WHITE);
            }

            s_i = curr_move._start%8;
            s_j = curr_move._start/8;
            if(player_color == COLOR_B)
            {
                s_i = 7-s_i;
                s_j = 7-s_j;
            }
            float from_x = board_origin.x+(s_i+.5f)*squares_size;
            float from_y = board_origin.y+(s_j+.5f)*squares_size;

            s_i = curr_move._end%8;
            s_j = curr_move._end/8;
            if(player_color == COLOR_B)
            {
                s_i = 7-s_i;
                s_j = 7-s_j;
            }
            float to_x = board_origin.x+(s_i+.5f)*squares_size;
            float to_y = board_origin.y+(s_j+.5f)*squares_size;

            piece = core->board->At(curr_move._start);

            float ease = easeOutCubic(min(.25f, anim_time)*4);
            x = to_x*(ease)+from_x*(1-ease);
            y = to_y*(ease)+from_y*(1-ease);
            piece_scale = .8f;
            DrawTexturePro(assets->piece_png[piece-1], {0, 0, 16, 16}, {x, y, squares_size*piece_scale, squares_size*piece_scale}, {squares_size*piece_scale*.5f, squares_size*piece_scale*.5f}, 0, WHITE);

            if(curr_move._type == CASTLING)
            {
                uint8_t start, end;
                if(curr_move._end == G8 || curr_move._end == G1)
                {
                    start = East(curr_move._end);
                    end = West(curr_move._end);
                }
                else
                {
                    start = West(West(curr_move._end));
                    end = East(curr_move._end);
                }
                s_i = start%8;
                s_j = start/8;
                if(player_color == COLOR_B)
                {
                    s_i = 7-s_i;
                    s_j = 7-s_j;
                }
                float from_x = board_origin.x+(s_i+.5f)*squares_size;
                float from_y = board_origin.y+(s_j+.5f)*squares_size;

                s_i = end%8;
                s_j = end/8;
                if(player_color == COLOR_B)
                {
                    s_i = 7-s_i;
                    s_j = 7-s_j;
                }
                float to_x = board_origin.x+(s_i+.5f)*squares_size;
                float to_y = board_origin.y+(s_j+.5f)*squares_size;

                piece = core->board->At(start);

                float ease = easeOutCubic(min(.25f, anim_time)*4);
                x = to_x*(ease)+from_x*(1-ease);
                y = to_y*(ease)+from_y*(1-ease);
                piece_scale = .8f;
                DrawTexturePro(assets->piece_png[piece-1], {0, 0, 16, 16}, {x, y, squares_size*piece_scale, squares_size*piece_scale}, {squares_size*piece_scale*.5f, squares_size*piece_scale*.5f}, 0, WHITE);
            }
        }
        else if(anim_time <= .375f)
        {
            uint8_t s_i = curr_move._end%8;
            uint8_t s_j = curr_move._end/8;
            if(player_color == COLOR_B)
            {
                s_i = 7-s_i;
                s_j = 7-s_j;
            }

            float x = board_origin.x+(s_i+.5f)*squares_size;
            float y = board_origin.y+(s_j+.5f)*squares_size;
            Rectangle rec = {x-squares_size*0.5f, y-squares_size*0.5f, squares_size, squares_size};
            DrawRectangleRec(rec, {27, 73, 101, 255});

            float scale_x = 1-min(.125f, anim_time-.25f)*8;
            rec = {x-squares_size*scale_x*0.5f, y-squares_size*0.5f, squares_size*scale_x, squares_size};
            if((curr_move._end%8)%2 == (curr_move._end/8)%2)
            {
                DrawRectangleRec(rec, {202, 233, 255, 255});
            }
            else
            {
                DrawRectangleRec(rec, {95, 168, 211, 255});
            }
            rec = {x-squares_size*.8f*scale_x*0.5f, y-squares_size*.8f*0.5f, squares_size*.8f*scale_x, squares_size*.8f};
            DrawTexturePro(assets->piece_png[core->board->At(curr_move._start)-1], {0, 0, 16, 16}, rec, {0, 0}, 0, WHITE);
        }
        else
        {
            uint8_t s_i = curr_move._end%8;
            uint8_t s_j = curr_move._end/8;
            if(player_color == COLOR_B)
            {
                s_i = 7-s_i;
                s_j = 7-s_j;
            }

            float x = board_origin.x+(s_i+.5f)*squares_size;
            float y = board_origin.y+(s_j+.5f)*squares_size;
            Rectangle rec = {x-squares_size*0.5f, y-squares_size*0.5f, squares_size, squares_size};
            DrawRectangleRec(rec, {27, 73, 101, 255});

            float scale_x = easeOutCubic(min(.125f, anim_time-.375f)*8);
            rec = {x-squares_size*scale_x*0.5f, y-squares_size*0.5f, squares_size*scale_x, squares_size};
            if((curr_move._end%8)%2 == (curr_move._end/8)%2)
            {
                DrawRectangleRec(rec, {202, 233, 255, 255});
            }
            else
            {
                DrawRectangleRec(rec, {95, 168, 211, 255});
            }
            rec = {x-squares_size*.8f*scale_x*0.5f, y-squares_size*.8f*0.5f, squares_size*.8f*scale_x, squares_size*.8f};
            DrawTexturePro(assets->piece_png[curr_move._inserted-1], {0, 0, 16, 16}, rec, {0, 0}, 0, WHITE);
        }
    }
}

SelectPromote::SelectPromote()
{
    position = {core->vp_width*.5f, core->vp_height*.5f};
    size.y = core->vp_height*.2f;
    size.x = size.y*4;
    Reset();
}

void SelectPromote::Reset()
{
    state = SP_OPEN;
    anim_time = 0;
    for(int i = 0; i < 4; i++)
    {
        piece_hover[i] = false;
        piece_scale[i] = 0;
        piece_anim_time[i] = 0;
        piece_delay[i] = .125f*((i+1)*.25f);
    }
}

void SelectPromote::Process()
{
    Vector2 m_pos = GetMousePosition();
    m_pos = GetScreenToWorld2D(m_pos, core->vp_cam);

    if(state == SP_OPEN)
    {
        if(anim_time <= .5f)
        {
            anim_time += core->delta_time;
            if(anim_time > .125f)
            {
                for(int i = 0; i < 4; i++)
                {
                    if(piece_delay[i] > 0)
                    {
                        piece_delay[i] -= core->delta_time;
                    }
                    else
                    {
                        if(piece_anim_time[i] <= .25f)
                        {
                            piece_anim_time[i] += core->delta_time;
                            piece_scale[i] = .6f*easeOutCubic(min(.25f, piece_anim_time[i])*4);
                        }
                    }
                }
            }
        }
        else
        {
            state = SP_SELECT;
        }
    }
    else if(state == SP_SELECT)
    {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            uint8_t piece[4];
            if(player_color == COLOR_W)
            {
                piece[0] = QW;
                piece[1] = BW;
                piece[2] = RW;
                piece[3] = NW;
            }
            else
            {
                piece[0] = QB;
                piece[1] = BB;
                piece[2] = RB;
                piece[3] = NB;
            }
            for(int i = 0; i < 4; i++){
                if(piece_hover[i])
                {
                    piece_sel = piece[i];
                    state = SP_DONE;
                }
            }
        }

        Rectangle bg_rec;
        bg_rec.width = size.x;
        bg_rec.height = size.y;
        bg_rec.x = position.x-bg_rec.width*.5f;
        bg_rec.y = position.y-bg_rec.height*.5f;

        for(int i = 0; i < 4; i++)
        {
            Rectangle piece_rec;
            piece_rec.width = bg_rec.height*piece_scale[i];
            piece_rec.height = bg_rec.height*piece_scale[i];
            piece_rec.x = bg_rec.x+bg_rec.height*(i+.5f)-piece_rec.width*.5f;
            piece_rec.y = bg_rec.y+bg_rec.height*.5f-piece_rec.height*.5f;
            if(CheckCollisionPointRec(m_pos, piece_rec))
            {
                piece_hover[i] = true;
            }
            else
            {
                piece_hover[i] = false;
            }

            if(piece_hover[i] && piece_scale[i] <= .8f)
            {
                piece_scale[i] += core->delta_time*2;
                piece_scale[i] = min(piece_scale[i], .8f);
            }

            if(!piece_hover[i] && piece_scale[i] >= .6f)
            {
                piece_scale[i] -= core->delta_time*2;
                piece_scale[i] = max(piece_scale[i], .6f);
            }
        }
    }
}

void SelectPromote::Render()
{
    Rectangle bg_rec;
    if(state == SP_OPEN)
    {
        float scale = easeOutCubic(min(.125f, anim_time)*8);
        bg_rec.width = size.x*scale;
        bg_rec.height = size.y*scale;
        bg_rec.x = position.x-bg_rec.width*.5f;
        bg_rec.y = position.y-bg_rec.height*.5f;
        DrawTextureNPatch(assets->empty_rec_f_png, assets->back_patch_info, bg_rec, {0, 0}, 0, {27, 73, 101, 150});
        DrawTextureNPatch(assets->empty_rec_s_png, assets->back_patch_info, bg_rec, {0, 0}, 0, WHITE);
    }
    else if(state == SP_SELECT)
    {
        bg_rec.width = size.x;
        bg_rec.height = size.y;
        bg_rec.x = position.x-bg_rec.width*.5f;
        bg_rec.y = position.y-bg_rec.height*.5f;
        DrawTextureNPatch(assets->empty_rec_f_png, assets->back_patch_info, bg_rec, {0, 0}, 0, {27, 73, 101, 150});
        DrawTextureNPatch(assets->empty_rec_s_png, assets->back_patch_info, bg_rec, {0, 0}, 0, WHITE);
    }

    uint8_t tex_id[4];
    if(player_color == COLOR_W)
    {
        tex_id[0] = QW-1;
        tex_id[1] = BW-1;
        tex_id[2] = RW-1;
        tex_id[3] = NW-1;
    }
    else
    {
        tex_id[0] = QB-1;
        tex_id[1] = BB-1;
        tex_id[2] = RB-1;
        tex_id[3] = NB-1;
    }
    for(int i = 0; i < 4; i++)
    {
        Rectangle piece_rec;
        piece_rec.x = bg_rec.x+bg_rec.height*(i+.5f);
        piece_rec.y = bg_rec.y+bg_rec.height*.5f;
        piece_rec.width = bg_rec.height*piece_scale[i];
        piece_rec.height = bg_rec.height*piece_scale[i];
        DrawTexturePro(assets->piece_png[tex_id[i]], {0, 0, 16, 16}, piece_rec, {piece_rec.width*.5f, piece_rec.height*.5f}, 0, WHITE);
    }
}

GameSelectCard::GameSelectCard()
{
    card1_size.y = core->vp_height*.25f;
    card1_size.x = card1_size.y*42.f/60.f;

    card2_size.y = core->vp_height*.25f;
    card2_size.x = card2_size.y*42.f/60.f;

    card1_pos.x = core->vp_width*.5f-card1_size.x;
    card1_pos.y = core->vp_height*.5f;

    card2_pos.x = core->vp_width*.5f+card2_size.x;
    card2_pos.y = core->vp_height*.5f;

    Reset();
}

void GameSelectCard::Reset()
{
    state = SC_OPEN;

    card1_anim_time = 0;
    card1_delay = 0;
    card1_hover = false;
    card1_scale = 0;

    card2_anim_time = 0;
    card2_delay = .125f;
    card2_hover = false;
    card2_scale = 0;

    int r = GetRandomValue(0, 1000000);
    if(r%2 == 0)
    {
        card1_type = CARD_SWITCH;
        card2_type = CARD_PLUS5;
    }
    else
    {
        card1_type = CARD_PLUS5;
        card2_type = CARD_SWITCH;
    }
}

void GameSelectCard::Process()
{
    Vector2 m_pos = GetMousePosition();
    m_pos = GetScreenToWorld2D(m_pos, core->vp_cam);

    if(state == SC_OPEN)
    {
        bool finished = true;
        if(card1_delay > 0)
        {
            finished = false;
            card1_delay -= core->delta_time;
        }
        else
        {
            if(card1_anim_time <= .25f)
            {
                finished = false;
                card1_anim_time += core->delta_time;
                card1_scale = easeOutCubic(min(.25f, card1_anim_time)*4);
            }
        }
        if(card2_delay > 0)
        {
            finished = false;
            card2_delay -= core->delta_time;
        }
        else
        {
            if(card2_anim_time <= .25f)
            {
                finished = false;
                card2_anim_time += core->delta_time;
                card2_scale = easeOutCubic(min(.25f, card2_anim_time)*4);
            }
        }
        if(finished)
        {
            state = SC_SELECT;
        }
    }
    else if(state == SC_SELECT)
    {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if(card1_hover)
            {
                card_sel = 1;
                card_sel_type = card1_type;
                state = SC_SHOW;

                card1_delay = 0;
                card1_anim_time = 0;
                card2_delay = .25f;
                card2_anim_time = 0;
                anim_time = 0;
            }
            if(card2_hover)
            {
                card_sel = 2;
                card_sel_type = card2_type;
                state = SC_SHOW;

                card1_delay = .25f;
                card1_anim_time = 0;
                card2_delay = 0;
                card2_anim_time = 0;
                anim_time = 0;
            }
        }

        Rectangle card1_rec;
        card1_rec.width = card1_size.x*card1_scale;
        card1_rec.height = card1_size.y*card1_scale;
        card1_rec.x = card1_pos.x-card1_rec.width*.5f;
        card1_rec.y = card1_pos.y-card1_rec.height*.5f;

        if(CheckCollisionPointRec(m_pos, card1_rec))
        {
            card1_hover = true;
        }
        else
        {
            card1_hover = false;
        }

        if(card1_hover && card1_scale < 1.25f)
        {
            card1_scale += core->delta_time*2;
            card1_scale = min(card1_scale, 1.25f);
        }
        if(!card1_hover && card1_scale > 1.f)
        {
            card1_scale -= core->delta_time*2;
            card1_scale = max(card1_scale, 1.f);
        }

        Rectangle card2_rec;
        card2_rec.width = card2_size.x*card2_scale;
        card2_rec.height = card2_size.y*card2_scale;
        card2_rec.x = card2_pos.x-card2_rec.width*.5f;
        card2_rec.y = card2_pos.y-card2_rec.height*.5f;

        if(CheckCollisionPointRec(m_pos, card2_rec))
        {
            card2_hover = true;
        }
        else
        {
            card2_hover = false;
        }

        if(card2_hover && card2_scale < 1.25f)
        {
            card2_scale += core->delta_time*2;
            card2_scale = min(card2_scale, 1.25f);
        }
        if(!card2_hover && card2_scale > 1.f)
        {
            card2_scale -= core->delta_time*2;
            card2_scale = max(card2_scale, 1.f);
        }
    }
    else if(state == SC_SHOW)
    {
        if(anim_time < .5f)
        {
            anim_time += core->delta_time;
            if(card1_delay > 0)
            {
                card1_delay -= core->delta_time;
            }
            else
            {
                if(card1_anim_time < .25f)
                {
                    card1_anim_time += core->delta_time;
                }
            }
            if(card2_delay > 0)
            {
                card2_delay -= core->delta_time;
            }
            else
            {
                if(card2_anim_time < .25f)
                {
                    card2_anim_time += core->delta_time;
                }
            }
        }
        else if(anim_time <= 1.5f)
        {
            anim_time += core->delta_time;
        }
        else
        {
            state = SC_CLOSE;
            anim_time = 0;
        }
    }
    else if(state == SC_CLOSE)
    {
        if(anim_time <= .25)
        {
            anim_time += core->delta_time;
        }
        else
        {
            state = SC_DONE;
        }
    }
}

void GameSelectCard::Render()
{
    if(state == SC_OPEN || state == SC_SELECT)
    {
        Rectangle card1_rec;
        card1_rec.width = card1_size.x*card1_scale;
        card1_rec.height = card1_size.y*card1_scale;
        card1_rec.x = card1_pos.x-card1_rec.width*.5f;
        card1_rec.y = card1_pos.y-card1_rec.height*.5f;

        Rectangle card2_rec;
        card2_rec.width = card2_size.x*card2_scale;
        card2_rec.height = card2_size.y*card2_scale;
        card2_rec.x = card2_pos.x-card2_rec.width*.5f;
        card2_rec.y = card2_pos.y-card2_rec.height*.5f;

        DrawTexturePro(assets->card_back_png, {0, 0, 42, 60}, card1_rec, {0, 0}, 0, WHITE);
        DrawTexturePro(assets->card_back_png, {0, 0, 42, 60}, card2_rec, {0, 0}, 0, WHITE);
    }
    else if(state == SC_SHOW)
    {
        if(anim_time < .5f)
        {
            if(card1_anim_time <= .125f)
            {
                float scale_x = 1-easeOutCubic(min(card1_anim_time, .125f)*8);
                Rectangle card1_rec;
                card1_rec.width = card1_size.x*card1_scale*scale_x;
                card1_rec.height = card1_size.y*card1_scale;
                card1_rec.x = card1_pos.x-card1_rec.width*.5f;
                card1_rec.y = card1_pos.y-card1_rec.height*.5f;
                DrawTexturePro(assets->card_back_png, {0, 0, 42, 60}, card1_rec, {0, 0}, 0, WHITE);
            }
            else
            {
                float scale_x = easeOutCubic(min(card1_anim_time-.125f, .125f)*8);
                Rectangle card1_rec;
                card1_rec.width = card1_size.x*card1_scale*scale_x;
                card1_rec.height = card1_size.y*card1_scale;
                card1_rec.x = card1_pos.x-card1_rec.width*.5f;
                card1_rec.y = card1_pos.y-card1_rec.height*.5f;
                if(card1_type == CARD_PLUS5)
                {
                    DrawTexturePro(assets->card_plus5_png, {0, 0, 42, 60}, card1_rec, {0, 0}, 0, WHITE);
                }
                else
                {
                    DrawTexturePro(assets->card_switch_png, {0, 0, 42, 60}, card1_rec, {0, 0}, 0, WHITE);
                }
            }

            if(card2_anim_time <= .125f)
            {
                float scale_x = 1-easeOutCubic(min(card2_anim_time, .125f)*8);
                Rectangle card2_rec;
                card2_rec.width = card2_size.x*card2_scale*scale_x;
                card2_rec.height = card2_size.y*card2_scale;
                card2_rec.x = card2_pos.x-card2_rec.width*.5f;
                card2_rec.y = card2_pos.y-card2_rec.height*.5f;

                DrawTexturePro(assets->card_back_png, {0, 0, 42, 60}, card2_rec, {0, 0}, 0, WHITE);
            }
            else
            {
                float scale_x = easeOutCubic(min(card2_anim_time, .125f)*8);
                Rectangle card2_rec;
                card2_rec.width = card2_size.x*card2_scale*scale_x;
                card2_rec.height = card2_size.y*card2_scale;
                card2_rec.x = card2_pos.x-card2_rec.width*.5f;
                card2_rec.y = card2_pos.y-card2_rec.height*.5f;
                if(card2_type == CARD_PLUS5)
                {
                    DrawTexturePro(assets->card_plus5_png, {0, 0, 42, 60}, card2_rec, {0, 0}, 0, WHITE);
                }
                else
                {
                    DrawTexturePro(assets->card_switch_png, {0, 0, 42, 60}, card2_rec, {0, 0}, 0, WHITE);
                }
            }
        }
        else if(anim_time <= .75f)
        {
            Rectangle card1_rec;
            card1_rec.width = card1_size.x*card1_scale;
            card1_rec.height = card1_size.y*card1_scale;
            card1_rec.x = card1_pos.x-card1_rec.width*.5f;
            card1_rec.y = card1_pos.y-card1_rec.height*.5f;
            if(card1_type == CARD_PLUS5)
            {
                DrawTexturePro(assets->card_plus5_png, {0, 0, 42, 60}, card1_rec, {0, 0}, 0, WHITE);
            }
            else
            {
                DrawTexturePro(assets->card_switch_png, {0, 0, 42, 60}, card1_rec, {0, 0}, 0, WHITE);
            }

            Rectangle card2_rec;
            card2_rec.width = card2_size.x*card2_scale;
            card2_rec.height = card2_size.y*card2_scale;
            card2_rec.x = card2_pos.x-card2_rec.width*.5f;
            card2_rec.y = card2_pos.y-card2_rec.height*.5f;
            if(card2_type == CARD_PLUS5)
            {
                DrawTexturePro(assets->card_plus5_png, {0, 0, 42, 60}, card2_rec, {0, 0}, 0, WHITE);
            }
            else
            {
                DrawTexturePro(assets->card_switch_png, {0, 0, 42, 60}, card2_rec, {0, 0}, 0, WHITE);
            }
        }
        else if(anim_time <= 1.f)
        {
            float scale1 = 1;
            Vector2 offset1 = card1_pos;

            float scale2 = 1;
            Vector2 offset2 = card2_pos;

            float ease = easeOutCubic(min(.25f, anim_time-.75f)*4);

            if(card_sel == 1)
            {
                scale2 = 1-ease;
                offset1.x = card1_pos.x*(1-ease)+core->vp_width*.5f*ease;
                offset1.y = card1_pos.y*(1-ease)+core->vp_height*.5f*ease;
            }
            else
            {
                scale1 = 1-ease;
                offset2.x = card2_pos.x*(1-ease)+core->vp_width*.5f*ease;
                offset2.y = card2_pos.y*(1-ease)+core->vp_height*.5f*ease;
            }

            Rectangle card1_rec;
            card1_rec.width = card1_size.x*card1_scale*scale1;
            card1_rec.height = card1_size.y*card1_scale*scale1;
            card1_rec.x = offset1.x-card1_rec.width*.5f;
            card1_rec.y = offset1.y-card1_rec.height*.5f;
            if(card1_type == CARD_PLUS5)
            {
                DrawTexturePro(assets->card_plus5_png, {0, 0, 42, 60}, card1_rec, {0, 0}, 0, WHITE);
            }
            else
            {
                DrawTexturePro(assets->card_switch_png, {0, 0, 42, 60}, card1_rec, {0, 0}, 0, WHITE);
            }

            Rectangle card2_rec;
            card2_rec.width = card2_size.x*card2_scale*scale2;
            card2_rec.height = card2_size.y*card2_scale*scale2;
            card2_rec.x = offset2.x-card2_rec.width*.5f;
            card2_rec.y = offset2.y-card2_rec.height*.5f;
            if(card2_type == CARD_PLUS5)
            {
                DrawTexturePro(assets->card_plus5_png, {0, 0, 42, 60}, card2_rec, {0, 0}, 0, WHITE);
            }
            else
            {
                DrawTexturePro(assets->card_switch_png, {0, 0, 42, 60}, card2_rec, {0, 0}, 0, WHITE);
            }
        }
        else
        {
            Rectangle card_rec;
            card_rec.width = card1_size.x*1.25f;
            card_rec.height = card1_size.y*1.25f;
            card_rec.x = core->vp_width*.5f-card_rec.width*.5f;
            card_rec.y = core->vp_height*.5f-card_rec.height*.5f;
            if(card_sel_type == CARD_PLUS5)
            {
                DrawTexturePro(assets->card_plus5_png, {0, 0, 42, 60}, card_rec, {0, 0}, 0, WHITE);
            }
            else
            {
                DrawTexturePro(assets->card_switch_png, {0, 0, 42, 60}, card_rec, {0, 0}, 0, WHITE);
            }
        }
    }
    else
    {
        float scale = 1-easeOutCubic(min(.25f, anim_time)*4);
        Rectangle card_rec;
        card_rec.width = card1_size.x*1.25f*scale;
        card_rec.height = card1_size.y*1.25f*scale;
        card_rec.x = core->vp_width*.5f-card_rec.width*.5f;
        card_rec.y = core->vp_height*.5f-card_rec.height*.5f;
        if(card_sel_type == CARD_PLUS5)
        {
            DrawTexturePro(assets->card_plus5_png, {0, 0, 42, 60}, card_rec, {0, 0}, 0, WHITE);
        }
        else
        {
            DrawTexturePro(assets->card_switch_png, {0, 0, 42, 60}, card_rec, {0, 0}, 0, WHITE);
        }
    }
}

GamePlayerCard::GamePlayerCard(std::string _id, std::string _rating, std::string _pass, float _duration)
{
    id = new AnimTextLetter(_id, _duration);
    rating = new AnimTextLetter(_rating, _duration);
    pass = new AnimTextLetter(_pass, _duration);
    color = new AnimTextLetter("", _duration);

    size.x = core->vp_height*.35f;
    size.y = core->vp_height*.25f;

    state = PC_OPEN;
    anim_state = 0;
}

void GamePlayerCard::Reset()
{
    id->Reset();
    rating->Reset();
    pass->Reset();
    color->Reset();

    state = PC_OPEN;
    anim_state = 0;
    scale = 0;
}

void GamePlayerCard::Open()
{
    Reset();
}

void GamePlayerCard::Close()
{
    state = PC_CLOSE;
}

void GamePlayerCard::Process()
{
    if(state == PC_OPEN)
    {
        if(anim_state <= .25f)
        {
            anim_state += core->delta_time;
            scale = easeOutCubic(min(.25f, anim_state)*4);
        }
        else
        {
            id->Process();
            rating->Process();
            pass->Process();
            color->Process();
        }
    }
    else
    {
        if(anim_state > 0)
        {
            anim_state -= core->delta_time;
            scale = easeOutCubic(max(0, anim_state)*4);
        }
    }
}

void GamePlayerCard::Render()
{
    Rectangle back_rec;
    back_rec.x = position.x;
    back_rec.y = position.y;
    back_rec.width = size.x*scale;
    back_rec.height = size.y*scale;

    DrawTextureNPatch(assets->empty_rec_f_png, assets->back_patch_info, back_rec, {0, 0}, 0, BLACK|50);
    DrawTextureNPatch(assets->empty_rec_s_png, assets->back_patch_info, back_rec, {0, 0}, 0, Color{202, 233, 255, 255}|150);

    Vector2 pos = {position.x+back_rec.width*.1f, position.y+back_rec.height*.15f};
    DrawText(assets->quaver_ttf[0], "id :", pos, assets->quaver_ttf[0].baseSize*scale, false, false, {202, 233, 255, 255});

    pos.x = position.x+back_rec.width*.25f;
    id->Render(assets->quaver_ttf[0], pos, assets->quaver_ttf[0].baseSize*scale, false, false, {202, 233, 255, 255});

    pos =  {position.x+back_rec.width*.1f, position.y+back_rec.height*.35f};
    DrawText(assets->quaver_ttf[0], "rating :", pos, assets->quaver_ttf[0].baseSize*scale, false, false, {202, 233, 255, 255});

    pos.x = position.x+back_rec.width*.4f;
    rating->Render(assets->quaver_ttf[0], pos, assets->quaver_ttf[0].baseSize*scale, false, false, {202, 233, 255, 255});

    pos = {position.x+back_rec.width*.1f, position.y+back_rec.height*.55f};
    DrawText(assets->quaver_ttf[0], "pass :", pos, assets->quaver_ttf[0].baseSize*scale, false, false, {202, 233, 255, 255});

    pos.x = position.x+back_rec.width*.35f;
    pass->Render(assets->quaver_ttf[0], pos, assets->quaver_ttf[0].baseSize*scale, false, false, {202, 233, 255, 255});

    pos = {position.x+back_rec.width*.1f, position.y+back_rec.height*.75f};
    DrawText(assets->quaver_ttf[0], "color :", pos, assets->quaver_ttf[0].baseSize*scale, false, false, {202, 233, 255, 255});

    pos.x = position.x+back_rec.width*.375f;
    color->Render(assets->quaver_ttf[0], pos, assets->quaver_ttf[0].baseSize*scale, false, false, {202, 233, 255, 255});
}

Game::Game()
{
    game_board = new GameBoard;
    select_promote = new SelectPromote;
    game_select_card = new GameSelectCard;

    remaining_moves_text = new AnimTextLetter("", .25f);

    plus5_text = new AnimText("", .25f);
    use_pass_text = new AnimText("", .25f);

    pass_btn1_text = new AnimText("", .25f);
    pass_btn2_text = new AnimText("", .25f);

    pass_btn1_pos = {core->vp_width*.3f, core->vp_height*.6f};
    pass_btn2_pos = {core->vp_width*.7f, core->vp_height*.6f};

    player1_card = new GamePlayerCard("you", "--", "not used", .5f);
    player1_card->position = {core->vp_height*1.38f, core->vp_height*.95f-player1_card->size.y};

    player2_card = new GamePlayerCard("", "", "not used", .5f);
    player2_card->position = {core->vp_height*1.38f, core->vp_height*.05f};

    select_card_text1 = new AnimText("", .25f);
    select_card_text2 = new AnimText("", .25f);
    select_card_text3 = new AnimText("", .25f);
    select_card_text4 = new AnimText("", .25f);
    select_card_text5 = new AnimText("", .25f);

    skip_text = new AnimTextLetter("( TAP )", .25f);
}

void Game::Reset(uint8_t _player_color, std::string _op_id, std::string _op_rate, std::string _op_pronoun, uint8_t _op_level)
{
    core->board->Reset();
    // core->board->SetPositionFromFENString("8/P7/8/8/8/8/k7/7K w - - 0 1"); // promotion move
    // core->board->SetPositionFromFENString("7k/5ppp/8/6N1/8/8/B4PPP/B4RK1 w - - 0 1"); // mate in 2 moves
    core->engine->SetLevel(_op_level);
    core->engine->RunVoidCommand("ucinewgame");
    core->engine->SetPosition(core->board->GetFENString());
    
    game_board->Reset();
    select_promote->Reset();
    game_select_card->Reset();

    player_color = _player_color;
    game_board->player_color = _player_color;
    select_promote->player_color = _player_color;

    player_have_pass = true;
    engine_have_pass = true;

    state = PLAYING;

    engine_thread_done = false;
    engine_thread_started = false;
    engine_done = false;
    player_done = false;

    remaining_full_moves = 10;

    plus5_text->Change("five more bonus moves added!", .25f);
    use_pass_text->Change("you have the pass. do you want to use it?", .25f);

    player2_card->id->Change(_op_id, .5f);
    player2_card->rating->Change(_op_rate, .5f);

    select_card_text1->Change("No one have the pass, It's time to switch sides!", .25f);
    select_card_text2->Change(_op_id+" used "+_op_pronoun+" pass.", .25f);
    select_card_text3->Change(_op_id+" not used "+_op_pronoun+" pass.", .25f);
    select_card_text4->Change(_op_id+" also used "+_op_pronoun+" pass", .25f);
    select_card_text5->Change(_op_id+" is thinking...", .25f);

    skip_text->Reset();

    pass_btn1_text->Change("use the pass.", .25f);
    pass_btn1_size = {0};
    pass_btn1_hover = false;

    pass_btn2_text->Change("don't use the pass.", .25f);
    pass_btn2_size = {0};
    pass_btn2_hover = false;

    if(player_color == COLOR_W)
    {
        player1_card->color->Change("white", .5f);
        player2_card->color->Change("black", .5f);
    }
    else
    {
        player1_card->color->Change("black", .5f);
        player2_card->color->Change("white", .5f);
    }
    player1_card->Reset();
    player2_card->Reset();

    remaining_moves_text->Change(std::to_string(remaining_full_moves), .25f);
    remaining_moves_anim_time = 0;

    text_anim_time = 0;
}

static void* EngineMakeMove(void *obj)
{
    Game *game = (Game*)obj;

    core->engine->SetPosition(core->board->GetFENString());
    game->engine_move = core->board->GetMoveFromString(core->engine->GetBestMove());
    game->engine_thread_done = true;

    return nullptr;
}

static void* EngineGetMate(void *obj)
{
    Game *game = (Game*)obj;

    core->engine->SetPosition(core->board->GetFENString());
    game->mate = core->engine->GetMate();
    game->engine_thread_done = true;

    return nullptr;
}

void Game::Process()
{
    Vector2 m_pos = GetMousePosition();
    m_pos = GetScreenToWorld2D(m_pos, core->vp_cam);
    float mouse_handled = false;

    if(state == PLAYING)
    {
        game_board->Process();
        if(game_board->state == BOARD_NORMAL)
        {
            if(core->board->IsGameFinished())
            {
                state = GAME_CLOSING;
                return;
            }
            else if(remaining_full_moves == 0)
            {
                state = SELECT_CARD;
                game_select_card->Reset();
                return;
            }
            if(core->board->_current_color == player_color)
            {
                if(game_board->user_moved)
                {
                    if(game_board->curr_move._type != PROMOTION)
                    {
                        game_board->user_moved = false;
                        if(core->board->_current_color == COLOR_B)
                        {
                            remaining_full_moves -= 1;
                            remaining_moves_text->Change(std::to_string(remaining_full_moves), .25f);
                        }
                        game_board->MakeMove(game_board->curr_move);
                    }
                    else
                    {
                        game_board->user_moved = false;
                        state = SELECT_PROMOTION;
                        select_promote->Reset();
                    }
                }
            }
            else
            {
                if(engine_thread_done)
                {
                    engine_thread_started = false;
                    engine_thread_done = false;
                    if(core->board->_current_color == COLOR_B)
                    {
                        remaining_full_moves -= 1;
                        remaining_moves_text->Change(std::to_string(remaining_full_moves), .25f);
                    }
                    game_board->MakeMove(engine_move);
                }
                else
                {
                    if(!engine_thread_started)
                    {
                        engine_thread_started = true;
                        pthread_t move_thread;
                        pthread_create(&move_thread, nullptr, EngineMakeMove, (void*)this);
                    }
                }
            }
        }
    }
    else if(state == SELECT_PROMOTION)
    {
        select_promote->Process();
        if(select_promote->state == SP_DONE)
        {
            state = PLAYING;
            game_board->curr_move._inserted = select_promote->piece_sel;
            if(core->board->_current_color == COLOR_B)
            {
                remaining_full_moves -= 1;
                remaining_moves_text->Change(std::to_string(remaining_full_moves), .25f);
            }
            game_board->MakeMove(game_board->curr_move);
        }
    }
    else if(state == SELECT_CARD)
    {
        if(game_board->state != BOARD_CLOSED)
        {
            if(game_board->state != BOARD_CLOSING)
            {
                game_board->Close();
                player1_card->Close();
                player2_card->Close();
            }
            game_board->Process();
            player1_card->Process();
            player2_card->Process();
        }
        else
        {
            if(game_select_card->state != SC_DONE)
            {
                game_select_card->Process();
            }
            else
            {
                if(game_select_card->card_sel_type == CARD_SWITCH)
                {
                    if(engine_have_pass)
                    {
                        if(!engine_done)
                        {
                            if(engine_thread_done)
                            {
                                engine_thread_done = false;
                                engine_thread_started = false;
                                if(player_color == COLOR_W)
                                {
                                    if(mate < 0 && mate >= -10)
                                    {
                                        engine_use_card = true;
                                    }
                                    else
                                    {
                                        engine_use_card = false;
                                    }
                                }
                                else
                                {
                                    if(mate > 0 && mate <= 10)
                                    {
                                        engine_use_card = true;
                                    }
                                    else
                                    {
                                        engine_use_card = false;
                                    }
                                }
                                engine_done = true;
                            }
                            else
                            {
                                if(!engine_thread_started)
                                {
                                    engine_thread_started = true;
                                    pthread_t thread;
                                    pthread_create(&thread, nullptr, EngineGetMate, (void*)this);
                                }
                            }
                        }
                    }
                    else
                    {
                        engine_use_card = false;
                        engine_done = true;
                    }

                    if(player_have_pass)
                    {
                        if(!player_done)
                        {
                            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                            {
                                if(pass_btn1_hover)
                                {
                                    player_use_card = true;
                                    player_done = true;
                                    mouse_handled = true;
                                }
                                else if(pass_btn2_hover)
                                {
                                    player_use_card = false;
                                    player_done = true;
                                    mouse_handled = true;
                                }
                            }

                            use_pass_text->Process();
                            pass_btn1_text->Process();
                            pass_btn2_text->Process();

                            Rectangle btn1_rec;
                            btn1_rec.width = pass_btn1_size.x;
                            btn1_rec.height = pass_btn1_size.y;
                            btn1_rec.x = pass_btn1_pos.x-btn1_rec.width*.5f;
                            btn1_rec.y = pass_btn1_pos.y-btn1_rec.height*.5f;
                            if(CheckCollisionPointRec(m_pos, btn1_rec))
                            {
                                pass_btn1_hover = true;
                            }
                            else
                            {
                                pass_btn1_hover = false;
                            }

                            Rectangle btn2_rec;
                            btn2_rec.width = pass_btn2_size.x;
                            btn2_rec.height = pass_btn2_size.y;
                            btn2_rec.x = pass_btn2_pos.x-btn2_rec.width*.5f;
                            btn2_rec.y = pass_btn2_pos.y-btn2_rec.height*.5f;
                            if(CheckCollisionPointRec(m_pos, btn2_rec))
                            {
                                pass_btn2_hover = true;
                            }
                            else
                            {
                                pass_btn2_hover = false;
                            }
                        }
                    }
                    else
                    {
                        player_use_card = false;
                        player_done = true;
                    }

                    if(player_done)
                    {
                        if(engine_done)
                        {
                            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !mouse_handled)
                            {
                                text_anim_time = 3;
                            }
                            if(text_anim_time <= 2.25f)
                            {
                                text_anim_time += core->delta_time;
                                if(!player_have_pass && !engine_have_pass)
                                {
                                    select_card_text1->Process();
                                }
                                else if(player_use_card && engine_use_card)
                                {
                                    select_card_text4->Process();
                                }
                                else if(engine_use_card)
                                {
                                    select_card_text2->Process();
                                }
                                else
                                {
                                    select_card_text3->Process();
                                }
                                skip_text->Process();
                            }
                            else
                            {
                                if(player_use_card || engine_use_card)
                                {
                                    if(player_use_card)
                                    {
                                        player1_card->pass->Change("used", .5f);
                                        player_have_pass = false;
                                    }
                                    if(engine_use_card)
                                    {
                                        player2_card->pass->Change("used", .5f);
                                        engine_have_pass = false;
                                    }
                                }
                                else
                                {
                                    if(player_color == COLOR_W)
                                    {
                                        player_color = COLOR_B;
                                        player1_card->color->Change("black", .5f);
                                        player2_card->color->Change("white", .5f);
                                    }
                                    else
                                    {
                                        player_color = COLOR_W;
                                        player1_card->color->Change("white", .5f);
                                        player2_card->color->Change("black", .5f);
                                    }
                                    game_board->player_color = player_color;
                                    select_promote->player_color = player_color;
                                }

                                remaining_full_moves += 10;
                                remaining_moves_text->Change(std::to_string(remaining_full_moves), .25f);

                                player_done = false;
                                engine_done = false;

                                state = PLAYING;
                                game_board->Open();
                                player1_card->Open();
                                player2_card->Open();

                                use_pass_text->Reset();
                                plus5_text->Reset();
                                pass_btn1_text->Reset();
                                pass_btn2_text->Reset();
                                select_card_text1->Reset();
                                select_card_text2->Reset();
                                select_card_text3->Reset();
                                select_card_text4->Reset();
                                select_card_text5->Reset();
                                skip_text->Reset();

                                text_anim_time = 0;
                            }
                        }
                        else
                        {
                            select_card_text5->Process();
                        }
                    }
                }
                else
                {
                    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !mouse_handled)
                    {
                        text_anim_time = 3;
                    }
                    if(text_anim_time <= 2.25f)
                    {
                        text_anim_time += core->delta_time;
                        plus5_text->Process();
                        skip_text->Process();
                    }
                    else
                    {
                        remaining_full_moves += 5;
                        remaining_moves_text->Change(std::to_string(remaining_full_moves), .25f);

                        state = PLAYING;
                        game_board->Open();
                        player1_card->Open();
                        player2_card->Open();

                        plus5_text->Reset();
                        skip_text->Reset();

                        text_anim_time = 0;
                    }
                }
            }
        }
    }
    if(state == GAME_CLOSING)
    {
        if(game_board->state != BOARD_CLOSED)
        {
            if(game_board->state != BOARD_CLOSING)
            {
                game_board->Close();
                player1_card->Close();
                player2_card->Close();
            }
            game_board->Process();
            player1_card->Process();
            player2_card->Process();
        }
        else
        {
            state = GAME_FINISHED;
        }
    }

    if(state == PLAYING || state == SELECT_PROMOTION){
        if(remaining_moves_anim_time < .5f)
        {
            remaining_moves_anim_time += core->delta_time;
        }
        else
        {
            remaining_moves_text->Process();
        }
    }
    else
    {
        if(remaining_moves_anim_time > 0)
        {
            remaining_moves_anim_time -= core->delta_time;
        }
    }

    player1_card->Process();
    player2_card->Process();
}

void Game::Render()
{
    if(game_board->state != BOARD_CLOSED)
    {
        game_board->Render();
    }
    if(state == SELECT_PROMOTION)
    {
        select_promote->Render();
    }
    else if(state == SELECT_CARD)
    {
        if(game_select_card->state != SC_DONE)
        {
            game_select_card->Render();
        }
        else
        {
            if(game_select_card->card_sel_type == CARD_SWITCH)
            {
                if(player_have_pass && !player_done)
                {
                    use_pass_text->Render(assets->quaver_ttf[1], {core->vp_width*.5f, core->vp_height*.4f}, core->vp_width*.5f, true, true, WHITE);
                    if(pass_btn1_hover)
                    {
                        pass_btn1_size = pass_btn1_text->Render(assets->quaver_ttf[1], pass_btn1_pos, core->vp_width*.3f, true, true, WHITE);
                    }
                    else
                    {
                        pass_btn1_size = pass_btn1_text->Render(assets->quaver_ttf[1], pass_btn1_pos, core->vp_width*.3f, true, true, {95, 168, 211, 255});
                    }
                    if(pass_btn2_hover)
                    {
                        pass_btn2_size = pass_btn2_text->Render(assets->quaver_ttf[1], pass_btn2_pos, core->vp_width*.3f, true, true, WHITE);
                    }
                    else
                    {
                        pass_btn2_size = pass_btn2_text->Render(assets->quaver_ttf[1], pass_btn2_pos, core->vp_width*.3f, true, true, {95, 168, 211, 255});
                    }
                }
                else
                {
                    if(engine_done)
                    {
                        if(!player_have_pass && !engine_have_pass)
                        {
                            select_card_text1->Render(assets->quaver_ttf[1], {core->vp_width*.5f, core->vp_height*.5f}, core->vp_width*.5f, true, true, WHITE);
                        }
                        else if(player_use_card && engine_use_card)
                        {
                            select_card_text4->Render(assets->quaver_ttf[1], {core->vp_width*.5f, core->vp_height*.5f}, core->vp_width*.5f, true, true, WHITE);
                        }
                        else if(engine_use_card)
                        {
                            select_card_text2->Render(assets->quaver_ttf[1], {core->vp_width*.5f, core->vp_height*.5f}, core->vp_width*.5f, true, true, WHITE);
                        }
                        else
                        {
                            select_card_text3->Render(assets->quaver_ttf[1], {core->vp_width*.5f, core->vp_height*.5f}, core->vp_width*.5f, true, true, WHITE);
                        }
                        skip_text->Render(assets->quaver_ttf[0], {core->vp_width*.5f, core->vp_height*.7f}, true, true, WHITE);
                    }
                    else
                    {
                        select_card_text5->Render(assets->quaver_ttf[1], {core->vp_width*.5f, core->vp_height*.5f}, core->vp_width*.5f, true, true, WHITE);
                    }
                }
            }
            else
            {
                plus5_text->Render(assets->quaver_ttf[1], {core->vp_width*.5f, core->vp_height*.5f}, core->vp_width*.5f, true, true, WHITE);
                skip_text->Render(assets->quaver_ttf[0], {core->vp_width*.5f, core->vp_height*.7f}, true, true, WHITE);
            }
        }
    }

    if(remaining_moves_anim_time > 0)
    {
        float scale = easeOutCubic(min(.5f, max(0, remaining_moves_anim_time))*2);
        remaining_moves_text->Render(assets->quaver_ttf[3], {core->vp_height*1.55f, core->vp_height*.5f}, assets->quaver_ttf[3].baseSize*scale, true, true, WHITE);
    }

    player1_card->Render();
    player2_card->Render();
}

SceneGame::SceneGame()
{
    game = new Game();
    game_render = LoadRenderTexture(core->vp_width, core->vp_height);

    pause_btn_pos = {core->vp_height*.05f, core->vp_height*.05f};
    pause_btn_size = {0, 0};
    pause_btn_text = new AnimTextLetter("pause", .25f);
    pause_btn_hover = false;

    options_btn_pos = {core->vp_height*.05f, core->vp_height*.15f};
    options_btn_size = {0, 0};
    options_btn_text = new AnimTextLetter("options", .25f);
    options_btn_hover = false;

    quit_btn_pos = {core->vp_height*.05f, core->vp_height*.25f};
    quit_btn_size = {0, 0};
    quit_btn_text = new AnimTextLetter("quit", .25f);
    quit_btn_hover = false;

    pause_text = new AnimText("The game is paused.", .25f);
    pause_text_pos = {core->vp_width*.5f, core->vp_height*.4f};

    pause_option_text = new AnimText("Return to the game", .25f);
    pause_option_pos = {core->vp_width*.5f, core->vp_height*.6f};
    pause_option_size = {0, 0};
    pause_option_hover = false;

    quit_text = new AnimText("Do you want to quit the game?", .25f);
    quit_text_pos = {core->vp_width*.5f, core->vp_height*.4f};

    quit_option1_text = new AnimText("Yes, quit the game", .25f);
    quit_option1_pos = {core->vp_width*.3f, core->vp_height*.6f};
    quit_option1_size = {0, 0};
    quit_option1_hover = false;

    quit_option2_text = new AnimText("No, return to the game", .25f);
    quit_option2_pos = {core->vp_width*.7f, core->vp_height*.6f};
    quit_option2_size = {0, 0};
    quit_option2_hover = false;
    
    finish_title_text = new AnimTextLetter("", .25f);
    finish_title_pos = {core->vp_width*.5f, core->vp_height*.4f};

    finish_text = new AnimTextLetter("", .25f);
    finish_text_pos = {core->vp_width*.5f, core->vp_height*.5f};

    finish_btn_text = new AnimTextLetter("Continue", .25f);
    finish_btn_pos = {core->vp_width*.5f, core->vp_height*.65f};
    finish_btn_size = {0, 0};
    finish_btn_hover = false;
}

void SceneGame::Reset(uint8_t _player_color, std::string _op_id, std::string _op_rate, std::string _op_pronoun, uint8_t _op_level)
{
    state = ON_GAME;
    game->Reset(_player_color, _op_id, _op_rate, _op_pronoun, _op_level);

    game_anim_time = .25f;
    game_scale = 1;
    game_open = true;

    pause_btn_hover = false;
    pause_btn_size = {0, 0};
    pause_btn_text->Reset();

    pause_text->Reset();

    pause_option_text->Reset();
    pause_option_size = {0, 0};
    pause_option_hover = false;

    options_btn_hover = false;
    options_btn_size = {0, 0};
    options_btn_text->Reset();

    quit_btn_hover = false;
    quit_btn_size = {0, 0};
    quit_btn_text->Reset();

    quit_text->Reset();
    
    quit_option1_text->Reset();
    quit_option1_size = {0, 0};
    quit_option1_hover = false;

    quit_option2_text->Reset();
    quit_option2_size = {0, 0};
    quit_option2_hover = false;

    finish_title_text->Reset();

    finish_text->Reset();

    finish_btn_text->Reset();
    finish_btn_size = {0, 0};
    finish_btn_hover = false;
}

void SceneGame::Process()
{
    if(state == SCENE_GAME_DONE)
    {
        return;
    }

    Vector2 m_pos = GetMousePosition();
    m_pos = GetScreenToWorld2D(m_pos, core->vp_cam);

    pause_btn_text->Process();
    options_btn_text->Process();
    quit_btn_text->Process();

    if(state != ON_FINISH)
    {
        Rectangle pause_rec;
        pause_rec.width = pause_btn_size.x;
        pause_rec.height = pause_btn_size.y;
        pause_rec.x = pause_btn_pos.x;
        pause_rec.y = pause_btn_pos.y;

        Rectangle options_rec;
        options_rec.width = options_btn_size.x;
        options_rec.height = options_btn_size.y;
        options_rec.x = options_btn_pos.x;
        options_rec.y = options_btn_pos.y;

        Rectangle quit_rec;
        quit_rec.width = quit_btn_size.x;
        quit_rec.height = quit_btn_size.y;
        quit_rec.x = quit_btn_pos.x;
        quit_rec.y = quit_btn_pos.y;

        if(CheckCollisionPointRec(m_pos, pause_rec))
        {
            pause_btn_hover = true;
        }
        else
        {
            pause_btn_hover = false;
        }

        if(CheckCollisionPointRec(m_pos, options_rec))
        {
            options_btn_hover = true;
        }
        else
        {
            options_btn_hover = false;
        }

        if(CheckCollisionPointRec(m_pos, quit_rec))
        {
            quit_btn_hover = true;
        }
        else
        {
            quit_btn_hover = false;
        }

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if(pause_btn_hover)
            {
                state = ON_PAUSE;
                pause_text->Reset();
                pause_option_text->Reset();
            }
            if(options_btn_hover)
            {
                state = ON_OPTIONS;
            }
            if(quit_btn_hover)
            {
                state = ON_QUIT;
                quit_text->Reset();
                quit_option1_text->Reset();
                quit_option2_text->Reset();
            }
        }
    }

    if(state == ON_FINISH)
    {
        finish_title_text->Process();
        finish_text->Process();
        finish_btn_text->Process();

        Rectangle finish_btn_rec;
        finish_btn_rec.width = finish_btn_size.x;
        finish_btn_rec.height = finish_btn_size.y;
        finish_btn_rec.x = finish_btn_pos.x-finish_btn_rec.width*.5f;
        finish_btn_rec.y = finish_btn_pos.y-finish_btn_rec.height*.5f;
        if(CheckCollisionPointRec(m_pos, finish_btn_rec))
        {
            finish_btn_hover = true;
        }
        else
        {
            finish_btn_hover = false;
        }

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if(finish_btn_hover)
            {
                state = SCENE_GAME_DONE;
            }
        }
    }
    else if(state == ON_GAME)
    {
        if(game_open)
        {
            game->Process();
            if(game->state == GAME_FINISHED)
            {
                state = ON_FINISH;
                pause_btn_text->Close();
                options_btn_text->Close();
                quit_btn_text->Close();

                if(core->board->_game_end_type == CHECKMATE)
                {
                    if(core->board->_current_color != game->player_color)
                    {
                        finish_title_text->Change("You Win !", .25f);
                    }
                    else
                    {
                        finish_title_text->Change("You Lose !", .25f);
                    }
                    finish_text->Change("by checkmate", .25f);
                }
                else
                {
                    finish_title_text->Change("Draw !", .25f);
                    if(core->board->_game_end_type == STALEMATE)
                    {
                        finish_text->Change("by stalemate", .25f);
                    }
                    else if(core->board->_game_end_type == DEAD_POSITION)
                    {
                        finish_text->Change("by insufficient materials", .25f);
                    }
                    else if(core->board->_game_end_type == FIFTY_MOVE)
                    {
                        finish_text->Change("by 50 move rule", .25f);
                    }
                }
            }
        }
        else
        {
            if(game_anim_time < .25f)
            {
                game_anim_time += core->delta_time;
                game_scale = easeOutCubic(min(game_anim_time, .25f)*4);
            }
            else
            {
                game_open = true;
            }
        }
    }
    else if(state == ON_PAUSE)
    {
        if(game_open)
        {
            if(game_anim_time > 0)
            {
                game_anim_time -= core->delta_time;
                game_scale = easeOutCubic(max(game_anim_time, 0)*4);
            }
            else
            {
                game_open = false;
            }
        }
        else
        {
            pause_text->Process();
            pause_option_text->Process();

            Rectangle pause_opt_rec;
            pause_opt_rec.width = pause_option_size.x;
            pause_opt_rec.height = pause_option_size.y;
            pause_opt_rec.x = pause_option_pos.x-pause_opt_rec.width*.5f;
            pause_opt_rec.y = pause_option_pos.y-pause_opt_rec.height*.5f;
            if(CheckCollisionPointRec(m_pos, pause_opt_rec))
            {
                pause_option_hover = true;
            }
            else
            {
                pause_option_hover = false;
            }

            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if(pause_option_hover)
                {
                    state = ON_GAME;
                }
            }
        }
    }
    else if(state == ON_OPTIONS)
    {
        if(game_open)
        {
            if(game_anim_time > 0)
            {
                game_anim_time -= core->delta_time;
                game_scale = easeOutCubic(max(game_anim_time, 0)*4);
            }
            else
            {
                game_open = false;
            }
        }
        else
        {

        }
    }
    else if(state == ON_QUIT)
    {
        if(game_open)
        {
            if(game_anim_time > 0)
            {
                game_anim_time -= core->delta_time;
                game_scale = easeOutCubic(max(game_anim_time, 0)*4);
            }
            else
            {
                game_open = false;
            }
        }
        else
        {
            quit_text->Process();
            quit_option1_text->Process();
            quit_option2_text->Process();

            Rectangle quit_opt1_rec;
            quit_opt1_rec.width = quit_option1_size.x;
            quit_opt1_rec.height = quit_option1_size.y;
            quit_opt1_rec.x = quit_option1_pos.x-quit_opt1_rec.width*.5f;
            quit_opt1_rec.y = quit_option1_pos.y-quit_opt1_rec.height*.5f;
            if(CheckCollisionPointRec(m_pos, quit_opt1_rec))
            {
                quit_option1_hover = true;
            }
            else
            {
                quit_option1_hover = false;
            }

            Rectangle quit_opt2_rec;
            quit_opt2_rec.width = quit_option2_size.x;
            quit_opt2_rec.height = quit_option2_size.y;
            quit_opt2_rec.x = quit_option2_pos.x-quit_opt2_rec.width*.5f;
            quit_opt2_rec.y = quit_option2_pos.y-quit_opt2_rec.height*.5f;
            if(CheckCollisionPointRec(m_pos, quit_opt2_rec))
            {
                quit_option2_hover = true;
            }
            else
            {
                quit_option2_hover = false;
            }

            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if(quit_option1_hover)
                {
                    state = SCENE_GAME_DONE;
                }
                if(quit_option2_hover)
                {
                    state = ON_GAME;
                }
            }
        }
    }
}

void SceneGame::Render()
{   
    if(state == SCENE_GAME_DONE)
    {
        return;
    }

    if(game_open || game_scale > 0)
    {
        BeginTextureMode(game_render);
        BeginMode2D(core->default_cam);
            ClearBackground(WHITE|0);
            game->Render();
        EndMode2D();
        EndTextureMode();
    }

    BeginMode2D(core->vp_cam);
    ClearBackground({27, 73, 101, 255});
    if(game_open || game_scale > 0)
    {   
        Rectangle game_rec;
        game_rec.width = core->vp_width*game_scale;
        game_rec.height = core->vp_height*game_scale;
        game_rec.x = core->vp_width*.5f;
        game_rec.y = core->vp_height*.5f;
        DrawTexturePro(game_render.texture, {0, core->vp_height, core->vp_width, -core->vp_height}, game_rec, {game_rec.width*.5f, game_rec.height*.5f}, 0, WHITE);
    }
    if(state != ON_FINISH)
    {
        if(pause_btn_hover)
        {
            pause_btn_size = pause_btn_text->Render(assets->quaver_ttf[1], pause_btn_pos, false, false, WHITE);
        }
        else
        {
            pause_btn_size = pause_btn_text->Render(assets->quaver_ttf[1], pause_btn_pos, false, false, {95, 168, 211, 255});
        }
        if(options_btn_hover)
        {
            options_btn_size = options_btn_text->Render(assets->quaver_ttf[1], options_btn_pos, false, false, WHITE);
        }
        else
        {
            options_btn_size = options_btn_text->Render(assets->quaver_ttf[1], options_btn_pos, false, false, {95, 168, 211, 255});
        }
        if(quit_btn_hover)
        {
            quit_btn_size = quit_btn_text->Render(assets->quaver_ttf[1], quit_btn_pos, false, false, WHITE);
        }
        else
        {
            quit_btn_size = quit_btn_text->Render(assets->quaver_ttf[1], quit_btn_pos, false, false, {95, 168, 211, 255});
        }
    }
    if(state == ON_PAUSE)
    {
        pause_text->Render(assets->quaver_ttf[1], pause_text_pos, core->vp_width*.5f, true, true, WHITE);
        if(pause_option_hover)
        {
            pause_option_size = pause_option_text->Render(assets->quaver_ttf[1], pause_option_pos, core->vp_width*.5f, true, true, WHITE);
        }
        else
        {
            pause_option_size = pause_option_text->Render(assets->quaver_ttf[1], pause_option_pos, core->vp_width*.5f, true, true, {95, 168, 211, 255});
        }
    }
    if(state == ON_QUIT)
    {
        quit_text->Render(assets->quaver_ttf[1], quit_text_pos, core->vp_width*.5f, true, true, WHITE);
        if(quit_option1_hover)
        {
            quit_option1_size = quit_option1_text->Render(assets->quaver_ttf[1], quit_option1_pos, core->vp_width*.5f, true, true, WHITE);
        }
        else
        {
            quit_option1_size = quit_option1_text->Render(assets->quaver_ttf[1], quit_option1_pos, core->vp_width*.5f, true, true, {95, 168, 211, 255});
        }
        if(quit_option2_hover)
        {
            quit_option2_size = quit_option2_text->Render(assets->quaver_ttf[1], quit_option2_pos, core->vp_width*.5f, true, true, WHITE);
        }
        else
        {
            quit_option2_size = quit_option2_text->Render(assets->quaver_ttf[1], quit_option2_pos, core->vp_width*.5f, true, true, {95, 168, 211, 255});
        }
    }
    if(state == ON_FINISH)
    {
        finish_title_text->Render(assets->quaver_ttf[3], finish_title_pos, true, true, WHITE);
        finish_text->Render(assets->quaver_ttf[1], finish_text_pos, true, true, WHITE);

        if(finish_btn_hover)
        {
            finish_btn_size = finish_btn_text->Render(assets->quaver_ttf[1], finish_btn_pos, true, true, WHITE);
        }
        else
        {
            finish_btn_size = finish_btn_text->Render(assets->quaver_ttf[1], finish_btn_pos, true, true, {95, 168, 211, 255});
        }
    }

    EndMode2D();
}