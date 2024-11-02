#include "gametank.h"
#include "drawing_funcs.h"
#include "input.h"
#include "persist.h"
#include "music.h"
#include "banking.h"
#include "gen/assets/auxilia.h"
#include "gen/assets/auxilia0.h"
#include "gen/assets/sfx.h"

#define VRAM_WORLD_TILESET_BANK  0
#define VRAM_SPRITES_BANK        1
#define VRAM_SPLASH_BANK         2
#define VRAM_CITY_TILESET_BANK   3
#define VRAM_FONTS_BANK          4
#define VRAM_ARMORS_F_BANK       5

#define WORLD_TILEMAP_BANK  0xFB
#define CITY_TILEMAP_BANK   0xFA
#define MISC_CODE_BANK      0xF9

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
unsigned char map;

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
Bit 2 (4): actionable
Bit 3 (8): automatic action
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
  2,  2,  3,  3,  3,  3,  3,  11
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
    {7096, VRAM_WORLD_TILESET_BANK, world_tileset_property},
    {7067, VRAM_CITY_TILESET_BANK, city_tileset_property}
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
    map = map_idx;
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

#define draw_tile_init(w, h, ramBank) \
    flagsMirror |= DMA_ENABLE | DMA_OPAQUE | DMA_IRQ | DMA_GCARRY; \
    flagsMirror &= ~(DMA_COLORFILL_ENABLE | DMA_OPAQUE); \
    *dma_flags = flagsMirror; \
    banksMirror &= BANK_RAM_MASK & ~BANK_SECOND_FRAMEBUFFER; \
    banksMirror |= bankflip | ramBank; \
    *bank_reg = banksMirror; \
    await_drawing(); \
    vram[WIDTH] = w; \
    vram[HEIGHT] = h;

#define draw_left_tile(x, y, gx,  gy) \
    await_drawing(); \
    vram[VX] = x; \
    vram[VY] = y; \
    vram[GX] = gx; \
    vram[GY] = gy; \
    draw_busy = 1; \
    vram[START] = 1;

#define draw_next_tile(x, gx,  gy) \
    await_drawing(); \
    vram[VX] = x; \
    vram[GX] = gx; \
    vram[GY] = gy; \
    draw_busy = 1; \
    vram[START] = 1;

#include "misc.h"

void interact(unsigned char tile) {
    if (map == 0) {
        change_rom_bank(CITY_TILEMAP_BANK);
        set_map(1);
        return;
    }
    // We leave the city
    if (tile == 63) {
        change_rom_bank(WORLD_TILEMAP_BANK);
        set_map(0);
        return;
    }

//    change_rom_bank(0xF9);
//    shop_armors();
//    change_rom_bank(0xFB);
}

void init_game() {
    seed = 42;
    player_x = -8;
    player_y = 0;
    player_dir = 0;
    player_step = 0;
    tile_bank = VRAM_WORLD_TILESET_BANK;
    map = 0;
    for (tmp=0; tmp<63; tmp++) {
        visible[tmp] = 0;
    }
}

int main () {
    init_graphics();
    init_dynawave();
    init_music();

    load_spritesheet((char*)&ASSET__auxilia__tileset_bmp, VRAM_WORLD_TILESET_BANK);
    load_spritesheet((char*)&ASSET__auxilia__sprites_bmp, VRAM_SPRITES_BANK);
    load_spritesheet((char*)&ASSET__auxilia0__splash_bmp, VRAM_SPLASH_BANK);
    load_spritesheet((char*)&ASSET__auxilia__tileset_city_bmp, VRAM_CITY_TILESET_BANK);
    load_spritesheet((char*)&ASSET__auxilia__font_bmp, VRAM_FONTS_BANK);
    load_spritesheet((char*)&ASSET__auxilia__armorsf_bmp, VRAM_ARMORS_F_BANK);

    clear_border(0);
    await_draw_queue();
    flip_pages();
    await_draw_queue();
    clear_border(0);
    await_draw_queue();

//    change_rom_bank(MISC_CODE_BANK);
//    shop_armors();
//    splash();
//    preamble();

    change_rom_bank(WORLD_TILEMAP_BANK);
    init_game();
    set_map(0);

    while (1) {                                     //  Run forever
        clear_screen(0);
        clear_border(0);
        await_draw_queue();

        // Draw the playfield
//        rect.x=-8;rect.y=player_y;
//        rect.w=16;rect.h=16;
//        rect.b=tile_bank | BANK_CLIP_X;

        tilemap_ptr2 = visible;

        tmp = *tilemap_ptr2;
        tmp_x = -8;
        tmp_y = 0;

        //*bank_reg = BANK_CLIP_X | BANK_CLIP_Y;
        draw_tile_init(16, 16, tile_bank | BANK_CLIP_X);

        draw_left_tile(-8, 0, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(8, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(24, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(40, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(56, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(72, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(88, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(104, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(120, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2 ++; tmp = *tilemap_ptr2;
        draw_left_tile(-8, 16, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(8, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(24, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(40, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(56, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(72, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(88, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(104, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(120, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2 ++; tmp = *tilemap_ptr2;
        draw_left_tile(-8, 32, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(8, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(24, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(40, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(56, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(72, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(88, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(104, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(120, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2 ++; tmp = *tilemap_ptr2;
        draw_left_tile(-8, 48, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(8, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(24, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(40, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(56, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(72, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(88, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(104, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(120, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2 ++; tmp = *tilemap_ptr2;
        draw_left_tile(-8, 64, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(8, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(24, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(40, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(56, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(72, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(88, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(104, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(120, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2 ++; tmp = *tilemap_ptr2;
        draw_left_tile(-8, 80, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(8, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(24, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(40, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(56, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(72, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(88, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(104, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(120, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2 ++; tmp = *tilemap_ptr2;
        draw_left_tile(-8, 96, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(8, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(24, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(40, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(56, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(72, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(88, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(104, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tilemap_ptr2++; tmp = *tilemap_ptr2;
        draw_next_tile(120, (tmp & 0x7) << 4, (tmp & 0xF8) << 1);
        await_drawing();

        draw_sprite(56, 48, 16, 16, 0, 0, VRAM_SPRITES_BANK);
        draw_sprite(0, 112, 96, 16, 0, 112, VRAM_SPRITES_BANK);
        await_draw_queue();

        PROFILER_END(0);
        sleep(1);
        flip_pages();
        PROFILER_START(0);

        if (player_step) {
            player_step--;
        } else {
            update_inputs();
            tmp2 = 0;
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
            // If we take action against an actionable tile
            if ((player1_buttons & INPUT_MASK_A) && (tmp2 & 0x4)) {
                interact(tmp);
            // If we're about to step an an automatic actionable tile
            } else if (tmp2 & 0x8) {
                interact(tmp);
            }
        }
        set_visible_tiles();

        tick_music();
    }

  return (0);
}
