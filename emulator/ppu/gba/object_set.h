#ifndef _WEBGBA_EMULATOR_PPU_GBA_OBJECT_SET_
#define _WEBGBA_EMULATOR_PPU_GBA_OBJECT_SET_

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint64_t objects[2];
} GbaPpuObjectSet;

static inline void GbaPpuObjectSetAdd(GbaPpuObjectSet* object_set,
                                      uint_fast8_t index) {
  assert(index < 128u);
  uint_fast8_t set_index = index >> 6u;
  object_set->objects[set_index] |= 1ull << (index & 0x3Fu);
}

static inline void GbaPpuObjectSetRemove(GbaPpuObjectSet* object_set,
                                         uint_fast8_t index) {
  assert(index < 128u);
  uint_fast8_t set_index = index >> 6u;
  uint64_t mask = 1ull << (index & 0x3Fu);
  object_set->objects[set_index] &= ~mask;
}

static inline bool GbaPpuObjectSetEmpty(const GbaPpuObjectSet* object_set) {
  return !(object_set->objects[0u] | object_set->objects[1u]);
}

static inline uint_fast8_t GbaPpuObjectSetPop(GbaPpuObjectSet* object_set) {
  assert(!GbaPpuObjectSetEmpty(object_set));
  const static int base[2u] = {0, 64};
  bool set_index = !object_set->objects[0u];
  int set_bit_index = __builtin_ffsll(object_set->objects[set_index]) - 1;
  object_set->objects[set_index] ^= 1ull << set_bit_index;
  return base[set_index] + set_bit_index;
}

static inline GbaPpuObjectSet GbaPpuObjectSetIntersection(
    const GbaPpuObjectSet* object_set0, const GbaPpuObjectSet* object_set1) {
  GbaPpuObjectSet result;
  result.objects[0] = object_set0->objects[0] & object_set1->objects[0];
  result.objects[1] = object_set0->objects[1] & object_set1->objects[1];
  return result;
}

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OBJECT_SET_