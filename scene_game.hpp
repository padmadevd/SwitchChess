#ifndef SCENE_GAME_HPP
#define SCENE_GAME_HPP

#include <utils.hpp>
#include <core.hpp>
#include <assets.hpp>
#include <chess.hpp>
#include <anim_text.hpp>

enum BoardState
{
    BOARD_OPENING,
    BOARD_NORMAL,
    ON_MAKE_MOVE,
    ON_UN_MAKE_MOVE,
    BOARD_CLOSING,
    BOARD_CLOSED
};

struct GameBoard
{
    BoardState state;

    Vector2 position;

    float squares_size;
    std::vector<Vector2> squares_position;

    std::vector<float> squares_anim_time;
    std::vector<float> squares_delay;
    std::vector<float> squares_scale;
    std::vector<uint8_t> squares_order;

    std::vector<float> letter_delay;
    std::vector<float> letter_anim_time;
    std::vector<float> letter_scale;

    uint8_t sel_index;
    float sel_anim_time;
    float sel_scale;
    std::vector<Move> moves;
    std::vector<float> moves_anim_time;
    std::vector<float> moves_delay;
    std::vector<uint8_t> moves_opacity;
    std::vector<uint8_t> moves_order;

    bool user_moved;

    Move curr_move;
    float anim_time;

    uint8_t player_color;

    GameBoard();
    void Reset();
    void Process();
    void Open();
    void MakeMove(Move _move);
    void UnMakeMove(Move _move);
    void Close();
    void Render();
};

enum SelectPromoteState
{
    SP_OPEN,
    SP_SELECT,
    SP_DONE
};

struct SelectPromote
{
    SelectPromoteState state;

    float anim_time;

    Vector2 position;
    Vector2 size;

    uint8_t player_color;

    float piece_anim_time[4];
    float piece_delay[4];
    float piece_scale[4];
    bool piece_hover[4];

    uint8_t piece_sel;

    SelectPromote();
    void Reset();
    void Process();
    void Render();
};

enum SelectCardState
{
    SC_OPEN,
    SC_SELECT,
    SC_SHOW,
    SC_CLOSE,
    SC_DONE
};

enum CardType
{
    CARD_PLUS5,
    CARD_SWITCH
};

struct GameSelectCard
{
    SelectCardState state;

    float anim_time;

    float card1_delay;
    float card1_anim_time;
    float card1_scale;
    CardType card1_type;
    Vector2 card1_pos;
    Vector2 card1_size;
    bool card1_hover;

    float card2_delay;
    float card2_anim_time;
    float card2_scale;
    CardType card2_type;
    Vector2 card2_pos;
    Vector2 card2_size;
    bool card2_hover;

    uint8_t card_sel;
    CardType card_sel_type;

    GameSelectCard();
    void Reset();
    void Process();
    void Render();
};

enum PlayerCardState
{
    PC_OPEN,
    PC_CLOSE,
};

struct GamePlayerCard
{
    PlayerCardState state;
    float anim_state;
    float scale;

    AnimTextLetter *id;
    AnimTextLetter *rating;
    AnimTextLetter *pass;
    AnimTextLetter *color;

    Vector2 position;
    Vector2 size;

    GamePlayerCard(std::string _id, std::string _rating, std::string _pass, float _duration);
    void Reset();
    void Open();
    void Process();
    void Close();
    void Render();
};

enum GameState
{
    SELECT_PROMOTION,
    PLAYING,
    SELECT_CARD,
    GAME_CLOSING,
    GAME_FINISHED
};

struct Game
{
    GameBoard *game_board;
    GameState state;

    uint8_t player_color;
    bool player_have_pass;
    bool engine_have_pass;

    bool engine_thread_started;
    bool engine_thread_done;
    Move engine_move;

    SelectPromote *select_promote;

    uint8_t remaining_full_moves;
    AnimTextLetter *remaining_moves_text;
    float remaining_moves_anim_time;

    GameSelectCard *game_select_card;

    AnimText *use_pass_text;
    AnimText *plus5_text;

    float text_anim_time;

    AnimText *pass_btn1_text;
    Vector2 pass_btn1_pos;
    Vector2 pass_btn1_size;
    bool pass_btn1_hover;

    AnimText *pass_btn2_text;
    Vector2 pass_btn2_pos;
    Vector2 pass_btn2_size;
    bool pass_btn2_hover;

    GamePlayerCard *player1_card;
    GamePlayerCard *player2_card;

    bool player_done;
    bool engine_done;

    int mate;
    bool engine_use_card;
    bool player_use_card;

    AnimText *select_card_text1;
    AnimText *select_card_text2;
    AnimText *select_card_text3;
    AnimText *select_card_text4;
    AnimText *select_card_text5;

    AnimTextLetter *skip_text;

    Game();
    void Reset(uint8_t _player_color, std::string _op_id, std::string _op_rate, std::string _op_pronoun, uint8_t _op_level);
    void Process();
    void Render();
};

enum SceneGameState
{
    ON_GAME,
    ON_PAUSE,
    ON_QUIT,
    ON_OPTIONS,
    ON_FINISH,
    SCENE_GAME_DONE
};

struct SceneGame
{
    SceneGameState state;

    Game *game;
    RenderTexture2D game_render;
    bool game_open;
    float game_anim_time;
    float game_scale;

    Vector2 pause_btn_pos;
    Vector2 pause_btn_size;
    float pause_btn_hover;
    AnimTextLetter *pause_btn_text;

    AnimText *pause_text;
    Vector2 pause_text_pos;

    AnimText *pause_option_text;
    Vector2 pause_option_pos;
    Vector2 pause_option_size;
    bool pause_option_hover;

    Vector2 options_btn_pos;
    Vector2 options_btn_size;
    float options_btn_hover;
    AnimTextLetter *options_btn_text;

    Vector2 quit_btn_pos;
    Vector2 quit_btn_size;
    float quit_btn_hover;
    AnimTextLetter *quit_btn_text;

    AnimText *quit_text;
    Vector2 quit_text_pos;

    AnimText *quit_option1_text;
    Vector2 quit_option1_pos;
    Vector2 quit_option1_size;
    bool quit_option1_hover;

    AnimText *quit_option2_text;
    Vector2 quit_option2_pos;
    Vector2 quit_option2_size;
    bool quit_option2_hover;

    AnimTextLetter *finish_title_text;
    Vector2 finish_title_pos;

    AnimTextLetter *finish_text;
    Vector2 finish_text_pos;

    AnimTextLetter *finish_btn_text;
    Vector2 finish_btn_pos;
    Vector2 finish_btn_size;
    bool finish_btn_hover;

    SceneGame();
    void Reset(uint8_t _player_color, std::string _op_id, std::string _op_rate, std::string _op_pronoun, uint8_t _op_level);
    void Process();
    void Render();
};

#endif