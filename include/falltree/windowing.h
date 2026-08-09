#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "types.h"
#include "dict.h"

#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <libdecor-0/libdecor.h>
#include <xkbcommon/xkbcommon.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define COMPOSITOR_KEY VOID_PTR("compositor")
#define SHM_KEY VOID_PTR("shm")

#define DISPLAY_KEY VOID_PTR("display")
#define REGISTRY_KEY VOID_PTR("registry")

#define SEAT_KEY VOID_PTR("seat")
#define KEYBOARD_KEY VOID_PTR("keyboard")
#define POINTER_KEY VOID_PTR("pointer")

#define XKB_CONTEXT_KEY VOID_PTR("xkb_context")
#define XKB_STATE_KEY VOID_PTR("xkb_state")
#define XKB_KEYMAP_KEY VOID_PTR("xkb_keymap")

#define SURFACE_KEY VOID_PTR("surface")
#define CONTEXT_KEY VOID_PTR("context")
#define FRAME_KEY VOID_PTR("frame")

#define BUFFER_KEY VOID_PTR("buffer")
#define DATA_KEY VOID_PTR("data")

static int wl_create_fd() {
    int fd = memfd_create("falltree-shared", MFD_CLOEXEC);
    if (fd == -1) {
        fprintf(stderr, "Error: Shared memory file descriptor creation failed\n");
    }

    return fd;
}

struct wl_buffer *wl_create_buffer(window win, int width, int height) {
    int stride = width * 4;
    int size = stride * height;

    int fd = wl_create_fd();

    if (ftruncate(fd, size) == -1) {
        fprintf(stderr, "Error: Failed to truncate file descriptor size\n");
        close(fd);
        exit(-1);
    }

    color_t *data = (color_t*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        fprintf(stderr, "Error: Failed to map window memory\n");
        exit(-1);
    }

    dict_add_elem(win->data, DATA_KEY, data);

    struct wl_shm_pool *pool = wl_shm_create_pool((struct wl_shm*)dict_get_elem(win->data, SHM_KEY)->val, fd, size);
    struct wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);
    wl_shm_pool_destroy(pool);

    close(fd);

    return buffer;
}

static void libdecor_error_handler(struct libdecor *context, enum libdecor_error err, const char *msg) {
    fprintf(stderr, "Error: libdecor error %d: %s", err, msg);
    exit(-1);
}

static struct libdecor_interface ldc_interface = {
    .error = libdecor_error_handler
};

static void libdecor_configure_handler(struct libdecor_frame *frame, struct libdecor_configuration *config, void *data) {
    window win = (window)data;

    struct wl_surface *surf = (struct wl_surface*)dict_get_elem(win->data, SURFACE_KEY)->val;

    wl_surface_attach(surf, (struct wl_buffer*)dict_get_elem(win->data, BUFFER_KEY)->val, 0, 0);
    wl_surface_damage_buffer(surf, 0, 0, win->width, win->height);

    struct libdecor_state *state = libdecor_state_new(win->width, win->height);
    libdecor_frame_commit(frame, state, config);
    libdecor_state_free(state);
    
    wl_display_flush((struct wl_display*)dict_get_elem(win->data, DISPLAY_KEY)->val);
}

static void libdecor_close_handler(struct libdecor_frame *frame, void *data) {
    ((window)data)->should_close = true;
}

static void libdecor_commit_handler(struct libdecor_frame *frame, void *data) {}

static struct libdecor_frame_interface frame_interface = {
    .configure = libdecor_configure_handler,
    .close = libdecor_close_handler,
    .commit = libdecor_commit_handler
};

static void wl_pointer_enter_handler(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t x, wl_fixed_t y) {}
static void wl_pointer_leave_handler(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface) {}

static void wl_pointer_motion_handler(void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y) {
    window win = (window)data;
    win->mouse.x = wl_fixed_to_int(x);
    win->mouse.y = wl_fixed_to_int(y);
}

static void wl_pointer_button_handler(void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
    window win = (window)data;

    if (state == WL_POINTER_BUTTON_STATE_PRESSED)
        win->mouse.button = button;
    else
        win->mouse.button = 0;
}

static void wl_pointer_axis_handler(void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value) {}
static void wl_pointer_axis_source_handler(void *data, struct wl_pointer *wl_pointer, uint32_t axis_source) {}
static void wl_pointer_axis_stop_handler(void *data, struct wl_pointer *wl_pointer, uint32_t time, uint32_t axis) {}
static void wl_pointer_axis_discrete_handler(void *data, struct wl_pointer *wl_pointer, uint32_t axis, int32_t discrete) {}
static void wl_pointer_frame_handler(void *data, struct wl_pointer *pointer) {}

static const struct wl_pointer_listener pointer_listener = {
    .enter = wl_pointer_enter_handler,
    .leave = wl_pointer_leave_handler,
    .motion = wl_pointer_motion_handler,
    .button = wl_pointer_button_handler,
    .axis = wl_pointer_axis_handler,
    .frame = wl_pointer_frame_handler,
    .axis_source = wl_pointer_axis_source_handler,
    .axis_stop = wl_pointer_axis_stop_handler,
    .axis_discrete = wl_pointer_axis_discrete_handler
};

static void wl_keyboard_keymap_handler(void *data, struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size) {
    window win = (window)data;

    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        fprintf(stderr, "Error: Bad keymap format\n");
        exit(-1);
    }

    char *map_shm = (char*)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    if (map_shm == MAP_FAILED) {
        fprintf(stderr, "Error: Failed to map keyboard shared memory\n");
        exit(-1);
    }

    struct xkb_keymap *keymap = xkb_keymap_new_from_string((struct xkb_context*)dict_get_elem(win->data, XKB_CONTEXT_KEY)->val, map_shm, (enum xkb_keymap_format)format, XKB_KEYMAP_COMPILE_NO_FLAGS);
    munmap(map_shm, size);
    close(fd);

    struct xkb_state *state = xkb_state_new(keymap);
    
    dict_elem keymap_elem = dict_get_elem(win->data, XKB_KEYMAP_KEY);
    dict_elem state_elem = dict_get_elem(win->data, XKB_STATE_KEY);

    if (keymap_elem != NULL) {
        xkb_keymap_unref((struct xkb_keymap*)keymap_elem->val);
        dict_set_elem(win->data, XKB_KEYMAP_KEY, keymap);
    } else
        dict_add_elem(win->data, XKB_KEYMAP_KEY, keymap);

    if (state_elem != NULL) {
        xkb_state_unref((struct xkb_state*)state_elem->val);
        dict_set_elem(win->data, XKB_STATE_KEY, state);
    } else
        dict_add_elem(win->data, XKB_STATE_KEY, state);
}

static void wl_keyboard_enter_handler(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys) {}
static void wl_keyboard_leave_handler(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface) {}

static void wl_keyboard_key_handler(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) {
    window win = (window)data;

    xkb_keysym_t sym = xkb_state_key_get_one_sym((struct xkb_state*)dict_get_elem(win->data, XKB_STATE_KEY)->val, key+8);
    ((window)data)->keyboard[xkb_keysym_to_utf32(sym)] = state == WL_KEYBOARD_KEY_STATE_PRESSED;
}

static void wl_keyboard_modifier_handler(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
    xkb_state_update_mask((struct xkb_state*)dict_get_elem(((window)data)->data, XKB_STATE_KEY)->val, mods_depressed, mods_latched, mods_locked, 0, 0, group);
}

static void wl_keyboard_repeat_info_handler(void *data, struct wl_keyboard *keyboard, int32_t rate, int32_t delay) {}

static const struct wl_keyboard_listener keyboard_listener = {
    .enter = wl_keyboard_enter_handler,
    .key = wl_keyboard_key_handler,
    .keymap = wl_keyboard_keymap_handler,
    .modifiers = wl_keyboard_modifier_handler,
    .repeat_info = wl_keyboard_repeat_info_handler,
    .leave = wl_keyboard_leave_handler
};

static void wl_seat_capability_handler(void *data, struct wl_seat *seat, uint32_t capabilities) {
    window win = (window)data;

    dict_elem pointer_elem = dict_get_elem(win->data, POINTER_KEY);
    bool has_pointer = capabilities & WL_SEAT_CAPABILITY_POINTER;

    if (has_pointer && pointer_elem == NULL) {
        struct wl_pointer *pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(pointer, &pointer_listener, win);
        dict_add_elem(win->data, POINTER_KEY, pointer);
    } else if (!has_pointer && pointer_elem != NULL) {
        struct wl_pointer *pointer = (struct wl_pointer*)pointer_elem->val;
        wl_pointer_release(pointer);
        dict_remove_elem(win->data, POINTER_KEY);
    }

    dict_elem keyboard_elem = dict_get_elem(win->data, KEYBOARD_KEY);
    bool has_keyboard = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;

    if (has_keyboard && keyboard_elem == NULL) {
        struct wl_keyboard *keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(keyboard, &keyboard_listener, win);
        dict_add_elem(win->data, KEYBOARD_KEY, keyboard);
    } else if (!has_keyboard && keyboard_elem != NULL) {
        struct wl_keyboard *keyboard = (struct wl_keyboard*)keyboard_elem->val;
        wl_keyboard_release(keyboard);
        dict_remove_elem(win->data, KEYBOARD_KEY);
    }
}

static void wl_seat_name_handler(void *data, struct wl_seat *seat, const char *name) {}

static const struct wl_seat_listener seat_listener = {
    .capabilities = wl_seat_capability_handler,
    .name = wl_seat_name_handler
};

static void wl_registry_global_handler(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
    window win = (window)data;
    
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        dict_add_elem(win->data, COMPOSITOR_KEY, wl_registry_bind(registry, name, &wl_compositor_interface, 4));
    } else if (strcmp(interface, wl_shm_interface.name) == 0) {
        dict_add_elem(win->data, SHM_KEY, wl_registry_bind(registry, name, &wl_shm_interface, 1));
    } else if (strcmp(interface, wl_seat_interface.name) == 0) {
        struct wl_seat *seat = (struct wl_seat*)wl_registry_bind(registry, name, &wl_seat_interface, 7);
        dict_add_elem(win->data, SEAT_KEY, seat);
        wl_seat_add_listener(seat, &seat_listener, win);
    }
}

static void wl_registry_global_remove_handler(void *data, struct wl_registry *registry, uint32_t name) {

}

static const struct wl_registry_listener registry_listener = {
    .global = wl_registry_global_handler,
    .global_remove = wl_registry_global_remove_handler
};

window window_init(char *title, int width, int height) {
    window win = (window)calloc(1, sizeof(*win));

    win->width = width;
    win->height = height;

    win->data = dict_init();

    if (getenv("WAYLAND_DISPLAY") != NULL)
        win->type = WAYLAND;
    else if (getenv("DISPLAY") != NULL)
        win->type = X11;
    else {
        fprintf(stderr, "Error: Could not connect to display server\n");
        exit(-1);
    }

    switch (win->type) {
        case WAYLAND: {
            struct wl_display *display = wl_display_connect(NULL);
            if (!display) {
                fprintf(stderr, "Error: Failed to connect to Wayland display\n");
                exit(-1);
            }

            struct wl_registry *registry = wl_display_get_registry(display);
            wl_registry_add_listener(registry, &registry_listener, win);

            struct xkb_context *xkb_ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
            dict_add_elem(win->data, XKB_CONTEXT_KEY, xkb_ctx);

            wl_display_roundtrip(display);

            struct wl_surface *surface = wl_compositor_create_surface((struct wl_compositor*)dict_get_elem(win->data, COMPOSITOR_KEY)->val);
            dict_add_elem(win->data, SURFACE_KEY, surface);

            struct wl_buffer *buf = wl_create_buffer(win, width, height);
            dict_add_elem(win->data, BUFFER_KEY, buf);

            struct libdecor *context = libdecor_new(display, &ldc_interface);
            struct libdecor_frame *frame = libdecor_decorate(context, surface, &frame_interface, win);

            dict_add_elem(win->data, DISPLAY_KEY, display);
            dict_add_elem(win->data, REGISTRY_KEY, registry);
            dict_add_elem(win->data, CONTEXT_KEY, context);
            dict_add_elem(win->data, FRAME_KEY, frame);

            libdecor_frame_set_title(frame, title);
            libdecor_frame_set_app_id(frame, title);
            libdecor_frame_map(frame);

            break;
        }
    }

    return win;
}

void window_frame(window win) {
    switch (win->type) {
        case WAYLAND: {
            if (libdecor_dispatch((struct libdecor*)dict_get_elem(win->data, CONTEXT_KEY)->val, 0) == -1) win->should_close = true;
            break;
        }
    }
}

void window_blit_pixels(window win, color_t *pixels) {
    switch (win->type) {
        case WAYLAND: {
            for (int i = 0; i < win->width*win->height; i++) {
                ((color_t*)dict_get_elem(win->data, DATA_KEY)->val)[i] = pixels[i] >> 8;
            }

            struct wl_surface *surf = (struct wl_surface*)dict_get_elem(win->data, SURFACE_KEY)->val;

            wl_surface_attach(surf, (struct wl_buffer*)dict_get_elem(win->data, BUFFER_KEY)->val, 0, 0);
            wl_surface_damage_buffer(surf, 0, 0, win->width, win->height);
            
            wl_surface_commit(surf);
            wl_display_flush((struct wl_display*)dict_get_elem(win->data, DISPLAY_KEY)->val);
            break;
        }
    }
}

void end_window(window win) {
    switch (win->type) {
        case WAYLAND: {
            wl_surface_destroy((struct wl_surface*)dict_get_elem(win->data, SURFACE_KEY)->val);
            wl_buffer_destroy((struct wl_buffer*)dict_get_elem(win->data, BUFFER_KEY)->val);
            munmap((color_t*)dict_get_elem(win->data, DATA_KEY)->val, win->width*win->height*4);
            libdecor_frame_unref((struct libdecor_frame*)dict_get_elem(win->data, FRAME_KEY)->val);
            libdecor_unref((struct libdecor*)dict_get_elem(win->data, CONTEXT_KEY)->val);
            wl_registry_destroy((struct wl_registry*)dict_get_elem(win->data, REGISTRY_KEY)->val);
            wl_display_disconnect((struct wl_display*)dict_get_elem(win->data, DISPLAY_KEY)->val);
            break;
        }
    }

    free_dict(win->data);
}

#ifdef __cplusplus
}
#endif