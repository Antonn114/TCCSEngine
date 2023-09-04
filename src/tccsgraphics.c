#include "tccsgraphics.h"
#include "tccsmath.h"

wchar_t TCCS_braille[256];

void TCCS_graphics_init(){
    TCCS_braille[0] = 0x2800;
    size_t i;
    for (i = 1; i < 256; i++){
        TCCS_braille[i] = TCCS_braille[i - 1] + 1;
    }
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
    if (screen->window) wclear(screen->window);
}

void TCCS_screen_update(TCCSScreen *screen){
    wclear(screen->window);
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