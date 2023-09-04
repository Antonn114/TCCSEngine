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
#include <locale.h>
#include <unistd.h>
#define STROKE_RADIUS 0
#define VIS_WIN_HEIGHT 480
#define VIS_WIN_WIDTH 640

#define WIN_HEIGHT 24
#define WIN_WIDTH 64
#define OFF_X 62
#define OFF_Y 8
TCCSScreen* screen;
WINDOW* win;
int main(int argc, char **argv){
    setlocale(LC_CTYPE, "");
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    start_color();
    TCCS_graphics_init();
    win = newwin(WIN_HEIGHT, WIN_WIDTH, OFF_Y, OFF_X);
    screen = TCCS_screen_create(VIS_WIN_HEIGHT, VIS_WIN_WIDTH);
    TCCS_screen_set_stroke_radius(screen, STROKE_RADIUS);
    TCCS_screen_set_window(screen, win);
    TCCS_screen_set_margin(screen, 0);
    TCCS_screen_set_threshold(screen, 1);
    refresh();
    
    int originx = VIS_WIN_WIDTH/2;
    int originy = VIS_WIN_HEIGHT/2;
    int y0 = -50;
    int x0 = -50;
    int x1 = 100;
    int y1 = -50;
    int x2 = -50;
    int y2 = 100;

    TCCS_screen_filltriangle(screen, originx + x0, originy + y0, originx +  x1, originy + y1, originx +  x2, originy + y2);
    TCCS_screen_update(screen);

    getch();

    float i = 0;
    
    for (i = 0; i < 100000000000.0; i+=0.1){
        TCCS_screen_clear(screen);
        TCCS_screen_drawtriangle(screen, 
        cos(i/2.0)*x0 - sin(i/4.0)*y0 + originx, sin(i/2.0)*x0 + cos(i/4.0)*y0 + originy,
        cos(i/2.0)*x1 - sin(i/4.0)*y1 + originx, sin(i/2.0)*x1 + cos(i/4.0)*y1 + originy,
        cos(i/2.0)*x2 - sin(i/4.0)*y2 + originx, sin(i/2.0)*x2 + cos(i/4.0)*y2 + originy);
        TCCS_screen_update(screen);
        usleep(20000);
    }

    TCCS_screen_destroy(screen);
    endwin();
    
}

