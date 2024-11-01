#include "gametank.h"
#include "drawing_funcs.h"
#include "input.h"
#include "persist.h"
#include "music.h"
#include "banking.h"
#include "gen/assets/auxilia.h"
#include "gen/assets/sfx.h"

#pragma data-name (push, "SAVE")
#pragma data-name (pop)

#pragma bss-name (push,"ZEROPAGE")
// Temp variables
unsigned char *tile;
unsigned char tmp, tmp2, tmp3, tile_val, tile_val2;
unsigned char tmp_x, tmp_y;
int long_val1, long_val2, long_tmp;
unsigned char player_tilemap;
unsigned char player_idx;
unsigned char player_dir;
unsigned char player_step;
char player_x;
char player_y;
unsigned char x_tile, y_tile;
unsigned char *tilemap_ptr;
unsigned char *tilemap_ptr2;
unsigned char *player_ptr;
struct TileVisibility *tile_vis;

unsigned char buffer1[9];
unsigned char visible[63];
unsigned char buffer2[9];
unsigned char tile_bank;

#pragma bss-name (pop)

const unsigned char dir_default[4] = { 1, 2, 2, 1};
const unsigned char ai_new_target_x[4] = { 4, 11, 5, 10 };
const unsigned char ai_new_target_y[4] = { 1, 13, 7,  7 };

#pragma data-name (push, "DATA")

// So that the tilemap starts at an 0x..10 address (easier for debugging)
unsigned char padding[8] = { 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55 };

/*
Bit 0 (1): can pass through
Bit 1 (2): can see through
Bit 2 (4): special action
*/
const unsigned char world_tileset_property[64] = {
  3,  3,  3,  3,  1,  1,  1,  6,
  3,  3,  3,  0,  0,  0,  0,  3,
  3,  6,  3,  2,  2,  2,  2,  3,
  3,  3,  3,  3,  6,  6,  6,  6,
  2,  3,  3,  3,  6,  6,  6,  6,
  2,  2,  3,  3,  3,  3,  3,  3,
  2,  2,  3,  3,  0,  3,  2,  3,
  2,  2,  3,  3,  3,  3,  3,  3
};

const unsigned char city_tileset_property[64] = {
  2,  2,  2,  2,  2,  2,  2,  2,
  2,  2,  2,  2,  2,  2,  2,  2,
  2,  2,  2,  2,  2,  3,  3,  3,
  3,  3,  3,  3,  0,  3,  3,  3,
  3,  3,  3,  3,  0,  2,  2,  2,
  2,  2,  3,  3,  3,  2,  2,  2,
  2,  2,  3,  3,  0,  2,  3,  3,
  2,  2,  3,  3,  3,  3,  3,  3
};

unsigned char tileset_property[64];

const unsigned char visibility_scan[64] = {
    31,    21,22,23,   30,32,   39,40,41,
    11,12,13,14,15,   20,24,   29,33,   38,42,   47,48,49,50,51,
    10,16,   19,25,   28,34,   37,43,   46,52,
    
    1,2,3,4,5,6,7,   55,56,57,58,59,60,61,
    0,8,   9,17,   18,26,   27,35,   36,44,   45,53,   54,62
};

const int tilemap_scan[64] = {
    388,   259,260,261,   387,389,   515,516,517,
    130,131,132,133,134,   258,262,   386,390,   514,518,   642,643,644,645,646,
    129,135,   257,263,   385,391,   513,519,   641,647,
    
    1,2,3,4,5,6,7,   769,770,771,772,773,774,775,
    0,8,   128,136,   256,264,   384,392,   512,520,   640,648,   768,776
};

const int screen_to_tilemap[64] = {
      0,   1,   2,   3,   4,   5,   6,   7,   8,
    128, 129, 130, 131, 132, 133, 134, 135, 136,
    256, 257, 258, 259, 260, 261, 262, 263, 264,
    384, 385, 386, 387, 388, 389, 390, 391, 392,
    512, 513, 514, 515, 516, 517, 518, 519, 520,
    640, 641, 642, 643, 644, 645, 646, 647, 648,
    768, 769, 770, 771, 772, 773, 774, 775, 776
};

struct Map {
    int entry_offset;
    unsigned char tile_bank;
    const unsigned char *tileset_property;
};

const struct Map maps[2] = {
    {7096, 0, world_tileset_property},
    {7067, 3, city_tileset_property}
};

struct TileVisibility {
    unsigned char otherTile1;
    unsigned char otherTile2;
    unsigned char otherTile3;
};

const struct TileVisibility visible_view[35] = {
    {255, 255, 255},

    {11, 12, 20},
    {12, 13, 14},
    {14, 15, 24},
    {20, 29, 38},
    {24, 33, 42},
    {38, 47, 48},
    {48, 49, 50},
    {50, 51, 42},

    {1, 255, 255},
    {2, 3, 255},
    {4, 255, 255},
    {5, 6, 255},
    {7, 255, 255},
    {10, 19, 255},
    {16, 25, 255},
    {28, 255, 255},
    {34, 255, 255},
    {37, 46, 255},
    {43, 52, 255},
    {55, 255, 255},
    {56, 57, 255},
    {58, 255, 255},
    {59, 60, 255},
    {61, 255, 255},

    {0, 9, 255},
    {8, 17, 255},
    {9, 18, 255},
    {17, 26, 255},
    {27, 255, 255},
    {35, 255, 255},
    {36, 45, 255},
    {44, 53, 255},
    {45, 54, 255},
    {53, 62, 255}
};

#pragma data-name (pop)

unsigned char player_ptr_default[4] = {17, 222, 30, 209};
unsigned char player_x_default[4] = { 8, 112, 112, 8 };
unsigned char player_y_default[4] = { 8, 104, 8, 104};
unsigned char player_tile_default[4] = { 8, 96, 64, 40 };
unsigned char player_score[4] = { 0, 0, 0, 0 };
unsigned char player_score_tile[4] = { 0, 0, 0, 0 };

// AI: where the CPU players want to go
unsigned char target_x[2] = { 10, 0 };
unsigned char target_y[2] = { 12, 0 };
unsigned char moving[2] = { 0, 0 };
unsigned char steps[2] = { 0, 0 };
// Tile to use to display the players
unsigned char player_tile[4] = { 8, 96, 64, 40 };
unsigned char player_invincible[4] = {0, 0, 0, 0};

const unsigned char ai_default_target_x[4] = {0, 0, 10, 5};
const unsigned char ai_default_target_y[4] = {0, 0, 2, 12};
unsigned char ai_target_x[4], ai_target_y[4];
unsigned char ai_direction_mask[4];

unsigned char seed;

// Whether there is a gun at the 4 locations
unsigned char gun[4] = { 0, 0, 0, 0 };  // 0 means there is a gun

unsigned char rand() {
    tmp = seed << 1;
    if (tmp & 0x80) {
        tmp ^= 0x1d;
    }
    seed = tmp;
    return seed;
}

void breakpoint() {}
void hit();

#include "tilemap.h"

void set_map(unsigned char map_idx) {
    struct Map *map = (struct Map *)&maps[map_idx];
    tilemap_ptr = (unsigned char *)&tilemap0[map->entry_offset];
    tilemap_ptr2 = tilemap_ptr;
    player_ptr = (unsigned char *)&tilemap0[map->entry_offset+128*3+4];
    tile_bank = map->tile_bank;
    for (tmp=0; tmp<64; tmp++) {
        tileset_property[tmp] = map->tileset_property[tmp];
    }
}

void set_visible_tiles() {
    for (tmp=0;tmp<64;tmp++) {
        visible[tmp]=52;
    }
    for (tmp=0;tmp<9;tmp++) {
        tmp2 = visibility_scan[tmp];
        long_tmp = tilemap_scan[tmp];
        visible[tmp2] = tilemap_ptr[long_tmp];
    }

    for (tmp=1; tmp<35; tmp++) {
        tmp2 = visibility_scan[tmp];
        long_tmp = tilemap_scan[tmp];
        tile_val = tilemap_ptr[long_tmp];
        tile_val2 = tileset_property[tile_val];
        if ((tile_val2 & 0x2) && visible[tmp2] != 52) {
            tile_vis = (struct TileVisibility *)&visible_view[tmp];
            tmp2 = tile_vis->otherTile1;
            long_tmp = screen_to_tilemap[tmp2];
            if (tmp2 != 255) visible[tmp2] = tilemap_ptr[long_tmp];
            tmp2 = tile_vis->otherTile2;
            long_tmp = screen_to_tilemap[tmp2];
            if (tmp2 != 255) visible[tmp2] = tilemap_ptr[long_tmp];
            tmp2 = tile_vis->otherTile3;
            long_tmp = screen_to_tilemap[tmp2];
            if (tmp2 != 255) visible[tmp2] = tilemap_ptr[long_tmp];
        }
    }
}

#define draw_first_tile(x, y, gx, gy) \
    await_drawing(); \
    vram[VX] = x; \
    vram[VY] = y; \
    vram[GX] = gx; \
    vram[GY] = gy; \
    vram[WIDTH] = 16;   \
    vram[HEIGHT] = 16;  \
    draw_busy = 1; \
    vram[START] = 1;

#define draw_tile(x, gx,  gy) \
    await_drawing(); \
    vram[VX] = x; \
    vram[GX] = gx; \
    vram[GY] = gy; \
    draw_busy = 1; \
    vram[START] = 1;

void splash() {
    draw_sprite(0, 0, 127, 127, 0, 0, 2);
    while (1) {
        clear_screen(0);
        clear_border(0);
        rand();
        draw_sprite(0, 0, 127, 127, 0, 0, 2);
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

        tmp_y = 0;

        for (tmp3=0; tmp3<20; tmp3++) {
            tmp_x = 2;
            tilemap_ptr = (unsigned char*)preamble_text[tmp3+tile_val];
            tmp = tilemap_ptr[0];
            while (tmp) {
                if (tmp >= 'a' && tmp <= 'z') {
                    tmp2 = (tmp - 'a') << 2;
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, tmp2, 61, 4);
                } else if (tmp >= 'A' && tmp <= 'M') {
                    tmp2 = (tmp - 'A') * 9;
                    draw_sprite_now(tmp_x, tmp_y, 9, 11, tmp2, 22, 4);
                    tmp_x += 5;
                    tmp_y += 6;
                } else if (tmp >= 'O' && tmp <= 'Z') {
                    tmp2 = (tmp - 'O') * 9;
                    draw_sprite_now(tmp_x, tmp_y, 9, 11, tmp2, 33, 4);
                    tmp_x += 5;
                    tmp_y += 6;
                } else if (tmp == ',') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 108, 61, 4);
                } else if (tmp == '.') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 104, 61, 4);
                } else if (tmp == '\'') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 116, 61, 4);
                } else if (tmp == '"') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 112, 61, 4);
                } else if (tmp == '-') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 120, 61, 4);
                } else if (tmp == '!') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 124, 61, 4);
                } else if (tmp == '_') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 0, 66, 4);
                }
                tmp_x += 5;
                tilemap_ptr++;
                tmp = tilemap_ptr[0];
            }
            tilemap_ptr++;
            tmp_y += 6;
        }

        await_draw_queue();
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

int main () {
    load_spritesheet((char*)&ASSET__auxilia__tileset_bmp, 0);
    load_spritesheet((char*)&ASSET__auxilia__sprites_bmp, 1);
    load_spritesheet((char*)&ASSET__auxilia__splash_bmp, 2);
    load_spritesheet((char*)&ASSET__auxilia__tileset_city_bmp, 3);
    load_spritesheet((char*)&ASSET__auxilia__font_bmp, 4);

    seed = 42;
    player_x = -8;
    player_y = 0;
    player_dir = 0;
    player_step = 0;
    tile_bank = 0;

    init_graphics();
    init_dynawave();
    init_music();

    flip_pages();
    clear_border(0);
    await_draw_queue();
    flip_pages();
    await_draw_queue();
    clear_border(0);

    *dma_flags = DMA_NMI | DMA_ENABLE | DMA_IRQ | DMA_OPAQUE | DMA_GCARRY;

    splash();

    preamble();

    for (tmp=0; tmp<63; tmp++) {
        visible[tmp] = 0;
    }

    change_rom_bank(0xFB);
    set_map(0);

    while (1) {                                     //  Run forever
        clear_screen(0);
        clear_border(0);

        // Draw the playfield
        rect.x=-8;rect.y=player_y;
        rect.w=16;rect.h=16;
        rect.b=tile_bank | BANK_CLIP_X;

        tilemap_ptr2 = visible;

        tmp = *tilemap_ptr2;
        tmp_x = -8;
        tmp_y = 0;

        //*bank_reg = BANK_CLIP_X | BANK_CLIP_Y;

/*
        draw_first_tile(-8, 0, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1);
        tmp_x=-8; tmp_y += 16; tilemap_ptr2 ++; tmp = *tilemap_ptr2;
        draw_first_tile(-8, tmp_y, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1);
        tmp_x=-8; tmp_y += 16; tilemap_ptr2 ++; tmp = *tilemap_ptr2;
        draw_first_tile(-8, tmp_y, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1);
        tmp_x=-8; tmp_y += 16; tilemap_ptr2 ++; tmp = *tilemap_ptr2;
        draw_first_tile(-8, tmp_y, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1);
        tmp_x=-8; tmp_y += 16; tilemap_ptr2 ++; tmp = *tilemap_ptr2;
        draw_first_tile(-8, tmp_y, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1);
        tmp_x=-8; tmp_y += 16; tilemap_ptr2 ++; tmp = *tilemap_ptr2;
        draw_first_tile(-8, tmp_y, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1);
        tmp_x=-8; tmp_y += 16; tilemap_ptr2 ++; tmp = *tilemap_ptr2;
        draw_first_tile(-8, tmp_y, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_tile(tmp_x, (tmp & 0x7) << 4, (tmp & 0xF8) << 1);
*/
//        draw_sprite_now(10, 10, 16, 16, 0, 0, 0);


        tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect();
        rect.x = player_x; rect.y += 16; tilemap_ptr2 ++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect();
        rect.x = player_x; rect.y += 16; tilemap_ptr2 ++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect();
        rect.x = player_x; rect.y += 16; tilemap_ptr2 ++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect();
        rect.x = player_x; rect.y += 16; tilemap_ptr2 ++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect();
        rect.x = player_x; rect.y += 16; tilemap_ptr2 ++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect();
        rect.x = player_x; rect.y += 16; tilemap_ptr2 ++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect(); rect.x += 16; tilemap_ptr2++; tmp = *tilemap_ptr2; tmp_x = (tmp & 0x7) << 4; tmp_y = (tmp & 0xF8) << 1;
        rect.gx=tmp_x;rect.gy=tmp_y;draw_sprite_rect();

        draw_sprite(56, 48, 16, 16, 0, 0, 1);

        await_draw_queue();

        PROFILER_END(0);
        sleep(1);
        flip_pages();
        PROFILER_START(0);

        if (player_step) {
            player_step--;
        } else {
            update_inputs();
            if((player1_buttons & INPUT_MASK_LEFT)) {
                tilemap_ptr2 = player_ptr-1;
                tmp = *tilemap_ptr2;
                tmp2 = tileset_property[tmp];
                if (tmp2 & 0x1) {
                    player_step = 4;
                    tilemap_ptr--;
                    player_ptr--;
                }
            } else if ((player1_buttons & INPUT_MASK_RIGHT)) {
                tmp = player_ptr[1];
                tmp2 = tileset_property[tmp];
                if (tmp2 & 0x1) {
                    player_step = 4;
                    tilemap_ptr++;
                    player_ptr++;
                }
            } else if (player1_buttons & INPUT_MASK_UP) {
                tmp = player_ptr[-128];
                tmp2 = tileset_property[tmp];
                if (tmp2 & 0x1) {
                    player_step = 4;
                    tilemap_ptr -= 128;
                    player_ptr -= 128;
                } else if (tmp2 & 0x4) {
                    change_rom_bank(0xFA);
                    set_map(1);
                }
            } else if (player1_buttons & INPUT_MASK_DOWN) {
                tmp = player_ptr[128];
                tmp2 = tileset_property[tmp];
                breakpoint();
                if (tmp2 & 0x1) {
                    player_step = 4;
                    tilemap_ptr += 128;
                    player_ptr += 128;
                }
            }
        }
        set_visible_tiles();

        tick_music();
    }

  return (0);
}
