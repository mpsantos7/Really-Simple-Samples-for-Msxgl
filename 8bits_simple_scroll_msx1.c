// Simplified Multi-Directional Scrolling Sample for MSX using MSXGL
// It is basically a stripped version of the s_scroll sample from Aineko.
// You need make a content folder for tiles like the original sample.
//
//
#include "msxgl.h"
#include "scroll.h"

// Fonts (for basic text)
#include "font/font_mgl_sample8.h"
// Tiles/Map
#include "content/tile/data_tile_gm2.h"
#include "content/tile/data_map_gm2.h"

i8 g_ScrollSpeed = 1;  // Fixed speed for simplicity
u8 g_VBlank = 0;

// VBlank hook
void VBlankHook() { g_VBlank = 1; }

// Wait for VBlank
void WaitVBlank() {
    while (g_VBlank == 0) {}
    g_VBlank = 0;
}

void main() {
    // Init video (assume MSX2+)
    VDP_SetMode(VDP_MODE_GRAPHIC2);
    VDP_SetLayoutTable(0x3800);
    VDP_SetColorTable(0x2000);
    VDP_SetPatternTable(0x0000);
    VDP_SetColor(0xF0);
    VDP_ClearVRAM();
    VDP_EnableVBlank(TRUE);
    Bios_SetHookCallback(H_TIMI, VBlankHook);

    // Init text (minimal)
    Print_SetTextFont(g_Font_MGL_Sample8, 96);
    VDP_FillLayout_GM2(96, 0, 0, 32, 24);
    Print_DrawText("SIMPLE SCROLL");

    // Load tiles
    VDP_LoadPattern_GM2(g_DataMapGM2_Patterns, 94, 0);
    VDP_LoadColor_GM2(g_DataMapGM2_Colors, 94, 0);

    // Init scroll
    Scroll_Initialize((u16)g_DataMapGM2_Names);
    Scroll_SetOffsetV(16);  // Initial offset

    u8 prevRow8 = 0xFF;
    while (1) {
        WaitVBlank();
        Scroll_Update();

        u8 row8 = Keyboard_Read(8);

        // Horizontal scroll
        if (IS_KEY_PRESSED(row8, KEY_RIGHT)) {
            Scroll_SetOffsetH(g_ScrollSpeed);
        } else if (IS_KEY_PRESSED(row8, KEY_LEFT)) {
            Scroll_SetOffsetH(-g_ScrollSpeed);
        }

        // Vertical scroll
        if (IS_KEY_PRESSED(row8, KEY_DOWN)) {
            Scroll_SetOffsetV(g_ScrollSpeed);
        } else if (IS_KEY_PRESSED(row8, KEY_UP)) {
            Scroll_SetOffsetV(-g_ScrollSpeed);
        }

        prevRow8 = row8;
    }
}
