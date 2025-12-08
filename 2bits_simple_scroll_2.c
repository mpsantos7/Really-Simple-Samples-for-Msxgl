// ____________________________
// ██▀▀█▀▀██▀▀▀▀▀▀▀█▀▀█ │ ▄▄▄ ▄▄
// ██ ▀ █▄ ▀██▄ ▀ ▄█ ▄▀▀ █ │ ▀█▄ ▄▀██ ▄█▄█ ██▀▄ ██ ▄███
// █ █ █ ▀▀ ▄█ █ █ ▀▄█ █▄ │ ▄▄█▀ ▀▄██ ██ █ ██▀ ▀█▄ ▀█▄▄
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀────────┘ ▀▀
// Simplified scrolling sample for MSX1 with clamping
//
// Changes:
// - Use clamping instead of wrapping to prevent going beyond map boundaries.
// - This maintains scene continuity by stopping at the map edges instead of jumping back.
// - Map width from documentation: 128 tiles (example in MSXGL scroll module docs).
// - Max horizontal offset: (128 - 32) * 8 = 768 pixels.
// - No visual jump; scrolling stops at ends, toggle direction to go back.
// - Prevents screen corruption without losing continuity.
//─────────────────────────────────────────────────────────────────────────────
#include "msxgl.h"
#include "scroll.h"
//=============================================================================
// DEFINES
//=============================================================================
// Library's logo
#define MSX_GL "\x02\x03\x04\x05"
// Map width in tiles (from MSXGL scroll sample docs)
#define MAP_WIDTH_TILES 128
// Max offset in pixels: (MAP_WIDTH_TILES - screen width) * 8
#define MAX_OFFSET_PIXELS ((MAP_WIDTH_TILES - 32) * 8)
//=============================================================================
// READ-ONLY DATA
//=============================================================================
// Fonts
#include "font/font_mgl_sample8.h"
// Tiles (simple map data)
#include "content/tile/data_tile_gm2.h"
#include "content/tile/data_map_gm2.h"  // Keep a simple map
//=============================================================================
// MEMORY DATA
//=============================================================================
// Scrolling speed (fixed at 2 pixels for fine scroll)
i8 g_ScrollSpeed = 2;
// Direction: positive = right, negative = left
i8 g_ScrollDir = 1;
// V-blank synchronization flag
u8 g_VBlank = 0;
// External from scroll.h
extern u16 g_Scroll_OffsetX;
//=============================================================================
// HELPER FUNCTIONS
//=============================================================================
// H_TIMI interrupt hook
void VBlankHook()
{
    g_VBlank = 1;
}
//-----------------------------------------------------------------------------
// Wait for V-Blank period
void WaitVBlank()
{
    while (g_VBlank == 0) {}
    g_VBlank = 0;
}
//=============================================================================
// MAIN LOOP
//=============================================================================
//-----------------------------------------------------------------------------
// Program entry point
void main()
{
    // Initialize video for MSX1: Graphic Mode 2
    VDP_SetMode(VDP_MODE_GRAPHIC2);

    // VRAM Tables Address (standard for G2)
    VDP_SetLayoutTable(0x3800);
    VDP_SetColorTable(0x2000);
    VDP_SetPatternTable(0x0000);

    // Setup video
    VDP_SetColor(0xF0);  // White on black
    VDP_ClearVRAM();
    VDP_EnableVBlank(TRUE);

    Bios_SetHookCallback(H_TIMI, VBlankHook);

    // Initialize text (simple font)
    Print_SetTextFont(g_Font_MGL_Sample8, 96);
    VDP_FillLayout_GM2(96, 0, 0, 32, 24);  // Fill screen with blank tile (char 96)

    // Simple header
    Print_DrawText(MSX_GL " SIMPLE SCROLL MSX1");
    Print_DrawLineH(0, 1, 32);  // Horizontal line

    // Footer with info
    Print_DrawLineH(0, 22, 32);
    Print_SetPosition(0, 23);
    Print_DrawFormat("Space: Toggle dir. Speed: %i px/frame", g_ScrollSpeed);

    // Load simple tiles (just a few for demo)
    VDP_LoadPattern_GM2(g_DataMapGM2_Patterns, 94, 0);  // Load 94 patterns
    VDP_LoadColor_GM2(g_DataMapGM2_Colors, 94, 0);

    // Initialize scroll module with map
    Scroll_Initialize((u16)g_DataMapGM2_Names);
    Scroll_SetOffsetV(0);  // No vertical scroll

    u8 row8 = 0;
    u8 prevRow8 = 0xFF;

    while (1)
    {
        // Wait for v-synch
        WaitVBlank();

        // Simple input: toggle direction on space press
        row8 = Keyboard_Read(8);
        if (IS_KEY_PRESSED(row8, KEY_SPACE) && !IS_KEY_PRESSED(prevRow8, KEY_SPACE))
        {
            g_ScrollDir = -g_ScrollDir;  // Toggle left/right
        }
        prevRow8 = row8;

        // Calculate delta
        i16 delta = g_ScrollSpeed * g_ScrollDir;

        // Compute new offset with clamping
        i16 new_offset = (i16)g_Scroll_OffsetX + delta;
        if (new_offset < 0)
            new_offset = 0;
        if (new_offset > MAX_OFFSET_PIXELS)
            new_offset = MAX_OFFSET_PIXELS;

        // Apply the clamped offset
        g_Scroll_OffsetX = (u16)new_offset;

        // Update scrolling rendering
        Scroll_Update();
    }
}
