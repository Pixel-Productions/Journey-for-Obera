#include <X11/Xlib.h>
#include <stdbool.h>
#include "types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

window create_window(char *title, int width, int height) {
    #ifdef __linux__
        Display *display = XOpenDisplay(NULL);

        Window win = XCreateSimpleWindow(display, DefaultRootWindow(display), 50, 50, width, height, 0, BlackPixel(display, 0), WhitePixel(display, 0));
        XMapWindow(display, win);

        XSelectInput(display, win, KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    #elif defined(_WIN32)
        
    #endif
}

bool

void poll_events(window win) {
    #ifdef __linux__
        XNextEvent(win->display, win->event);

        switch (win->event->type) {
            case KeyPress: {
                win->input[win->event->xkey.keycode] = true;
                break;
            }
            case KeyRelease: {
                win->input[win->event->xkey.keycode] = false;
                break;
            }
            case ButtonPress: {
                win->mouse_down = true;
                break;
            }
            case ButtonRelease: {
                win->mouse_down = false;
                break;
            }
            case MotionNotify: {
                win->px = win->event->xmotion.x;
                win->py = win->event->xmotion.y;
            }
        }
    #elif defined(_WIN32)

    #endif
}

void end_window(window win) {
    #ifdef __linux__
        XCloseDisplay(win->display);
    #elif defined(_WIN32)

    #endif
}

#ifdef __cplusplus
}
#endif