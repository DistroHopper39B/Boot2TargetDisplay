/*
 * Copyright (C) 2025 Sylas Hollander.
 * PURPOSE: General purpose video functions for linear framebuffers
 * SPDX-License-Identifier: MIT
 */

#include <boot2target.h>
#include <cons.h>
#include <font.h>
#include <tinyprintf.h>

EFI_GUID gEfiGraphicsOutputProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

void *memcpy(void *, const void *, size_t);
void *memset(void *dst, int c, size_t n);

LINEAR_FB           fb;
static CONS_PRIV   con;

static
void VideoPrintChar(char c, UINT32 x, UINT32 y, UINT32 fg_color, UINT32 bg_color)
{
    UINT32    char_position = c * ISO_CHAR_HEIGHT;

    // Set up pixel location
    UINT32 *pixel = (UINT32 *) ((char *) fb.base + (y * ISO_CHAR_HEIGHT) * fb.pitch + (x * ISO_CHAR_WIDTH * 4));

    // Print character to screen
    for (UINT8 line = 0; line < ISO_CHAR_HEIGHT; line++)
    {
        UINT8 char_line = iso_font[char_position];
        for (UINT8 column = 0; column < ISO_CHAR_WIDTH; column++)
        {
            UINT8 mask = char_line >> column;
            pixel[column] = ((mask & 1) ? fg_color : bg_color);
        }
        char_position++;
        pixel = (UINT32 *) ((char *) pixel + fb.pitch);
    }
}

static
void ConsPrintChar(void *p, char c)
{
    // CASE 1: Video not enabled yet.
    // Return silently on this case.
    if (!fb.enabled)
        return;

    // CASE 2: Text wrapped around.
    // Add a newline in this case.
    if (con.cursor_x >= con.width)
    {
        con.cursor_x = 0;
        con.cursor_y++;
    }

    // CASE 3: Screen is full.
    // Start over.
    if (con.cursor_y >= con.height)
    {
        ConsClearScreen(con.bg_color);
    }

    // Now we print the character.
    switch (c)
    {
        // CASE 1: newline.
        case '\r': // CR
        case '\n': // LF
        {
            con.cursor_x = 0;
            con.cursor_y++;
            break;
        }
        // CASE 2: backspace.
        case '\b':
        {
            con.cursor_x--;
            VideoPrintChar(c, con.cursor_x, con.cursor_y, con.fg_color, con.bg_color);
            break;
        }
        // CASE 3: any other character.
        default:
        {
            VideoPrintChar(c, con.cursor_x, con.cursor_y, con.fg_color, con.bg_color);
            con.cursor_x++;
            break;
        }
    }
}

BOOLEAN ConsClearScreen(UINT32 color)
{
    if (!fb.enabled)
        return FALSE;

    UINT64 Color64 = (UINT64) color << 32 | color;

    for (UINT32 line = 0; line < fb.height; line++)
    {
        UINT64 *screen = (UINT64 *) ((char *) fb.base + line * fb.pitch);
        for (UINT32 column = 0; column < fb.width / 2; column++)
        {
            *screen++ = Color64;
        }
    }

    con.cursor_x = 0;
    con.cursor_y = 0;

    return TRUE;
}

BOOLEAN ConsChangeFgColor(UINT32 fg_color)
{
    if (!fb.enabled)
        return FALSE;

    con.fg_color        = fg_color;

    return TRUE;
}

BOOLEAN ConsChangeBgColor(UINT32 bg_color)
{
    if (!fb.enabled)
        return FALSE;

    con.bg_color        = bg_color;

    return TRUE;
}

// Platform specific video initialization code.
BOOLEAN
InitializeConsole(VOID)
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL            *Gop;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION    *GopInfo;
    EFI_STATUS                              Status;
    UINTN                                   SizeOfInfo;
    UINT32                                  BackgroundColor;

    Status = gBS->LocateProtocol(
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        (VOID **) &Gop
        );

    if (EFI_ERROR(Status))
    {
        return FALSE;
    }

    Status = Gop->QueryMode(
        Gop,
        Gop->Mode == NULL ? 0 : Gop->Mode->Mode,
        &SizeOfInfo,
        &GopInfo);

    if (EFI_ERROR(Status))
    {
        return FALSE;
    }

    // Assume first pixel's color is the background color.
    BackgroundColor = ((UINT32 *) Gop->Mode->FrameBufferBase)[0];
    
    memset(&fb, 0, sizeof(fb));
    fb.base            = Gop->Mode->FrameBufferBase;
    fb.size            = Gop->Mode->FrameBufferSize;

    fb.pitch           = GopInfo->PixelsPerScanLine * 4;
    fb.width           = GopInfo->HorizontalResolution;
    fb.height          = GopInfo->VerticalResolution;
    fb.depth           = 32;
    fb.pixels_per_row  = GopInfo->PixelsPerScanLine;

    fb.red_size        = 8;
    fb.red_shift       = 16;
    fb.green_size      = 8;
    fb.green_shift     = 8;
    fb.blue_size       = 8;
    fb.blue_shift      = 0;
    fb.reserved_size   = 8;
    fb.reserved_shift  = 0;

    // set up text console
    memset(&con, 0, sizeof(con));
    con.cursor_x        = 0;
    con.cursor_y        = 0;
    con.width           = fb.width / ISO_CHAR_WIDTH;
    con.height          = fb.height / ISO_CHAR_HEIGHT;
    con.size            = con.width * con.height;

    // change FG/BG color according to the background color
    con.fg_color        = (BackgroundColor != COLOR_BLACK) ? COLOR_BLACK : COLOR_WHITE;
    con.bg_color        = BackgroundColor;

    init_printf(NULL, ConsPrintChar);

    fb.enabled = TRUE;

    ConsClearScreen(BackgroundColor);

    return TRUE;
}