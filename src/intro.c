#include "auxilia.h"

extern unsigned char tmp, tmp2, tmp3, tmp_x, tmp_y, tile_val;
extern unsigned char *tilemap_ptr;
extern unsigned char *tmp_tilemap_ptr;
extern unsigned char *player_ptr;
extern int long_val1;

#pragma zpsym ("tmp");
#pragma zpsym ("tmp2");
#pragma zpsym ("tmp3");
#pragma zpsym ("tmp_x");
#pragma zpsym ("tmp_y");
#pragma zpsym ("tile_val");
#pragma zpsym ("long_val1");
#pragma zpsym ("tilemap_ptr");
#pragma zpsym ("tmp_tilemap_ptr");
#pragma zpsym ("player_ptr");

#pragma code-name(push, "PROG1")
void splash() {
    draw_sprite(0, 0, 127, 127, 0, 0, VRAM_SPLASH_BANK);
    while (1) {
        clear_screen(0);
        clear_border(0);
        draw_sprite(0, 0, 127, 127, 0, 0, VRAM_SPLASH_BANK);
        await_draw_queue();
        sleep(1);
        flip_pages();

        update_inputs();
        if (player1_buttons) break;
    }

    while (1) {
        update_inputs();
        if (!player1_buttons) break;
    }
}

const char *preamble_text[40] = {
    "_______________________",
    "An evil wizard is",
    "threatening the land of",
    "continentalia, sending",
    "hordes of monsters who",
    "will happily die for him,",
    "even when sent on suicide",
    "missions. the wizard is",
    "pure evil, so the only ",
    "course of action is the",
    "so-called \"beat-the-",
    "stuffing-outta-him\" ",
    "diplomacy. except he's",
    "powerful af.",
    "",
    "There is however a",
    "prophecy about a magical",
    "artifact that could beat",
    "him, but it's super hard",
    "to get. as if someone",
    "placed it in the least",
    "convenient location on",
    "purpose.",
    "",
    "Instead of sending his",
    "army's best elements on",
    "this dangerous quest, the",
    "ruler of the land, lord",
    "continental, prefers to",
    "rely on an inexperienced",
    "rando. this is absolutely",
    "not because he's afraid",
    "his population would",
    "overthrow him should he",
    "deplete the army keeping",
    "people in check.",
    "",
    "Congratulations hero!",
    "yes, you. no backsies.",
    ""
};

void preamble() {
    tile_val=0;

    while (1) {
        clear_screen(0);
        clear_border(0);
        await_draw_queue();

        tmp_y = 0;

        for (tmp3=0; tmp3<20; tmp3++) {
            tmp_x = 2;
            tmp_tilemap_ptr = (unsigned char*)preamble_text[tmp3+tile_val];
            tmp = tmp_tilemap_ptr[0];
            while (tmp) {
                if (tmp >= 'a' && tmp <= 'z') {
                    tmp2 = (tmp - 'a') << 2;
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, tmp2, 61, VRAM_FONTS_BANK);
                } else if (tmp >= 'A' && tmp <= 'N') {
                    tmp2 = (tmp - 'A') * 9;
                    draw_sprite_now(tmp_x, tmp_y, 9, 11, tmp2, 22, VRAM_FONTS_BANK);
                    tmp_x += 5;
                    tmp_y += 6;
                } else if (tmp >= 'O' && tmp <= 'Z') {
                    tmp2 = (tmp - 'O') * 9;
                    draw_sprite_now(tmp_x, tmp_y, 9, 11, tmp2, 33, VRAM_FONTS_BANK);
                    tmp_x += 5;
                    tmp_y += 6;
                } else if (tmp == ',') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 108, 61, VRAM_FONTS_BANK);
                } else if (tmp == '.') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 104, 61, VRAM_FONTS_BANK);
                } else if (tmp == '\'') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 116, 61, VRAM_FONTS_BANK);
                } else if (tmp == '"') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 112, 61, VRAM_FONTS_BANK);
                } else if (tmp == '-') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 120, 61, VRAM_FONTS_BANK);
                } else if (tmp == '!') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 124, 61, VRAM_FONTS_BANK);
                } else if (tmp == '_') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 0, 66, VRAM_FONTS_BANK);
                }
                tmp_x += 5;
                tmp_tilemap_ptr++;
                tmp = tmp_tilemap_ptr[0];
            }
            tmp_tilemap_ptr++;
            tmp_y += 6;
        }

        await_drawing();
        sleep(1);
        flip_pages();

        update_inputs();
        if (player1_buttons & INPUT_MASK_UP) {
            if (tile_val != 0) {
                tile_val--;
            }
        } else if (player1_buttons & INPUT_MASK_DOWN) {
            if (tile_val != 20) {
                tile_val++;
            }
        }

        if (player1_buttons & INPUT_MASK_A) break;
    }    
}

#pragma code-name (pop)
