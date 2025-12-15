// Plataforma MSX1 - Conversão BASIC → C + MSXGL (Versão Corrigida Final v3)
// MSX1
// Original in Basic by Raul Portales
// This version features tile collision detection that is more faithful to the original in BASIC.
// on the book "Modern MSX BASIC Game Development"
//─────────────────────────────────────────────────────────────────────────────
#include "msxgl.h"
#include "font/font_mgl_sample8.h"
//=============================================================================
// DEFINES
//=============================================================================
#define TILE_EMPTY 0
#define TILE_BRICK 1
#define TILE_LAVA 2
#define TILE_DOOR 3
// Endereços VRAM (SCREEN 2 padrão)
#define LAYOUT_ADDR 0x1800 // Name table (mapa)
#define SPRITE_ATTR 0x1B00 // Atributos de sprites
#define SPRITE_PAT 0x3800 // Padrões de sprites
#define PATTERN_TABLE0 0 // Padrões de tiles
#define PATTERN_TABLE1 2048 // Padrões de tiles
#define PATTERN_TABLE2 4096 // Padrões de tiles
#define COLOR_TABLE0 0x2000 // Cores de tiles
#define COLOR_TABLE1 0x2800 // Cores de tiles
#define COLOR_TABLE2 0x3000 // Cores de tiles
// Limites da tela MSX1 (256x192 pixels)
#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192
#define SPRITE_SIZE 8

//=============================================================================
// READ-ONLY DATA
//=============================================================================
// Padrões de tiles (4 tiles x 8 bytes = 32 bytes) - simples, assumidos (não no BASIC original)
const u8 g_TilePatterns[32] = {
    // Tile 0: Empty (vazio)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // Tile 1: brick
    0x00, 0x7F, 0x7F, 0x7F, 0x00, 0xF7, 0xF7, 0xF7,
    // Tile 2: lava
    0xFD, 0xEE, 0xDF, 0x33, 0xFD, 0xEE, 0xDF, 0x33,
    // Tile 3: Door (porta - retângulo com linhas)
    0x3C, 0x7F, 0xDB, 0xFF, 0xFF, 0xDB, 0x7E, 0x3C
};
// Cores de tiles (4 tiles x 8 bytes = 32 bytes) - fg/bg (MSX colors: 1=black, 15=white, 11=lt yellow, 9=lt red)
const u8 g_TileColors[32] = {
    // Tile 0: Empty (transparente/black)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // preto background
    // Tile 1: Brick
    0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, // cinza preto
    // Tile 2: Lava
    0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, // red
    // Tile 3: Door
    0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0 // 9=lt red, 1=black
};
// Sprite pattern 8x8
const u8 g_SpritePattern[8] = {
    0x18,0x18,0x3C,0x5A,0x58,0x14,0x14,0x14
};
// 0x18,0x18,0x3C,0x5A,0x58,0x14,0x14,0x14  <<-- ball sprite original.

//=============================================================================
// MEMORY DATA
//=============================================================================
u8 g_PlayerX = 8; // player control
u8 g_PlayerY = 8; 
u8 newX=0, newY=0; 
bool bContinue = TRUE;
// Mapa (768 bytes: 24 linhas x 32 colunas) - exato do DATA 1200-1350 (u8 para modificação)
u8 g_Map[768] = {
    // Linha 0
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Linha 1
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Linha 2
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Linha 3
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Linha 4
    1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Linha 5
    1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Linha 6
    1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Linha 7
    1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Linha 8
    1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Linha 9
    1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Linha 10
    1,0,0,2,2,2,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Linha 11
    1,0,0,2,2,2,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Linha 12
    1,0,0,2,2,2,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Linha 13
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Linha 14
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Linhas 15-23: vazias (0) - 9 linhas x 32 zeros
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
//=============================================================================
// FUNCTIONS
//=============================================================================
//-----------------------------------------------------------------------------
// Inicializa a tela e carrega dados
void Initialize()
{
    // Configura modo gráfico 2 (SCREEN 2)
    VDP_SetMode(VDP_MODE_GRAPHIC2);
    VDP_SetColor(0x10); // Borda preta
    VDP_ClearVRAM(); // Limpa VRAM (opcional)
	// fonts
	Print_SetTextFont(g_Font_MGL_Sample8, 4);
    // Carrega padrões de tiles
    VDP_WriteVRAM_16K(g_TilePatterns, PATTERN_TABLE0, 32);
	VDP_WriteVRAM_16K(g_TilePatterns, PATTERN_TABLE1, 32);
	VDP_WriteVRAM_16K(g_TilePatterns, PATTERN_TABLE2, 32);
    VDP_WriteVRAM_16K(g_TileColors, COLOR_TABLE0, 32);
	VDP_WriteVRAM_16K(g_TileColors, COLOR_TABLE1, 32);
	VDP_WriteVRAM_16K(g_TileColors, COLOR_TABLE2, 32);
    // Carrega mapa (name table)
    VDP_WriteVRAM_16K(g_Map, LAYOUT_ADDR, 768);
	 // Configura sprites (8x8, sem zoom) - habilita implicitamente ao setar flag
 
    // Carrega padrões de sprites
    VDP_SetSpriteFlag(VDP_SPRITE_SIZE_8);
    VDP_WriteVRAM_16K(g_SpritePattern, SPRITE_PAT, 8);
	//sprite init
	VDP_SetSpriteColorSM1(0,COLOR_LIGHT_YELLOW);
	VDP_SetSpritePositionX(0,g_PlayerX);
	VDP_SetSpritePositionY(0,g_PlayerY);

}

// helper functions
void handle_tiles(u8 tile1, u8 tile2, u8 newX, u8 newY)
{
    // Permite movimento só se AMBOS os pontos estiverem vazios
    if (tile1 == TILE_EMPTY && tile2 == TILE_EMPTY)
    {
        g_PlayerX = newX;
        g_PlayerY = newY;
    }

    // Verifica lava ou porta (prioridade: lava ganha se os dois tiverem)
    if (tile1 == TILE_LAVA || tile2 == TILE_LAVA)
    {
        g_PlayerX = newX;           // entra na lava (opcional)
        g_PlayerY = newY;
        Print_SetPosition(18, 2);
        Print_DrawText("Game Over!");
        bContinue = FALSE;
    }
    else if (tile1 == TILE_DOOR || tile2 == TILE_DOOR)
    {
        g_PlayerX = newX;
        g_PlayerY = newY;
        Print_SetPosition(18, 3);
        Print_DrawText("You Win!");
        bContinue = FALSE;
    }
}

void main()
{
    Initialize();

    while (bContinue)
    {
        Halt();

        u8 tile1, tile2;

        if (Keyboard_IsKeyPressed(KEY_RIGHT))
        {
            tile1 = g_Map[ (g_PlayerY / 8) * 32 + ((g_PlayerX + 7) / 8) ];
            tile2 = g_Map[ ((g_PlayerY + 6) / 8) * 32 + ((g_PlayerX + 7) / 8) ];
            handle_tiles(tile1, tile2, g_PlayerX + 1, g_PlayerY);
        }
        else if (Keyboard_IsKeyPressed(KEY_LEFT))
        {
            tile1 = g_Map[ (g_PlayerY / 8) * 32 + ((g_PlayerX - 1) / 8) ];
            tile2 = g_Map[ ((g_PlayerY + 6) / 8) * 32 + ((g_PlayerX - 1) / 8) ];
            handle_tiles(tile1, tile2, g_PlayerX - 1, g_PlayerY);
        }
        else if (Keyboard_IsKeyPressed(KEY_UP))
        {
            tile1 = g_Map[ ((g_PlayerY - 1) / 8) * 32 + (g_PlayerX / 8) ];
            tile2 = g_Map[ ((g_PlayerY - 1) / 8) * 32 + ((g_PlayerX + 6) / 8) ];
            handle_tiles(tile1, tile2, g_PlayerX, g_PlayerY - 1);
        }
        else if (Keyboard_IsKeyPressed(KEY_DOWN))
        {
            tile1 = g_Map[ ((g_PlayerY + 7) / 8) * 32 + (g_PlayerX / 8) ];
            tile2 = g_Map[ ((g_PlayerY + 7) / 8) * 32 + ((g_PlayerX + 6) / 8) ];
            handle_tiles(tile1, tile2, g_PlayerX, g_PlayerY + 1);
        }

        VDP_SetSpritePositionX(0, g_PlayerX);
        VDP_SetSpritePositionY(0, g_PlayerY);

        if (Keyboard_IsKeyPressed(KEY_ESC))
            bContinue = FALSE;
    }

    Bios_Exit(0);
}
