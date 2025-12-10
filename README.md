# Really simple C samples made with the MSXGL Library for MSX computers.

mcu_3sprites.c and mcu_3sprites60degrees.c animate sprites, the former with two sprites at 180 degrees and the latter with three sprites at 60 degrees. 
Both use trigonometric tables with 31 values.

fillVRAMSC1_example2.c applies VDP_FillVRAM_16K function

directPSGaccess.c makes a sound when pressing the space bar

Akg_music_play.c plays an akg song defined in teste1pos.h. It is for 32k rom.

sprites_collision.c test hardware sprite collision.

writesc5.c writes messages to screen5 and allows you to enter your name and receive a "Como vai .. ?"  (How are you, ____?). 
Compile as MSX 2. 

Screen5BigTitle.c write a message with a big font in screen 5.

The scrolls sample needs to copy  the folder content/tile to the project.

tile_dungeon.c is more extense, however it is based on the example tdungeon.bas from the book wrote by Raul Portales with a simplified collision detection.
