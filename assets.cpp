#include <assets.hpp>

Assets::Assets()
{
    for(int i = 1; i <= 5; i++)
    {
        micro5_ttf[i-1] = LoadFont("./assets/Micro5.ttf", core->vp_height*.05f*i);
    }
    for(int i = 1; i <= 5; i++)
    {
        quaver_ttf[i-1] = LoadFont("./assets/quaver.ttf", core->vp_height*.025f*i);
    }

    piece_png[0] = LoadTexture("./assets/rook_b.png");
    piece_png[1] = LoadTexture("./assets/knight_b.png");
    piece_png[2] = LoadTexture("./assets/bishop_b.png");
    piece_png[3] = LoadTexture("./assets/queen_b.png");
    piece_png[4] = LoadTexture("./assets/king_b.png");
    piece_png[5] = LoadTexture("./assets/pawn_b.png");
    piece_png[6] = LoadTexture("./assets/rook_w.png");
    piece_png[7] = LoadTexture("./assets/knight_w.png");
    piece_png[8] = LoadTexture("./assets/bishop_w.png");
    piece_png[9] = LoadTexture("./assets/queen_w.png");
    piece_png[10] = LoadTexture("./assets/king_w.png");
    piece_png[11] = LoadTexture("./assets/pawn_w.png");

    card_back_png = LoadTexture("./assets/card_back.png");
    card_plus5_png = LoadTexture("./assets/card_plus5.png");
    card_switch_png = LoadTexture("./assets/card_switch.png");

    point_f_png = LoadTexture("./assets/point_f.png");
    point_s_png = LoadTexture("./assets/point_s.png");

    empty_rec_f_png = LoadTexture("./assets/empty_rec_f.png");
    empty_rec_s_png = LoadTexture("./assets/empty_rec_s.png");
    back_patch_info.layout = NPATCH_NINE_PATCH;
    back_patch_info.top = 11;
    back_patch_info.bottom = 21;
    back_patch_info.left = 11;
    back_patch_info.right = 21;
    back_patch_info.source = {0, 0, 32, 32};
}