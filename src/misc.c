#include "auxilia.h"

void interact(unsigned char tile);
void set_visible_tiles();

extern unsigned char tmp, tmp2, tmp3, tmp_x, tmp_y, tile_val;
extern unsigned char *tilemap_ptr;
extern unsigned char *tmp_tilemap_ptr;
extern unsigned char *player_ptr;
extern int long_val1;
extern int attr_hp;
extern int attr_xp;
extern int attr_gp;
extern int attr_mana;
extern unsigned char attr_gender;
extern unsigned char attr_level;
extern unsigned char attr_strength;
extern unsigned char attr_dext;
extern unsigned char attr_stamina;
extern unsigned char attr_int;
extern unsigned char attr_hp_digits[4];
extern unsigned char attr_xp_digits[4];
extern unsigned char attr_gp_digits[4];
extern unsigned char attr_mana_digits[4];
extern unsigned char attr_level_digits[2];
extern unsigned char visible[63];
extern unsigned char player_step;
extern unsigned char tileset_bank;
extern unsigned char tilemap_bank;
extern unsigned char map_id;
extern unsigned char tilemap_x;
extern unsigned char tilemap_y;

extern unsigned char monster_tile_idx[NB_MONSTERS];
extern unsigned char *monster_tilemap_ptr[NB_MONSTERS];
extern unsigned char monster_x[NB_MONSTERS];
extern unsigned char monster_y[NB_MONSTERS];
extern unsigned char monster_visible_x[NB_MONSTERS];
extern unsigned char monster_visible_y[NB_MONSTERS];
extern unsigned char monster_hp[NB_MONSTERS];
extern unsigned char monster_gp[NB_MONSTERS];
extern unsigned char monster_xp[NB_MONSTERS];
extern unsigned char monster_strength[NB_MONSTERS];
extern unsigned char *world_tilemap_ptr;
extern unsigned char world_tilemap_x;
extern unsigned char world_tilemap_y;
extern const struct Map maps[3];
extern unsigned char tileset_property[64];
//extern const struct Monster available_monsters[1];

const struct Monster available_monsters[1] = {
    { 24, 0, 0, 0, 0, 0, 10, 10, 10, 2 }
};

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
#pragma zpsym ("player_step");
#pragma zpsym ("visible");
#pragma zpsym ("attr_hp");
#pragma zpsym ("attr_xp");
#pragma zpsym ("attr_gp");
#pragma zpsym ("attr_mana");
#pragma zpsym ("attr_gender");
#pragma zpsym ("attr_level");
#pragma zpsym ("attr_strength");
#pragma zpsym ("attr_dext");
#pragma zpsym ("attr_stamina");
#pragma zpsym ("attr_int");
#pragma zpsym ("attr_hp_digits");
#pragma zpsym ("attr_xp_digits");
#pragma zpsym ("attr_gp_digits");
#pragma zpsym ("attr_mana_digits");
#pragma zpsym ("attr_level_digits");
#pragma zpsym ("tileset_bank");
#pragma zpsym ("tilemap_bank");
#pragma zpsym ("map_id");
#pragma zpsym ("tilemap_x");
#pragma zpsym ("tilemap_y");

#pragma code-name(push, "PROG0")
void init_game() {
    player_step = 0;
    tileset_bank = VRAM_WORLD_TILESET_BANK;
    tilemap_bank = WORLD_TILEMAP_BANK;
    for (tmp=0; tmp<63; tmp++) {
        visible[tmp] = 0;
    }
    for (tmp=0; tmp<NB_MONSTERS; tmp++) {
        monster_tile_idx[tmp] = 0;
    }

    // Temp: setup one monster near the castle
    monster_tile_idx[0] = available_monsters[0].tile_idx;
    monster_visible_x[0] = available_monsters[0].visible_x;
    monster_visible_y[0] = available_monsters[0].visible_y;
    monster_hp[0] = available_monsters[0].hp;
    monster_gp[0] = available_monsters[0].gp;
    monster_xp[0] = available_monsters[0].xp;
    monster_strength[0] = available_monsters[0].strength;
    monster_x[0]= 57;
    monster_y[0] = 57;
    monster_tilemap_ptr[0] = (unsigned char *)(0x8000 + 57*128+57);

    world_tilemap_ptr = (unsigned char *)(maps[0].entry_offset + 0x8000);
    world_tilemap_x = maps[0].entry_x;
    world_tilemap_y = maps[0].entry_y;
    tilemap_ptr = world_tilemap_ptr;
    tilemap_x = world_tilemap_x;
    tilemap_y = world_tilemap_y;

    attr_hp = 100;
    attr_gp = 0;
    attr_xp = 0;
    attr_mana = 10;
    attr_level = 1;
    attr_hp_digits[0] = 0;
    attr_hp_digits[1] = 0;
    attr_hp_digits[2] = 4;
    attr_hp_digits[3] = 0;
    attr_gp_digits[0] = 0;
    attr_gp_digits[1] = 0;
    attr_gp_digits[2] = 0;
    attr_gp_digits[3] = 0;
    attr_level_digits[0] = 4;
    attr_level_digits[1] = 0;
    attr_xp_digits[0] = 0;
    attr_xp_digits[1] = 0;
    attr_xp_digits[2] = 0;
    attr_xp_digits[3] = 0;
    attr_mana_digits[0] = 0;
    attr_mana_digits[1] = 0;
    attr_mana_digits[2] = 0;
    attr_mana_digits[3] = 0;
}

void restore_dashboard() {
    draw_dashboard();
    await_draw_queue();
    sleep(1);
    flip_pages();
    draw_dashboard();
    await_draw_queue();
    sleep(1);
}

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

const char *armors_female_name[5] = {
    "Leather",
    "Studded leather",
    "Reinforced",
    "Metal",
    "Full plate"
};

const char *armors_female_desc[5] = {
    "50 coin\0\0The basic armor.",
    "200 coin\0\0Reinforced for enhanced\0protection.",
    "500 coin\0\0A solid armor which\0can sustain severe\0blows.",
    "1000 coin\0\0Believe or not\0the high heels allow\0you to outrun dinosaurs.",
    "2000 coin\0\0Nobody knows why\0but this armor offers\0the best protection."
};

void display_armor_names() {
    tmp_x = 2;
    tmp_y = 5;
    breakpoint();
    for (tmp3=0; tmp3<5; tmp3++) {
        tmp_tilemap_ptr = (unsigned char *)armors_female_name[tmp3];
        tmp = tmp_tilemap_ptr[0];

        while (tmp != 0) {
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
            }
            breakpoint();
            tmp_tilemap_ptr++;
            tmp_x += 5;
            tmp = tmp_tilemap_ptr[0];
        }
        tmp_x = 2;
        tmp_y += 6;
    }
    await_drawing();
}

void display_armor(unsigned char idx) {
    if (idx < 3) {
        tmp_x = idx*42;
        tmp_y = 0;
    } else {
        tmp_x = (idx-3)*42;
        tmp_y = 64;
    }
    
    draw_sprite_now(85, 10, 42, 64, tmp_x, tmp_y, VRAM_ARMORS_F_BANK);
    await_drawing();

    tmp_tilemap_ptr = (unsigned char *)armors_female_desc[idx];
    tmp_x = 2;
    tmp_y = 80;
    tmp = tmp_tilemap_ptr[0];
    while (tmp != '.') {
        if (tmp == 0) {
            tmp_x = -3;
            tmp_y += 6;
        } else if (tmp >= 'a' && tmp <= 'z') {
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
        } else if (tmp >= '0' && tmp <= '9') {
            tmp2 = (tmp - '0') << 2;
            draw_sprite_now(tmp_x, tmp_y, 4, 5, tmp2, 67, VRAM_FONTS_BANK);
        }
        
        tmp_x += 5;
        //tmp_y += 6;

        tmp_tilemap_ptr++;
        tmp = tmp_tilemap_ptr[0];
    }
    await_drawing();

}

void shop_armors() {
    tile_val = 0;

    while (1) {
        clear_screen(0);
        clear_border(0);
        await_draw_queue();

        draw_box_now(1, 12*tile_val+5, 82, 12, 188);
        await_drawing();

        display_armor_names();
        display_armor(tile_val);
        sleep(1);
        flip_pages();

        update_inputs();
        if (player1_buttons & ~player1_old_buttons& INPUT_MASK_UP) {
            if (tile_val != 0) {
                tile_val--;
            }
        } else if (player1_buttons & ~player1_old_buttons & INPUT_MASK_DOWN) {
            if (tile_val != 4) {
                tile_val++;
            }
        }
        if (player1_buttons & INPUT_MASK_B) break;
    }
}

void interact_castle(unsigned char tile) {
    if (tile == 63) {
        change_rom_bank(WORLD_TILEMAP_BANK);
        set_map(0);
        return;
    }
    if (player_ptr >= (unsigned char *)0x940b && player_ptr <= (unsigned char *)0x9413) {
        shop_armors();
        restore_dashboard();
        return;
    }
}

void recompute_dashboard() {
    long_val1 = attr_hp;
    attr_hp_digits[3] = long_val1 / 1000;
    long_val1 -= 1000*attr_hp_digits[3];
    attr_hp_digits[2] = long_val1 / 100;
    long_val1 -= 100*attr_hp_digits[2];
    attr_hp_digits[1] = long_val1 / 10;
    long_val1 -= 10*attr_hp_digits[1];
    attr_hp_digits[3] = attr_hp_digits[3] << 2;
    attr_hp_digits[2] = attr_hp_digits[2] << 2;
    attr_hp_digits[1] = attr_hp_digits[1] << 2;
    attr_hp_digits[0] = long_val1 << 2;

    long_val1 = attr_gp;
    attr_gp_digits[3] = long_val1 / 1000;
    long_val1 -= 1000*attr_gp_digits[3];
    attr_gp_digits[2] = long_val1 / 100;
    long_val1 -= 100*attr_gp_digits[2];
    attr_gp_digits[1] = long_val1 / 10;
    long_val1 -= 10*attr_gp_digits[1];
    attr_gp_digits[3] = attr_gp_digits[3] << 2;
    attr_gp_digits[2] = attr_gp_digits[2] << 2;
    attr_gp_digits[1] = attr_gp_digits[1] << 2;
    attr_gp_digits[0] = long_val1 << 2;

    long_val1 = attr_xp;
    attr_xp_digits[3] = long_val1 / 1000;
    long_val1 -= 1000*attr_xp_digits[3];
    attr_xp_digits[2] = long_val1 / 100;
    long_val1 -= 100*attr_xp_digits[2];
    attr_xp_digits[1] = long_val1 / 10;
    long_val1 -= 10*attr_xp_digits[1];
    attr_xp_digits[3] = attr_xp_digits[3] << 2;
    attr_xp_digits[2] = attr_xp_digits[2] << 2;
    attr_xp_digits[1] = attr_xp_digits[1] << 2;
    attr_xp_digits[0] = long_val1 << 2;
    restore_dashboard();
}

void draw_dashboard() {
 // Draw attributes
        await_draw_queue();
        await_drawing();
        draw_box_now(0, 112, 127, 16, 0);
        await_drawing();
        draw_tile_init(4, 5, VRAM_FONTS_BANK | BANK_CLIP_X);
        draw_left_tile(25, 113, attr_hp_digits[3], 67);
        draw_left_tile(30, 113, attr_hp_digits[2], 67);
        draw_left_tile(35, 113, attr_hp_digits[1], 67);
        draw_left_tile(40, 113, attr_hp_digits[0], 67);
        draw_left_tile(25, 120, attr_mana_digits[3], 67);
        draw_left_tile(30, 120, attr_mana_digits[2], 67);
        draw_left_tile(35, 120, attr_mana_digits[1], 67);
        draw_left_tile(40, 120, attr_mana_digits[0], 67);
        draw_left_tile(69, 113, attr_gp_digits[3], 67);
        draw_left_tile(74, 113, attr_gp_digits[2], 67);
        draw_left_tile(79, 113, attr_gp_digits[1], 67);
        draw_left_tile(84, 113, attr_gp_digits[0], 67);
        draw_left_tile(69, 120, attr_xp_digits[3], 67);
        draw_left_tile(74, 120, attr_xp_digits[2], 67);
        draw_left_tile(79, 120, attr_xp_digits[1], 67);
        draw_left_tile(84, 120, attr_xp_digits[0], 67);
        draw_left_tile(113, 113, attr_level_digits[1], 67);
        draw_left_tile(118, 113, attr_level_digits[0], 67);
        await_drawing();
        draw_sprite(0, 112, 112, 16, 0, 112, VRAM_SPRITES_BANK);
}

void attack_monster() {
    for (tmp=0; tmp<NB_MONSTERS; tmp++) {
        if (!monster_tile_idx[tmp]) continue;
        if (monster_tilemap_ptr[tmp] == tmp_tilemap_ptr) {
            monster_hp[tmp]-=2;
            if (monster_hp[tmp] == 0) {
                play_sound_effect((char*)&ASSET__sfx__hit_bin, 2);
                monster_tile_idx[tmp] = 0;
                attr_gp += monster_gp[tmp];
                attr_xp += monster_xp[tmp];
                recompute_dashboard();
            } else {
                play_sound_effect((char*)&ASSET__sfx__gunshot_bin, 1);
            }
        }
    }
}

void monster_action() {
    for (tmp=0; tmp<NB_MONSTERS; tmp++) {
    if (!monster_tile_idx[tmp]) continue;
    if ((monster_visible_x[tmp] == 3 || monster_visible_x[tmp] == 5) && monster_visible_y[tmp] == 3) {
        play_sound_effect((char*)&ASSET__sfx__hit_bin, 2);
        attr_hp -= monster_strength[tmp];
        recompute_dashboard();
    }
}
}

void main_loop() {
//        while (1) {                                     //  Run forever
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
        draw_tile_init(16, 16, tileset_bank | BANK_CLIP_X);

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
            tile_val = monster_tile_idx[tmp];
            if (tile_val & 0x80) {
                draw_sprite(monster_visible_x[tmp]*16-8, monster_visible_y[tmp]*16, 16, 16, (tile_val & 0x7) << 3, (tile_val & 0x78) << 1, VRAM_SPRITES_BANK | BANK_CLIP_X);
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
            tmp = 0;
            if((player1_buttons & INPUT_MASK_LEFT)) {
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
                tmp_tilemap_ptr = player_ptr-128;
                tmp = visible[22];
                tmp2 = tileset_property[tmp];
                if (tmp2 & 0x1 && !(tmp & 0x80)) {
                    player_step = 4;
                    tilemap_ptr -= 128;
                    tilemap_y--;
                    player_ptr -= 128;
                }
            } else if (player1_buttons & INPUT_MASK_DOWN) {
                tmp_tilemap_ptr = player_ptr+128;
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
                // There is a monster => we're attacking!
                } else if (tmp & 0x80) {
                    attack_monster();
                }
            // If we're about to step an an automatic actionable tile
            } else if (tmp2 & 0x8) {
                interact(tmp);
            }

            // Monster movement (world only)
            if (map_id == 0 && (player1_buttons & ~player1_old_buttons)) {
                monster_action();
            }
        }

}

#pragma code-name (pop)
