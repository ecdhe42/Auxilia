#include "auxilia.h"

void interact_castle();
void init_game();
void recompute_dashboard();

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
unsigned char player_step;
unsigned char x_tile, y_tile;
unsigned char *tilemap_ptr;
unsigned char *tmp_tilemap_ptr;
unsigned char *player_ptr;
unsigned char tilemap_x;
unsigned char tilemap_y;
struct TileVisibility *tile_vis;
char diff_x, diff_y;
unsigned char buffer1[9];
unsigned char visible[63];
unsigned char buffer2[9];
unsigned char tile_bank;
unsigned char map_id;

int attr_hp;
int attr_xp;
int attr_gp;
int attr_mana;
unsigned char attr_gender;
unsigned char attr_level;
unsigned char attr_strength;
unsigned char attr_dext;
unsigned char attr_stamina;
unsigned char attr_int;
unsigned char attr_hp_digits[4];
unsigned char attr_xp_digits[4];
unsigned char attr_gp_digits[4];
unsigned char attr_mana_digits[4];
unsigned char attr_level_digits[2];
#pragma bss-name (pop)

#pragma data-name (push, "DATA")
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
  6,  6,  6,  6,  6,  6,  6,  6,
  6,  6,  6,  6,  6,  6,  6,  6,
  6,  6,  6,  6,  6,  3,  3,  3,
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
    unsigned char entry_x;
    unsigned char entry_y;
    unsigned char tile_bank;
    const unsigned char *tileset_property;
};

struct Monster monsters[NB_MONSTERS];

const struct Map maps[2] = {
    {0x1BB8, 0x38, 0x37, VRAM_WORLD_TILESET_BANK, world_tileset_property},
    {0x1B9B, 0x1B, 0x37, VRAM_CITY_TILESET_BANK, city_tileset_property}
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

void breakpoint() {}

#include "tilemap.h"

void set_map(unsigned char map_idx) {
    struct Map *map = (struct Map *)&maps[map_idx];
    map_id = map_idx;
    tilemap_ptr = (unsigned char *)&tilemap0[map->entry_offset];
    tilemap_x = map->entry_x;
    tilemap_y = map->entry_y;

    tmp_tilemap_ptr = tilemap_ptr;
    player_ptr = (unsigned char *)&tilemap0[map->entry_offset+128*3+4];
    tile_bank = map->tile_bank;
    for (tmp=0; tmp<64; tmp++) {
        tileset_property[tmp] = map->tileset_property[tmp];
    }
}

/*
Sets the visible tiles, not showing hidden tiles
Tiles with the top bit on means there is a monster
*/
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

    // Check whether monsters are on the visible tilemap
    for (tmp=0; tmp<NB_MONSTERS; tmp++) {
        if (!monsters[tmp].tile_idx) continue;
        tmp_x = (char)(monsters[tmp].x) - (char)tilemap_x;
        tmp_y = (char)(monsters[tmp].y) - (char)tilemap_y;
        if (//diff_x < 128 &&
            tmp_x < 9 &&
//            diff_y < 128 &&
            tmp_y < 7) {
            tmp2 = tmp_y*9 + tmp_x;
            visible[tmp2] |= 0x80;
            monsters[tmp].tile_idx |= 0x80;
            monsters[tmp].visible_x = tmp_x;
            monsters[tmp].visible_y = tmp_y;
        } else {
            monsters[tmp].tile_idx &= 0x7F;
        }
    }
}

//#include "misc.h"

void interact(unsigned char tile) {
    if (map_id == 0) {
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

    change_rom_bank(MISC_CODE_BANK);
    interact_castle(tile);
    change_rom_bank(CITY_TILEMAP_BANK);
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

    change_rom_bank(MISC_CODE_BANK);
    init_game();

    clear_border(0);
    draw_dashboard();
    await_draw_queue();
    flip_pages();
    await_draw_queue();
    clear_border(0);
    draw_dashboard();
    await_draw_queue();

    change_rom_bank(WORLD_TILEMAP_BANK);
    set_map(0);

    while (1) {                                     //  Run forever
//        clear_screen(0);
//        clear_border(0);
        await_draw_queue();

        // Draw the playfield
//        rect.x=-8;rect.y=player_y;
//        rect.w=16;rect.h=16;
//        rect.b=tile_bank | BANK_CLIP_X;

        tmp_tilemap_ptr = visible;

        tmp = *tmp_tilemap_ptr;
        tmp_x = -8;
        tmp_y = 0;

        //*bank_reg = BANK_CLIP_X | BANK_CLIP_Y;
        draw_tile_init(16, 16, tile_bank | BANK_CLIP_X);

        draw_left_tile(-8, 0, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(8, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(24, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(40, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(56, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(72, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(88, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(104, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(120, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr ++; tmp = *tmp_tilemap_ptr;
        draw_left_tile(-8, 16, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(8, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(24, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(40, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(56, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(72, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(88, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(104, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(120, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr ++; tmp = *tmp_tilemap_ptr;
        draw_left_tile(-8, 32, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(8, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(24, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(40, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(56, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(72, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(88, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(104, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(120, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr ++; tmp = *tmp_tilemap_ptr;
        draw_left_tile(-8, 48, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(8, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(24, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(40, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(56, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(72, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(88, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(104, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(120, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr ++; tmp = *tmp_tilemap_ptr;
        draw_left_tile(-8, 64, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(8, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(24, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(40, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(56, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(72, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(88, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(104, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(120, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr ++; tmp = *tmp_tilemap_ptr;
        draw_left_tile(-8, 80, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(8, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(24, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(40, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(56, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(72, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(88, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(104, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(120, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr ++; tmp = *tmp_tilemap_ptr;
        draw_left_tile(-8, 96, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(8, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(24, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(40, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(56, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(72, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(88, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(104, (tmp & 0x7) << 4, (tmp & 0xF8) << 1); tmp_tilemap_ptr++; tmp = *tmp_tilemap_ptr;
        draw_next_tile(120, (tmp & 0x7) << 4, (tmp & 0xF8) << 1);

        await_drawing();

        draw_sprite(56, 48, 16, 16, 0, 0, VRAM_SPRITES_BANK);
        for (tmp=0; tmp<NB_MONSTERS; tmp++) {
            tile_val = monsters[tmp].tile_idx;
            if (tile_val & 0x80) {
                draw_sprite(monsters[tmp].visible_x*16-8, monsters[tmp].visible_y*16, 16, 16, (tile_val & 0x7) << 3, (tile_val & 0x78) << 1, VRAM_SPRITES_BANK | BANK_CLIP_X);
            }
        }
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
//                tmp_tilemap_ptr = player_ptr-1;
                tmp_tilemap_ptr = player_ptr-1;
                tmp = visible[30];
                tmp2 = tileset_property[tmp];
                if (tmp2 & 0x1 && !(tmp & 0x80)) {
                    player_step = 4;
                    tilemap_ptr--;
                    tilemap_x--;
                    player_ptr--;
                }
            } else if ((player1_buttons & INPUT_MASK_RIGHT)) {
                tmp_tilemap_ptr = player_ptr+1;
                tmp = visible[32];
                tmp2 = tileset_property[tmp];
                if (tmp2 & 0x1 && !(tmp & 0x80)) {
                    player_step = 4;
                    tilemap_ptr++;
                    tilemap_x++;
                    player_ptr++;
                }
            } else if (player1_buttons & INPUT_MASK_UP) {
                tmp = visible[22];
                tmp2 = tileset_property[tmp];
                if (tmp2 & 0x1 && !(tmp & 0x80)) {
                    player_step = 4;
                    tilemap_ptr -= 128;
                    tilemap_y--;
                    player_ptr -= 128;
                }
            } else if (player1_buttons & INPUT_MASK_DOWN) {
                tmp = visible[40];
                tmp2 = tileset_property[tmp];
                if (tmp2 & 0x1 && !(tmp & 0x80)) {
                    player_step = 4;
                    tilemap_ptr += 128;
                    tilemap_y++;
                    player_ptr += 128;
                }
            }
            // If we take action against an actionable tile
            if (player1_buttons & ~player1_old_buttons & INPUT_MASK_A) {
                if (tmp2 & 0x4) {
                    interact(tmp);
                } else if (tmp & 0x80) {
                    for (tmp=0; tmp<NB_MONSTERS; tmp++) {
                        breakpoint();
                        if (!monsters[tmp].tile_idx) continue;
                        if (monsters[tmp].tilemap_ptr == tmp_tilemap_ptr) {
                            monsters[tmp].hp--;
                            if (monsters[tmp].hp == 0) {
                                play_sound_effect((char*)&ASSET__sfx__hit_bin, 2);
                                monsters[tmp].tile_idx = 0;
                                attr_gp += monsters[tmp].gp;
                                attr_xp += monsters[tmp].xp;
                                change_rom_bank(MISC_CODE_BANK);
                                recompute_dashboard();
                                change_rom_bank(WORLD_TILEMAP_BANK);
                            } else {
                                play_sound_effect((char*)&ASSET__sfx__gunshot_bin, 1);
                            }
                        }
                    }
                }
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
