#include <scene_home.hpp>

ImageAnim::ImageAnim(const char *path, float _width, Vector2 _pos)
{
    Image img = LoadImage(path);
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
    w = img.width;
    h = img.height;

    cell_anim_time = std::vector<float>(w*h, 0);
    cell_anim_delay = std::vector<float>(w*h, 0);
    cell_scale = std::vector<float>(w*h, 0);
    cell_pos = std::vector<Vector2>(w*h);
    cell_color = std::vector<Color>(w*h);

    cell_order = std::vector<int>(w*h);
    for(int i = 0; i < w*h; i++)
    {
        cell_order[i] = i;
    }

    position = _pos;
    scale = 1;
    width = _width;
    cell_size = width/w;
    height = h*cell_size;

    Vector2 origin = Vector2{-width*.5f, -height*.5f};
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            cell_pos[j*w+i] = origin + Vector2{(i+.5f)*cell_size, (j+.5f)*cell_size};
            cell_color[j*w+i] = WHITE|GetImageColor(img, i, j).r;
        }
    }

    std::sort(cell_order.begin(), cell_order.end(), [this](const int &a, const int &b){
        float dista = a/w+a%w;
        float distb = b/w+b%w;
        return dista < distb;
    });

    Reset();
    UnloadImage(img);
}

void ImageAnim::Reset()
{
    for(int i : cell_order)
    {
        cell_anim_delay[i] = 1.f*(i%w+i/w)/float(w+h);
        cell_anim_time[i] = 0.f;
        cell_scale[i] = 0.f;
    }
    done = false;
}

void ImageAnim::Process()
{
    done = true;
    for(int i = 0; i < w*h; i++){
        if(cell_anim_delay[i] > 0)
        {
            cell_anim_delay[i] -= core->delta_time;
            done = false;
        }
        else
        {
            if(cell_anim_time[i] < 1.f)
            {
                cell_anim_time[i] += core->delta_time;
                cell_scale[i] = easeOutCubic(min(1.f, cell_anim_time[i]));
                done = false;
            }
        }
    }
}

void ImageAnim::Render()
{
    for(int i = 0; i < w*h; i++)
    {
        Vector2 pos = position+cell_pos[i]*scale;
        DrawRectanglePro({pos.x, pos.y, cell_size*cell_scale[i]*scale, cell_size*cell_scale[i]*scale}, {cell_size*cell_scale[i]*scale*.5f, cell_size*cell_scale[i]*scale*.5f}, 0, cell_color[i]);
    }
}

SceneHome::SceneHome()
{
    autoplay_render = LoadRenderTexture(core->vp_height, core->vp_height);
    SetTextureWrap(autoplay_render.texture, TEXTURE_WRAP_REPEAT);

    title = new ImageAnim("./assets/title.png", core->vp_width*.8f, {core->vp_width*.5f, core->vp_height*.5f});
    raylib_anim = new ImageAnim("./assets/raylib_48x48.png", core->vp_height*.6f, {core->vp_width*.5f, core->vp_height*.5f});
    raylib_anim_time = 0;
    title_anim_time = 0;

    play_btn_text = new AnimTextLetter("play with engine", .25f);
    play_btn_pos = {core->vp_width*.5f, core->vp_height*.4f};
    play_btn_size = {0, 0};
    play_btn_hover = false;

    play_online_btn_text = new AnimTextLetter("play with internet", .25f);
    play_online_btn_pos = {core->vp_width*.5f, core->vp_height*.5f};

    options_btn_text = new AnimTextLetter("change some options", .25f);
    options_btn_pos = {core->vp_width*.5f, core->vp_height*.6f};
    options_btn_size = {0, 0};
    options_btn_hover = false;

    help_btn_text = new AnimTextLetter("learn to play", .25f);
    help_btn_pos = {core->vp_width*.5f, core->vp_height*.7f};
    help_btn_size = {0, 0};
    help_btn_hover = false;

    credits_btn_text = new AnimTextLetter("see the credits", .25f);
    credits_btn_pos = {core->vp_width*.5f, core->vp_height*.8f};
    credits_btn_size = {0, 0};
    credits_btn_hover = false;

    quit_btn_text = new AnimTextLetter("quit the game", .25f);
    quit_btn_pos = {core->vp_width*.5f, core->vp_height*.9f};
    quit_btn_size = {0, 0};
    quit_btn_hover = false;

    Reset();
}

void SceneHome::Reset()
{
    autoplay_offset = 0;
    play_btn_text->Reset();
    play_btn_size = {0, 0};
    play_btn_hover = false;

    play_online_btn_text->Reset();

    options_btn_text->Reset();
    options_btn_size = {0, 0};
    options_btn_hover = false;

    help_btn_text->Reset();
    help_btn_size = {0, 0};
    help_btn_hover = false;

    credits_btn_text->Reset();
    credits_btn_size = {0, 0};
    credits_btn_hover = false;

    quit_btn_text->Reset();
    quit_btn_size = {0, 0};
    quit_btn_hover = false;

    scene_done = false;
}

void SceneHome::Process()
{
    autoplay->Process();
    autoplay_offset += core->delta_time*100;
    if(autoplay_offset > core->vp_height)
    {
        autoplay_offset = 0;
    }

    if(raylib_anim_time < 3.f)
    {
        raylib_anim_time += core->delta_time;
        raylib_anim->Process();
        return;
    }

    if(title_anim_time < 3.f)
    {
        title_anim_time += core->delta_time;
        if(title_anim_time > 2.f)
        {
            float ease = easeOutCubic(min(max(title_anim_time-2.f, 0), 1.f));
            title->scale = 0.5f*ease + 1.f*(1-ease);
            title->position = Vector2{core->vp_width*.5f, core->vp_height*.2f}*ease + Vector2{core->vp_width*.5f, core->vp_height*.5f}*(1-ease);
        }
        title->Process();
        return;
    }
    
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if(play_btn_hover)
        {
            scene_done = true;
            option_sel = PLAY_ENGINE;
        }
        else if(options_btn_hover)
        {
            scene_done = true;
            option_sel = CHANGE_OPTIONS;
        }
        else if(help_btn_hover)
        {
            scene_done = true;
            option_sel = HELP;
        }
        else if(credits_btn_hover)
        {
            scene_done = true;
            option_sel = CREDITS;
        }
        else if(quit_btn_hover)
        {
            scene_done = true;
            option_sel = QUIT;
        }
    }

    title->Process();

    Vector2 m_pos = GetMousePosition();
    m_pos = GetScreenToWorld2D(m_pos, core->vp_cam);

    play_btn_text->Process();
    play_online_btn_text->Process();
    options_btn_text->Process();
    help_btn_text->Process();
    credits_btn_text->Process();
    quit_btn_text->Process();

    Rectangle play_btn_rec;
    play_btn_rec.width = play_btn_size.x;
    play_btn_rec.height = play_btn_size.y;
    play_btn_rec.x = play_btn_pos.x-play_btn_rec.width*.5f;
    play_btn_rec.y = play_btn_pos.y-play_btn_rec.height*.5f;
    if(CheckCollisionPointRec(m_pos, play_btn_rec))
    {
        play_btn_hover = true;
    }
    else
    {
        play_btn_hover = false;
    }

    Rectangle options_btn_rec;
    options_btn_rec.width = options_btn_size.x;
    options_btn_rec.height = options_btn_size.y;
    options_btn_rec.x = options_btn_pos.x-options_btn_rec.width*.5f;
    options_btn_rec.y = options_btn_pos.y-options_btn_rec.height*.5f;
    if(CheckCollisionPointRec(m_pos, options_btn_rec))
    {
        options_btn_hover = true;
    }
    else
    {
        options_btn_hover = false;
    }

    Rectangle help_btn_rec;
    help_btn_rec.width = help_btn_size.x;
    help_btn_rec.height = help_btn_size.y;
    help_btn_rec.x = help_btn_pos.x-help_btn_rec.width*.5f;
    help_btn_rec.y = help_btn_pos.y-help_btn_rec.height*.5f;
    if(CheckCollisionPointRec(m_pos, help_btn_rec))
    {
        help_btn_hover = true;
    }
    else
    {
        help_btn_hover = false;
    }

    Rectangle credits_btn_rec;
    credits_btn_rec.width = credits_btn_size.x;
    credits_btn_rec.height = credits_btn_size.y;
    credits_btn_rec.x = credits_btn_pos.x-credits_btn_rec.width*.5f;
    credits_btn_rec.y = credits_btn_pos.y-credits_btn_rec.height*.5f;
    if(CheckCollisionPointRec(m_pos, credits_btn_rec))
    {
        credits_btn_hover = true;
    }
    else
    {
        credits_btn_hover = false;
    }

    Rectangle quit_btn_rec;
    quit_btn_rec.width = quit_btn_size.x;
    quit_btn_rec.height = quit_btn_size.y;
    quit_btn_rec.x = quit_btn_pos.x-quit_btn_rec.width*.5f;
    quit_btn_rec.y = quit_btn_pos.y-quit_btn_rec.height*.5f;
    if(CheckCollisionPointRec(m_pos, quit_btn_rec))
    {
        quit_btn_hover = true;
    }
    else
    {
        quit_btn_hover = false;
    }

}

void SceneHome::Render()
{
    autoplay->Render(autoplay_render);

    if(raylib_anim_time < 3.f)
    {
        BeginMode2D(core->vp_cam);
        ClearBackground({27, 73, 101, 255});
        DrawTexturePro(autoplay_render.texture, {autoplay_offset, core->vp_height, core->vp_width, -core->vp_height}, {0, 0, core->vp_width, core->vp_height}, {0, 0}, 0, WHITE);
        raylib_anim->Render();
        EndMode2D();
        return;
    }

    if(title_anim_time < 3.f)
    {
        BeginMode2D(core->vp_cam);
        ClearBackground({27, 73, 101, 255});
        DrawTexturePro(autoplay_render.texture, {autoplay_offset, core->vp_height, core->vp_width, -core->vp_height}, {0, 0, core->vp_width, core->vp_height}, {0, 0}, 0, WHITE);
        title->Render();
        EndMode2D();
        return;
    }

    BeginMode2D(core->vp_cam);
    ClearBackground({27, 73, 101, 255});
    DrawTexturePro(autoplay_render.texture, {autoplay_offset, core->vp_height, core->vp_width, -core->vp_height}, {0, 0, core->vp_width, core->vp_height}, {0, 0}, 0, WHITE);
    title->Render();

    if(play_btn_hover)
    {
        play_btn_size = play_btn_text->Render(assets->quaver_ttf[1], play_btn_pos, true, true, WHITE);
    }
    else
    {
        play_btn_size = play_btn_text->Render(assets->quaver_ttf[1], play_btn_pos, true, true, {95, 168, 211, 255});
    }

    play_online_btn_text->Render(assets->quaver_ttf[1], play_online_btn_pos, true, true, {95, 168, 211, 255});
    
    if(options_btn_hover)
    {
        options_btn_size = options_btn_text->Render(assets->quaver_ttf[1], options_btn_pos, true, true, WHITE);
    }
    else
    {
        options_btn_size = options_btn_text->Render(assets->quaver_ttf[1], options_btn_pos, true, true, {95, 168, 211, 255});
    }

    if(help_btn_hover)
    {
        help_btn_size = help_btn_text->Render(assets->quaver_ttf[1], help_btn_pos, true, true, WHITE);
    }
    else
    {
        help_btn_size = help_btn_text->Render(assets->quaver_ttf[1], help_btn_pos, true, true, {95, 168, 211, 255});
    }

    if(credits_btn_hover)
    {
        credits_btn_size = credits_btn_text->Render(assets->quaver_ttf[1], credits_btn_pos, true, true, WHITE);
    }
    else
    {
        credits_btn_size = credits_btn_text->Render(assets->quaver_ttf[1], credits_btn_pos, true, true, {95, 168, 211, 255});
    }

    if(quit_btn_hover)
    {
        quit_btn_size = quit_btn_text->Render(assets->quaver_ttf[1], quit_btn_pos, true, true, WHITE);
    }
    else
    {
        quit_btn_size = quit_btn_text->Render(assets->quaver_ttf[1], quit_btn_pos, true, true, {95, 168, 211, 255});
    }

    EndMode2D();
}