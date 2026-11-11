#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include <pthread.h>
#include <math.h>
#include "object.h"
#include "list.h"
#include "dict.h"
#include "stack.h"
#include "tree.h"
#include "types.h"
#include "stb/stb_image.h"
#include "glad/glad.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

image load_image(char *pth) {
    image img = (image)malloc(sizeof(*img));

    FILE *file = fopen(pth, "rb");

    int channels;
    unsigned char *data = stbi_load_from_file(file, &img->width, &img->height, &channels, STBI_rgb_alpha);
    img->data = (color_t*)malloc(img->width*img->height*sizeof(color_t));

    for (int i = 0; i < img->width*img->height; i++) {
        img->data[i] = COLOR(data[i*4], data[i*4 + 1], data[i*4 + 2], data[i*4 + 3]);
    }

    fclose(file);

    return img;
}

void free_image(image img) {
    free(img->data);
    free(img);
}

tileset load_tileset(char *pth) {
    tileset set = (tileset)malloc(sizeof(*set));

    FILE *file = fopen(pth, "rb");

    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    rewind(file);

    unsigned char data[len];
    fread(data, len, 1, file);

    int plen = data[0];
    char ipth[plen];
    memcpy(ipth, data+1, plen);

    set->map = load_image(ipth);

    set->tw = data[plen+1];
    set->th = data[plen+2];
    set->tox = data[plen+3];
    set->toy = data[plen+4];
    set->tsx = data[plen+5];
    set->tsy = data[plen+6];

    int tlen = data[plen+7];
    set->tilec = tlen;
    set->tiles = (tile*)malloc(tlen*sizeof(tile));
    for (int i = 0; i < tlen; i++) {
        tile tl;
        tl.x = data[plen+8+i*3]*set->tw;
        tl.y = data[plen+9+i*3]*set->th;
        tl.hascoll = data[plen+10+i*3];

        set->tiles[i] = tl;
    }

    fclose(file);

    return set;
}

void free_tileset(tileset set) {
    free(set->map->data);
    free(set->map);
    free(set);
}

tilemap load_tilemap(char *pth) {
    tilemap map = (tilemap)malloc(sizeof(*map));

    FILE *file = fopen(pth, "rb");

    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    rewind(file);

    unsigned char *data = (unsigned char*)malloc(len);
    fread(data, 1, len, file);

    map->width = data[1] << 8 | data[0];
    map->height = data[3] << 8 | data[2];

    map->data = (int*)malloc(map->width*map->height*sizeof(int));

    for (int i = 0; i < map->width*map->height; i++) {
        map->data[i] = data[i+4];
    }

    fclose(file);

    return map;
}

void free_tilemap(tilemap map) {
    free(map->data);
    free(map);
}

shape load_shape(char *pth) {
    shape shp = (shape)malloc(sizeof(*shp));

    FILE *file = fopen(pth, "rb");

    unsigned char header[4];
    fread(header, 1, 4, file);

    shp->width = header[1] << 8 | header[0];
    shp->height = header[3] << 8 | header[2];

    int size = shp->width*shp->height;
    
    shp->data = (bool*)malloc(size);
    fseek(file, 4, SEEK_SET);
    fread(shp->data, 1, size, file);

    fclose(file);

    return shp;
}

void free_shape(shape shp) {
    free(shp->data);
    free(shp);
}

font load_font(char *pth) {
    font fnt = (font)malloc(sizeof(*fnt));

    FILE *file = fopen(pth, "rb");

    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    rewind(file);

    unsigned char data[len];
    fread(data, 1, len, file);

    fnt->width = data[1] << 8 | data[0];
    fnt->height = data[3] << 8 | data[2];

    int plen = data[4];
    char ipth[plen];
    memcpy(ipth, data+5, plen);

    fnt->img = load_image(ipth);

    fclose(file);

    return fnt;
}

void free_font(font fnt) {
    free(fnt->img->data);
    free(fnt->img);
    free(fnt);
}


#define KEY_RIGHT GLFW_KEY_RIGHT
#define KEY_LEFT GLFW_KEY_LEFT
#define KEY_UP GLFW_KEY_UP
#define KEY_DOWN GLFW_KEY_DOWN
#define KEY_ESCAPE GLFW_KEY_ESCAPE

static void key_handler(GLFWwindow *win, int key, int scancode, int action, int mods) {
    game gm = (game)glfwGetWindowUserPointer(win);

    if (key >= 32 && key <= 96 || key >= 256 && key <= 301) {
        if (action == GLFW_PRESS)
            gm->keyboard[key] = true;
        else if (action == GLFW_RELEASE)
            gm->keyboard[key] = false;
    }
}

static void cursor_pos_handler(GLFWwindow *win, double x, double y) {
    game gm = (game)glfwGetWindowUserPointer(win);

    gm->mouse.x = (int)(x/gm->scale);
    gm->mouse.y = (int)(y/gm->scale);
}

static void mouse_button_handler(GLFWwindow *win, int button, int action, int mods) {
    game gm = (game)glfwGetWindowUserPointer(win);
                
    if (action == GLFW_PRESS)
        gm->mouse.button = button;
    else if (action == GLFW_RELEASE)
        gm->mouse.button = -1;
}

// Initialize a new game with window label `title`, window width `width`, window height `height`, and pixel scale `scale`.
game game_init(char *title, int width, int height, int scale) {
    game gm = (game)malloc(sizeof(*gm));

    glfwInit();
    glfwWindowHintString(GLFW_WAYLAND_APP_ID, title);
    glfwWindowHint(GLFW_RESIZABLE, false);

    gm->win = glfwCreateWindow(width*scale, height*scale, title, NULL, NULL);

    glfwSetWindowUserPointer(gm->win, gm);

    glfwSetKeyCallback(gm->win, key_handler);
    glfwSetCursorPosCallback(gm->win, cursor_pos_handler);
    glfwSetMouseButtonCallback(gm->win, mouse_button_handler);

    //start_audio();

    gm->width = width;
    gm->height = height;
    gm->scale = scale;

    gm->bsize = width*height;
    gm->scaled_size = width*scale*height*scale;

    gm->bgcol = 0;

    gm->pbuf = (color_t*)calloc(width*height, sizeof(color_t));
    gm->cbuf = (short*)calloc(width*height, sizeof(short));

    gm->active_scene = 0;
    gm->scenes = list_init();

    gm->free_queue = stack_init();

    pthread_mutex_init(&gm->mutex, NULL);

    return gm;
}

static void put_pixel(game gm, int x, int y, color_t color) {
    if (x >= 0 && x < gm->width && y >= 0 && y < gm->height) {
        float a = (color & 0xFF) / 255.0f;
        int r2 = color >> 24 & 0xFF;
        int g2 = color >> 16 & 0xFF;
        int b2 = color >> 8 & 0xFF;
        
        int r1 = gm->pbuf[IDX(x, y, gm->width)] >> 24 & 0xFF;
        int g1 = gm->pbuf[IDX(x, y, gm->width)] >> 16 & 0xFF;
        int b1 = gm->pbuf[IDX(x, y, gm->width)] >> 8 & 0xFF;

        int final = COLOR((int)LERP(r1, r2, a), (int)LERP(g1, g2, a), (int)LERP(b1, b2, a), (int)(a*255));

        gm->pbuf[IDX(x, y, gm->width)] = final;
    }
}

static void blit_image(game gm, int x, int y, image img) {
    for (int j = 0; j < img->width; j++) {
        for (int k = 0; k < img->height; k++) {
            put_pixel(gm, x+j, y+k, img->data[IDX(j, k, img->width)]);
        }
    }
}

static void blit_shape(game gm, int x, int y, shape shp, short uid) {
    for (int j = 0; j < shp->width; j++) {
        for (int k = 0; k < shp->height; k++) {
            if (x+j >= 0 && x+j < gm->width && y+k >= 0 && y+k < gm->height && gm->cbuf[IDX(x+j, y+k, gm->width)] == 0)
                gm->cbuf[IDX(x+j, y+k, gm->width)] = shp->data[IDX(j, k, shp->width)]*uid;
        }
    }
}

// Initialize a new scene.
scene scene_init() {
    scene scn = (scene)malloc(sizeof(*scn));

    scn->objects = list_init();
    scn->lights = list_init();
    scn->particles = list_init();

    return scn;
}

// Add object `obj` to scene `scn` and sort scene objects by Z-index.
void scene_add_object(scene scn, object obj) {
    list_add_elem(scn->objects, obj);

    int min_idx = 0;
 	for (int i = 0; i < scn->objects->len; i++) {
		min_idx = i;

	  	for (int j = i+1; j < scn->objects->len; j++) {
			if (((object)list_get_elem(scn->objects, min_idx))->zidx > ((object)list_get_elem(scn->objects, j))->zidx) {
				min_idx = j;
			}
  		}

		object o0 = (object)list_get_elem(scn->objects, i);
        object o1 = (object)list_get_elem(scn->objects, min_idx);
        list_set_elem(scn->objects, i, o1);
        list_set_elem(scn->objects, min_idx, o0);
 	}
}

// Remove object `obj` from scene `scn`.
void scene_remove_object(scene scn, object obj) {
    for (int i = 0; i < scn->objects->len; i++)
        if (((object)list_get_elem(scn->objects, i)) == obj)
            list_remove_elem(scn->objects, i);
}

// Add a light source to scene `scn`.
void scene_add_light(scene scn, light lgh) {
    list_add_elem(scn->lights, lgh);
}

// Remove a light source from scene `scn`.
void scene_remove_light(scene scn, light lgh) {
    for (int i = 0; i < scn->lights->len; i++)
        if (list_get_elem(scn->lights, i) == lgh)
            list_remove_elem(scn->lights, i);
}

// Spawn a particle in scene `scn`.
particle spawn_particle(scene scn, image img, int dur, vec2 pos, vec2 linvel) {
    particle part = (particle)malloc(sizeof(*part));

    part->rem = dur;
    part->img = img;
    part->pos = pos;
    part->linvel = linvel;

    list_add_elem(scn->particles, part);

    return part;
}

// Prematurely remove a particle from scene `scn`.
void remove_particle(scene scn, particle part) {
    for (int i = 0; i < scn->particles->len; i++)
        if (list_get_elem(scn->particles, i) == part)
            list_remove_elem(scn->particles, i);
}

// Add scene `scn` to game `gm`.
void game_add_scene(game gm, scene scn) {
    list_add_elem(gm->scenes, scn);
}

// Register an asset in game `gm` 's asset manager to be freed when the game closes.
void *register_asset(game gm, enum ASSET_TYPE type, char *pth) {
    asset ast = (asset)malloc(sizeof(*ast));

    ast->type_name = type;

    void *val;
    switch (type) {
        case IMAGE: {
            image img = load_image(pth);

            ast->type.image = img;
            val = img;
            break;
        }
        case TILESET: {
            tileset set = load_tileset(pth);

            ast->type.tileset = set;
            val = set;
            break;
        }
        case TILEMAP: {
            tilemap map = load_tilemap(pth);

            ast->type.tilemap = map;
            val = map;
            break;
        }
        case SHAPE: {
            shape shp = load_shape(pth);

            ast->type.shape = shp;
            val = shp;
            break;
        }
        case FONT: {
            font fnt = load_font(pth);

            ast->type.font = fnt;
            val = fnt;
            break;
        }
    }

    return val;
}

static void render_object(game gm, object obj, int ox, int oy) {
    switch (obj->type_name) {
        case RECT: {
            for (int x = 0; x < obj->type.rect->width; x++) {
                for (int y = 0; y < obj->type.rect->height; y++) {
                    put_pixel(gm, obj->transform.pos.x+ox+x, obj->transform.pos.y+oy+y, obj->type.rect->color);
                }
            }

            break;
        }
        case SPRITE: {
            blit_image(gm, obj->transform.pos.x+ox, obj->transform.pos.y+oy, obj->type.sprite->img);
            break;
        }
        case ANIMATED_SPRITE: {
            image img = obj->type.animated_sprite->img;

            int w = obj->type.animated_sprite->w;
            int h = obj->type.animated_sprite->h;
            int hframes = obj->type.animated_sprite->hframes;
            int vframes = obj->type.animated_sprite->vframes;
            int sx = obj->type.animated_sprite->sx;
            int sy = obj->type.animated_sprite->sy;
            int sox = obj->type.animated_sprite->ox;
            int soy = obj->type.animated_sprite->oy;

            int frame = obj->type.animated_sprite->frame;

            int fx = frame % hframes;
            int fy = frame / hframes;

            for (int j = 0; j < w; j++) {
                for (int k = 0; k < h; k++) {
                    put_pixel(gm, obj->transform.pos.x+ox+j, obj->transform.pos.y+oy+k, img->data[IDX(sox + fx*sx + fx*w + j, soy + fy*sy + fy*h + k, img->width)]);
                }
            }
            
            break;
        }
        case TILE_GRID: {
            tileset set = obj->type.tile_grid->set;
            tilemap map = obj->type.tile_grid->map;

            for (int x = 0; x < map->width; x++) {
                for (int y = 0; y < map->height; y++) {
                    int idx = map->data[IDX(x, y, map->width)];

                    if (idx) {
                        tile tl = set->tiles[idx-1];

                        for (int j = 0; j < set->tw; j++) {
                            for (int k = 0; k < set->th; k++) {
                                put_pixel(gm, obj->transform.pos.x+ox+set->tw*x+j, obj->transform.pos.y+oy+set->th*y+k, set->map->data[IDX(tl.x+j, tl.y+k, set->map->width)]);
                            }
                        }
                    }
                }
            }

            break;
        }
        case TEXT: {
            font fnt = obj->type.text->fnt;
            char *string = obj->type.text->string;

            int hor = 0;
            int vert = 0;
            
            for (int i = 0; i < strlen(string); i++) {
                if (hor == obj->type.text->width) {
                    hor = 0;
                    vert++;
                }

                char ch = string[i];
                if (ch != '\n') {
                    for (int j = 0; j < fnt->width; j++) {
                        for (int k = 0; k < fnt->height; k++) {
                            put_pixel(gm, obj->transform.pos.x+ox+j + hor*fnt->width, obj->transform.pos.y+oy+k + vert*fnt->height, fnt->img->data[IDX((ch-32)*fnt->width+j, k, fnt->img->width)]);
                        }
                    }
                    hor++;
                } else {
                    hor = 0;
                    vert++;
                }
            }

            break;
        }
    }
}

static void render_object_tree(game gm, list objects, list lights, int ox, int oy) {
    for (int i = 0; i < objects->len; i++) {
        object obj = (object)list_get_elem(objects, i);

        if (obj->visible) {
            render_object(gm, (object)list_get_elem(objects, i), ox, oy);

            render_object_tree(gm, obj->children, obj->lights, ox + obj->transform.pos.x, oy + obj->transform.pos.y);
        }
    }

    for (int i = 0; i < lights->len; i++) {
        light lgh = (light)list_get_elem(lights, i);

        int x = ox + lgh->pos.x;
        int y = oy + lgh->pos.y;

        for (int j = x - lgh->radius; j < x + lgh->radius; j++) {
            for (int k = y - lgh->radius; k < y + lgh->radius; k++) {
                put_pixel(gm, j, k, (float)lgh->radius / (float)sqrt((x - j)*(x - j) + (y - k)*(y - k)) * lgh->color);
            }
        }
    }
}

static void *render_game(void *arg) {
    game gm = (game)arg;

    glfwMakeContextCurrent(gm->win);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glRasterPos2i(-1, 1);
    glPixelZoom((float)gm->scale, -(float)gm->scale);

    while (!glfwWindowShouldClose(gm->win)) {
        glfwPollEvents();

        memset(gm->pbuf, 0, gm->bsize*sizeof(color_t));
        
        scene scn = (scene)list_get_elem(gm->scenes, gm->active_scene);
        render_object_tree(gm, scn->objects, scn->lights, 0, 0);

        for (int i = 0; i < scn->particles->len; i++) {
            particle part = (particle)list_get_elem(scn->particles, i);

            blit_image(gm, part->pos.x, part->pos.y, part->img);
        }

        glDrawPixels(gm->width, gm->height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, gm->pbuf);

        glfwSwapBuffers(gm->win);
    }

    glfwDestroyWindow(gm->win);
    glfwTerminate();

    return NULL;
}

// Queue object `obj` to be freed at the end of the frame.
void object_queue_free(game gm, object obj) {
    stack_push(gm->free_queue, obj);
}


static void object_pretick(game gm, object obj, int ox, int oy) {
    switch (obj->type_name) {
        case AREA: {
            area ar = obj->type.area;

            ar->colliding = false;
            
            for (int j = 0; j < ar->width; j++) {
                for (int k = 0; k < ar->height; k++) {
                    int x = obj->transform.pos.x+ox+j;
                    int y = obj->transform.pos.y+oy+k;

                    if (x >= 0 && x < gm->width && y >= 0 && y < gm->height && gm->cbuf[IDX(x, y, gm->width)])
                        ar->colliding = true;
                }
            }

            break;
        }
    }
}

static void tick_object_tree(game gm, list objects, enum EVENT_PHASE phase, int ox, int oy) {
    switch (phase) {
        case PRE: {
            for (int i = 0; i < objects->len; i++) {
                object obj = (object)list_get_elem(objects, i);

                obj->transform.offset.x = ox;
                obj->transform.offset.y = oy;

                object_pretick(gm, obj, ox, oy);

                if (obj->tick != NULL)
                    obj->tick(gm, obj);

                tick_object_tree(gm, obj->children, PRE, ox + obj->transform.pos.x, oy + obj->transform.pos.y);
            }

            break;
        }
        case POST: {
            for (int i = 0; i < objects->len; i++) {
                object obj = (object)list_get_elem(objects, i);
                
                if (obj->is_collider)
                    blit_shape(gm, obj->transform.pos.x + ox, obj->transform.pos.y + oy, obj->shp, obj->uid);
                
                tick_object_tree(gm, obj->children, POST, ox + obj->transform.pos.x, oy + obj->transform.pos.y);
            }

            break;
        }
    }
}

// Start game `gm` 's main loop.
void game_start(game gm) {
    pthread_create(&gm->render_thread, NULL, render_game, gm);
    pthread_detach(gm->render_thread);

    while (!glfwWindowShouldClose(gm->win)) {
        gm->tick(gm, PRE);

        scene scn = (scene)list_get_elem(gm->scenes, gm->active_scene);

        tick_object_tree(gm, scn->objects, PRE, 0, 0);
        
        memset(gm->cbuf, 0, gm->bsize*sizeof(short));
        tick_object_tree(gm, scn->objects, POST, 0, 0);

        for (int i = 0; i < scn->particles->len; i++) {
            particle part = (particle)list_get_elem(scn->particles, i);

            if (part->rem == 0) {
                list_remove_elem(scn->particles, i);
                free(part);

                continue;
            }

            part->rem--;
            part->pos.x += part->linvel.x;
            part->pos.y += part->linvel.y;
        }

        gm->tick(gm, POST);

        while (gm->free_queue->size != 0) {
            free_object((object)stack_pop(gm->free_queue));
        }

        usleep(16667);
    }

    for (int i = 0; i < gm->scenes->len; i++) {
        scene scn = (scene)list_get_elem(gm->scenes, i);
        
        for (int n = 0; n < scn->objects->len; n++)
            free_object((object)list_get_elem(scn->objects, n));

        for (int n = 0; n < scn->lights->len; n++)
            free((light)list_get_elem(scn->lights, n));

        for (int n = 0; n < scn->particles->len; n++)
            free((particle)list_get_elem(scn->particles, n));

        free_list(scn->objects);
        free_list(scn->lights);
        free_list(scn->particles);
        free(scn);
    }

    free_list(gm->scenes);

    for (int i = 0; i < gm->assets->len; i++) {
        asset ast = (asset)list_get_elem(gm->assets, i);

        switch (ast->type_name) {
            case IMAGE: {
                free(ast->type.image);
                break;
            }
            case TILESET: {
                free(ast->type.tileset);
                break;
            }
            case TILEMAP: {
                free(ast->type.tilemap);
                break;
            }
            case SHAPE: {
                free(ast->type.shape);
                break;
            }
            case FONT: {
                free(ast->type.font);
                break;
            }
        }

        free(ast);
    }

    free_list(gm->assets);

    free_stack(gm->free_queue);

    free(gm->pbuf);
    free(gm->cbuf);

    //end_audio();

    free(gm);
}

#pragma endregion

#ifdef __cplusplus
}
#endif