#ifndef _WEBGBA_EMULATOR_PPU_GBA_SET_
#define _WEBGBA_EMULATOR_PPU_GBA_SET_

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint64_t objects[2];
} GbaPpuSet;

static inline void GbaPpuSetAdd(GbaPpuSet* object_set, uint_fast8_t value) {
  assert(value < 128u);
  uint_fast8_t set_index = value >> 6u;
  object_set->objects[set_index] |= 1ull << (value & 0x3Fu);
}

static inline void GbaPpuSetRemove(GbaPpuSet* object_set, uint_fast8_t value) {
  assert(value < 128u);
  uint_fast8_t set_index = value >> 6u;
  uint64_t mask = 1ull << (value & 0x3Fu);
  object_set->objects[set_index] &= ~mask;
}

static inline void GbaPpuSetClear(GbaPpuSet* object_set) {
  object_set->objects[0u] = 0u;
  object_set->objects[1u] = 0u;
}

static inline bool GbaPpuSetEmpty(const GbaPpuSet* object_set) {
  return !(object_set->objects[0u] | object_set->objects[1u]);
}

static inline uint_fast8_t GbaPpuSetPop(GbaPpuSet* object_set) {
  assert(!GbaPpuSetEmpty(object_set));
  static const int base[2u] = {0, 64};
  bool set_index = !object_set->objects[0u];
  int set_bit_index = __builtin_ctzll(object_set->objects[set_index]);
  object_set->objects[set_index] ^= 1ull << set_bit_index;
  return base[set_index] + set_bit_index;
}

static inline GbaPpuSet GbaPpuSetIntersection(const GbaPpuSet* object_set0,
                                              const GbaPpuSet* object_set1) {
  GbaPpuSet result;
  result.objects[0] = object_set0->objects[0] & object_set1->objects[0];
  result.objects[1] = object_set0->objects[1] & object_set1->objects[1];
  return result;
}

#endif  // _WEBGBA_EMULATOR_PPU_GBA_SET_