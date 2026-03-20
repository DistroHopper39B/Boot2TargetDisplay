/*
 * Copyright (C) 2025 Sylas Hollander.
 * PURPOSE: Header for general purpose video functions for linear framebuffers
 * SPDX-License-Identifier: MIT
*/

#pragma once

typedef __SIZE_TYPE__ size_t;

typedef struct
{
    BOOLEAN   enabled;

    UINT64    base;
    UINT64    size;

    UINT32    pitch;
    UINT32    width;
    UINT32    height;
    UINT32    depth;
    UINT32    pixels_per_row;

    UINT8     red_size;
    UINT8     red_shift;
    UINT8     green_size;
    UINT8     green_shift;
    UINT8     blue_size;
    UINT8     blue_shift;
    UINT8     reserved_size;
    UINT8     reserved_shift;
} LINEAR_FB;

typedef struct
{
    UINT32    cursor_x;
    UINT32    cursor_y;
    UINT32    width;
    UINT32    height;
    UINT32    size;
    UINT32    fg_color;
    UINT32    bg_color;
} CONS_PRIV;

#define RGBA_TO_NATIVE(fb, color) \
((((color >> 24) & 0xFF) << fb->red_shift) | \
(((color >> 16) & 0xFF) << fb->green_shift) | \
(((color >> 8) & 0xFF) << fb->blue_shift) | \
(((color) & 0xFF) << fb->reserved_shift))

#define COLOR_RED       0x00FF0000
#define COLOR_YELLOW    0x00FFFF00
#define COLOR_GREEN     0x0000FF00
#define COLOR_AQUA      0x0000FFFF
#define COLOR_BLUE      0x000000FF
#define COLOR_BLACK     0x00000000
#define COLOR_WHITE     0xFFFFFFFF

/* Functions */
BOOLEAN ConsClearScreen(UINT32 color);
BOOLEAN ConsChangeFgColor(UINT32 fg_color);
BOOLEAN ConsChangeBgColor(UINT32 bg_color);