#pragma code-name(push, "PROG4")
void splash() {
    draw_sprite(0, 0, 127, 127, 0, 0, SPLASH_BANK);
    while (1) {
        clear_screen(0);
        clear_border(0);
        rand();
        draw_sprite(0, 0, 127, 127, 0, 0, SPLASH_BANK);
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
            tilemap_ptr = (unsigned char*)preamble_text[tmp3+tile_val];
            tmp = tilemap_ptr[0];
            while (tmp) {
                if (tmp >= 'a' && tmp <= 'z') {
                    tmp2 = (tmp - 'a') << 2;
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, tmp2, 61, FONTS_BANK);
                } else if (tmp >= 'A' && tmp <= 'N') {
                    tmp2 = (tmp - 'A') * 9;
                    draw_sprite_now(tmp_x, tmp_y, 9, 11, tmp2, 22, FONTS_BANK);
                    tmp_x += 5;
                    tmp_y += 6;
                } else if (tmp >= 'O' && tmp <= 'Z') {
                    tmp2 = (tmp - 'O') * 9;
                    draw_sprite_now(tmp_x, tmp_y, 9, 11, tmp2, 33, FONTS_BANK);
                    tmp_x += 5;
                    tmp_y += 6;
                } else if (tmp == ',') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 108, 61, FONTS_BANK);
                } else if (tmp == '.') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 104, 61, FONTS_BANK);
                } else if (tmp == '\'') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 116, 61, FONTS_BANK);
                } else if (tmp == '"') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 112, 61, FONTS_BANK);
                } else if (tmp == '-') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 120, 61, FONTS_BANK);
                } else if (tmp == '!') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 124, 61, FONTS_BANK);
                } else if (tmp == '_') {
                    draw_sprite_now(tmp_x, tmp_y, 4, 5, 0, 66, FONTS_BANK);
                }
                tmp_x += 5;
                tilemap_ptr++;
                tmp = tilemap_ptr[0];
            }
            tilemap_ptr++;
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
    for (tmp3=0; tmp3<5; tmp3++) {
        tilemap_ptr = (unsigned char *)armors_female_name[tmp3];
        tmp = tilemap_ptr[0];

        while (tmp != 0) {
            if (tmp >= 'a' && tmp <= 'z') {
                tmp2 = (tmp - 'a') << 2;
                draw_sprite_now(tmp_x, tmp_y, 4, 5, tmp2, 61, FONTS_BANK);
            } else if (tmp >= 'A' && tmp <= 'N') {
                tmp2 = (tmp - 'A') * 9;
                draw_sprite_now(tmp_x, tmp_y, 9, 11, tmp2, 22, FONTS_BANK);
                tmp_x += 5;
                tmp_y += 6;
            } else if (tmp >= 'O' && tmp <= 'Z') {
                tmp2 = (tmp - 'O') * 9;
                draw_sprite_now(tmp_x, tmp_y, 9, 11, tmp2, 33, FONTS_BANK);
                tmp_x += 5;
                tmp_y += 6;
            }
            tilemap_ptr++;
            tmp_x += 5;
            tmp = tilemap_ptr[0];
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
    
    draw_sprite_now(85, 10, 42, 64, tmp_x, tmp_y, ARMORS_F_BANK);
    await_drawing();

    tilemap_ptr = (unsigned char *)armors_female_desc[idx];
    tmp_x = 2;
    tmp_y = 80;
    tmp = tilemap_ptr[0];
    while (tmp != '.') {
        if (tmp == 0) {
            tmp_x = -3;
            tmp_y += 6;
        } else if (tmp >= 'a' && tmp <= 'z') {
            tmp2 = (tmp - 'a') << 2;
            draw_sprite_now(tmp_x, tmp_y, 4, 5, tmp2, 61, FONTS_BANK);
        } else if (tmp >= 'A' && tmp <= 'N') {
            tmp2 = (tmp - 'A') * 9;
            draw_sprite_now(tmp_x, tmp_y, 9, 11, tmp2, 22, FONTS_BANK);
            tmp_x += 5;
            tmp_y += 6;
        } else if (tmp >= 'O' && tmp <= 'Z') {
            tmp2 = (tmp - 'O') * 9;
            draw_sprite_now(tmp_x, tmp_y, 9, 11, tmp2, 33, FONTS_BANK);
            tmp_x += 5;
            tmp_y += 6;
        } else if (tmp >= '0' && tmp <= '9') {
            tmp2 = (tmp - '0') << 2;
            draw_sprite_now(tmp_x, tmp_y, 4, 5, tmp2, 67, FONTS_BANK);
        }
        
        tmp_x += 5;
        //tmp_y += 6;

        tilemap_ptr++;
        tmp = tilemap_ptr[0];
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
    }
}
#pragma code-name (pop)
