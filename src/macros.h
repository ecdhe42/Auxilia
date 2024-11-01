#define DRAW_PLAYER(nb, flag, gy)  \
        tmp = player_invincible[nb];     \
        tmp2 = player_step[nb] >> 1;  \
        tmp2 = tmp2 << 3; \
        tmp2 += player_tile[nb];    \
        if (!tmp) {     \
            draw_sprite(player_x[nb], player_y[nb]-2, 8, 10, tmp2, gy, 0);    \
        } else {    \
            if (tmp & 0x4) {    \
                draw_sprite(player_x[nb], player_y[nb]-2, 8, 10, tmp2, gy, 0);    \
            }   \
            player_invincible[nb]--; \
            if (!player_invincible[nb]) {   \
                tile_val = tilemap[player_ptr[nb]];  \
                tilemap[player_ptr[nb]] = tile_val | flag; \
            }   \
        }

#define MOVE_PLAYER(nb, flag, mask) \
        if (player_dir_x[nb] != 0) {    \
            if (player_dir_x[nb] == 2) {    \
                if (((player_x[nb] & 7) == 0) && (tilemap[player_ptr[nb]-1] == 0xFF)) {   \
                    player_dir_x[nb] = 0;   \
                } else {    \
                    player_x[nb]--;  \
                    player_step[nb] = (player_step[nb]+1) & 7;  \
                    tmp = player_x[nb] & 7;  \
                    if (tmp == 4) { \
                        tilemap[player_ptr[nb]] &= mask;     \
                        player_ptr[nb]--;    \
                        tile_val = tilemap[player_ptr[nb]];  \
                        tmp = tile_val & 0xF;       \
                        if (tmp >= 8 && bullets[nb] == 0) {  \
                            gun[tmp-8] = 0xFF;  \
                            tmp = 0;    \
                            tile_val &= 0xF0;   \
                            bullets[nb] = 3; \
                        }   \
                        if (player_invincible[nb]) {    \
                            tilemap[player_ptr[nb]] = tile_val; \
                        } else {    \
                            tilemap[player_ptr[nb]] = tile_val | flag;   \
                        }   \
                    } else if (tmp == 0) {  \
                        player_dir_x[nb] = 0;   \
                    }   \
                }   \
            } else {    \
               if (((player_x[nb] & 7) == 0) && (tilemap[player_ptr[nb]+1] == 0xFF)) {    \
                    player_dir_x[nb] = 0;   \
                } else {    \
                    player_x[nb]++;  \
                    player_step[nb] = (player_step[nb]+1) & 7;  \
                    tmp = player_x[nb] & 7;  \
                    if (tmp == 4) { \
                        tilemap[player_ptr[nb]] &= mask; \
                        player_ptr[nb]++;    \
                        tile_val = tilemap[player_ptr[nb]];  \
                        tmp = tile_val & 0xF;   \
                        if (tmp >= 8 && bullets[nb] == 0) {  \
                            gun[tmp-8] = 0xFF;  \
                            tmp = 0;    \
                            tile_val &= 0xF0;   \
                            bullets[nb] = 3; \
                        }   \
                        if (player_invincible[nb]) {    \
                            tilemap[player_ptr[nb]] = tile_val; \
                        } else {    \
                            tilemap[player_ptr[nb]] = tile_val | flag;   \
                        }   \
                    } else if (tmp == 0) {  \
                        player_dir_x[nb] = 0;   \
                    }   \
                }   \
            }   \
        } else if (player_dir_y[nb] != 0) { \
            if (player_dir_y[nb] == 2) {    \
                if (((player_y[nb] & 7) == 0) && (tilemap[player_ptr[nb]-16] == 0xFF)) {  \
                    player_dir_y[nb] = 0;   \
                } else {    \
                    player_y[nb]--;  \
                    player_step[nb] = (player_step[nb]+1) & 7;  \
                    tmp = player_y[nb] & 7;  \
                    if (tmp == 4) {     \
                        tilemap[player_ptr[nb]] &= mask;     \
                        player_ptr[nb]-=16;  \
                        tile_val = tilemap[player_ptr[nb]];  \
                        tmp = tile_val & 0xF;   \
                        if (tmp >= 8 && bullets[nb] == 0) {  \
                            gun[tmp-8] = 0xFF;  \
                            tmp = 0;    \
                            tile_val &= 0xF0;   \
                            bullets[nb] = 3; \
                        }   \
                        if (player_invincible[nb]) {    \
                            tilemap[player_ptr[nb]] = tile_val; \
                        } else {    \
                            tilemap[player_ptr[nb]] = tile_val | flag;   \
                        }   \
                    } else if (tmp == 0) {  \
                        player_dir_y[nb] = 0;   \
                    }   \
                }   \
            } else {    \
               if (((player_y[nb] & 7) == 0) && (tilemap[player_ptr[nb]+16] == 0xFF)) {   \
                    player_dir_y[nb] = 0;   \
                } else {    \
                    player_y[nb]++;  \
                    player_step[nb] = (player_step[nb]+1) & 7;  \
                    tmp = player_y[nb] & 7;  \
                    if (tmp == 4) {         \
                        tilemap[player_ptr[nb]] &= mask;    \
                        player_ptr[nb]+=16;  \
                        tile_val = tilemap[player_ptr[nb]];  \
                        tmp = tile_val & 0xF;   \
                        if (tmp >= 8 && bullets[nb] == 0) {  \
                            gun[tmp-8] = 0xFF;  \
                            tmp = 0;    \
                            tile_val &= 0xF0;   \
                            bullets[nb] = 3; \
                        }   \
                        if (player_invincible[nb]) {    \
                            tilemap[player_ptr[nb]] = tile_val; \
                        } else {    \
                            tilemap[player_ptr[nb]] = tile_val | flag;   \
                        }   \
                    } else if (tmp == 0) {  \
                        player_dir_y[nb] = 0;   \
                    }   \
                }   \
            }   \
        }

#define MOVE_BULLET(nb, mask) \
        if (bullet_x[nb]) {  \
            if (bullet_dir_x[nb] != 0) { \
                if (bullet_dir_x[nb] == 2) { \
                    bullet_x[nb]--;  \
                    bullet_x[nb]--;  \
                    tmp = bullet_x[nb] & 7;  \
                    if (tmp == 6) { \
                        bullet_ptr[nb]--;    \
                    }  \
                } else {    \
                    bullet_x[nb]++;  \
                    bullet_x[nb]++;  \
                    tmp = bullet_x[nb] & 7;  \
                    if (tmp == 0) { \
                        bullet_ptr[nb]++;    \
                    }   \
                }   \
            } else {    \
                if (bullet_dir_y[nb] == 2) { \
                    bullet_y[nb]--; \
                    bullet_y[nb]--;  \
                    tmp = bullet_y[nb] & 7;  \
                    if (tmp == 6) { \
                        bullet_ptr[nb]-= 16; \
                    }   \
                } else {    \
                    bullet_y[nb]++;  \
                    bullet_y[nb]++;  \
                    tmp = bullet_y[nb] & 7;  \
                    if (tmp == 0) { \
                        bullet_ptr[nb]+=16;  \
                    }   \
                }   \
            }   \
            tile_val = tilemap[bullet_ptr[nb]];  \
            tmp = tile_val & mask;  \
            if (tile_val == 0xFF) { \
                bullet_dir_x[nb] = 0;    \
                bullet_x[nb] = 0;    \
            } else if (tmp) {   \
                player_idx = nb; \
                hit();  \
            } else {    \
                draw_box(bullet_x[nb], bullet_y[nb], 1, 1, 7);    \
            }   \
        }

#define CHECK_PLAYER_FIRE(nb, player_buttons, player_old_buttons)  \
        if (bullets[nb] != 0 && bullet_x[nb] == 0 && (player_buttons & ~player_old_buttons & INPUT_MASK_A)) {   \
            bullets[nb]--;   \
            play_sound_effect((char*)&ASSET__sfx__gunshot_bin, 1); \
            bullet_ptr[nb] = player_ptr[nb];  \
            switch(player_dir[nb]) {    \
                case 0: \
                    bullet_x[nb] = (player_x[nb]+6) & 0xFE;   \
                    bullet_y[nb] = player_y[nb]+4;    \
                    bullet_dir_x[nb] = 1;    \
                    bullet_dir_y[nb] = 0;    \
                    break;  \
                case 1: \
                    bullet_x[nb] = player_x[nb] & 0xFE;   \
                    bullet_y[nb] = player_y[nb]+4;    \
                    bullet_dir_x[nb] = 2;    \
                    bullet_dir_y[nb] = 0;    \
                    break;  \
                case 2: \
                    bullet_x[nb] = player_x[nb]+4;    \
                    bullet_y[nb] = player_y[nb] & 0xFE;   \
                    bullet_dir_x[nb] = 0;    \
                    bullet_dir_y[nb] = 1;    \
                    break;  \
                case 3: \
                    bullet_x[nb] = player_x[nb]+4;    \
                    bullet_y[nb] = (player_y[nb]+6) & 0xFE;   \
                    bullet_dir_x[nb] = 0;    \
                    bullet_dir_y[nb] = 2;    \
                    break;  \
            }   \
        }

#define AI_MOVE_PLAYER(nb)  \
    if (player_dir_x[nb] == 0 && player_dir_y[nb] == 0) {   \
            tile_val = ai_tilemap_dir[player_ptr[nb]];   \
            tile_val &= ai_direction_mask[nb];   \
			if (tile_val == 0) {    \
				tile_val = ai_tilemap_dir[player_ptr[nb]];   \
			}   \
            long_val1 = 4000;   \
			x_tile = player_x[nb] >> 3;  \
			y_tile = player_y[nb] >> 3;  \
			if (ai_target_x[nb] == x_tile && ai_target_y[nb] == y_tile) { \
				tmp = rand() & 0x3; \
				tmp2 = ai_new_target_x[tmp];    \
				ai_target_x[nb] = tmp2;  \
				tmp2 = ai_new_target_y[tmp];    \
				ai_target_y[nb] = tmp2;  \
			}   \
            if (tile_val & 0x1) {   \
                tmp = ai_target_x[nb] >= x_tile+1 ? ai_target_x[nb] - x_tile-1 : x_tile+1 - ai_target_x[nb];   \
                long_val1 = tmp*tmp;    \
                tmp = ai_target_y[nb] >= y_tile ? ai_target_y[nb] - y_tile : y_tile - ai_target_y[nb]; \
				long_tmp = tmp*tmp; \
                long_val1 += long_tmp;  \
                player_dir_x[nb] = 1;   \
                player_dir_y[nb] = 0;   \
                player_dir[nb] = 0; \
                ai_direction_mask[nb] = 13;  \
            }   \
            if (tile_val & 0x2) {   \
                tmp = ai_target_x[nb] >= x_tile-1 ? ai_target_x[nb] - x_tile+1 : x_tile-1 - ai_target_x[nb];   \
                long_val2 = tmp*tmp;    \
                tmp = ai_target_y[nb] >= y_tile ? ai_target_y[nb] - y_tile : y_tile - ai_target_y[nb]; \
				long_tmp = tmp*tmp; \
                long_val2 += long_tmp;  \
                if (long_val2 < long_val1) {    \
                    long_val1 = long_val2;  \
                    player_dir_x[nb] = 2;   \
                    player_dir_y[nb] = 0;   \
					player_dir[nb] = 1; \
					ai_direction_mask[nb] = 14;  \
                }   \
            }   \
            if (tile_val & 0x4) {   \
                tmp = ai_target_x[nb] >= x_tile ? ai_target_x[nb] - x_tile : x_tile - ai_target_x[nb]; \
                long_val2 = tmp*tmp;    \
                tmp = ai_target_y[nb] >= y_tile+1 ? ai_target_y[nb] - y_tile-1 : y_tile+1 - ai_target_y[nb];   \
				long_tmp = tmp*tmp; \
                long_val2 += long_tmp;  \
                if (long_val2 < long_val1) {    \
                    long_val1 = long_val2;  \
                    player_dir_x[nb] = 0;   \
                    player_dir_y[nb] = 1;   \
					player_dir[nb] = 2; \
					ai_direction_mask[nb] = 7;   \
                }   \
            }   \
            if (tile_val & 0x8) {   \
                tmp = ai_target_x[nb] >= x_tile ? ai_target_x[nb] - x_tile : x_tile - ai_target_x[nb]; \
                long_val2 = tmp*tmp;    \
                tmp = ai_target_y[nb] >= y_tile-1 ? ai_target_y[nb] - y_tile+1 : y_tile-1 - ai_target_y[nb];    \
				long_tmp = tmp*tmp; \
                long_val2 += long_tmp;  \
                if (long_val2 < long_val1) {    \
                    long_val1 = long_val2;  \
                    player_dir_x[nb] = 0;   \
                    player_dir_y[nb] = 2;   \
					player_dir[nb] = 3; \
					ai_direction_mask[nb] = 11;   \
                }   \
            }   \
            player_step[nb] = 0; \
        }

#define AI_PLAYER_FIRE(nb)  \
		if (bullets[nb] != 0 && bullet_x[nb] == 0) {    \
            tmp = player_ptr[nb];   \
			if (player_dir_x[nb] == 1 &&   \
                ((player_y[0] == player_y[nb] && player_x[0] > player_x[nb]) || (player_y[1] == player_y[nb] && player_x[1] > player_x[nb])) &&   \
                (tilemap[tmp+1] != 255)) { \
				bullets[nb]--;  \
                play_sound_effect((char*)&ASSET__sfx__gunshot_bin, 1); \
	            bullet_ptr[nb] = player_ptr[nb];    \
				bullet_x[nb] = (player_x[nb]+6) & 0xFE; \
				bullet_y[nb] = player_y[nb];    \
				bullet_dir_x[nb] = 1;   \
				bullet_dir_y[nb] = 0;   \
			} else if (player_dir_x[nb] == 2 && \
                ((player_y[0] == player_y[nb] && player_x[0] < player_x[nb]) || (player_y[1] == player_y[nb] && player_x[1] < player_x[nb])) && \
                (tilemap[tmp-1] != 255)) {  \
				bullets[nb]--;  \
                play_sound_effect((char*)&ASSET__sfx__gunshot_bin, 1); \
	            bullet_ptr[nb] = player_ptr[nb];    \
				bullet_x[nb] = player_x[nb] & 0xFE; \
				bullet_y[nb] = player_y[nb];    \
				bullet_dir_x[nb] = 2;   \
				bullet_dir_y[nb] = 0;   \
			} else if (player_dir_y[nb] == 1 && \
                ((player_x[0] == player_x[nb] && player_y[0] > player_y[nb]) || (player_x[1] == player_x[nb] && player_y[1] > player_y[nb])) && \
                (tilemap[tmp+16] != 255)) {  \
				bullets[nb]--;  \
                play_sound_effect((char*)&ASSET__sfx__gunshot_bin, 1); \
	            bullet_ptr[nb] = player_ptr[nb];    \
				bullet_x[nb] = player_x[nb];    \
                bullet_y[nb] = (player_y[nb]+6) & 0xFC;    \
                bullet_dir_x[nb] = 0;    \
                bullet_dir_y[nb] = 1;   \
			} else if (player_dir_y[nb] == 2 &&    \
                ((player_x[0] == player_x[nb] && player_y[0] < player_y[nb]) || (player_x[1] == player_x[nb] && player_y[1] < player_y[nb])) && \
                (tilemap[tmp-16] != 255)) {  \
				bullets[nb]--;  \
                play_sound_effect((char*)&ASSET__sfx__gunshot_bin, 1); \
	            bullet_ptr[nb] = player_ptr[nb];    \
				bullet_x[nb] = player_x[nb];    \
				bullet_y[nb] = player_y[nb] & 0xFC; \
				bullet_dir_x[nb] = 0;   \
                bullet_dir_y[nb] = 2;   \
			}   \
			if (bullets[nb] == 0) { \
				tmp = ai_default_target_x[nb];  \
				ai_target_x[nb] = tmp;  \
				tmp = ai_default_target_y[nb];  \
				ai_target_y[nb] = tmp;  \
			}   \
		}
