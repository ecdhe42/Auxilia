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

#define WORLD_TILEMAP_BANK      0xFB
#define CITY_TILEMAP_BANK       0xFA
#define DUNGEON_TILEMAP_BANK    0xF9
#define MISC_CODE_BANK          0xFD

#define NB_MONSTERS         10

void breakpoint();
void set_map(unsigned char map_id);
unsigned char rand();
void draw_dashboard();

struct Monster {
    unsigned char tile_idx;
    unsigned char *tilemap_ptr;
    unsigned char x;
    unsigned char y;
    unsigned char visible_x;
    unsigned char visible_y;
    unsigned char hp;
    unsigned char gp;
    unsigned char xp;
    unsigned char strength;
};

struct Map {
    int entry_offset;
    unsigned char entry_x;
    unsigned char entry_y;
    unsigned char tileset_bank;
    unsigned char tilemap_bank;
    const unsigned char *tileset_property;
};

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
