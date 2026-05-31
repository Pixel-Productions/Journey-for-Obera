#define _DEFAULT_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <memory.h>
#include <math.h>

#define SCREEN_WIDTH 100
#define SCREEN_HEIGHT 60

#pragma region FRAMEWORK

typedef unsigned int color_t;

struct vec2 {
    int x, y;
};

struct image {
    color_t *data;
    int width;
    int height;
};

#define PI 3.1415

#define IDX(x, y, xb) ((x) + (y) * (xb))

#define SIDX(x, y) (IDX(x, y, SCREEN_WIDTH))

#define COLOR(r, g, b, a) ((color_t)(a << 24 | r << 16 | g << 8 | b))

#define LENOF(arr) (sizeof(arr) / sizeof(arr[0]))

#define LERP(a, b, x) (a+(b-a)*x)

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define CLAMP(a, x, y) (MIN(MAX((a), (x)), (y)))

#pragma endregion

#pragma region IMAGES

struct image loadimage(char path[]) {
    struct image img;

    FILE *file = fopen(path, "rb");

    unsigned char header[54];
    fread(header, 1, 54, file);

    img.width = header[18];
    img.height = abs(header[22]);

    int size = img.width*img.height;

    bool ud = header[22] > 0;

    int *raw = (int*)malloc(size*sizeof(int));
    fseek(file, header[10], SEEK_SET);
    fread(raw, sizeof(int), size*sizeof(int), file);
    
    color_t *data = (color_t*)malloc(size*sizeof(color_t));
    for (int x = 0; x < img.width; x++) {
        for (int y = 0; y < img.height; y++) {
            int i = IDX(x, y, img.width);
            data[(ud ? IDX(x, img.height-y-1, img.width) : i)] = COLOR(raw[i] >> 16, raw[i] >> 8, raw[i], raw[i] >> 24);
        }
    }

    img.data = data;

    fclose(file);

    return img;
}

void freeimage(struct image img) {
    free(img.data);
}

#pragma endregion

#pragma region DRAW METHODS

void clear() {
    printf("%s", "\033[H");
}

void putpix(int buffer[], int x, int y, color_t color) {
    float a = (color >> 24 & 0xFF) / 255.0f;
    int r2 = color >> 16 & 0xFF;
    int g2 = color >> 8 & 0xFF;
    int b2 = color & 0xFF;
    
    int r1 = buffer[SIDX(x, y)] >> 16 & 0xFF;
    int g1 = buffer[SIDX(x, y)] >> 8 & 0xFF;
    int b1 = buffer[SIDX(x, y)] & 0xFF;

    color_t final = COLOR((int)LERP(r1, r2, a), (int)LERP(g1, g2, a), (int)LERP(b1, b2, a), (int)(a*255));

    buffer[SIDX(x, y)] = final;
}

void blitcol(int buffer[], int minx, int miny, int maxx, int maxy, color_t color) {
    for (int x = minx; x < maxx; x++) {
        for (int y = miny; y < maxy; y++) {
            putpix(buffer, x, y, color);
        }
    }
}

void blitimg(int buffer[], int x, int y, struct image img) {
    for (int j = 0; j < img.width; j++) {
        for (int k = 0; k < img.height; k++) {
            putpix(buffer, x+j, y+k, img.data[IDX(j, k, img.width)]);
        }
    }
}

#pragma endregion

void render(int buffer[]) {
    for (int y = 0; y < SCREEN_HEIGHT; y += 2) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            printf("\033[48;2;%d;%d;%d;38;2;%d;%d;%dm▄\033[0m", buffer[SIDX(x, y)] >> 16 & 0xFF, buffer[SIDX(x, y)] >> 8 & 0xFF, buffer[SIDX(x, y)] & 0xFF, buffer[SIDX(x, y+1)] >> 16 & 0xFF, buffer[SIDX(x, y+1)] >> 8 & 0xFF, buffer[SIDX(x, y+1)] & 0xFF);
        }
        printf("%s", "\r\n");
    }
}

#pragma region INPUT

struct termios old;

void scanon() {
    struct termios new;

    tcgetattr(STDIN_FILENO, &old);
    new = old;

    new.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    new.c_oflag &= ~OPOST;
    new.c_cflag &= ~(CSIZE | PARENB);
    new.c_cflag |= CS8;
    new.c_lflag &= ~(ICANON | ECHO | ECHONL | ISIG | IEXTEN);

    new.c_cc[VMIN] = 0;
    new.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &new);
}

void scanoff() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
}

#pragma endregion

// Screen buffers
#define BUF_LEN SCREEN_WIDTH*SCREEN_HEIGHT
#define BUF_SIZE BUF_LEN*sizeof(color_t)

color_t fbuf[BUF_LEN] = { 0 };
color_t bbuf[BUF_LEN] = { 0 };

// Images & game data
struct image tree;

struct image shoura_left;
struct image shoura_right;

int time = 0;

struct vec2 pos;
struct vec2 vel;
int maxspeed = 2;
int facingdir = 0;

bool inputmap[255] = { false };

int main() {
    // Image loading
    tree = loadimage("assets/textures/fall_tree.bmp");

    shoura_left = loadimage("assets/textures/shoura_left.bmp");
    shoura_right = loadimage("assets/textures/shoura_right.bmp");

    // Ready game
    pos.x = 0;
    pos.y = 0;

    vel.x = 0;
    vel.y = 0;

    // Input setup
    scanon();

    char c;

    while (!inputmap['`']) {
        memset(inputmap, false, sizeof(inputmap));

        while (read(STDIN_FILENO, &c, 1) != 0) {
            inputmap[c] = true;
        }

        if (inputmap['d']) {
            vel.x = MIN(vel.x+1, maxspeed);
            facingdir = 1;
        } else if (inputmap['a']) {
            vel.x = MAX(vel.x-1, -maxspeed);
            facingdir = 0;
        } else if (vel.x != 0) {
            vel.x -= (vel.x / abs(vel.x));
        }

        pos.x += vel.x;
        pos.y += vel.y;

        memset(fbuf, 0, BUF_SIZE);

        blitcol(fbuf, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR(0, 0, CLAMP((int)((sin(time*PI*10)+0.5)*255), 0, 255), 255));
        blitimg(fbuf, 0, 0, tree);
        blitimg(fbuf, pos.x, pos.y, (facingdir == 1 ? shoura_right : shoura_left));

        if (memcmp(fbuf, bbuf, BUF_SIZE) != 0) {
            memcpy(bbuf, fbuf, BUF_SIZE);

            clear();
            render(fbuf);
        }

        time += 1;
    }

    freeimage(tree);

    freeimage(shoura_left);
    freeimage(shoura_right);

    scanoff();

    return 0;
}