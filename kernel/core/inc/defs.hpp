/**@file
 * @brief
 */
#ifndef _DEFS_HPP_
#define _DEFS_HPP_

extern "C" {
#include <stddef.h>
#include <stdint.h>
}

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#define KB(x) (x * 1024UL)
#define MB(x) (KB(x) * 1024UL)
#define GB(x) (MB(x) * 1024UL)

#endif /* _DEFS_HPP_ */
