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
    size_t* faces_vcount;
    size_t v_cap, f_cap;
    size_t v_size, f_size;
} TCCSMesh;

TCCSMesh* TCCS_mesh_init();
void TCCS_mesh_resize_vertexlist(TCCSMesh* mesh, size_t new_size);
int TCCS_mesh_push_vertex(TCCSMesh* mesh, TCCSVertex vertex);
void TCCS_mesh_resize_facelist(TCCSMesh* mesh, size_t new_size);
int TCCS_mesh_push_face(TCCSMesh* mesh, size_t v_count, uint32_t* v_indices);
TCCSMesh* TCCS_mesh_create_fromobj(const char* filename);
void TCCS_mesh_destroy(TCCSMesh* mesh);

typedef struct TCCSScene{
    TCCSMesh *meshes;
    TCCSVertex *position;
    TCCSScreen *screen;
    TCCSVertex campos;
    float y_far, y_near;
    float fov;
    float cam_aspectratio;
    float origin_x, origin_y;
    size_t m_cap, m_size;
    TCCSMatrixF32* mat_perspective_projection;
} TCCSScene;

TCCSScene* TCCS_scene_init();
void TCCS_scene_set_screen(TCCSScene* scene, TCCSScreen * screen);
void TCCS_scene_set_cam(TCCSScene* scene, float camx, float camy, float camz, float y_far, float y_near, float fov);
void TCCS_scene_resize_meshlist(TCCSScene* scene, size_t new_size);
int TCCS_scene_push_mesh(TCCSScene* scene, TCCSMesh mesh, TCCSVertex pos);

void TCCS_scene_render_mesh(TCCSScene* scene, size_t idx);

void TCCS_scene_destroy(TCCSScene* scene);

#endif /* __TCCSGRAPHICS_H__ */
