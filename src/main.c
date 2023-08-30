/*
 ==========================================================
 =  _____ ____ ____ ____  _____             _             =
 = |_   _/ ___/ ___/ ___|| ____|_ __   __ _(_)_ __   ___  =
 =   | || |  | |   \___ \|  _| | '_ \ / _` | | '_ \ / _ \ =
 =   | || |__| |___ ___) | |___| | | | (_| | | | | |  __/ =
 =   |_| \____\____|____/|_____|_| |_|\__, |_|_| |_|\___| =
 =                                    |___/               =
 *=========================================================

 *  TCCSEngine Mark 0.1
 *
 *  status: not working :(
 *
 *  Current features:
 *  - Rendering models with vertices and edges
 *  - Camera (WASD for xy-plane movement, QE for z-axis movement, IOP for camera orientation) 
 *
 *  Future:
 *  - Face rendering
 *  - TUI design (with vi keybinds for navigation)
 *  - OpenGL/Vulkan/DirectX rendering for runtime gameplay

 * */


#include <ncurses.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#define PI 3.14159265358979323846
typedef struct Vector2{
    double x, y;
} Vector2;

/*
 *
 * TYPES
 *
 */

typedef struct Vector3{
    double x, y, z;
} Vector3;
typedef struct Camera{
    Vector3 pos;
    Vector3 rot; // in rad
    Vector3 surface_position;
} Camera;

typedef struct Vertex{
    Vector3 pos;
    Vector2 screen_pos;
} Vertex;

typedef struct Edge{
    Vertex *v1, *v2;
} Edge;

int vc, ec;
Vertex** vertexList;
Edge** edgeList;
Camera cam;
WINDOW *win;

/* function: read_model
 * description: reads a model from a file
 *
 * BEGIN MODEL FILE STRUCTURE
 * 
 * vertexCount
 * vertex_0_x vertex_0_y vertex_0_z
 * vertex_1_x vertex_1_y vertex_0_z
 * etc.
 * edgeCount
 * edge_0_vertex1_index_from_list edge_0_vertex2_index_from_list
 * edge_1_vertex1_index_from_list edge_1_vertex2_index_from_list*
 * etc.
 *
 * END MODEL FILE STRUCTURE
 */
int read_model(char* filename){
    FILE *f = fopen(filename, "r");
    if (f == NULL) return 1;
    fscanf(f, "%d", &vc);
    if (vc > 100000) return 1;
    vertexList = (struct Vertex**)malloc(vc * sizeof(struct Vertex*));
    if (vertexList == NULL) return 1;
    for (int i = 0; i < vc; i++){
        vertexList[i] = (struct Vertex*) malloc(sizeof(struct Vertex));
        fscanf(f, "%lf%lf%lf", &vertexList[i]->pos.x, &vertexList[i]->pos.y, &vertexList[i]->pos.z);
    }
    fscanf(f, "%d", &ec);
    if (ec > 100000) return 1;
    edgeList = (struct Edge**)malloc(ec * sizeof(struct Edge*));
    if (edgeList == NULL) return 1;
    for (int i = 0; i < ec; i++){
        edgeList[i] = (struct Edge*) malloc(sizeof(struct Edge));
        int x, y;
        fscanf(f, "%d%d", &x, &y);
        edgeList[i]->v1 = vertexList[x];
        edgeList[i]->v2 = vertexList[y];
    }
    return 0;
}

/* function: get_vertex
 * description: calculates a vertex's coordinates projected onto the screen
 * 
 * external reference: https://en.wikipedia.org/wiki/3D_projection#Mathematical_formula
 *
 */

void get_vertex(struct Vertex* v){
    double x = v->pos.x - cam.pos.x, y = v->pos.y - cam.pos.y, z = v->pos.z - cam.pos.z;

    double _x = cos(cam.rot.x)*(sin(cam.rot.z)*y + cos(cam.rot.z)*x) - sin(cam.rot.y)*z;

    double _y = sin(cam.rot.x)*(cos(cam.rot.y)*z + sin(cam.rot.y)*y + cos(cam.rot.z)*x) + cos(cam.rot.x)*(cos(cam.rot.z)*y - sin(cam.rot.z)*x);

    double _z = cos(cam.rot.x)*(cos(cam.rot.y)*z + sin(cam.rot.y)*y - sin(cam.rot.z)*x) + cos(cam.rot.x)*(cos(cam.rot.z)*y - sin(cam.rot.z)*x);
    v->screen_pos.x = cam.surface_position.z/_z*_x + cam.surface_position.x;
    v->screen_pos.y = cam.surface_position.z/_z*_y + cam.surface_position.y;
}

/* function: draw_vertex
 * description: draws a vertex projected onto the screen
 */
void draw_vertex(struct Vertex* v){
   if (v->screen_pos.x < 0 || v->screen_pos.y < 0) return;
    move((int)(v->screen_pos.y + 0.5), (int)(v->screen_pos.x + 0.5));
    printw("@"); 
}
/* function: draw_edge
 * description: draws an edge between two vertices, splits edge into multiple nodes (default: div=20) that represent the edge 
 */
void draw_edge(struct Edge *e){
    double div = 20;
    Vector2 currpos = e->v1->screen_pos;
    Vector2 destpos = e->v2->screen_pos;
    Vector2 delta;
    delta.x = (destpos.x - currpos.x)/div;
    delta.y = (destpos.y - currpos.y)/div;
    for (int i = 0; i < 20; i++){
        currpos.x += delta.x;
        currpos.y += delta.y;
        if (currpos.x < 0 || currpos.y < 0) continue;
        move((int)(currpos.y + 0.5), (int)(currpos.x + 0.5));
        printw("*");
    }
}

/* function: display_model
 * description: draws the model onto the screen, along with some debugging info like the camera's position and the camera's orientation
 */
void display_model(){
    clear();
    for (int i = 0; i < vc; i++){
        get_vertex(vertexList[i]);
    }
    for (int i = 0; i < ec; i++){
        draw_edge(edgeList[i]);
    }
    for (int i = 0; i < vc; i++){
        draw_vertex(vertexList[i]);
    }
    mvprintw(10, 0, "CAMPOS: %lf %lf %lf", cam.pos.x, cam.pos.y, cam.pos.z); 
    mvprintw(11, 0, "CAMROT: %lf %lf %lf", cam.rot.x, cam.rot.y, cam.rot.z);
    refresh();
}
/* function: rotate_and_display (unused)
 * description: draws a rotating model onto the screen, according to the given RPM and FPS
 */
void rotate_and_display(double rpm, double fps){
    double db_fc = 60.0/rpm*fps;
    double delta = 2*PI/db_fc;
    double delay = 1000000.0/fps;
    double rot = 0;
    for (int i = 0; i < db_fc; i++, rot += delta){
        clear();
        for (int j = 0; j < vc; j++){
            draw_vertex(vertexList[j]);
        }
        refresh();
        usleep(delay);
    }
}
/* function: reset_cam
 * description: resets the camera's attributes
 */
void reset_cam(){
    cam.pos.x = 0;
    cam.pos.y = 0;
    cam.pos.z = 0;
    cam.rot.x = 0;
    cam.rot.y = 0;
    cam.rot.z = 0;
    cam.surface_position.x = 1;
    cam.surface_position.y = 1;
    cam.surface_position.z = 1;
}
int main(int argc, char** argv) {
    if (argc <= 1) return 0;
    if (read_model(argv[1])){
        printf("Failed to read model\n");
        return 1;
    }
    reset_cam();
    initscr();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, true);
    refresh();
    while (true){
        // rotate_and_display(60.0, 1.0);
        display_model();
        int c = getch();
        if (c == 'a'){
            cam.pos.x --;
        }else if (c == 'd'){
            cam.pos.x ++;
        }else if (c == 'w'){
            cam.pos.y++;
        }else if (c == 's'){
            cam.pos.y--;
        }else if (c == 'q'){
            cam.pos.z--;
        }else if (c == 'e'){
            cam.pos.z++;
        }else if (c == 'i'){
            cam.rot.x = (cam.rot.x + PI/6);
            if (cam.rot.x > PI) cam.rot.x -= PI;
        }else if (c == 'o'){
            cam.rot.y = (cam.rot.y + PI/6);
            if (cam.rot.y > PI*2.0) cam.rot.y -= PI*2;
        }else if (c == 'p'){
            cam.rot.z = (cam.rot.z + PI/6);
            if (cam.rot.z > PI*2.0) cam.rot.z -= PI*2;
        }else if (c == 'r'){
            reset_cam();
        }else break;
    }
    endwin();
    return 0;
}
