#ifndef __TCCSGRAPHICS_H__
#define __TCCSGRAPHICS_H__

#include "tccsmath.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ncurses.h>
#include <wchar.h>

extern wchar_t TCCS_braille[256];

void TCCS_graphics_init();

typedef struct TCCSScreen{
    uint32_t **m;
    size_t r, c;

    uint32_t stroke_radius;
    WINDOW* window;

    size_t win_height, win_width;
    size_t pool_height, pool_width;
    size_t margin;

    uint32_t threshold;

} TCCSScreen;

TCCSScreen* TCCS_screen_create(size_t r, size_t c);
void TCCS_screen_destroy(TCCSScreen *screen);

void TCCS_screen_drawpoint      (TCCSScreen *screen, int x, int y);
void TCCS_screen_drawline       (TCCSScreen *screen, int x1, int y1, int x2, int y2);
void TCCS_screen_drawtriangle   (TCCSScreen *screen, int x1, int y1, int x2, int y2, int x3, int y3);

void TCCS_screen_fillpoint  (TCCSScreen *screen, int x, int y);
void TCCS_screen_fillline   (TCCSScreen *screen, int x1, int y1, int x2, int y2);

void TCCS_screen_filltriangle_flat  (TCCSScreen *screen, int x1, int y1, int x2, int y2, int x3, int y3);
void TCCS_screen_filltriangle       (TCCSScreen *screen, int x1, int y1, int x2, int y2, int x3, int y3);

void TCCS_screen_set_window(TCCSScreen* screen, WINDOW* window);
void TCCS_screen_set_margin(TCCSScreen* screen, size_t margin);
void TCCS_screen_set_threshold(TCCSScreen* screen, uint32_t threshold);
void TCCS_screen_set_stroke_radius(TCCSScreen* screen, uint32_t stroke_radius);
void TCCS_screen_clear(TCCSScreen *screen);
void TCCS_screen_update(TCCSScreen *screen); 

typedef struct TCCSVertex{
    float x, y, z;
} TCCSVertex;

typedef struct TCCSMesh{
    TCCSVertex* vertices;
    uint32_t** faces;
} TCCSMesh;

TCCSMesh* TCCS_mesh_create(size_t vc, size_t fc);
TCCSMesh* TCCS_mesh_create_fromobj(const char* filename);
void TCCS_mesh_destroy(TCCSMesh* mesh);

typedef struct TCCSScene{
    TCCSMesh *meshes;
} TCCSScene;

TCCSScene* TCCS_scene_create(size_t mc);
void TCCS_scene_destroy(TCCSScene* scene);
void TCCS_scene_mesh_add(TCCSScene* scene, TCCSMesh mesh);
void TCCS_scene_project(TCCSScene* scene, TCCSScreen* screen);

#endif /* __TCCSGRAPHICS_H__ */
