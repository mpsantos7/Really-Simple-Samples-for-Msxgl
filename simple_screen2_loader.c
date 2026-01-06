// ____________________________
// ██▀▀█▀▀██▀▀▀▀▀▀▀█▀▀█        │   ▄▄▄                ▄▄      
// ██  ▀  █▄  ▀██▄ ▀ ▄█ ▄▀▀ █  │  ▀█▄  ▄▀██ ▄█▄█ ██▀▄ ██  ▄███
// █  █ █  ▀▀  ▄█  █  █ ▀▄█ █▄ │  ▄▄█▀ ▀▄██ ██ █ ██▀  ▀█▄ ▀█▄▄
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀────────┘                 ▀▀
//  Screen 2 Loader Sample (MSX1)
//─────────────────────────────────────────────────────────────────────────────

//=============================================================================
// INCLUDES
//=============================================================================
#include "msxgl.h"
#include "skull_p.h"  // extern const u8 skull_pat[]; skull_S2[]; skull_pal[];

//=============================================================================
// DEFINES
//=============================================================================

// Tamanhos baseados no export; ajuste se necessário (use sizeof no código se possível)
#define PATTERN_SIZE 6144  // Para patterns e palete (color table), assumindo full bitmap
#define NAME_SIZE 768      // Para tabela de nomes (skull_S2)

//=============================================================================
// FUNCTIONS
//=============================================================================

//-----------------------------------------------------------------------------
// Carregar Screen 2 com os novos arrays
void LoadScreen2()
{
    VDP_SetMode(VDP_MODE_GRAPHIC2);

    //VDP_FillVRAM_16K(0, 0x0000, 0x4000);  // Limpa VRAM

    // Carrega patterns (skull_pat)
    VDP_WriteVRAM_16K(skull_pat, 0x0000, PATTERN_SIZE);
    VDP_WriteVRAM_16K(skull_S2, 0x1800, NAME_SIZE);
    VDP_WriteVRAM_16K(skull_pal, 0x2000, PATTERN_SIZE);
   
    // Carrega name table (skull_S2 - tabela de nomes)
  
}

//=============================================================================
// MAIN LOOP
//=============================================================================

//-----------------------------------------------------------------------------
void main()
{
    LoadScreen2();

    while (!Keyboard_IsKeyPressed(KEY_ESC))
    {
        Halt();
    }

    Bios_Exit(0);
}
