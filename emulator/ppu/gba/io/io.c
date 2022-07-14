#include "emulator/ppu/gba/io/io.h"

#include <assert.h>
#include <stdlib.h>

#define DISPCNT_OFFSET 0x00u
#define GREENSWP_OFFSET 0x02u
#define DISPSTAT_OFFSET 0x04u
#define VCOUNT_OFFSET 0x06u
#define BG0CNT_OFFSET 0x08u
#define BG1CNT_OFFSET 0x0Au
#define BG2CNT_OFFSET 0x0Cu
#define BG3CNT_OFFSET 0x0Eu
#define BG0HOFS_OFFSET 0x10u
#define BG0VOFS_OFFSET 0x12u
#define BG1HOFS_OFFSET 0x14u
#define BG1VOFS_OFFSET 0x16u
#define BG2HOFS_OFFSET 0x18u
#define BG2VOFS_OFFSET 0x1Au
#define BG3HOFS_OFFSET 0x1Cu
#define BG3VOFS_OFFSET 0x1Eu
#define BG2PA_OFFSET 0x20u
#define BG2PB_OFFSET 0x22u
#define BG2PC_OFFSET 0x24u
#define BG2PD_OFFSET 0x26u
#define BG2X_OFFSET 0x28u
#define BG2X_OFFSET_HI 0x2Au
#define BG2Y_OFFSET 0x2Cu
#define BG2Y_OFFSET_HI 0x2Eu
#define BG3PA_OFFSET 0x30u
#define BG3PB_OFFSET 0x32u
#define BG3PC_OFFSET 0x34u
#define BG3PD_OFFSET 0x36u
#define BG3X_OFFSET 0x38u
#define BG3X_OFFSET_HI 0x3Au
#define BG3Y_OFFSET 0x3Cu
#define BG3Y_OFFSET_HI 0x3Eu
#define WIN0H_OFFSET 0x40u
#define WIN1H_OFFSET 0x42u
#define WIN0V_OFFSET 0x44u
#define WIN1V_OFFSET 0x46u
#define WININ_OFFSET 0x48u
#define WINOUT_OFFSET 0x4Au
#define MOSAIC_OFFSET 0x4Cu
#define BLDCNT_OFFSET 0x50u
#define BLDALPHA_OFFSET 0x52u
#define BLDY_OFFSET 0x54u

typedef struct {
  GbaPpuRegisters *registers;
  GbaPpuIoDirtyBits *dirty;
  MemoryContextFree free_routine;
  void *free_address;
} GbaPpuIo;

static bool GbaPpuIoLoad16LE(const void *context, uint32_t address,
                             uint16_t *value) {
  assert((address & 0x1u) == 0u);

  const GbaPpuIo *io = (const GbaPpuIo *)context;

  switch (address) {
    case DISPCNT_OFFSET:
      *value = io->registers->dispcnt.value;
      return true;
    case GREENSWP_OFFSET:
      *value = io->registers->greenswp;
      return true;
    case DISPSTAT_OFFSET:
      *value = io->registers->dispstat.value;
      return true;
    case VCOUNT_OFFSET:
      *value = io->registers->vcount;
      return true;
    case BG0CNT_OFFSET:
      *value = io->registers->bgcnt[0u].value;
      return true;
    case BG1CNT_OFFSET:
      *value = io->registers->bgcnt[1u].value;
      return true;
    case BG2CNT_OFFSET:
      *value = io->registers->bgcnt[2u].value;
      return true;
    case BG3CNT_OFFSET:
      *value = io->registers->bgcnt[3u].value;
      return true;
    case WININ_OFFSET:
      *value = io->registers->winin.value;
      return true;
    case WINOUT_OFFSET:
      *value = io->registers->winout.value;
      return true;
    case BLDCNT_OFFSET:
      *value = io->registers->bldcnt.value;
      return true;
  }

  return false;
}

static bool GbaPpuIoLoad32LE(const void *context, uint32_t address,
                             uint32_t *value) {
  assert((address & 0x3u) == 0u);

  uint16_t low_bits;
  bool low = GbaPpuIoLoad16LE(context, address, &low_bits);
  if (!low) {
    return false;
  }

  uint16_t high_bits;
  bool high = GbaPpuIoLoad16LE(context, address + 2u, &high_bits);
  if (high) {
    *value = (((uint32_t)high_bits) << 16u) | (uint32_t)low_bits;
  } else {
    *value = low_bits;
  }

  return true;
}

static bool GbaPpuIoLoad8(const void *context, uint32_t address,
                          uint8_t *value) {
  uint32_t read_address = address & 0xFFFFFFFEu;

  uint16_t value16;
  bool success = GbaPpuIoLoad16LE(context, read_address, &value16);
  if (success) {
    *value = (address == read_address) ? value16 : value16 >> 8u;
  }

  return success;
}

static bool GbaPpuIoStore16LE(void *context, uint32_t address, uint16_t value) {
  assert((address & 0x1u) == 0u);

  if (address >= GBA_PPU_REGISTERS_SIZE) {
    return false;
  }

  GbaPpuIo *io = (GbaPpuIo *)context;

  // If address equals VCOUNT_OFFSET, we are attempting to write to a read-only
  // register. In this case, ignore the write and leave the register unmodified.
  if (address == VCOUNT_OFFSET) {
    return true;
  }

  // If address equals DISPSTAT_OFFSET, and any of the lower 3 bits of value are
  // set, we are attempting to modify read-only bits in the DISPSTAT register.
  // In this case, ignore mask out those bits in value so they are not modified.
  if (address == DISPSTAT_OFFSET) {
    value &= 0xFFF8u;
  }

  bool dirty = io->registers->half_words[address >> 1u] != value;
  io->registers->half_words[address >> 1u] = value;

  switch (address) {
    case DISPCNT_OFFSET:
      io->dirty->obj_mosaic |= dirty;  // This is a little hacky
      break;
    case BG2X_OFFSET:
    case BG2X_OFFSET_HI:
      io->registers->internal.affine[0u].row_start[0u] =
          io->registers->affine[0u].x;
      io->registers->internal.affine[0u].current[0u] =
          io->registers->affine[0u].x;
      break;
    case BG2Y_OFFSET:
    case BG2Y_OFFSET_HI:
      io->registers->internal.affine[0u].row_start[1u] =
          io->registers->affine[0u].y;
      io->registers->internal.affine[0u].current[1u] =
          io->registers->affine[0u].y;
      break;
    case BG3X_OFFSET:
    case BG3X_OFFSET_HI:
      io->registers->internal.affine[1u].row_start[0u] =
          io->registers->affine[1u].x;
      io->registers->internal.affine[1u].current[0u] =
          io->registers->affine[1u].x;
      break;
    case BG3Y_OFFSET:
    case BG3Y_OFFSET_HI:
      io->registers->internal.affine[1u].row_start[1u] =
          io->registers->affine[1u].y;
      io->registers->internal.affine[1u].current[1u] =
          io->registers->affine[1u].y;
      break;
    case MOSAIC_OFFSET:
      io->dirty->obj_mosaic |= dirty;
      break;
  }

  return true;
}

static bool GbaPpuIoStore32LE(void *context, uint32_t address, uint32_t value) {
  GbaPpuIoStore16LE(context, address, value);
  GbaPpuIoStore16LE(context, address + 2u, value >> 16u);
  return true;
}

static bool GbaPpuIoStore8(void *context, uint32_t address, uint8_t value) {
  GbaPpuIo *io = (GbaPpuIo *)context;

  uint32_t read_address = address & 0xFFFFFFFEu;
  uint16_t value16 = io->registers->half_words[read_address >> 1u];
  if (address == read_address) {
    value16 &= 0xFF00;
    value16 |= value;
  } else {
    value16 &= 0x00FF;
    value16 |= (uint16_t)value << 8u;
  }

  GbaPpuIoStore16LE(context, read_address, value16);

  return true;
}

void GbaPpuIoFree(void *context) {
  GbaPpuIo *io = (GbaPpuIo *)context;
  io->free_routine(io->free_address);
  free(io);
}

Memory *GbaPpuIoAllocate(GbaPpuRegisters *registers, GbaPpuIoDirtyBits *dirty,
                         MemoryContextFree free_routine, void *free_address) {
  GbaPpuIo *io = (GbaPpuIo *)malloc(sizeof(GbaPpuIo));
  if (io == NULL) {
    return NULL;
  }

  io->registers = registers;
  io->dirty = dirty;
  io->free_routine = free_routine;
  io->free_address = free_address;

  Memory *result = MemoryAllocate(
      io, GbaPpuIoLoad32LE, GbaPpuIoLoad16LE, GbaPpuIoLoad8, GbaPpuIoStore32LE,
      GbaPpuIoStore16LE, GbaPpuIoStore8, GbaPpuIoFree);
  if (result == NULL) {
    free(io);
    return NULL;
  }

  return result;
}