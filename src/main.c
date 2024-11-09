#include "auxilia.h"

void splash();
void preamble();
void interact_castle(unsigned char tile);
void interact_sunglow(unsigned char tile);
void init_game();
void recompute_dashboard();
void monster_action();
void attack_monster();
void main_loop();

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
unsigned char tileset_bank;
unsigned char tilemap_bank;
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
  2,  6,  3,  3,  3,  3,  3,  3
};

const unsigned char city_tileset_property[64] = {
  6,  6,  6,  6,  6,  6,  6,  6,
  6,  6,  6,  6,  6,  6,  6,  6,
  6,  6,  6,  6,  6,  3,  3,  1,
  3,  3,  3,  3,  0,  3,  6,  6,
  3,  3,  3,  3,  0,  6,  6,  6,
  2,  2,  3,  2,  3,  2,  2,  2,
  2,  2,  3,  3,  0,  2,  3,  6,
  2,  2,  3,  3,  3,  3,  3,  11
};

unsigned char tileset_property[64];
unsigned char *world_tilemap_ptr;
unsigned char world_tilemap_x;
unsigned char world_tilemap_y;

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

// The reason we're using a bunch of variable instead of a struct
// is that, when converted into 6502 asm, going through an array of
// struct is expensive (e.g. you need to do a multiplication of sizeof(Monster)
// Using an array for each field is much easier (LDA monster_tile_idx,y)
unsigned char monster_tile_idx[NB_MONSTERS];
unsigned char *monster_tilemap_ptr[NB_MONSTERS];
unsigned char monster_x[NB_MONSTERS];
unsigned char monster_y[NB_MONSTERS];
unsigned char monster_visible_x[NB_MONSTERS];
unsigned char monster_visible_y[NB_MONSTERS];
unsigned char monster_hp[NB_MONSTERS];
unsigned char monster_gp[NB_MONSTERS];
unsigned char monster_xp[NB_MONSTERS];
unsigned char monster_strength[NB_MONSTERS];
unsigned char monster_hit[NB_MONSTERS];

//struct Monster monsters[NB_MONSTERS];
// We use these structures only when switching maps so the overhead
// isn't that big of a deal
struct Monster monsters_world[NB_MONSTERS];
struct Monster monsters_dungeon[NB_MONSTERS];

const struct Map maps[4] = {
    {0x1BB8, 0x38, 0x37, VRAM_WORLD_TILESET_BANK, WORLD_TILEMAP_BANK, world_tileset_property},  // World
    {0x1B9B, 0x1B, 0x37, VRAM_CITY_TILESET_BANK, CITY_TILEMAP_BANK, city_tileset_property},     // Castle
    {0xD42, 66, 26, VRAM_CITY_TILESET_BANK, CITY_TILEMAP_BANK, city_tileset_property},          // Sunglow
    {0xB9C, 28, 23, VRAM_CITY_TILESET_BANK, DUNGEON_TILEMAP_BANK, city_tileset_property}        // Dungeon
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
    breakpoint();
    // If we're leaving the world map, save the state (player position, monsters)
    if (map_id == 0) {
        for (tmp=0; tmp<NB_MONSTERS; tmp++) {
            monsters_world[tmp].tile_idx = monster_tile_idx[tmp];
            monsters_world[tmp].tilemap_ptr = monster_tilemap_ptr[tmp];
            monsters_world[tmp].x = monster_x[tmp];
            monsters_world[tmp].y = monster_y[tmp];
            monsters_world[tmp].visible_x = monster_visible_x[tmp];
            monsters_world[tmp].visible_y = monster_visible_y[tmp];
            monsters_world[tmp].hp = monster_hp[tmp];
            monsters_world[tmp].gp = monster_gp[tmp];
            monsters_world[tmp].xp = monster_xp[tmp];
            monsters_world[tmp].strength = monster_strength[tmp];
            monster_tile_idx[tmp] = 0;
        }
        world_tilemap_ptr = tilemap_ptr;
        world_tilemap_x = tilemap_x;
        world_tilemap_y = tilemap_y;
    }
    map_id = map_idx;
    // If we're back to the world map, restore the state (player position, monsters)
    if (map_id == 0) {
        for (tmp=0; tmp<NB_MONSTERS; tmp++) {
            monster_tile_idx[tmp] = monsters_world[tmp].tile_idx;
            monster_tilemap_ptr[tmp] = monsters_world[tmp].tilemap_ptr;
            monster_x[tmp] = monsters_world[tmp].x;
            monster_y[tmp] = monsters_world[tmp].y;
            monster_visible_x[tmp] = monsters_world[tmp].visible_x;
            monster_visible_y[tmp] = monsters_world[tmp].visible_y;
            monster_hp[tmp] = monsters_world[tmp].hp;
            monster_gp[tmp] = monsters_world[tmp].gp;
            monster_xp[tmp] = monsters_world[tmp].xp;
            monster_strength[tmp] = monsters_world[tmp].strength;
            monster_hit[tmp] = 0;
        }
        tilemap_ptr = world_tilemap_ptr;
        tilemap_x = world_tilemap_x;
        tilemap_y = world_tilemap_y;
    } else {
        tilemap_ptr = (unsigned char *)&tilemap0[map->entry_offset];
        tilemap_x = map->entry_x;
        tilemap_y = map->entry_y;
    }

    player_ptr = tilemap_ptr + 128*3+4;
    tileset_bank = map->tileset_bank;
    tilemap_bank = map->tilemap_bank;
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
        if (!monster_tile_idx[tmp]) continue;
        tmp_x = (char)(monster_x[tmp]) - (char)tilemap_x;
        tmp_y = (char)(monster_y[tmp]) - (char)tilemap_y;
        if (tmp_x < 9 &&
            tmp_y < 7) {
            tmp2 = tmp_y*9 + tmp_x;
            visible[tmp2] |= 0x80;
            monster_tile_idx[tmp] |= 0x80;
            monster_visible_x[tmp] = tmp_x;
            monster_visible_y[tmp] = tmp_y;
        } else {
            monster_tile_idx[tmp] &= 0x7F;
        }
    }
}

void interact(unsigned char tile) {
    if (map_id == 0) {
        // Entering dungeon
        if (player_ptr == (unsigned char *)0xa6cb) {
            set_map(3);
        // Entering Sunglow
        } else if (player_ptr == (unsigned char *)0x87d0) {
            set_map(2);
        } else {
            // Entering the castle
            set_map(1);
        }
        return;
    } else if (map_id == 1) {
        // We leave the city
        if (tile == 63) {
            set_map(0);
            return;
        }

        interact_castle(tile);
    } else if (map_id == 2) {
        // We leave the city
        if (tile == 63) {
            set_map(0);
            return;
        }
        interact_sunglow(tile);
    } else if (map_id == 3) {
        // We leave the dungeon
        set_map(0);
        return;
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

    change_rom_bank(INTRO_CODE_BANK);
//    splash();
//    preamble();

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

    set_map(0);

    while (1) { 
        main_loop();

        change_rom_bank(tilemap_bank);
        set_visible_tiles();
        change_rom_bank(MISC_CODE_BANK);

        tick_music();
    }

  return (0);
}
