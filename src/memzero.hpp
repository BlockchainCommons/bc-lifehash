#ifndef __MEMZERO_H__
#define __MEMZERO_H__

#include <stddef.h>

namespace LifeHash
{

// Memory-zeroing utility used by the SHA256 algorithm.
void memzero(void* const pnt, const size_t len);

} // namespace LifeHash

#endif
