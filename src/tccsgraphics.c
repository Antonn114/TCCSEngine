#include "tccsgraphics.h"
#include "tccsmath.h"
#include <stdlib.h>

wchar_t TCCS_braille[256];

void TCCS_graphics_init(){
    TCCS_braille[0] = 0x2800;
    size_t i;
    for (i = 1; i < 256; i++){
        TCCS_braille[i] = TCCS_braille[i - 1] + 1;
    }
    TCCS_braille[0] = ' ';
}

TCCSScreen* TCCS_screen_create(size_t r, size_t c){
    TCCSScreen *screen = (TCCSScreen*) malloc(sizeof(TCCSScreen));
    if (screen){
        screen->r = r;
        screen->c = c;
        screen->m = (uint32_t**) malloc(sizeof(uint32_t*) * r);
        if (screen->m){
            size_t i;
            for (i = 0; i < r; i++){
                screen->m[i] = (uint32_t*) malloc(sizeof(uint32_t)*c);
            }
        }else{
            free(screen);
            return NULL;
        }
        TCCS_screen_clear(screen);
    }else{
        return NULL;
    }
    return screen;
}

void TCCS_screen_destroy(TCCSScreen *screen){
    size_t i;
    for (i = 0; i < screen->r; i++){
        free(screen->m[i]);
    }
    free(screen->m);
    delwin(screen->window);
    free(screen);
}

void TCCS_screen_drawpoint(TCCSScreen *screen, int x, int y){
    size_t i, j;
    uint32_t starti = y - screen->stroke_radius > 0 ? y - screen->stroke_radius : 0;
    uint32_t endi = y + screen->stroke_radius < screen->r ? y + screen->stroke_radius : screen->r - 1;
    uint32_t startj = x - screen->stroke_radius > 0 ? x - screen->stroke_radius : 0;
    uint32_t endj = x + screen->stroke_radius < screen->c ? x + screen->stroke_radius : screen->c - 1;
    for (i = starti; i <= endi; i++){
        for (j = startj; j <= endj; j++){
            screen->m[i][j]++;
        }
    }
}
void TCCS_screen_drawline(TCCSScreen *screen, int x1, int y1, int x2, int y2){
    if (x1 < 0 && x2 < 0) return;
    if (y1 < 0 && y2 < 0) return;
    if (x1 > screen->c && x2 > screen->c) return;
    if (y1 > screen->r && y2 > screen->c) return;
    int dx = abs(x2 - x1);
    int8_t sx = x1 < x2 ? 1 : -1;
    int dy = abs(y2 - y1);
    int8_t sy = y1 < y2 ? 1 : -1;
    int x = x1, y = y1;
    uint8_t ex = 0;
    if (dy > dx){
        swapi32(&dx, &dy);
        ex = 1;
    }
    int e = 2*dy - dx;
    TCCS_screen_drawpoint(screen, x, y);
    uint32_t i = 0;
    while(i < dx){
        while (e >= 0){
            if (ex){
                x += sx;
            }else{
                y += sy;
            }
            e -= 2*dx;
        }
        if (ex){
            y += sy;
        }else{
            x += sx;
        }
        e += 2*dy;
        i++;
        TCCS_screen_drawpoint(screen, x, y);
    }
}

void TCCS_screen_drawtriangle (TCCSScreen *screen, int x1, int y1, int x2, int y2, int x3, int y3){
    TCCS_screen_drawline(screen, x1, y1, x2, y2);
    TCCS_screen_drawline(screen, x2, y2, x3, y3);
    TCCS_screen_drawline(screen, x3, y3, x1, y1);
}

void TCCS_screen_fillpoint(TCCSScreen *screen, int x, int y){
    if (x < 0 || y < 0) return;
    if (x >= screen->c || y >= screen->r) return;
    screen->m[y][x]++;
}

void TCCS_screen_fillline(TCCSScreen *screen, int x1, int y1, int x2, int y2){
    int dx = abs(x2 - x1);
    int8_t sx = x1 < x2 ? 1 : -1;
    int dy = abs(y2 - y1);
    int8_t sy = y1 < y2 ? 1 : -1;
    int x = x1, y = y1;
    uint8_t ex = 0;
    if (dy > dx){
        swapi32(&dx, &dy);
        ex = 1;
    }
    int e = 2*dy - dx;
    TCCS_screen_fillpoint(screen, x, y);
    uint32_t i = 0;
    while(i < dx){
        while (e >= 0){
            if (ex){
                x += sx;
            }else{
                y += sy;
            }
            e -= 2*dx;
        }
        if (ex){
            y += sy;
        }else{
            x += sx;
        }
        e += 2*dy;
        i++;
        TCCS_screen_fillpoint(screen, x, y);
    }
}

void TCCS_screen_filltriangle_flat(TCCSScreen *screen, int x1,int y1, int x2, int y2, int x3, int y3){
    float m1 = ((float)x2 - (float)x1)/((float)y2 - (float)y1);
    float m2 = ((float)x3 - (float)x1)/((float)y3 - (float)y1);
    float curx1 = x1, curx2 = x1;
    uint32_t dy = abs(y1 - y2);
    float sy = y1 < y2 ? 1 : -1;
    m1 *= sy;
    m2 *= sy;
    uint32_t i = 0;
    while(i <= dy){
        TCCS_screen_fillline(screen, curx1, y1, curx2, y1);
        i++;
        curx1 += m1;
        curx2 += m2;
        y1 += sy;
    }
}

void TCCS_screen_filltriangle(TCCSScreen *screen, int x1, int y1, int x2, int y2, int x3, int y3){
    if (y1 > y2){
        swapi32(&y1, &y2);
        swapi32(&x1, &x2);
    }
    if (y1 > y3){
        swapi32(&y1, &y3);
        swapi32(&x1, &x3);
    }
    if (y2 > y3){
        swapi32(&y2, &y3);
        swapi32(&x2, &x3);
    }
    float x4 = (float)x1 + (float)(((float)y2 - (float)y1)/((float)y3 - (float)y1))*((float)x3 - (float)x1);

    TCCS_screen_filltriangle_flat(screen, x1, y1, x2, y2, x4, y2);
    TCCS_screen_filltriangle_flat(screen, x3, y3, x2, y2, x4, y2);
}

void TCCS_screen_set_window(TCCSScreen *screen, WINDOW *window){
    if (screen->window == window) return;
    screen->window = window;
    getmaxyx(screen->window, screen->win_height, screen->win_width);
    screen->pool_height = screen->r/screen->win_height/4;
    screen->pool_width = screen->c/screen->win_width/2;
    screen->margin = 0;
}

void TCCS_screen_set_margin(TCCSScreen *screen, size_t margin){
    if (screen->margin == margin) return;
    screen->margin = margin;
    if (screen->window == NULL) return;
    screen->pool_height = screen->r/(screen->win_height - margin*2)/4;
    screen->pool_width = screen->c/(screen->win_width - margin*2)/2;
}

void TCCS_screen_set_threshold(TCCSScreen *screen, uint32_t threshold){
    screen->threshold = threshold;
}

void TCCS_screen_set_stroke_radius(TCCSScreen *screen, uint32_t stroke_radius){
    screen->stroke_radius = stroke_radius;
}

void TCCS_screen_clear(TCCSScreen *screen){
    size_t i, j;
    for (i = 0; i < screen->r; i++){
        for (j = 0; j < screen->c; j++){
            screen->m[i][j] = 0;
        }
    }
}

void TCCS_screen_update(TCCSScreen *screen){
    if (screen->window) wclear(screen->window);
    uint32_t i, j, k, x, y;
    uint32_t ii, jj;
    uint16_t type;
    uint32_t col = 0;
    for (i = 0; i < screen->r; i += screen->pool_height*4){
        for (j = 0; j < screen->c; j += screen->pool_width*2){
            type = 0;
            for (k = 0; k < 8; k++){
                col = 0;
                for (x = 0; x < screen->pool_height; x++){
                    for (y = 0; y < screen->pool_width; y++){
                        col += screen->m[i + ((k >> 1) << 2) + x][j + (k & 1)*screen->pool_width + y];
                    }
                }
                type |= (1 << k)*(col >= screen->threshold);
            }
            wprintw(screen->window, "%lc", TCCS_braille[type]);
        }
    }
    box(screen->window, 0, 0);
    wrefresh(screen->window);
}

TCCSMesh* TCCS_mesh_init(){
    TCCSMesh* mesh = (TCCSMesh*)malloc(sizeof(TCCSMesh));
    if (mesh){
        mesh->v_cap = mesh->f_cap = 8;
        mesh->v_size = mesh->f_size = 0;
        mesh->vertices = (TCCSVertex*)malloc(sizeof(TCCSVertex)*mesh->v_cap);
        mesh->faces = (uint32_t**)malloc(sizeof(uint32_t*)*mesh->f_cap);
        mesh->faces_vcount = (size_t*)malloc(sizeof(size_t)*mesh->f_cap);
    }else{
        return NULL;
    }
    return mesh;
}

void TCCS_mesh_destroy(TCCSMesh *mesh){
    size_t i;
    for (i = 0; i < mesh->f_size; i++){
        free(mesh->faces[i]);
    }
    free(mesh->faces_vcount);
    free(mesh->faces);
    free(mesh->vertices);
    free(mesh);
}

void TCCS_mesh_resize_vertexlist(TCCSMesh* mesh, size_t new_size){
    if (mesh){
        TCCSVertex* vertices = realloc(mesh->vertices, sizeof(TCCSVertex) * new_size);
        if (vertices){
            mesh->vertices = vertices;
            mesh->v_cap = new_size;
        }
    }
}

int TCCS_mesh_push_vertex(TCCSMesh* mesh, TCCSVertex vertex){
    if (mesh){
        if (mesh->v_size == mesh->v_cap){
            TCCS_mesh_resize_vertexlist(mesh, mesh->v_cap*3/2 + 1);
        }
        mesh->vertices[mesh->v_size++] = vertex;

        return mesh->v_size - 1;
    }
    return -1;
}

void TCCS_mesh_resize_facelist(TCCSMesh* mesh, size_t new_size){
    if (mesh){
        uint32_t** faces = realloc(mesh->faces, sizeof(uint32_t*) * new_size);
        size_t* faces_vcount = realloc(mesh->faces_vcount, sizeof(size_t) * new_size);
        if (faces && faces_vcount){
            mesh->faces = faces;
            mesh->faces_vcount = faces_vcount;
            mesh->f_cap = new_size;
        }
    }
}

int TCCS_mesh_push_face(TCCSMesh* mesh, size_t v_count, uint32_t* v_indices){
    if (mesh){
        if (mesh->f_size == mesh->f_cap){
            TCCS_mesh_resize_facelist(mesh, mesh->f_cap*3/2 + 1);
        }
        mesh->faces_vcount[mesh->f_size] = v_count;
        mesh->faces[mesh->f_size] = (uint32_t*)malloc(sizeof(uint32_t) * v_count);
        memcpy(mesh->faces[mesh->f_size], v_indices, sizeof(*mesh->faces[mesh->f_size]) * v_count);
        mesh->f_size++;
        return mesh->f_size - 1;
    }
    return -1;
}

TCCSMesh* TCCS_mesh_create_fromobj(const char *filename){
    TCCSMesh *mesh = TCCS_mesh_init();
    if (mesh){
        FILE* f = fopen(filename, "r");
        if (f == NULL){
            return NULL;
        }
        ssize_t read;
        char* line = NULL;
        size_t len = 0;
        char* buf;
        TCCSVertex foo;
        TCCS_mesh_push_vertex(mesh, foo);
        while((read = getline(&line, &len, f)) != -1){
            buf = strtok(line, " \n");
            if (buf != NULL){
                if (buf[0] == 'v'){
                    TCCSVertex v;
                    buf = strtok(NULL, " \n\t\0");
                    v.x = atof(buf);
                    buf = strtok(NULL, " \n\t\0");
                    v.z = atof(buf);
                    buf = strtok(NULL, " \n\t\0");
                    v.y = atof(buf);
                    TCCS_mesh_push_vertex(mesh, v);
                }
                if (buf[0] == 'f'){
                    uint32_t a[3];
                    buf = strtok(NULL, " \n\t\0");
                    a[0] = atoi(buf);
                    buf = strtok(NULL, " \n\t\0");
                    a[1] = atoi(buf);
                    buf = strtok(NULL, " \n\t\0");
                    a[2] = atoi(buf);
                    TCCS_mesh_push_face(mesh, 3, a);
                }
            }
        }
        fclose(f);
    }else{
        return NULL;
    }
    return mesh;
}

TCCSScene* TCCS_scene_init(){
    TCCSScene* scene = (TCCSScene*)malloc(sizeof(TCCSScene));
    if (scene){
        scene->m_cap = 1;
        scene->m_size = 0;
        scene->meshes = (TCCSMesh*)malloc(scene->m_cap*sizeof(TCCSMesh));
        scene->position = (TCCSVertex*)malloc(scene->m_cap*sizeof(TCCSVertex));
        scene->mat_perspective_projection = TCCS_matrix_f32_create(4, 4);
        size_t i, j;
        for (i = 0; i < 4; i++){
            for (j = 0; j < 4; j++){
                scene->mat_perspective_projection->m[i][j] = 0;
            }
        }
    }else{
        return NULL;
    }
    return scene;
}


void TCCS_scene_set_screen(TCCSScene * scene, TCCSScreen * screen){
    scene->screen = screen;
    scene->cam_aspectratio = screen->c*1.0/(screen->r*1.0);
    scene->origin_x = screen->c/2.0;
    scene->origin_y = screen->r/2.0;
}
void TCCS_scene_set_cam(TCCSScene *scene, float camx, float camy, float camz, float y_far, float y_near, float fov){
    scene->campos.x = camx;
    scene->campos.y = camy;
    scene->campos.z = camz;
    scene->y_far = y_far;
    scene->y_near = y_near;
    scene->fov = fov;
    // scene->mat_perspective_projection (x, y, z, w = 1)
    scene->mat_perspective_projection->m[0][0] = 1.0/(scene->cam_aspectratio*tan(scene->fov*M_PI/360.0));
    scene->mat_perspective_projection->m[1][1] = scene->y_far/(scene->y_far - scene->y_near);
    scene->mat_perspective_projection->m[3][1] = -scene->y_far*scene->y_near/(scene->y_far - scene->y_near);
    scene->mat_perspective_projection->m[2][2] = 1.0/tan(scene->fov*M_PI/360.0);
    scene->mat_perspective_projection->m[1][3] = 1;
}

void TCCS_scene_resize_meshlist(TCCSScene* scene, size_t new_size){
    if (scene){
        TCCSMesh* meshes = realloc(scene->meshes, sizeof(TCCSMesh) * new_size);
        TCCSVertex* position = realloc(scene->position, sizeof(TCCSVertex) * new_size);
        if (meshes && position){
            scene->meshes = meshes;
            scene->position = position;
            scene->m_cap = new_size;
        }
    }
}

int TCCS_scene_push_mesh(TCCSScene* scene, TCCSMesh mesh, TCCSVertex pos){
    if (scene){
        if (scene->m_size == scene->m_cap){
            TCCS_scene_resize_meshlist(scene, scene->m_cap*3/2 + 1);
        }
        scene->meshes[scene->m_size] = mesh;
        scene->position[scene->m_size++] = pos;
        return scene->m_size - 1;
    }
    return -1;
}

void TCCS_scene_render_mesh(TCCSScene *scene, size_t idx){
    assert(idx < scene->m_size);
    TCCSVertex* transformed_vertices = scene->meshes[idx].vertices;
    TCCSMatrixF32** perspective_pos = (TCCSMatrixF32**)malloc(sizeof(TCCSMatrixF32*) * scene->meshes[idx].v_size);
    size_t i;
    for (i = 0; i < scene->meshes[idx].v_size; i++){
        perspective_pos[i] = TCCS_matrix_f32_create(1, 4);
        perspective_pos[i]->m[0][0] = transformed_vertices[i].x + scene->position[idx].x;
        perspective_pos[i]->m[0][1] = transformed_vertices[i].y + scene->position[idx].y;
        perspective_pos[i]->m[0][2] = transformed_vertices[i].z + scene->position[idx].z;
        perspective_pos[i]->m[0][3] = 1;
        perspective_pos[i] = TCCS_matrix_f32_multiply(perspective_pos[i], scene->mat_perspective_projection);
        if (perspective_pos[i]->m[0][3] != 0){
            perspective_pos[i]->m[0][0] /= perspective_pos[i]->m[0][3];
            perspective_pos[i]->m[0][1] /= perspective_pos[i]->m[0][3];
            perspective_pos[i]->m[0][2] /= perspective_pos[i]->m[0][3];
        }
        perspective_pos[i]->m[0][0]++;
        perspective_pos[i]->m[0][2]++;
        perspective_pos[i]->m[0][0] *= scene->screen->c*0.5;
        perspective_pos[i]->m[0][2] *= scene->screen->r*0.5;
    }
    for (i = 0; i < scene->meshes[idx].f_size; i++){
        assert (scene->meshes[idx].faces_vcount[i] == 3);
        uint32_t *faces = scene->meshes[idx].faces[i];
        float x1, x2, x3, y1, y2, y3;
        x1 = perspective_pos[faces[0]]->m[0][0];
        y1 = perspective_pos[faces[0]]->m[0][2];
        x2 = perspective_pos[faces[1]]->m[0][0];
        y2 = perspective_pos[faces[1]]->m[0][2];
        x3 = perspective_pos[faces[2]]->m[0][0];
        y3 = perspective_pos[faces[2]]->m[0][2];
        // back-face culling
        //if (normal(x1, -y1, x2, -y2, x3, -y3) < 0.0f) continue;
        TCCS_screen_drawtriangle(scene->screen, x1, y1, x2, y2, x3, y3);
    }
    refresh();
}
