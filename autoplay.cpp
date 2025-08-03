#include <autoplay.hpp>

#include <pthread.h>

AutoPlay::AutoPlay()
{
    engine = core->engine;
    board = core->board;
    Reset();
}

void AutoPlay::Reset()
{
    board->Reset();
    core->engine->SetLevel(8);
    core->engine->RunVoidCommand("ucinewgame");
    core->engine->SetPosition(core->board->GetFENString());

    engine_thread_done = false;
    engine_thread_started = false;
    engine_time = 0;
}

static void* EngineMakeMove(void *obj)
{
    AutoPlay *autoplay = (AutoPlay*)obj;

    core->engine->SetPosition(core->board->GetFENString());
    autoplay->engine_move = core->engine->GetBestMove();
    autoplay->engine_thread_done = true;

    return nullptr;
}

void AutoPlay::Process()
{
    if(engine_thread_done)
    {
        if(engine_time < 1.f)
        {
            engine_time += core->delta_time;
        }
        else
        {
            engine_thread_started = false;
            engine_thread_done = false;
            engine_time = 0;
            board->MakeMove(engine_move);
            if(board->IsGameFinished())
            {
                board->Reset();
            }
        }
    }
    else
    {
        if(!engine_thread_started)
        {
            engine_thread_started = true;
            pthread_t move_thread;
            pthread_create(&move_thread, nullptr, EngineMakeMove, (void*)this);
        }
        engine_time += core->delta_time;
    }
}

void AutoPlay::Render(RenderTexture2D _render_target)
{
    float board_size = _render_target.texture.width;
    float board_offset = board_size*0.02f;
    float border_size = board_size*0.03f;
    float cell_size = (board_size*0.9f)/8.f;

    BeginTextureMode(_render_target);
    ClearBackground({27, 73, 101, 255});
    BeginMode2D(core->default_cam);
        for(int i = 0; i < 8; i++)
        {
            for(int j = 0; j < 8; j++)
            {
                if(i%2 == j%2)
                {
                    DrawRectangle(board_offset+border_size+i*cell_size, board_offset+border_size+j*cell_size, cell_size, cell_size, {202, 233, 255, 255});
                }
                else
                {
                    DrawRectangle(board_offset+border_size+i*cell_size, board_offset+border_size+j*cell_size, cell_size, cell_size, {95, 168, 211, 255});
                }
            }
        }
        for(int i = 0; i < 8; i++)
        {
            for(int j = 0; j < 8; j++)
            {
                if(board->_squares[j*8+i] != EMPTY)
                {
                    DrawTexturePro(assets->piece_png[board->_squares[j*8+i]-1], {0, 0, 16, 16}, {board_offset+border_size+(i+0.5f)*cell_size, board_offset+border_size+(j+0.5f)*cell_size, cell_size*0.8f, cell_size*0.8f}, {cell_size*0.4f, cell_size*0.4f}, 0, WHITE);
                }
            }
        }
        DrawRectangleLinesEx({board_offset, board_offset, board_size-2*board_offset, board_size-2*board_offset}, 2, {202, 233, 255, 255});
        DrawRectangleLinesEx({board_offset+border_size, board_offset+border_size, board_size*0.9f, board_size*0.9f}, 2, {202, 233, 255, 255});
        for(int i = 0; i < 8; i++)
        {
            std::string text = "";
            text += char('a'+i);
            // DrawText(assets->quaver_ttf[0], text.c_str(), {board_offset+border_size+(i+0.5f)*cell_size, board_offset+border_size*.5f}, true, true, {202, 233, 255, 255});
            DrawText(assets->quaver_ttf[0], text.c_str(), {board_offset+border_size+(i+0.5f)*cell_size, board_offset+border_size+board_size*.9f+border_size*.5f}, true, true, {202, 233, 255, 255});
        }
        for(int j = 0; j < 8; j++)
        {
            std::string text = "";
            text += char('0'+8-j);
            DrawText(assets->quaver_ttf[0], text.c_str(), {board_offset+border_size*.5f, board_offset+border_size+(j+0.5f)*cell_size}, {202, 233, 255, 255});
            // DrawText(assets->quaver_ttf[0], text.c_str(), {board_offset+border_size+board_size*.9f+border_size*.5f, board_offset+border_size+(j+0.5f)*cell_size}, {202, 233, 255, 255});
        }
        DrawRectangle(0, 0, board_size, board_size, BLACK|200);
    EndMode2D();
    EndTextureMode();
}