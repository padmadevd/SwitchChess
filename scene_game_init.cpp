#include <scene_game_init.hpp>

PlayerCard::PlayerCard(std::string _id, std::string _age, std::string _rating, std::string _level, std::string _desc)
{
    id = new AnimTextLetter(_id, .25f);

    age_key = new AnimTextLetter("age :", .25f);
    age_value = new AnimTextLetter(_age, .25f);

    rating_key = new AnimTextLetter("rating :", .25f);
    rating_value = new AnimTextLetter(_rating, .25f);

    level_key = new AnimTextLetter("level :", .25f);
    level_value = new AnimTextLetter(_level, .25f);

    desc_text = new AnimText(_desc, .25f);

    size = {core->vp_height*.35f, core->vp_height*.18f};

    anim_time = 0;
    scale_y = .25f;
    scale = 1.f;
    hovered = false;
    selected = false;
}

void PlayerCard::Reset()
{
    anim_time = 0;
    scale_y = .25f;
    scale = 1.f;
    hovered = false;
    selected = false;

    id->Reset();

    age_key->Reset();
    age_value->Reset();

    rating_key->Reset();
    rating_value->Reset();

    level_key->Reset();
    level_value->Reset();

    desc_text->Reset();
}

void PlayerCard::Process()
{
    Vector2 m_pos = GetMousePosition();
    m_pos = GetScreenToWorld2D(m_pos, core->vp_cam);

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hovered)
    {
        selected = true;
    }

    Rectangle back_rec;
    back_rec.x = position.x;
    back_rec.y = position.y;
    back_rec.width = size.x*scale;
    back_rec.height = size.y*scale_y*scale;
    if(CheckCollisionPointRec(m_pos, back_rec))
    {
        hovered = true;
    }
    else
    {
        hovered = false;
    }

    id->Forward();
    if(hovered || selected)
    {
        if(anim_time < .5f)
        {
            anim_time += core->delta_time;
            scale_y = .25f + .75f*easeOutCubic(min(anim_time, .25f)*4);
            scale = 1.f + .5f*easeOutCubic(min(anim_time, .25f)*4);
        }
        if(FloatEquals(scale, 1.5f))
        {
            age_key->Forward();
            age_value->Forward();

            rating_key->Forward();
            rating_value->Forward();

            level_key->Forward();
            level_value->Forward();

            desc_text->Forward();
        }
    }
    else
    {
        if(anim_time > 0.f)
        {
            anim_time -= core->delta_time;
            scale_y = .25f + .75f*easeOutCubic(max(min(.25f, anim_time), 0.f)*4);
            scale = 1.f + .5f*easeOutCubic(max(min(.25f, anim_time), 0.f)*4);
        }
        if(FloatEquals(scale, 1.5f))
        {
            age_key->Backward();
            age_value->Backward();

            rating_key->Backward();
            rating_value->Backward();

            level_key->Backward();
            level_value->Backward();

            desc_text->Backward();
        }
    }
}

void PlayerCard::Render(Vector2 _position)
{
    position = _position;

    Rectangle back_rec;
    back_rec.x = position.x;
    back_rec.y = position.y;
    back_rec.width = size.x*scale;
    back_rec.height = size.y*scale_y*scale;

    DrawTextureNPatch(assets->empty_rec_f_png, assets->back_patch_info, back_rec, {0, 0}, 0, BLACK|50);

    if(selected)
    {
        DrawTextureNPatch(assets->empty_rec_s_png, assets->back_patch_info, back_rec, {0, 0}, 0, {149, 213, 178, 255});
    }
    else
    {
        DrawTextureNPatch(assets->empty_rec_s_png, assets->back_patch_info, back_rec, {0, 0}, 0, {202, 233, 255, 255});
    }

    Vector2 pos = {position.x+back_rec.width*.5f, position.y+size.y*scale*.14f};
    id->Render(assets->quaver_ttf[1], pos, assets->quaver_ttf[1].baseSize*scale*.5f, true, true, {202, 233, 255, 255});

    if(FloatEquals(scale, 1.5f))
    {
        pos =  {position.x+back_rec.width*.05f, position.y+back_rec.height*.3f};
        age_key->Render(assets->quaver_ttf[0], pos, false, true, {202, 233, 255, 255});
        pos =  {position.x+back_rec.width*.17f, position.y+back_rec.height*.3f};
        age_value->Render(assets->quaver_ttf[0], pos, false, true, {202, 233, 255, 255});

        pos =  {position.x+back_rec.width*.05f, position.y+back_rec.height*.4f};
        rating_key->Render(assets->quaver_ttf[0], pos, false, true, {202, 233, 255, 255});
        pos =  {position.x+back_rec.width*.27f, position.y+back_rec.height*.4f};
        rating_value->Render(assets->quaver_ttf[0], pos, false, true, {202, 233, 255, 255});

        pos =  {position.x+back_rec.width*.05f, position.y+back_rec.height*.5f};
        level_key->Render(assets->quaver_ttf[0], pos, false, true, {202, 233, 255, 255});
        pos =  {position.x+back_rec.width*.22f, position.y+back_rec.height*.5f};
        level_value->Render(assets->quaver_ttf[0], pos, false, true, {202, 233, 255, 255});

        pos =  {position.x+back_rec.width*.05f, position.y+back_rec.height*.65f};
        desc_text->Render(assets->quaver_ttf[0], pos, back_rec.width*.9f, false, false, {202, 233, 255, 255});
    }
}

ColorBox::ColorBox(uint8_t _color, Vector2 _pos)
{
    color = _color;
    position = _pos;
    size = {core->vp_height*.1f, core->vp_height*.1f};

    anim_time = 0;
    scale = 0;
    hovered = false;
    selected = false;
}

void ColorBox::Reset()
{
    anim_time = 0;
    scale = 0;
    hovered = false;
    selected = false;
}

void ColorBox::Process()
{
    Vector2 m_pos = GetMousePosition();
    m_pos = GetScreenToWorld2D(m_pos, core->vp_cam);

    if(anim_time < .25f)
    {
        anim_time += core->delta_time;
        scale = easeOutCubic(min(.25f, anim_time)*4);
    }
    else
    {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hovered)
        {
            selected = true;
        }
        Rectangle back_rec;
        back_rec.x = position.x;
        back_rec.y = position.y;
        back_rec.width = size.x*scale;
        back_rec.height = size.y*scale;
        if(CheckCollisionPointRec(m_pos, back_rec))
        {
            hovered = true;
        }
        else
        {
            hovered = false;
        }
        if(hovered || selected)
        {
            if(anim_time < .5f)
            {
                anim_time += core->delta_time;
            }
            scale = 1.f + .5f*easeOutCubic(max(min(anim_time-.25f, .25f), 0)*4);
        }
        else
        {
            if(anim_time > .25f)
            {
                anim_time -= core->delta_time;
            }
            scale = 1.f + .5f*easeOutCubic(min(max(anim_time-.25f, 0), .25f)*4);
        }
    }
}

void ColorBox::Render(Vector2 _pos)
{
    position = _pos;
    if(FloatEquals(scale, 0))
    {
        return;
    }

    Rectangle back_rec;
    back_rec.x = position.x;
    back_rec.y = position.y;
    back_rec.width = size.x*scale;
    back_rec.height = size.y*scale;

    if(color == COLOR_W)
    {
        DrawTextureNPatch(assets->empty_rec_f_png, assets->back_patch_info, back_rec, {0, 0}, 0, WHITE);
    }
    else
    {
        DrawTextureNPatch(assets->empty_rec_f_png, assets->back_patch_info, back_rec, {0, 0}, 0, BLACK);
    }
    if(selected)
    {
        DrawTextureNPatch(assets->empty_rec_s_png, assets->back_patch_info, back_rec, {0, 0}, 0, {149, 213, 178, 255});
    }
    else
    {
        DrawTextureNPatch(assets->empty_rec_s_png, assets->back_patch_info, back_rec, {0, 0}, 0, {202, 233, 255, 255});
    }
}

SceneGameInit::SceneGameInit()
{   
    autoplay_render = LoadRenderTexture(core->vp_height, core->vp_height);
    SetTextureWrap(autoplay_render.texture, TEXTURE_WRAP_REPEAT);
    autoplay_offset = 0;

    id[0] = "Toby Sparks";
    age[0] = "8";
    pronoun[0] = "his";
    rating[0] = "< 1000";
    level[0] = 1;
    level_name[0] = "Beginner";
    desc[0] = "Cheeky and clever, a child prodigy who plays chess between cartoons and homework. Just getting started.";

    id[1] = "Lina Patel";
    age[1] = "10";
    pronoun[1] = "her";
    rating[1] = ">= 1000, < 1300";
    level[1] = 2;
    level_name[1] = "Novice";
    desc[1] = "Curious and quick-thinking, learns chess from her grandfather and already beats older kids";

    id[2] = "Diego Cruz";
    age[2] = "16";
    pronoun[2] = "his";
    rating[2] = ">= 1300, < 1600";
    level[2] = 3;
    level_name[2] = "Experienced";
    desc[2] = "Cool and competitive, a teen who juggles school and national chess tournaments";

    id[3] = "Aya Nakamura";
    age[3] = "17";
    pronoun[3] = "her";
    rating[3] = ">= 1600, < 1900";
    level[3] = 4;
    level_name[3] = "Specialist";
    desc[3] = "Quiet and precise, a focused high school player with solid knowledge of tactics and positions";

    id[4] = "Musa Dlamini";
    age[4] = "25";
    pronoun[4] = "his";
    rating[4] = ">= 1900, < 2100";
    level[4] = 5;
    level_name[4] = "Expert";
    desc[4] = "Creative and bold, teaches chess in schools and competes regularly in regional events";

    id[5] = "Elena Vostrikova";
    age[5] = "33";
    pronoun[5] = "her";
    rating[5] = ">= 2100, < 2300";
    level[5] = 6;
    level_name[5] = "Master";
    desc[5] = "Disciplined and determined, a sharp and strategic international player";

    id[6] = "Johan Andersen";
    age[6] = "48";
    pronoun[6] = "his";
    rating[6] = ">= 2300, < 2500";
    level[6] = 7;
    level_name[6] = "Senior Master";
    desc[6] = "Experienced and thoughtful, a respected coach and former national champion";

    id[7] = "Nora Bennett";
    age[7] = "70";
    pronoun[7] = "her";
    rating[7] = ">= 2500";
    level[7] = 8;
    level_name[7] = "Grandmaster";
    desc[7] = "Wise and witty, a legendary chess master who still dominates club games";

    for(int i = 0; i < 8; i++)
    {
        player_card[i] = new PlayerCard(id[i], age[i], rating[i], level_name[i], desc[i]);
    }

    back_btn_text = new AnimTextLetter("Return to home", .25f);
    back_btn_pos = {core->vp_height*.05f, core->vp_height*.05f};
    back_btn_size = {0, 0};
    back_btn_hover = false;

    start_btn_text = new AnimTextLetter("Start the game", .25f);
    start_btn_pos = {core->vp_width-core->vp_height*.05f, core->vp_height*.95f};
    start_btn_size = {0, 0};
    start_btn_hover = false;

    choose_player_text = new AnimTextLetter("Choose your opponent", .25f);
    choose_player_pos = {core->vp_width-core->vp_height*.05f, core->vp_height*.95f};

    choose_color_text = new AnimTextLetter("Choose your color", .25f);
    choose_color_pos = {core->vp_width-core->vp_height*.05f, core->vp_height*.95f};

    color_w = new ColorBox(COLOR_W, {core->vp_width*.65f, core->vp_height*.5f});
    color_b = new ColorBox(COLOR_B, {core->vp_width*.65f, core->vp_height*.5f});

    scene_done = false;
    start_game = false;
}

void SceneGameInit::Reset()
{
    autoplay_offset = 0;

    back_btn_text->Reset();
    back_btn_size = {0, 0};
    back_btn_hover = false;

    start_btn_text->Reset();
    start_btn_size = {0, 0};
    start_btn_hover = false;

    player_index = 100;
    color = COLOR_NONE;

    for(int i = 0; i < 8; i++)
    {
        player_card[i]->Reset();
    }
    color_w->Reset();
    color_b->Reset();

    choose_player_text->Reset();
    choose_color_text->Reset();

    scene_done = false;
    start_game = false;
}

void SceneGameInit::Process()
{
    if(scene_done)
    {
        return;
    }

    autoplay->Process();
    autoplay_offset += core->delta_time*100;
    if(autoplay_offset > core->vp_height)
    {
        autoplay_offset = 0;
    }

    Vector2 m_pos = GetMousePosition();
    m_pos = GetScreenToWorld2D(m_pos, core->vp_cam);

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && back_btn_hover)
    {
        scene_done = true;
        start_game = false;
    }

    back_btn_text->Process();
    Rectangle back_btn_rec;
    back_btn_rec.x = back_btn_pos.x;
    back_btn_rec.y = back_btn_pos.y;
    back_btn_rec.width = back_btn_size.x;
    back_btn_rec.height = back_btn_size.y;
    if(CheckCollisionPointRec(m_pos, back_btn_rec))
    {
        back_btn_hover = true;
    }
    else
    {
        back_btn_hover = false;
    }

    for(int i = 0; i < 8; i++)
    {
        player_card[i]->Process();
        if(player_card[i]->selected)
        {
            if(player_index == 100)
            {
                player_index = i;
            }
            else if(i != player_index)
            {
                player_card[player_index]->selected = false;
                player_index = i;
            }
        }
    }

    if(player_index != 100)
    {
        color_w->Process();
        color_b->Process();

        if(color_w->selected && color != COLOR_W)
        {
            if(color == COLOR_B)
            {
                color_b->selected = false;
            }
            color = COLOR_W;
        }
        if(color_b->selected && color != COLOR_B)
        {
            if(color == COLOR_W)
            {
                color_w->selected = false;
            }
            color = COLOR_B;
        }
    }

    if(player_index == 100)
    {
        choose_player_text->Process();
    }
    else if(color == COLOR_NONE)
    {
        choose_color_text->Process();
    }
    else
    {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && start_btn_hover)
        {
            scene_done = true;
            start_game = true;
        }
        start_btn_text->Process();
        Rectangle start_btn_rec;
        start_btn_rec.width = start_btn_size.x;
        start_btn_rec.height = start_btn_size.y;
        start_btn_rec.x = start_btn_pos.x-start_btn_size.x;
        start_btn_rec.y = start_btn_pos.y-start_btn_size.y;
        if(CheckCollisionPointRec(m_pos, start_btn_rec))
        {
            start_btn_hover = true;
        }
        else
        {
            start_btn_hover = false;
        }
    }
}

void SceneGameInit::Render()
{
    if(scene_done)
    {
        return;
    }
    
    autoplay->Render(autoplay_render);

    BeginMode2D(core->vp_cam);
    ClearBackground({27, 73, 101, 255});
    DrawTexturePro(autoplay_render.texture, {autoplay_offset, core->vp_height, core->vp_width, -core->vp_height}, {0, 0, core->vp_width, core->vp_height}, {0, 0}, 0, WHITE);

    if(back_btn_hover)
    {
        back_btn_size = back_btn_text->Render(assets->quaver_ttf[1], back_btn_pos, false, false, WHITE);
    }
    else
    {
        back_btn_size = back_btn_text->Render(assets->quaver_ttf[1], back_btn_pos, false, false, {95, 168, 211, 255});
    }

    Vector2 pos = {core->vp_width*.25f, core->vp_height*.15f};
    for(int i = 0; i < 8; i++)
    {
        player_card[i]->Render(pos);
        pos.y += player_card[i]->size.y*player_card[i]->scale*player_card[i]->scale_y+core->vp_height*.01f;
    }

    if(player_index != 100)
    {
        Vector2 color_pos = {core->vp_width*.65f, core->vp_height*.4f};
        color_w->Render(color_pos);
        color_pos.y += color_w->size.y*color_w->scale+core->vp_height*.01f;
        color_b->Render(color_pos);
    }

    if(player_index == 100)
    {
        choose_player_text->RenderRB(assets->quaver_ttf[1], choose_player_pos, WHITE);
    }
    else if(color == COLOR_NONE)
    {
        choose_color_text->RenderRB(assets->quaver_ttf[1], choose_color_pos, WHITE);
    }
    else
    {
        if(start_btn_hover)
        {
            start_btn_size = start_btn_text->RenderRB(assets->quaver_ttf[1], start_btn_pos, WHITE);
        }
        else
        {
            start_btn_size = start_btn_text->RenderRB(assets->quaver_ttf[1], start_btn_pos, {95, 168, 211, 255});
        }
    }

    EndMode2D();
}