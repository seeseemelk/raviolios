/**@file
 * @brief
 */
#ifndef _STRING_H_
#define _STRING_H_

#include <stddef.h>

extern "C" {

void* memcpy(void* dest, const void* src, size_t count);
void* memset(void* dest, int ch, size_t count);

}

#endif /* _STRING_H_ */
