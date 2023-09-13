/*
 * ====================================================== *
 *  _____ ____ ____ ____  _____             _             *
 * |_   _/ ___/ ___/ ___|| ____|_ __   __ _(_)_ __   ___  *
 *   | || |  | |   \___ \|  _| | '_ \ / _` | | '_ \ / _ \ *
 *   | || |__| |___ ___) | |___| | | | (_| | | | | |  __/ *
 *   |_| \____\____|____/|_____|_| |_|\__, |_|_| |_|\___| *
 *                                    |___/               *
 * ====================================================== *
 * 
 * Main focus: testing
 **/

#include "tccsgraphics.h"
#include <curses.h>
#include <locale.h>
#include <unistd.h>
#define STROKE_RADIUS 0

#define VIS_WIN_HEIGHT 480
#define VIS_WIN_WIDTH 640

#define WIN_HEIGHT 24
#define WIN_WIDTH 64
TCCSScreen* screen;
WINDOW* win;
uint32_t foo[] = {0, 0, 0};

void push_quad(TCCSMesh* mesh, uint32_t a, uint32_t b, uint32_t c, uint32_t d){
    foo[0] = a, foo[1] = b, foo[2] = c; TCCS_mesh_push_face(mesh, 3, foo);
    foo[0] = a, foo[1] = c, foo[2] = d; TCCS_mesh_push_face(mesh, 3, foo); 
}

int main(int argc, char **argv){
    setlocale(LC_CTYPE, "");
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    start_color();
    TCCS_graphics_init();
    uint32_t win_height, win_width;
    getmaxyx(stdscr, win_height, win_width);
    win = newwin(WIN_HEIGHT, WIN_WIDTH, (win_height - WIN_HEIGHT)/2, (win_width - WIN_WIDTH)/2);
    screen = TCCS_screen_create(VIS_WIN_HEIGHT, VIS_WIN_WIDTH);
    TCCS_screen_set_stroke_radius(screen, STROKE_RADIUS);
    TCCS_screen_set_window(screen, win);
    TCCS_screen_set_margin(screen, 0);
    TCCS_screen_set_threshold(screen, 1);
    refresh();

    TCCSMesh* testmesh = TCCS_mesh_init();
    TCCSVertex vertex = {0, 0, 0};
    vertex.x = 1, vertex.y = 1, vertex.z = 1; // 0
    TCCS_mesh_push_vertex(testmesh, vertex);
    vertex.x = 1, vertex.y = 1, vertex.z = -1; // 1
    TCCS_mesh_push_vertex(testmesh, vertex);
    vertex.x = 1, vertex.y = -1, vertex.z = 1; // 2
    TCCS_mesh_push_vertex(testmesh, vertex);
    vertex.x = 1, vertex.y = -1, vertex.z = -1; // 3
    TCCS_mesh_push_vertex(testmesh, vertex);
    vertex.x = -1, vertex.y = 1, vertex.z = 1; // 4
    TCCS_mesh_push_vertex(testmesh, vertex);
    vertex.x = -1, vertex.y = 1, vertex.z = -1; // 5
    TCCS_mesh_push_vertex(testmesh, vertex);
    vertex.x = -1, vertex.y = -1, vertex.z = 1; // 6
    TCCS_mesh_push_vertex(testmesh, vertex);
    vertex.x = -1, vertex.y = -1, vertex.z = -1; // 7
    TCCS_mesh_push_vertex(testmesh, vertex);

    push_quad(testmesh, 6, 4, 0, 2);
    push_quad(testmesh, 7, 3, 1, 5);
    push_quad(testmesh, 3, 2, 0, 1);
    push_quad(testmesh, 7, 5, 4, 6);
    push_quad(testmesh, 5, 1, 0, 4);
    push_quad(testmesh, 7, 6, 2, 3);

    TCCSScene* scene = TCCS_scene_init();
    TCCS_scene_set_screen(scene, screen);
    TCCS_scene_set_cam(scene, 0, 0, 0, 60.0, 0.1, 80);

    TCCSVertex testmeshpos;
    testmeshpos.x = 0;
    testmeshpos.y = 50;
    testmeshpos.z = 0;

    TCCS_scene_push_mesh(scene, *testmesh, testmeshpos);

    float i;
    for (i = 0; i < 10000.0; i+=0.1){
        TCCS_screen_clear(scene->screen);
        size_t j;
        TCCSMesh *copy_of_testmesh = TCCS_mesh_init();
        for (j = 0; j < testmesh->v_size; j++){
            TCCS_mesh_push_vertex(copy_of_testmesh, testmesh->vertices[j]);
            float _x = testmesh->vertices[j].x, _z = testmesh->vertices[j].z, _y = testmesh->vertices[j].y;
            testmesh->vertices[j].x = cos(i)*_x + sin(i)*_z; 
            testmesh->vertices[j].z = -sin(i)*_x + cos(i)*_z;
            _x = testmesh->vertices[j].x, _z = testmesh->vertices[j].z, _y = testmesh->vertices[j].y;
            testmesh->vertices[j].z = cos(i/3.0)*_z + sin(i/3.0)*_y;
            testmesh->vertices[j].y = -sin(i/3.0)*_z + cos(i/3.0)*_y;
        }
        TCCS_scene_render_mesh(scene, 0);
        TCCS_screen_update(scene->screen);
        for (j = 0; j < testmesh->v_size; j++){
            testmesh->vertices[j].x = copy_of_testmesh->vertices[j].x;
            testmesh->vertices[j].y = copy_of_testmesh->vertices[j].y;
            testmesh->vertices[j].z = copy_of_testmesh->vertices[j].z;
        }
        TCCS_mesh_destroy(copy_of_testmesh);
        usleep(40000);
    }
    getch();
    TCCS_screen_destroy(screen);
    endwin();
    
}

