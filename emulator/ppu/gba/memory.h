#ifndef _WEBGBA_EMULATOR_PPU_GBA_MEMORY_
#define _WEBGBA_EMULATOR_PPU_GBA_MEMORY_

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#define GBA_LARGE_PALETTE_SIZE 256u
#define GBA_NUM_SMALL_PALETTES 16u
#define GBA_SMALL_PALETTE_SIZE 16u

typedef union {
  uint16_t large_palette[GBA_LARGE_PALETTE_SIZE];
  uint16_t small_palettes[GBA_NUM_SMALL_PALETTES][GBA_SMALL_PALETTE_SIZE];
} GbaPpuPaletteSegment;

#define PALETTE_SIZE 1024u

typedef union {
  struct {
    GbaPpuPaletteSegment bg;
    GbaPpuPaletteSegment obj;
  };
  uint32_t words[PALETTE_SIZE >> 2u];
  uint16_t half_words[PALETTE_SIZE >> 1u];
  uint8_t bytes[PALETTE_SIZE];
} GbaPpuPaletteMemory;

static_assert(sizeof(GbaPpuPaletteMemory) == PALETTE_SIZE,
              "sizeof(GbaPpuPaletteMemory) != PALETTE_SIZE");

typedef union {
  struct {
    unsigned char first : 4;
    unsigned char second : 4;
  };
  uint8_t value;
} STilePixelPair;

#define GBA_TILE_1D_SIZE 8u

typedef struct {
  STilePixelPair pixels[GBA_TILE_1D_SIZE][GBA_TILE_1D_SIZE >> 1u];
} STile;

typedef struct {
  uint8_t pixels[GBA_TILE_1D_SIZE][GBA_TILE_1D_SIZE];
} DTile;

#define GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES 512u
#define GBA_TILE_MODE_TILE_BLOCK_NUM_D_TILES 256u

typedef union {
  STile s_tiles[GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES];
  DTile d_tiles[GBA_TILE_MODE_TILE_BLOCK_NUM_D_TILES];
} TileBlock;

#define GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS 4u

typedef struct {
  TileBlock blocks[GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS];
} BackgroundTiles;

typedef struct {
  unsigned short index : 10;
  bool h_flip : 1;
  bool v_flip : 1;
  unsigned char palette : 4;
} TileMapEntry;

#define GBA_TILE_MAP_BLOCK_1D_SIZE 32u
#define GBA_TILE_MAP_BLOCK_1D_SIZE_PIXELS \
  (GBA_TILE_MAP_BLOCK_1D_SIZE * GBA_TILE_1D_SIZE)
#define GBA_AFFINE_TILE_MAP_INDICES_PER_BLOCK 2048u

typedef union {
  TileMapEntry entries[GBA_TILE_MAP_BLOCK_1D_SIZE][GBA_TILE_MAP_BLOCK_1D_SIZE];
  uint8_t indices[GBA_AFFINE_TILE_MAP_INDICES_PER_BLOCK];
} TileMapBlock;

#define GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS 32u

typedef struct {
  TileMapBlock blocks[GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS];
} TileMap;

typedef union {
  BackgroundTiles tiles;
  TileMap tile_map;
} TileModeBackgroundMemory;

#define GBA_TILE_MODE_NUM_OBJECT_S_TILES 1024u
#define GBA_TILE_MODE_NUM_OBJECT_D_TILES 512u

typedef union {
  STile s_tiles[GBA_TILE_MODE_NUM_OBJECT_S_TILES];
  DTile d_tiles[GBA_TILE_MODE_NUM_OBJECT_D_TILES];
} TileModeObjectMemory;

typedef struct {
  TileModeBackgroundMemory bg;
  TileModeObjectMemory obj;
} TileModeMemory;

#define GBA_FULL_FRAME_WIDTH 240
#define GBA_FULL_FRAME_HEIGHT 160

typedef struct {
  uint16_t pixels[GBA_FULL_FRAME_HEIGHT][GBA_FULL_FRAME_WIDTH];
  uint8_t unused[5120u];
} BitmapMode3BackgroundMemory;

#define GBA_BITMAP_MODE_NUM_OBJECT_S_TILES 512u
#define GBA_BITMAP_MODE_NUM_OBJECT_D_TILES 256u

typedef union {
  STile s_tiles[GBA_BITMAP_MODE_NUM_OBJECT_S_TILES];
  DTile d_tiles[GBA_BITMAP_MODE_NUM_OBJECT_D_TILES];
} BitmapModeObjectMemory;

typedef struct {
  BitmapMode3BackgroundMemory bg;
  BitmapModeObjectMemory obj;
} BitmapMode3Memory;

typedef struct {
  uint8_t pixels[GBA_FULL_FRAME_HEIGHT][GBA_FULL_FRAME_WIDTH];
  uint8_t unused[2560u];
} BitmapMode4BackgroundPage;

typedef struct {
  BitmapMode4BackgroundPage pages[2u];
} BitmapMode4BackgroundMemory;

typedef struct {
  BitmapMode4BackgroundMemory bg;
  BitmapModeObjectMemory obj;
} BitmapMode4Memory;

#define GBA_REDUCED_FRAME_WIDTH 160
#define GBA_REDUCED_FRAME_HEIGHT 128

typedef struct {
  uint16_t pixels[GBA_REDUCED_FRAME_HEIGHT][GBA_REDUCED_FRAME_WIDTH];
} BitmapMode5BackgroundPage;

typedef struct {
  BitmapMode5BackgroundPage pages[2u];
} BitmapMode5BackgroundMemory;

typedef struct {
  BitmapMode5BackgroundMemory bg;
  BitmapModeObjectMemory obj;
} BitmapMode5Memory;

#define VRAM_SIZE (96u * 1024u)

typedef union {
  TileModeMemory mode_012;
  BitmapMode3Memory mode_3;
  BitmapMode4Memory mode_4;
  BitmapMode5Memory mode_5;
  uint32_t words[VRAM_SIZE >> 2u];
  uint16_t half_words[VRAM_SIZE >> 1u];
  uint8_t bytes[VRAM_SIZE];
} GbaPpuVideoMemory;

static_assert(sizeof(GbaPpuVideoMemory) == VRAM_SIZE,
              "sizeof(GbaPpuVideoMemory) != VRAM_SIZE");

typedef struct {
  union {
    signed char y_coordinate;
    unsigned char y_coordinate_u;
  };
  bool affine : 1u;
  bool flex_param_0 : 1u;  // Double Size or Object Disable
  unsigned char obj_mode : 2u;
  bool obj_mosaic : 1u;
  bool palette_mode : 1u;
  unsigned char obj_shape : 2u;
  short x_coordinate : 9u;
  unsigned char flex_param_1 : 5u;  // Rotate/Scale Select or Horiz/Vert Flip
  unsigned char obj_size : 2u;
  unsigned short character_name : 10u;
  unsigned char priority : 2u;
  unsigned char palette : 4u;
  unsigned char unused;
} ObjectAttribute;

typedef struct {
  uint16_t unused0[3u];
  int16_t pa;
  uint16_t unused1[3u];
  int16_t pb;
  uint16_t unused2[3u];
  int16_t pc;
  uint16_t unused3[3u];
  int16_t pd;
} ObjectRotateScaleParameter;

#define OAM_NUM_OBJECTS 128u
#define OAM_NUM_ROTATE_SCALE_GROUPS 32u
#define OAM_SIZE 1024u

typedef union {
  ObjectAttribute object_attributes[OAM_NUM_OBJECTS];
  ObjectRotateScaleParameter rotate_scale[OAM_NUM_ROTATE_SCALE_GROUPS];
  uint32_t words[OAM_SIZE >> 2u];
  uint16_t half_words[OAM_SIZE >> 1u];
  uint8_t bytes[OAM_SIZE];
} GbaPpuObjectAttributeMemory;

static_assert(sizeof(GbaPpuObjectAttributeMemory) == OAM_SIZE,
              "sizeof(GbaPpuObjectAttributeMemory) != OAM_SIZE");

typedef struct {
  GbaPpuPaletteMemory palette;
  GbaPpuVideoMemory vram;
  GbaPpuObjectAttributeMemory oam;
} GbaPpuMemory;

#endif  // _WEBGBA_EMULATOR_PPU_GBA_MEMORY_