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

#include <ncurses.h>
#include <wchar.h>
#include <locale.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#define STROKE_RADIUS 16 
#define VIS_WIN_HEIGHT 480
#define VIS_WIN_WIDTH 640

#define WIN_HEIGHT 24
#define WIN_WIDTH 64
#define OFF_X 62
#define OFF_Y 8

typedef struct Vector2{
    float x, y;
} Vector2;

typedef struct Vector3{
    float x, y, z;
} Vector3;

wchar_t braille[256];
WINDOW *win;
uint16_t vis_board[VIS_WIN_HEIGHT][VIS_WIN_WIDTH];
int scr_x, scr_y;
void drawpoint(uint16_t x, uint16_t y, uint16_t delta){
    uint16_t i, j;
    uint16_t starti = y - STROKE_RADIUS > 0 ? y - STROKE_RADIUS : 0;
    uint16_t endi = y + STROKE_RADIUS < VIS_WIN_HEIGHT ? y + STROKE_RADIUS : VIS_WIN_HEIGHT;
    uint16_t startj = x - STROKE_RADIUS > 0 ? x - STROKE_RADIUS : 0;
    uint16_t endj = x + STROKE_RADIUS < VIS_WIN_WIDTH ? x + STROKE_RADIUS : VIS_WIN_WIDTH;
    for (i = starti; i <= endi; i++){
        for (j = startj; j <= endj; j++){
            vis_board[i][j] = delta;
        }
    }
}

void drawline(float x1, float y1, float x2, float y2){
    float dx = x2 - x1;
    float dy = y2 - y1;
    float x = x1, y = y1;
    float step = fmax(fabs(dx), fabs(dy));
    dx /= step;
    dy /= step;
    uint32_t i = 0;
    while (i <= step){
        drawpoint(x + 0.5, y + 0.5, 1);
        x += dx;
        y += dy;
        i++;
    }
}
void drawtriangle(float x1, float y1, float x2, float y2, float x3, float y3){
    drawline(x1, y1, x2, y2);
    drawline(x2, y2, x3, y3);
    drawline(x3, y3, x1, y1);
}
uint16_t threshold;
uint16_t smallheight, smallwidth;
void clear_vis(){
    memset(vis_board, 0, sizeof vis_board);
}
void redraw(WINDOW* w){
    wclear(w);
    uint16_t i, j, k, l, x, y, type;
    float col = 0;
    for (i = 0; i < VIS_WIN_HEIGHT; i += smallheight*4){
        for (j = 0; j < VIS_WIN_WIDTH; j += smallwidth*2){
            type = 0;
            for (k = 0; k < 8; k++){
                col = 0;
                for (x = i + ((k >> 1) << 2); x < i + ((k >> 1) << 2) + smallheight; x++){
                    for (y = j + (k & 1)*smallwidth; y < j + (k & 1)*smallwidth + smallwidth; y++){
                        col += vis_board[x][y];
                    }
                }
                type |= (1 << k)*(col >= threshold);
            }
            wprintw(w, "%lc", braille[type]);
        }
    }
    box(w, 0, 0);
    wrefresh(w);
}

void init(){
    smallheight = VIS_WIN_HEIGHT/WIN_HEIGHT/4;
    smallwidth = VIS_WIN_WIDTH/WIN_WIDTH/2;
    threshold = 5;
    getmaxyx(stdscr, scr_y, scr_x);
    uint16_t i;
    braille[0] = 0x2800;
    for (i = 1; i < 256; i++){
        braille[i] = braille[i - 1] + 1;
    }
}
int main(int argc, char **argv){
    setlocale(LC_CTYPE, "");
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    start_color();
    init();
    clear();
    win = newwin(WIN_HEIGHT, WIN_WIDTH, OFF_Y, OFF_X);
    refresh();
    box(win, 0, 0);
    mvwprintw(win, 1, 1, "%d %d %d %d", smallheight, smallwidth, scr_y, scr_x);
    wrefresh(win);
    getch();
    drawtriangle(0, 0, 300, 300, 300, 150);
    redraw(win);
    getch();
    float t;
    uint32_t x0 = VIS_WIN_WIDTH/2, y0 = VIS_WIN_HEIGHT/2;
    uint32_t x1 = 200, y1 = 0;
    uint32_t x2 = 0, y2 = 200;
    for (t = 0; t <  10000; t+= 0.2){
        clear_vis();
        drawtriangle(x0, y0, cos(t)*x1 - sin(t)*y1 + x0, sin(t)*x1 + cos(t)*y1 + y0, cos(t)*x2 - sin(t)*y2 + x0, sin(t)*x2 + cos(t)*y2 + y0);
        redraw(win);
        usleep(40000); 
    }
    endwin();
}

