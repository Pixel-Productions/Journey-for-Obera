#define _XOPEN_SOURCE_EXTENDED 1
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#define SCREEN_WIDTH 100
#define SCREEN_HEIGHT 70

#pragma region FRAMEWORK

struct vec2 {
    int x, y;
};

struct image {
    int *data;
    int width;
    int height;
};

#define IDX(x, y, xb) (x + y * xb)

#define SIDX(x, y) (IDX(x, y, SCREEN_WIDTH))

#define COLOR(r, g, b) (r << 16 | g << 8 | b)

#define LENOF(arr) (sizeof(arr) / sizeof(arr[0]))

int *subarr(int *arr, int start, int end) {
    int *newarr = (int*)malloc((end-start)*sizeof(arr[0]));

    for (int i = start; i < end; i++) {
        int nidx = i-start;
        newarr[nidx] = arr[i];
    }

    return newarr;
}

#pragma endregion

#pragma region IMAGES

struct image loadimage(char path[]) {
    struct image img;

    FILE *file = fopen(path, "rb");

    unsigned char header[54];
    fread(header, 1, 54, file);

    int *data = (int*)malloc(header[18]*header[20]*sizeof(int));
    fseek(file, header[10], SEEK_SET);
    fread(data, 1, SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(int), file);

    img.width = header[18];
    img.height = header[20];

    img.data = data;
    
    for (int i = 0; i < img.width * img.height; i++) {
        unsigned char bgr[3];
        fread(bgr, 1, 3, file);
        img.data[i] = COLOR(bgr[2], bgr[1], bgr[0]);
    }

    fclose(file);

    return img;
}

void freeimage(struct image img) {
    free(img.data);
}

#pragma endregion

#pragma region DRAW METHODS

void clear() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void blitcolor(int buffer[], int minx, int miny, int maxx, int maxy, int color) {
    for (int x = minx; x < maxx; x++) {
        for (int y = miny; y < maxy; y++) {
            buffer[SIDX(x, y)] = color;
        }
    }
}

void blitimg(int buffer[], int x, int y, struct image img) {
    for (int j = x; j < img.width; j++) {
        for (int k = y; k < img.height; k++) {
            buffer[SIDX(x+j, y+k)] = img.data[IDX(j, k, img.width)];
        }
    }
}

#pragma endregion

void render(int buffer[]) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        for (int y = 0; y < SCREEN_HEIGHT; y += 2) {
            printf("%s%c%c%c%c%c%s%c%c%c%c%c%s", "\033[38;2;", buffer[SIDX(x, y)], ';', buffer[SIDX(x, y)] >> 8, ';', buffer[SIDX(x, y)] >> 16, ";48;2;", buffer[SIDX(x, y+1)], ';', buffer[SIDX(x, y+1)] >> 8, ';', buffer[SIDX(x, y+1)] >> 16, "m▄\033[0m");
        }
        printf("%c", '\n');
    }
}

#pragma region INPUT

int getkey() {
    struct termios old, new;
    int c;

    tcgetattr(STDIN_FILENO, &old);
    new = old;

    new.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    new.c_oflag &= ~OPOST;
    new.c_cflag &= ~(CSIZE | PARENB);
    new.c_cflag |= CS8;
    new.c_lflag &= ~(ICANON | ECHO | ECHONL | ISIG | IEXTEN);

    tcsetattr(STDIN_FILENO, TCSANOW, &new);

    c = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &old);

    return c;
}

#pragma endregion

int fbuf[SCREEN_HEIGHT*SCREEN_WIDTH];

struct image tree;

int main() {
    tree = loadimage("assets/fall_tree.bmp");

    int key = 0;

    while (key != (int)'`') {
        key = getkey();

        blitimg(fbuf, 0, 0, tree);

        clear();
        render(fbuf);
    }

    freeimage(tree);

    return 0;
}